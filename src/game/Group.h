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

#ifndef _GROUP_H_
#define _GROUP_H_

enum PartyErrors
{
	ERR_PARTY_NO_ERROR				  = 0,
	ERR_PARTY_CANNOT_FIND			   = 1,
	ERR_PARTY_IS_NOT_IN_YOUR_PARTY	  = 2,
	ERR_PARTY_UNK = 3,
	ERR_PARTY_IS_FULL				   = 4,
	ERR_PARTY_ALREADY_IN_GROUP		  = 5,
	ERR_PARTY_YOU_ARENT_IN_A_PARTY	  = 6,
	ERR_PARTY_YOU_ARE_NOT_LEADER		= 7,
	ERR_PARTY_WRONG_FACTION			 = 8,
	ERR_PARTY_IS_IGNORING_YOU		   = 9,
};

enum GroupTypes
{
	GROUP_TYPE_PARTY					= 0,
	GROUP_TYPE_RAID					 = 1,
};

enum MaxGroupCount
{
	MAX_GROUP_SIZE_PARTY				= 5,
	MAX_GROUP_SIZE_RAID				 = 40,
};

enum QuickGroupUpdateFlags
{
	PARTY_UPDATE_FLAG_POSITION			= 1,
	PARTY_UPDATE_FLAG_ZONEID			= 2,
};

struct PlayerInfo;
typedef struct
{
	PlayerInfo * player_info;
	Player * player;
}GroupMember;

class Group;
class Player;

typedef std::list<GroupMember> GroupMembersSet;

class SubGroup	  // Most stuff will be done through here, not through the "Group" class.
{
public:
	friend class Group;

	SubGroup(Group* parent, uint32 id):m_Parent(parent),m_Id(id)
	{
	}

	~SubGroup();

	inline GroupMembersSet::iterator GetGroupMembersBegin(void) { return m_GroupMembers.begin(); }
	inline GroupMembersSet::iterator GetGroupMembersEnd(void)   { return m_GroupMembers.end();   }

	bool AddPlayer(PlayerInfo * info, Player *pPlayer);
	void RemovePlayer(PlayerInfo * info, Player *pPlayer, bool forced_remove);
	bool HasMember(uint64 guid);
	

	inline bool IsFull(void)				{ return m_GroupMembers.size() >= MAX_GROUP_SIZE_PARTY; }
	inline uint32 GetMemberCount(void)		{ return m_GroupMembers.size(); }
	
	inline uint32 GetID(void)			   { return m_Id; }
	inline void SetID(uint32 newid)		 { m_Id = newid; }

	inline void   SetParent(Group* parent)  { m_Parent = parent; }
	inline Group* GetParent(void)		   { return m_Parent; }

	void   Disband(bool bRemoveGroup);

protected:

	GroupMembersSet	 m_GroupMembers;
	Group*			  m_Parent;
	uint32			  m_Id;

};

class SERVER_DECL Group
{
public:
	friend class SubGroup;

	static Group* Create();

	Group();
	~Group();

	// Adding/Removal Management
	bool AddMember(PlayerInfo * info, Player* pPlayer, int32 subgroupid=-1);
	void RemovePlayer(PlayerInfo * info, Player* pPlayer, bool forced_remove);
	void UpdateMember(PlayerInfo * info, Player * pPlayer);

	// Leaders and Looting
	void SetLeader(Player* pPlayer,bool silent);
	void SetLooter(Player *pPlayer, uint8 method, uint16 threshold);
	Player* GetnextRRlooter();

	// Transferring data to clients
	void Update(bool delayed = false);

	void SendPacketToAll(WorldPacket *packet);
	void SendPacketToAllButOne(WorldPacket *packet, Player *pSkipTarget);
	void SendNullUpdate(Player *pPlayer);

	// Group Combat
	void SendPartyKillLog(Object * player, Object * Unit);

	// Destroying/Converting
	void Disband();
	Player* FindFirstPlayer();
	
	// Accessing functions
	inline SubGroup* GetSubGroup(uint32 Id)
	{
		if(Id >= 8)
			return 0;

		return m_SubGroups[Id];
	}

	inline uint32 GetSubGroupCount(void) { return m_SubGroupCount; }

	inline uint8 GetMethod(void) { return m_LootMethod; }
	inline uint8 GetThreshold(void) { return m_LootThreshold; }
	inline Player* GetLeader(void) { return m_Leader; }
	inline Player* GetLooter(void) { return m_Looter; }

	void MovePlayer(PlayerInfo* info, uint8 subgroup);

	bool HasMember(Player *pPlayer);
	bool HasMember(PlayerInfo * info);
	inline uint32 MemberCount(void) { return m_MemberCount; }
	inline bool IsFull() { return ((m_GroupType == GROUP_TYPE_PARTY && m_MemberCount >= MAX_GROUP_SIZE_PARTY) || (m_GroupType == GROUP_TYPE_RAID && m_MemberCount >= MAX_GROUP_SIZE_RAID)); }

	SubGroup* FindFreeSubGroup();

	void ExpandToRaid();

	void SaveToDB();
	void LoadFromDB(Field *fields);

	inline uint8 GetGroupType() { return m_GroupType; }
	inline uint32 GetID() { return m_Id; }

	void UpdateOutOfRangePlayer(Player * pPlayer, uint32 Flags, bool Distribute, WorldPacket * Packet);
	void UpdateAllOutOfRangePlayersFor(Player * pPlayer);
	void HandleUpdateFieldChange(uint32 Index, Player * pPlayer);
	void HandlePartialChange(uint32 Type, Player * pPlayer);

	uint64 m_targetIcons[8];
	bool m_disbandOnNoMembers;
	inline Mutex& getLock() { return m_groupLock; }
	inline void Lock() { m_groupLock.Acquire(); }
	inline void Unlock() { return m_groupLock.Release(); }

protected:
	
	Player* m_Leader;
	Player* m_Looter;
	Player* lastRRlooter; //used to determine next RR looter
	uint8 m_LootMethod;
	uint16 m_LootThreshold;
	uint8 m_GroupType;
	uint32 m_Id;

	SubGroup* m_SubGroups[8];
	uint8 m_SubGroupCount;
	uint32 m_MemberCount;
	Mutex m_groupLock;
	bool m_dirty;
	bool m_updateblock;
};

#endif  // _GROUP_H_
