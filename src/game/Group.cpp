/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

enum PartyUpdateFlags
{
	GROUP_UPDATE_FLAG_NONE						= 0,		// 0x00000000
	GROUP_UPDATE_FLAG_ONLINE					= 1,		// 0x00000001  uint8
	GROUP_UPDATE_FLAG_HEALTH					= 2,		// 0x00000002  uint16
	GROUP_UPDATE_FLAG_MAXHEALTH					= 4,		// 0x00000004  uint16
	GROUP_UPDATE_FLAG_POWER_TYPE				= 8,		// 0x00000008  uint16
	GROUP_UPDATE_FLAG_POWER						= 16,		// 0x00000010  uint16
	GROUP_UPDATE_FLAG_MAXPOWER					= 32,		// 0x00000020  uint16
	GROUP_UPDATE_FLAG_LEVEL						= 64,		// 0x00000040  uint16
	GROUP_UPDATE_FLAG_ZONEID					= 128,		// 0x00000080  uint16
	GROUP_UPDATE_FLAG_POSITION					= 256,		// 0x00000100  uint16, uint16
	GROUP_UPDATE_FLAG_UNK_STRANGE				= 512,		// 0x00000200  uint64, uint16 for each uint64
	GROUP_UPDATE_FLAG_UNK_1						= 1024,		// 0x00000400  uint64
	GROUP_UPDATE_FLAG_PET_NAME					= 2048,		// 0x00000800  string
	GROUP_UPDATE_FLAG_PET_UNK_1					= 4096,		// 0x00001000  uint16
	GROUP_UPDATE_FLAG_PET_UNK_2					= 8192,		// 0x00002000  uint16
	GROUP_UPDATE_FLAG_PET_UNK_3					= 16384,	// 0x00004000  uint16
	GROUP_UPDATE_FLAG_PET_UNK_4					= 32768,	// 0x00008000  uint8
	GROUP_UPDATE_FLAG_UNK_3						= 65535,	// 0x00010000  uint16
	GROUP_UPDATE_FLAG_UNK_4						= 131070,	// 0x00020000  uint16
	GROUP_UPDATE_FLAG_UNK_5						= 262144,	// 0x00040000  uint64, uint16 for each uint64
};

enum PartyUpdateFlagGroups
{
	GROUP_UPDATE_TYPE_FULL_CREATE				=	GROUP_UPDATE_FLAG_ONLINE | GROUP_UPDATE_FLAG_HEALTH | GROUP_UPDATE_FLAG_MAXHEALTH |
													GROUP_UPDATE_FLAG_POWER | GROUP_UPDATE_FLAG_LEVEL |
													GROUP_UPDATE_FLAG_ZONEID | GROUP_UPDATE_FLAG_MAXPOWER | GROUP_UPDATE_FLAG_POSITION,
	GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY		=   0x7FFC1800,
};

Group::Group()
{
	m_GroupType = GROUP_TYPE_PARTY;	 // Always init as party

	// Create initial subgroup
    memset(m_SubGroups,0, sizeof(SubGroup*)*8);
	m_SubGroups[0] = new SubGroup(this, 0);

	m_Leader = NULL;
	m_Looter = NULL;
	m_LootMethod = PARTY_LOOT_GROUP;
	m_LootThreshold = 2;
	m_SubGroupCount = 1;
	m_MemberCount = 0;

	//EventMgr::getSingleton().AddEvent(this, &Group::UpdatePositions, EVENT_GROUP_POSITION_UPDATE, 5000, 0);
	m_Id = objmgr.GenerateGroupId();
	ObjectMgr::getSingleton().AddGroup(this);
	lastRRlooter = NULL;
	m_dirty=false;
	m_updateblock=false;
	m_disbandOnNoMembers = true;
	memset(m_targetIcons, 0, sizeof(uint64) * 8);
	m_isqueued=false;
}

Group::~Group()
{
	// SAFE
	//EventMgr::getSingleton().RemoveEvents(this);
	ObjectMgr::getSingleton().RemoveGroup(this);
}

SubGroup::~SubGroup()
{
	GroupMembersSet::iterator itr = m_GroupMembers.begin();
	for(; itr != m_GroupMembers.end(); ++itr)
	{
		if(itr->player)
			itr->player->SetSubGroup(0);
	}

	m_GroupMembers.clear();
}

bool SubGroup::HasMember(uint64 guid)
{
	for(GroupMembersSet::iterator itr = m_GroupMembers.begin(); itr != m_GroupMembers.end(); ++itr)
		if(itr->player_info->guid==guid) return true;

	return false;
}

void SubGroup::RemovePlayer(PlayerInfo * info, Player *pPlayer, bool forced_remove)
{
	GroupMembersSet::iterator itr = m_GroupMembers.begin();
	for(; itr != m_GroupMembers.end(); ++itr)
	{
		if(itr->player_info == info)
		{
			if(pPlayer == NULL && !forced_remove)
			{
				/* player temporary remove (set to offline) */
				if(itr->player != NULL)
					itr->player->RemoveReference(&itr->player);

				info->subGroup=GetID();
			}
			else
			{
				/* full remove (player is removed from party) */
				if(itr->player != NULL)
					itr->player->RemoveReference(&itr->player);

				m_GroupMembers.erase(itr);
				m_Parent->m_MemberCount--;
				info->subGroup=0;
				if(pPlayer)
					pPlayer->SetSubGroup(0);
				m_Parent->m_dirty=true;
			}
			break;
		}
	}
}

bool SubGroup::AddPlayer(PlayerInfo * info, Player *pPlayer)
{
	if(IsFull())
		return false;

	if(pPlayer)
	{
		sLog.outDebug("GROUP: Adding player %s to subgroup %d", pPlayer->GetName(), m_Id);
		pPlayer->SetSubGroup(GetID());
	}

	info->subGroup=GetID();
	/* check if we already exist */
	for(GroupMembersSet::iterator itr = m_GroupMembers.begin(); itr != m_GroupMembers.end(); ++itr)
	{
		if(itr->player_info == info)
		{
			/* we're simply updating */
			if(itr->player != NULL)
				itr->player->RemoveReference(&itr->player);

			if(pPlayer)
				pPlayer->AddReference(&itr->player);

			return true;
		}
	}

	GroupMember mbr;
	mbr.player = NULL;
	mbr.player_info = info;
	m_GroupMembers.push_back(mbr);
	m_Parent->m_MemberCount++;
	m_Parent->m_dirty=true;
	GroupMembersSet::iterator itr = --m_GroupMembers.end();
	ASSERT(itr->player_info == info);
	if(pPlayer)
		pPlayer->AddReference(&itr->player);

	return true;
}

SubGroup * Group::FindFreeSubGroup()
{
	for(uint32 i = 0; i < m_SubGroupCount; i++)
		if(!m_SubGroups[i]->IsFull())
			return m_SubGroups[i];

	return NULL;
}

bool Group::AddMember(PlayerInfo * info, Player* pPlayer, int32 subgroupid)
{
	m_groupLock.Acquire();
	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}

	if(!IsFull())
	{
		SubGroup* subgroup = (subgroupid>0) ? m_SubGroups[subgroupid] : FindFreeSubGroup();
		if(subgroup == NULL)
		{
			// weird shit..
			if(pPlayer)
				sLog.outDebug("GROUP: Tried to add member %s but FindFreeSubGroup returned NULL!", pPlayer->GetName());

			m_groupLock.Release();
			return false;
		}

		if(subgroup->AddPlayer(info, pPlayer))
		{
			if(pPlayer)
			{
				pPlayer->SetGroup(this);

				if(m_Leader == NULL)
				{
					// We're the only member? Set us to the leader.
					pPlayer->AddReference(&m_Leader);
				}

				/* process any pending updates beforehand */
				pPlayer->ProcessPendingUpdates();

				//UpdateAllOutOfRangePlayersFor(pPlayer);
				sEventMgr.AddEvent(pPlayer,&Player::EventGroupFullUpdate,EVENT_PLAYER_UPDATE,1500,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			}

			Update();	// Send group update
			info->m_Group=this;

			m_groupLock.Release();
			return true;
		}
		else
		{
			m_groupLock.Release();
			info->m_Group=NULL;
			pPlayer->SetGroup(NULL);
			return false;
		}

	}
	else
	{
		m_groupLock.Release();
		return false;
	}
}

void Group::SetLeader(Player* pPlayer, bool silent)
{
	if(m_Leader != NULL)
		m_Leader->RemoveReference(&m_Leader);

	m_Leader = pPlayer;
	m_dirty=true;
	if(!silent)
	{
		WorldPacket *data = new WorldPacket;
		data->Initialize(SMSG_GROUP_SET_LEADER);
		*data << ((pPlayer) ? pPlayer->GetName() : "NONE");
	
		SendPacketToAll(data);
		delete data;
	}

	Update();
}

void Group::Update(bool delayed)
{
	if(m_updateblock)
		return;

	WorldPacket data(50 + (m_MemberCount * 20));
	GroupMembersSet::iterator itr1, itr2;

	uint32 i=0,j=0;
	SubGroup *sg1=NULL;
	SubGroup *sg2=NULL;
	m_groupLock.Acquire();

	for(i=0;i<m_SubGroupCount;i++)
	{
		sg1 = m_SubGroups[i];
		for(itr1 = sg1->GetGroupMembersBegin(); itr1!=sg1->GetGroupMembersEnd(); ++itr1)
		{
			/* skip offline players */
			if(!itr1->player) continue;
			if(delayed) itr1->player->ProcessPendingUpdates();

			/*          Check for Soulstone Caster            */
			/*          SoulStone effect removed if           */
			/* the giver and reciever are not in party / raid */
			bool removeSoulStone = true;
			uint32 SoulStoneGiver = itr1->player->SoulStoneReceiver;
			if(!SoulStoneGiver || itr1->player->GetGUID() == SoulStoneGiver) removeSoulStone = false;

			data.Initialize(SMSG_GROUP_LIST);
			data << uint8(m_GroupType);	//0=party,1=raid
			data << uint8(0);   // unk
			data << uint8(sg1->GetID());
			data << uint8(0);	// unk2
			data << uint64(0);	// unk3
			data << uint32(m_MemberCount-1);	// we don't include self
			for(j=0;j<m_SubGroupCount;j++)
			{
				sg2 = m_SubGroups[j];
				for(itr2 = sg2->GetGroupMembersBegin(); itr2 != sg2->GetGroupMembersEnd(); ++itr2)
				{
					if(itr1->player_info != itr2->player_info)
					{
						/*check the reciever id against all group members*/
						if(itr2->player_info->guid == SoulStoneGiver) removeSoulStone = false;

						data << itr2->player_info->name << itr2->player_info->guid << uint32(0);		// highguid
						if(itr2->player)
							data << uint8(1);
						else
							data << uint8(0);
						
						/*if(m_GroupType == GROUP_TYPE_RAID && (*itr2) == sg2->m_SubGroupLeader)
							data << uint8(80 + sg2->GetID());
						else*/	  // how do we do this? 80 obviously doesn't work.
							data << uint8(sg2->GetID());
						data << uint8(0);
					}
				}
			}

			if(m_Leader != NULL) data << m_Leader->GetGUID();
			else data << uint64(0);

			data << uint8(m_LootMethod);

			if(m_Looter != NULL) data << m_Looter->GetGUID();
			else data << uint64(0);

			data << uint8(m_LootThreshold);

            if(m_Leader != NULL) 
                data << (uint8)m_Leader->iInstanceType;
			else data << uint8(0); // lol maybe we should save group difficulty also if there is no leader

			/*data << uint64(0);	  // new in 2.0.3, dunno what it is
			data << uint64(0);	  // new in 2.0.3, dunno what it is*/
			
			itr1->player->GetSession()->SendPacket(&data);

			/* remove Soulstone Aura if the caster and recipient */
			/*        are not in the same group any more         */
			if(removeSoulStone)
				itr1->player->removeSoulStone();
		}		
	}

	if(m_dirty)
	{
		m_dirty=false;
		SaveToDB();
	}

	m_groupLock.Release();
}

void Group::Disband()
{
	m_groupLock.Acquire();

	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}

	uint32 i = 0;
	for(i = 0; i < m_SubGroupCount; i++)
	{
		SubGroup *sg = m_SubGroups[i];
		sg->Disband(true);
	}

	m_groupLock.Release();
	CharacterDatabase.Execute("DELETE FROM groups WHERE group_id = %u", m_Id);
	sInstanceMgr.OnGroupDestruction(this);
	delete this;	// destroy ourselves, the destructor removes from eventmgr and objectmgr.
}

void SubGroup::Disband(bool bRemoveGroup)
{
	WorldPacket data;
	data.SetOpcode(SMSG_GROUP_DESTROYED);

	WorldPacket data2;
	data2.SetOpcode(SMSG_PARTY_COMMAND_RESULT);
	data2 << uint32(2) << uint8(0) << uint32(0);	// you leave the group

	GroupMembersSet::iterator itr = m_GroupMembers.begin();
	for(; itr != m_GroupMembers.end();)
	{
		if(itr->player)
		{
			itr->player->GetSession()->SendPacket(&data);
			itr->player->GetSession()->SendPacket(&data2);
			itr->player->SetSubGroup(0);

			if(bRemoveGroup)
			{
				itr->player->SetGroup(NULL);
				m_Parent->SendNullUpdate(itr->player);
			}

			/* remove SoulStones if caster and reciever are not the same player */
			if(itr->player->SoulStoneReceiver && itr->player->SoulStoneReceiver!=itr->player->GetGUID())
				itr->player->removeSoulStone();

			itr->player->RemoveReference(&itr->player);
		}

		if(bRemoveGroup)
			itr->player_info->m_Group=NULL;

		m_Parent->m_MemberCount--;
		itr = m_GroupMembers.erase(itr);
	}

	m_Parent->m_SubGroups[m_Id] = NULL;
	delete this;
}

Player* Group::FindFirstPlayer()
{
//	SubGroup *sg = NULL;
	GroupMembersSet::iterator itr;
	m_groupLock.Acquire();

	uint32 i = 0;
	for(; i < m_SubGroupCount; i++) {
		for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
			if(itr->player)
			{
				m_groupLock.Release();
				return itr->player;   // return first
			}
		}
	}

	m_groupLock.Release();
	return NULL;
}

void Group::RemovePlayer(PlayerInfo * info, Player* pPlayer, bool forced_remove)
{
	WorldPacket data;
	m_groupLock.Acquire();
	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}
	
	SubGroup *sg=NULL;/* = GetSubGroup(pPlayer->GetSubGroup());
	ASSERT(sg); // something wrong here if that isn't right*/

	if(!pPlayer)
	{
		/* great, we gotta search all subgroups for him */
		for(int i = 0; i < m_SubGroupCount; ++i) {
			for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
				if(itr->player_info == info)
				{
					sg = m_SubGroups[i];
					break;
				}
			}
		}
	}
	else
	{
		sg = GetSubGroup(pPlayer->GetSubGroup());
		if(!sg->HasMember(pPlayer->GetGUID()))
		{
			sg=NULL;
			for(int i = 0; i < m_SubGroupCount; ++i) {
				for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
					if(itr->player_info == info)
					{
						sg = m_SubGroups[i];
						break;
					}
				}
			}
		}
	}
	
	if(!sg)
	{
		m_groupLock.Release();
		return;
	}

	sg->RemovePlayer(info, pPlayer,forced_remove);
	if(pPlayer!=NULL || forced_remove)
	{
		info->m_Group=NULL;
		info->subGroup=0;
	}

	if(pPlayer)
	{
		sInstanceMgr.PlayerLeftGroup(this, pPlayer);
		pPlayer->SetGroup(NULL);
		if(pPlayer->GetSession() != NULL)
		{
			SendNullUpdate(pPlayer);

			//pPlayer->RemoveAllAreaAuras();

			data.SetOpcode(SMSG_GROUP_DESTROYED);
			pPlayer->GetSession()->SendPacket(&data);

			data.Initialize(SMSG_PARTY_COMMAND_RESULT);
			data << uint32(2) << uint8(0) << uint32(0);  // you leave the group
			pPlayer->GetSession()->SendPacket(&data);
		}
	}

	if(m_MemberCount < 2)
	{
		if(m_disbandOnNoMembers)
		{
			/* remove some left-over references */
			if(m_Looter)
				m_Looter->RemoveReference(&m_Looter);

			if(m_Leader)
				m_Leader->RemoveReference(&m_Leader);

			m_groupLock.Release();
			Disband();
			return;
		}
	}

	/* eek! ;P */
	Player *newPlayer = FindFirstPlayer();

	if(m_Looter && m_Looter->GetGUID() == info->guid)
	{
		m_Looter->RemoveReference(&m_Looter);
		m_Looter = newPlayer;
		m_dirty=true;
	}

	if(m_Leader && m_Leader->GetGUID() == info->guid)
	{
		if(pPlayer != NULL)
		{
			/* full remove - display message */
			SetLeader(newPlayer, false);
		}
		else
		{
			/* partial remove - no message */
			m_Leader->RemoveReference(&m_Leader);
			m_Leader = newPlayer;
			m_dirty=true;
			Update(true);
		}
	}
	else
		Update(true);

	m_groupLock.Release();
}

void Group::SendPacketToAll(WorldPacket *packet)
{
	SendPacketToAllButOne(packet, NULL);
}

void Group::ExpandToRaid()
{
	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}
	// Very simple ;)

	uint32 i = 1;
	m_groupLock.Acquire();
	m_SubGroupCount = 8;

	for(; i < m_SubGroupCount; i++)
		m_SubGroups[i] = new SubGroup(this, i);

	m_GroupType = GROUP_TYPE_RAID;
	m_dirty=true;
	Update();
	m_groupLock.Release();
}

void Group::SetLooter(Player *pPlayer, uint8 method, uint16 threshold)
{ 
	if(m_Looter)
		m_Looter->RemoveReference(&m_Looter);

	m_LootMethod = method;
	m_Looter = pPlayer;
	m_LootThreshold  = threshold;
	m_dirty=true;
	Update();
}

//!! function is not fair. If new player pops in/out he might get a piece of the action to soon or let first player get more loots :)
Player* Group::GetnextRRlooter()
{
	//another methode would be to directly store a node from list so we can jump directly to next one. But that is not safe :P
	GroupMembersSet::iterator itr1;
	m_groupLock.Acquire();
	Player *firsthit=NULL,*prevp=NULL;
	for(int i = 0; i < m_SubGroupCount; i++)
	{
		for(itr1 = m_SubGroups[i]->GetGroupMembersBegin(); itr1 != m_SubGroups[i]->GetGroupMembersEnd(); ++itr1)
		{
			if(!itr1->player) continue;

			if(firsthit==NULL)
				firsthit = itr1->player;
			if(prevp==lastRRlooter)
			{
				m_groupLock.Release();
				return itr1->player;
			}
			prevp = itr1->player;
		}
	}
	//if we got here it means player eighter exited group or it was last one
	m_groupLock.Release();
	return firsthit;
}

void Group::SendPacketToAllButOne(WorldPacket *packet, Player *pSkipTarget)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;
	m_groupLock.Acquire();
	for(; i < m_SubGroupCount; i++)
		for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
			if(itr->player && itr->player != pSkipTarget) itr->player->GetSession()->SendPacket(packet);

	m_groupLock.Release();
}

bool Group::HasMember(Player * pPlayer)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;
	m_groupLock.Acquire();

	for(; i < m_SubGroupCount; i++) {
		for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
			if(itr->player == pPlayer)
			{
				m_groupLock.Release();
				return true;
			}
		}
	}

	m_groupLock.Release();
	return false;
}

bool Group::HasMember(PlayerInfo * info)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;

	m_groupLock.Acquire();

	for(; i < m_SubGroupCount; i++) {
		for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
			if(itr->player_info == info)
			{
				m_groupLock.Release();
				return true;
			}
		}
	}

	m_groupLock.Release();
	return false;
}

void Group::MovePlayer(PlayerInfo *info, uint8 subgroup)
{
	ASSERT(subgroup < m_SubGroupCount);
	m_groupLock.Acquire();
	SubGroup *sg=NULL;
	Player * pl=NULL;

	/* great, we gotta search all subgroups for him */
	for(int i = 0; i < m_SubGroupCount; ++i) {
		for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr) {
			if(itr->player_info == info)
			{
				sg = m_SubGroups[i];
				pl = itr->player;
				break;
			}
		}
	}

	if(!sg)
	{
		m_groupLock.Release();
		return;
	}
	
	sg->RemovePlayer(info, NULL, true);

	// Grab the new group, and insert
	sg = m_SubGroups[subgroup];
	sg->AddPlayer(info,pl);
	info->subGroup=sg->GetID();

	Update();
	m_groupLock.Release();
}

void Group::SendNullUpdate(Player *pPlayer)
{
	// this packet is 24 bytes long.		// AS OF 2.1.0
	uint8 buffer[24];
	memset(buffer, 0, 24);
	pPlayer->GetSession()->OutPacket(SMSG_GROUP_LIST, 24, buffer);
}

// player is object class becouse its called from unit class
void Group::SendPartyKillLog(Object * player, Object * Unit)
{
	if (!player || !Unit || !HasMember(((Player*)player)))
		return;

	WorldPacket data(SMSG_PARTYKILLLOG, 16);
	data << player->GetGUID();
	data << Unit->GetGUID();
	SendPacketToAll(&data);
}

void Group::LoadFromDB(Field *fields)
{
	m_updateblock=true;
	m_Id = fields[0].GetUInt32();
	m_GroupType = fields[1].GetUInt8();
	m_SubGroupCount = fields[2].GetUInt8();
	m_LootMethod = fields[3].GetUInt8();
	m_LootThreshold = fields[4].GetUInt8();

	// create groups
	for(int i = 1; i < m_SubGroupCount; ++i)
		m_SubGroups[i] = new SubGroup(this, i);

	// assign players into groups
	for(int i = 0; i < m_SubGroupCount; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			uint32 guid = fields[5 + (i*5) + j].GetUInt32();
			PlayerInfo * inf = objmgr.GetPlayerInfo(guid);
			if(inf == NULL) continue;
			AddMember(inf, NULL, i);
			m_dirty=false;
		}
	}
	m_updateblock=false;
}

void Group::SaveToDB()
{
	if(!m_disbandOnNoMembers)	/* don't save bg groups */
		return;

	std::stringstream ss;
	//uint32 i = 0;
	uint32 fillers = 8 - m_SubGroupCount;

	ss << "REPLACE INTO groups VALUES("
		<< m_Id << ","
		<< uint32(m_GroupType) << ","
		<< uint32(m_SubGroupCount) << ","
		<< uint32(m_LootMethod) << ","
		<< uint32(m_LootThreshold) << ",";

	for(uint32 i = 0; i < m_SubGroupCount; ++i)
	{
		uint32 j = 0;
		for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); j<5 && itr != m_SubGroups[i]->GetGroupMembersEnd(); ++j, ++itr)
		{
			ss << itr->player_info->guid << ",";
		}

		for(; j < 5; ++j)
			ss << "0,";
	}

	for(uint32 i = 0; i < fillers; ++i)
		ss << "0,0,0,0,0,";

	ss << (uint32)UNIXTIME << ")";
	/*printf("==%s==\n", ss.str().c_str());*/
	CharacterDatabase.Execute(ss.str().c_str());
}

void Group::UpdateOutOfRangePlayer(Player * pPlayer, uint32 Flags, bool Distribute, WorldPacket * Packet)
{
	WorldPacket * data = Packet;
	if(!Packet)
		data = new WorldPacket(SMSG_PARTY_MEMBER_STATS, 500);

	if(pPlayer->GetPowerType() != POWER_TYPE_MANA)
		Flags |= GROUP_UPDATE_FLAG_POWER_TYPE;

	/*Flags |= GROUP_UPDATE_FLAG_PET_NAME;
	Flags |= GROUP_UPDATE_FLAG_PET_UNK_1;*/

	data->Initialize(SMSG_PARTY_MEMBER_STATS);
	*data << pPlayer->GetNewGUID();
	*data << Flags;

	if(Flags & GROUP_UPDATE_FLAG_ONLINE)
	{
		if(pPlayer->IsPvPFlagged())
			*data << uint8(3);
		else
			*data << uint8(1);
	}

	if(Flags & GROUP_UPDATE_FLAG_HEALTH)
		*data << uint16(pPlayer->GetUInt32Value(UNIT_FIELD_HEALTH));

	if(Flags & GROUP_UPDATE_FLAG_MAXHEALTH)
		*data << uint16(pPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

	if(Flags & GROUP_UPDATE_FLAG_POWER_TYPE)
		*data << uint8(pPlayer->GetPowerType());

	if(Flags & GROUP_UPDATE_FLAG_POWER)
		*data << uint16(pPlayer->GetUInt32Value(UNIT_FIELD_POWER1 + pPlayer->GetPowerType()));

	if(Flags & GROUP_UPDATE_FLAG_MAXPOWER)
		*data << uint16(pPlayer->GetUInt32Value(UNIT_FIELD_MAXPOWER1 + pPlayer->GetPowerType()));

	if(Flags & GROUP_UPDATE_FLAG_LEVEL)
		*data << uint16(pPlayer->getLevel());

	if(Flags & GROUP_UPDATE_FLAG_ZONEID)
		*data << uint16(pPlayer->GetZoneId());

	if(Flags & GROUP_UPDATE_FLAG_POSITION)
	{
		*data << int16(pPlayer->GetPositionX()) << int16(pPlayer->GetPositionY());			// wtf packed floats? O.o
		pPlayer->m_last_group_position = pPlayer->GetPosition();
	}

	if(Flags & GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY)
	{
		// Full update - we have to put some weird extra shit on the end :/
        
		/*
		{SERVER} Packet: (0x02F2) CMSG_PET_UNLEARN PacketSize = 46

		07 EB BB 69 - guid
		F7 1B FC 7F - mask - 1111111111111000001101111110111 - full
		// known parts
		0000000000000000000000000000001 - online
		0000000000000000000000000000010 - health
		0000000000000000000000000000100 - max health
		0000000000000000000000000010000 - power
		0000000000000000000000000100000 - max power
		0000000000000000000000001000000 - level
		0000000000000000000000010000000 - zoneid
		0000000000000000000000100000000 - level
		0000000000000000000001000000000 - position

		// unknown parts
		1111111111111000001101111110111 - full
		0000000000001000000000000000000 - GROUP_UPDATE_FLAG_UNK_5 = 262144,	// 0x00040000  uint64, uint16 for each uint64
		1111111111111000000000000000000 - 

		03 - online
		7D 02 - health
		0E 04 - max health
		37 05 - power
		BA|07 - max power
		1B 00 - level
		28 00 - zoneid
		39 D8 B9 03 - position

		-- this is what we append on the end of a full update.. no idea what is is
		01 00 00 00
		00 00 00 FF
		4A 78 00 58
		26 00 00 00
		00 00 00 00
		FF
		-------------------------------------------------------------------

		*/
		*data << uint32(0x00000001);
		*data << uint32(0xFF000000);
		*data << uint32(0x5800784A);
		*data << uint32(0x00000026);
		*data << uint32(0x00000000);
		*data << uint8(0xFF);
	}

	if(Distribute)
	{
		Player * plr;
		float dist = sWorld.m_UpdateDistance;
		m_groupLock.Acquire();
		for(uint32 i = 0; i < m_SubGroupCount; ++i)
		{
			for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd();)
			{
				plr = itr->player;
				++itr;

				if(plr && plr != pPlayer && plr->GetMapMgr() == pPlayer->GetMapMgr())
				{
					if(plr->CalcDistance(pPlayer) > dist)
						plr->GetSession()->SendPacket(data);
				}
			}
		}
		m_groupLock.Release();
	}

	if(!Packet)
		delete data;
}

void Group::UpdateAllOutOfRangePlayersFor(Player * pPlayer)
{
	WorldPacket data(150);
	WorldPacket data2(150);

	if(m_SubGroupCount>8)
		return;

	/* tell the other players about us */
	UpdateOutOfRangePlayer(pPlayer, GROUP_UPDATE_TYPE_FULL_CREATE, true, &data2);

	/* tell us any other players we don't know about */
	Player * plr;
	bool u1, u2;
	UpdateMask myMask;
	myMask.SetCount(PLAYER_END);
	UpdateMask hisMask;
	hisMask.SetCount(PLAYER_END);

	m_groupLock.Acquire();
	for(uint32 i = 0; i < m_SubGroupCount; ++i)
	{
		for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
		{
			plr = itr->player;
			if(!plr || plr == pPlayer) continue;

			if(!plr->IsVisible(pPlayer))
			{
				UpdateOutOfRangePlayer(plr, GROUP_UPDATE_TYPE_FULL_CREATE, false, &data);
				pPlayer->GetSession()->SendPacket(&data);
			}
			else
			{
				if(pPlayer->GetSubGroup() == plr->GetSubGroup())
				{
					/* distribute quest fields to other players */
					hisMask.Clear();
					myMask.Clear();
					u1 = u2 = false;
					for(uint32 i = PLAYER_QUEST_LOG_1_1; i < PLAYER_QUEST_LOG_25_01; ++i)
					{
						if(plr->GetUInt32Value(i))
						{
							hisMask.SetBit(i);
							u1 = true;
						}

						if(pPlayer->GetUInt32Value(i))
						{
							u2 = true;
							myMask.SetBit(i);
						}
					}

					if(u1)
					{
						data.clear();
                        plr->BuildValuesUpdateBlockForPlayer(&data, &hisMask);
						pPlayer->PushUpdateData(&data, 1);
					}

					if(u2)
					{
						data.clear();
						pPlayer->BuildValuesUpdateBlockForPlayer(&data, &myMask);
						plr->PushUpdateData(&data, 1);
					}
				}
			}
		}
	}

	m_groupLock.Release();
}

void Group::HandleUpdateFieldChange(uint32 Index, Player * pPlayer)
{
	uint32 Flags = 0;
	m_groupLock.Acquire();
	switch(Index)
	{
	case UNIT_FIELD_HEALTH:
		Flags = GROUP_UPDATE_FLAG_HEALTH;
		break;
		
	case UNIT_FIELD_MAXHEALTH:
		Flags = GROUP_UPDATE_FLAG_MAXHEALTH;
		break;

	case UNIT_FIELD_POWER1:
	case UNIT_FIELD_POWER2:
	case UNIT_FIELD_POWER3:
	case UNIT_FIELD_POWER4:
		Flags = GROUP_UPDATE_FLAG_POWER;
		break;

	case UNIT_FIELD_MAXPOWER1:
	case UNIT_FIELD_MAXPOWER2:
	case UNIT_FIELD_MAXPOWER3:
	case UNIT_FIELD_MAXPOWER4:
		Flags = GROUP_UPDATE_FLAG_MAXPOWER;
		break;

	case UNIT_FIELD_LEVEL:
		Flags = GROUP_UPDATE_FLAG_LEVEL;
		break;
	}

	if(Flags)
		UpdateOutOfRangePlayer(pPlayer, Flags, true, 0);

	m_groupLock.Release();
}

void Group::HandlePartialChange(uint32 Type, Player * pPlayer)
{
	uint32 Flags = 0;
	m_groupLock.Acquire();

	switch(Type)
	{
	case PARTY_UPDATE_FLAG_POSITION:
		Flags = GROUP_UPDATE_FLAG_POSITION;
		break;

	case PARTY_UPDATE_FLAG_ZONEID:
		Flags = GROUP_UPDATE_FLAG_ZONEID;
		break;
	}

	if(Flags)
		UpdateOutOfRangePlayer(pPlayer, Flags, true, 0);

	m_groupLock.Release();
}

void WorldSession::HandlePartyMemberStatsOpcode(WorldPacket & recv_data)
{
	if(!_player->IsInWorld())
		return;

	uint64 guid;
	recv_data >> guid;

	Player * plr = _player->GetMapMgr()->GetPlayer((uint32)guid);

	if(!_player->GetGroup() || !plr)
		return;

	WorldPacket data(200);
	if(!_player->GetGroup()->HasMember(plr))
		return;			// invalid player

	if(_player->IsVisible(plr))
		return;

	_player->GetGroup()->UpdateOutOfRangePlayer(plr, GROUP_UPDATE_TYPE_FULL_CREATE | GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY, false, &data);
	data.SetOpcode(CMSG_PET_UNLEARN);
	SendPacket(&data);
}

Group* Group::Create()
{
	return new Group;
}
