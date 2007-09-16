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

//Pakcet Building
/////////////////

WorldPacket* BuildQuestQueryResponse(Quest *qst)
{
	// 2048 bytes should be more than enough. The fields cost ~200 bytes.
	// better to allocate more at startup than have to realloc the buffer later on.

	WorldPacket *data = new WorldPacket(SMSG_QUEST_QUERY_RESPONSE, 2048);
   
	*data << uint32(qst->id);					   // Quest ID
	*data << uint32(2);							 // Unknown, always seems to be 2
	*data << uint32(qst->max_level);				// Quest level

	if(qst->quest_sort > 0)
		*data << uint32(qst->quest_sort);	  // Negative if pointing to a sort.
	else
		*data << uint32(qst->zone_id);			  // Positive if pointing to a zone.

	*data << uint32(qst->type);					 // Info ID / Type
	*data << uint32(qst->required_rep_faction);	 // Faction ID
	*data << uint32(qst->required_rep_value);	   // Faction Amount
	*data << uint32(0);							 // Unknown (always 0)
	*data << uint32(0);							 // Unknown (always 0)
	*data << uint32(0);							 // Unknown (always 0) was added for dirty fix remove this
	*data << uint32(qst->next_quest_id);			// Next Quest ID
	*data << uint32(qst->reward_money);			 // Copper reward
 // disabled for dirty fix remove this   *data << uint32(qst->reward_xp_as_money);	   // Copper given instead of XP
	*data << uint32(qst->required_money);		   // Required Money
	*data << uint32(qst->reward_spell);			 // Spell added to spellbook upon completion
	*data << uint32(qst->effect_on_player);		 // Spell casted on player upon completion
	*data << uint32(qst->srcitem);				  // Item given at the start of a quest (srcitem)
	*data << uint32(qst->quest_flags);			  // Quest Flags

	// (loop 4 times)
	for(uint32 i = 0; i < 4; ++i)
	{
		*data << qst->reward_item[i];			   // Forced Reward Item [i]
		*data << qst->reward_itemcount[i];		  // Forced Reward Item Count [i]
	}

	// (loop 6 times)
	for(uint32 i = 0; i < 6; ++i)
	{
		*data << qst->reward_choiceitem[i];		 // Choice Reward Item [i]
		*data << qst->reward_choiceitemcount[i];	// Choice Reward Item Count [i]
	}

	*data << qst->point_mapid;					  // Unknown
	*data << qst->point_x;						  // Unknown
	*data << qst->point_y;						  // Unknown
	*data << qst->point_opt;						// Unknown
	
	*data << qst->title;							// Title / name of quest
	*data << qst->objectives;					   // Objectives / description
	*data << qst->details;						  // Details
	*data << qst->endtext;						  // Subdescription

	// (loop 4 times)
	for(uint32 i = 0; i < 4; ++i)
	{
		*data << qst->required_mob[i];			  // Kill mob entry ID [i]
		*data << qst->required_mobcount[i];		 // Kill mob count [i]

		*data << qst->required_item[i];			 // Collect item [i]
		*data << qst->required_itemcount[i];		// Collect item count [i]
	}

	*data << qst->objectivetexts[0];				// Objective 1 - Used as text if mob not set
	*data << qst->objectivetexts[1];				// Objective 2 - Used as text if mob not set
	*data << qst->objectivetexts[2];				// Objective 3 - Used as text if mob not set
	*data << qst->objectivetexts[3];				// Objective 4 - Used as text if mob not set

	return data;
}


/*****************
* QuestLogEntry *
*****************/
QuestLogEntry::QuestLogEntry()
{
	mInitialized = false;
	m_quest = NULL;
	mDirty = false;
	m_slot = -1;
	_questScript = NULL;
	completed=0;
}

void QuestLogEntry::Init(Quest* quest, Player* plr, uint32 slot)
{
	ASSERT(quest);
	ASSERT(plr);

	m_quest = quest;
	m_plr = plr;
	m_slot = slot;

	// update slot
	plr->SetQuestLogSlot(this, slot);
	
	mDirty = true;

	memset(m_mobcount, 0, 4*4);
	memset(m_explored_areas, 0, 4*4);

	if(m_quest->time)
		m_time_left = m_quest->time * 1000;
	else
		m_time_left = 0;

	LoadScript();
	CALL_QUESTSCRIPT_EVENT(this, OnQuestStart)(plr);
}

void QuestLogEntry::SaveToDB()
{
	ASSERT(m_slot != -1);
	if(!mDirty)
		return;

	CharacterDatabase.Execute("DELETE FROM questlog WHERE player_guid=%u AND quest_id=%u", m_plr->GetGUIDLow(), m_quest->id);
	std::stringstream ss;
	ss << "INSERT INTO questlog (player_guid, quest_id, slot, time_left, explored_area1, explored_area2, explored_area3, explored_area4, mob_kill1, mob_kill2, mob_kill3, mob_kill4) VALUES(";
	ss << m_plr->GetGUIDLow() << "," << m_quest->id << "," << m_slot << "," << m_time_left;
	for(int i = 0; i < 4; ++i)
		ss << "," << m_explored_areas[i];
	
	for(int i = 0; i < 4; ++i)
		ss << "," << m_mobcount[i];

	ss << ")";
	
	CharacterDatabase.Execute(ss.str().c_str());
}

bool QuestLogEntry::LoadFromDB(Field *fields)
{
	// index,playerguid,questid,timeleft,area0,area1,area2,area3,kill0,kill1,kill2,kill3
	int f = 4;
	ASSERT(m_plr && m_quest);
	m_time_left = fields[f].GetUInt32();	f++;
	for(int i = 0; i < 4; ++i)
	{
		m_explored_areas[i] = fields[f].GetUInt32();	f++;
		CALL_QUESTSCRIPT_EVENT(this, OnExploreArea)(m_explored_areas[i], m_plr);
	}

	for(int i = 0; i < 4; ++i)
	{
		m_mobcount[i] = fields[f].GetUInt32();	f++;
		if(GetQuest()->required_mobtype[i] == QUEST_MOB_TYPE_CREATURE)
			CALL_QUESTSCRIPT_EVENT(this, OnCreatureKill)(GetQuest()->required_mob[i], m_plr);
		else
			CALL_QUESTSCRIPT_EVENT(this, OnGameObjectActivate)(GetQuest()->required_mob[i], m_plr);
	}
	mDirty = false;
	return true;
}

bool QuestLogEntry::CanBeFinished()
{
	uint32 i;
	for(i = 0; i < 4; ++i)
	{
		if(m_quest->required_mob[i])
		{
			if(m_mobcount[i] < m_quest->required_mobcount[i])
			{
				return false;
			}
		}
	}

	for(i = 0; i < 4; ++i)
	{
		if(m_quest->required_item[i])
		{
			if(m_plr->GetItemInterface()->GetItemCount(m_quest->required_item[i]) < m_quest->required_itemcount[i])
			{
				return false;
			}
		}
	}

	for(i = 0; i < 4; ++i)
	{
		if(m_quest->required_triggers[i])
		{
			if(m_explored_areas[i] == 0)
				return false;
		}
	}

	return true;
}

void QuestLogEntry::SubtractTime(uint32 value)
{
	if(this->m_time_left  <=value)
		m_time_left = 0;
	else
		m_time_left-=value;
}

void QuestLogEntry::SetMobCount(uint32 i, uint32 count)
{
	ASSERT(i<4);
	m_mobcount[i] = count;
	mDirty = true;
}

void QuestLogEntry::SetTrigger(uint32 i)
{
	ASSERT(i<4);
	m_explored_areas[i] = 1;
	mDirty = true;
}

void QuestLogEntry::SetSlot(int32 i)
{
	ASSERT(i!=-1);
	m_slot = i;
}

void QuestLogEntry::Finish()
{
	uint32 base = GetBaseField(m_slot);
	m_plr->SetUInt32Value(base + 0, 0);
	m_plr->SetUInt32Value(base + 1, 0);
	m_plr->SetUInt32Value(base + 2, 0);

	// clear from player log
	m_plr->SetQuestLogSlot(NULL, m_slot);
	m_plr->PushToRemovedQuests(m_quest->id);

	// delete ourselves
	delete this;
}

void QuestLogEntry::UpdatePlayerFields()
{
	if(!m_plr)
		return;

	uint32 base = GetBaseField(m_slot);
	m_plr->SetUInt32Value(base + 0, m_quest->id);

	// next field is kills and shit like that
	uint32 field1 = 0;
	int i;
	
	// explored areas
	if(m_quest->count_requiredtriggers)
	{
		uint32 count = 0;
		for(i = 0; i < 4; ++i)
		{
			if(m_quest->required_triggers[i])
			{
				if(m_explored_areas[i] == 1)
				{
					count++;
				}
			}
		}

		if(count == m_quest->count_requiredtriggers)
		{
			field1 |= 0x01000000;
		}
	}

	// mob hunting
	if(m_quest->count_required_mob)
	{
		uint8 cnt;
		for(int i = 0; i < 4; ++i)
		{
			if(m_quest->required_mob[i] && m_mobcount[i] > 0)
			{
				// 1 << (offset * 6)
				cnt = m_mobcount[i];
				field1 |= (cnt << (i*6));
			}
		}
	}

	m_plr->SetUInt32Value(base + 1, field1);
	m_plr->SetUInt32Value(base + 2, m_time_left);
}

void QuestLogEntry::SendQuestComplete()
{
	WorldPacket data(4);
	data.SetOpcode(SMSG_QUESTUPDATE_COMPLETE);
	data << m_quest->id;
	m_plr->GetSession()->SendPacket(&data);
	CALL_QUESTSCRIPT_EVENT(this, OnQuestComplete)(m_plr);
}

void QuestLogEntry::SendUpdateAddKill(uint32 i)
{
	sQuestMgr.SendQuestUpdateAddKill(m_plr, m_quest->id, m_quest->required_mob[i], m_mobcount[i], m_quest->required_mobcount[i], 0);
}

void QuestLogEntry::LoadScript()
{
	//_questScript = sScriptMgr.CreateQuestScriptClassForEntry(GetQuest()->id, this);
}

void QuestLogEntry::CallScriptUpdate()
{
	ASSERT(_questScript);
	_questScript->EventUpdate();
}

QuestLogEntry::~QuestLogEntry()
{
	//sEventMgr.RemoveEvents(this);
	if(_questScript != 0)
		_questScript->Destroy();
}

