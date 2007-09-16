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

static int32 STANDING_TO_RANK[8] = {-36000, -6000, -3000, 3000, 6000, 12000, 21000, 1000};
//static uint32 INITIALIZE_FACTIONS[128] = { 21, 92, 93, 59, 349, 70, 369, 471, 470, 169, 469, 67, 529, 76, 530, 81, 68, 54, 72, 47, 69, 86, 83, 549, 551, 550, 589, 577, 46, 289, 570, 571, 569, 574, 576, 609, 947, 946, 935, 730, 729, 749, 980, 809, 890, 889, 891, 892, 930, 909, 270, 510, 509, 910, 911, 922, 990, 932, 936, 933, 941, 934, 967, 942, 970, 978, 989, 1005, 1011, 1012, 1015, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#define FACTION_FLAG_AT_WAR  2
#define FACTION_FLAG_VISIBLE 1

Standing Player::GetReputationRankFromStanding(int32 Standing_)
{
	uint32 i = 0;
	for(; i < 8; )
	{
		if(Standing_ > STANDING_TO_RANK[i])
		{
			++i;
		}
		else
		{
			return Standing(i);
		}
	}
	return STANDING_EXALTED;
}

inline void SetFlagAtWar(uint8 & flag)
{
	if(flag & FACTION_FLAG_AT_WAR)
		return;

	flag |= FACTION_FLAG_AT_WAR;
}

inline void UnsetFlagAtWar(uint8 & flag)
{
	if(!(flag & FACTION_FLAG_AT_WAR))
		return;

	flag &= ~FACTION_FLAG_AT_WAR;
}

inline void SetFlagVisible(uint8 & flag)
{
	if(flag & FACTION_FLAG_VISIBLE)
		return;

	flag |= FACTION_FLAG_VISIBLE;
}

inline void UnsetFlagVisible(uint8 & flag)
{
	if(!(flag & FACTION_FLAG_VISIBLE))
		return;

	flag &= ~FACTION_FLAG_VISIBLE;
}

inline bool AtWar(uint8 flag) { return flag & FACTION_FLAG_AT_WAR; }
inline bool Visible(uint8 flag) { return flag & FACTION_FLAG_VISIBLE; }

inline bool RankChanged(int32 Standing, int32 Change)
{
	if(Player::GetReputationRankFromStanding(Standing) != Player::GetReputationRankFromStanding(Standing + Change))
		return true;
	else
		return false;
}

inline bool RankChangedFlat(int32 Standing, int32 NewStanding)
{
	if(Player::GetReputationRankFromStanding(Standing) != Player::GetReputationRankFromStanding(NewStanding))
		return true;
	else
		return false;
}

void Player::smsg_InitialFactions()
{
	WorldPacket data(SMSG_INITIALIZE_FACTIONS, 644);
	data << uint32(128);
	FactionReputation * rep;
	for(uint32 i = 0; i < 128; ++i)
	{
		rep = reputationByListId[i];
		if(rep)
		{
			/*if(rep->flag & 2 && rep->standing >= 0)
			{
				printf("listid %u\n", i);
			}*/
			data << rep->flag << rep->CalcStanding();
		}
		else
			data << uint8(0) << uint32(0);
	}
	m_session->SendPacket(&data);
}

void Player::_InitialReputation()
{
	// Generate Initial Reputation Values
	FactionReputation * rep;
	FactionDBC * f;
	int32 BaseRep = 0;
	uint32 j;
	uint32 RaceMask = getRaceMask();
	for(uint32 i = 0; i < sFactionStore.GetNumRows(); ++i)
	{
		f = static_cast<FastIndexedDataStore<FactionDBC>*>(FactionStore::getSingletonPtr())->LookupRow(i);
		if(f == 0) continue;

		// Get Base Reputation
		for(j = 0; j < 4; ++j)
		{
			if(f->baseRepMask[j] & RaceMask)
			{
				BaseRep = f->baseRepValue[j];
				break;
			}
		}

		if(j != 4 && f->RepListId >= 0)
		{
			// This is one we can add.
			rep = new FactionReputation;
			rep->standing = BaseRep;
			rep->flag = 0;
			rep->baseStanding = BaseRep;
			if(GetReputationRankFromStanding(BaseRep) <= STANDING_HOSTILE)
				SetFlagAtWar(rep->flag);

			// Visible-ness shit.
			if((f->parentFaction == 469 && GetTeam() == 0) ||		// Alliance own faction.
				(f->parentFaction == 67 && GetTeam() == 1) )		 // Horde own faction.
			{
				SetFlagVisible(rep->flag);
			}				 
				
			reputationByListId[f->RepListId] = rep;
		}
		else
		{
			// No default standing.
			continue;
		}

		m_reputation[f->ID] = rep;
	}
}

int32 Player::GetStanding(uint32 Faction)
{
	ReputationMap::iterator itr = m_reputation.find(Faction);
	if(itr == m_reputation.end()) return 0;
	else return itr->second->standing;
}

int32 Player::GetBaseStanding(uint32 Faction)
{
	ReputationMap::iterator itr = m_reputation.find(Faction);
	if(itr == m_reputation.end()) return 0;
	else return itr->second->baseStanding;
}

void Player::SetStanding(uint32 Faction, int32 Value)
{
	ReputationMap::iterator itr = m_reputation.find(Faction);
	FactionDBC * dbc = sFactionStore.LookupEntry(Faction);
	if(dbc == 0) return;

	if(itr == m_reputation.end())
	{
		// New faction!
		FactionReputation * rep = new FactionReputation;
		rep->flag = 0;
		rep->standing = Value;
		rep->baseStanding = 0;
		m_reputation[dbc->ID] = rep;
		if(dbc->RepListId >= 0)
			reputationByListId[dbc->RepListId] = rep;
	}
	else
	{
		// Increment it.
		if(RankChangedFlat(itr->second->standing, Value))
		{
			itr->second->standing = Value;
			UpdateInrangeSetsBasedOnReputation();
		}
		else
		{
			itr->second->standing = Value;
		}

		// Set visible if invisible.
		if(!Visible(itr->second->flag))
		{
			SetFlagVisible(itr->second->flag);
			if(IsInWorld())
				m_session->OutPacket(SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId);
		}

		// Set at war if we're beyond hostile.
		Standing rank = GetReputationRankFromStanding(itr->second->standing);
		if(rank <= STANDING_HOSTILE && !AtWar(itr->second->flag))
			SetFlagAtWar(itr->second->flag);
		else if(rank >= STANDING_NEUTRAL && AtWar(itr->second->flag))
			UnsetFlagAtWar(itr->second->flag);

		if(IsInWorld())
		{
			WorldPacket data(SMSG_SET_FACTION_STANDING, 12);
			data << uint32(itr->second->flag) << dbc->RepListId << itr->second->CalcStanding();
			m_session->SendPacket(&data);
		}
	}
}

Standing Player::GetStandingRank(uint32 Faction)
{
	return Standing(GetReputationRankFromStanding(GetStanding(Faction)));
}

bool Player::IsHostileBasedOnReputation(FactionDBC * dbc)
{
	assert(dbc->RepListId < 128 && dbc->RepListId >= 0);
	FactionReputation * rep = reputationByListId[dbc->RepListId];
	if(!rep) return false;

	if(rep->flag & FACTION_FLAG_AT_WAR || GetReputationRankFromStanding(rep->standing) <= STANDING_HOSTILE)
		return true;
	else
		return false;

	/*map<uint32, FactionReputation>::iterator itr = m_reputation.find(Faction);
	if(itr == m_reputation.end()) return false;
	if(itr->second.flag & FACTION_FLAG_AT_WAR || GetReputationRankFromStanding(itr->second.standing) <= STANDING_HOSTILE)
		return true;
	else
		return false;*/
}

void Player::ModStanding(uint32 Faction, int32 Value)
{
	ReputationMap::iterator itr = m_reputation.find(Faction);
	FactionDBC * dbc = sFactionStore.LookupEntry(Faction);
	if(dbc == 0) return;

	if(itr == m_reputation.end())
	{
		// New faction!
		FactionReputation * rep = new FactionReputation;
		rep->flag = 0;
		rep->standing = Value;
		rep->baseStanding = 0;
		m_reputation[dbc->ID] = rep;
		if(dbc->RepListId >= 0)
			reputationByListId[dbc->RepListId] = rep;
	}
	else
	{
		// Bonus
		if(pctReputationMod > 0)
		{
			float d = float(float(pctReputationMod) / 100.0f);
			Value += FL2UINT( float( float(Value) * d ) );
		}

		// Increment it.
		if(RankChanged(itr->second->standing, Value))
		{
			itr->second->standing += Value;
			UpdateInrangeSetsBasedOnReputation();
		}
		else
		{
			itr->second->standing += Value;
		}

		// Set visible if invisible.
		if(!Visible(itr->second->flag))
		{
			SetFlagVisible(itr->second->flag);
			if(IsInWorld())
				m_session->OutPacket(SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId);
		}

		// Set at war if we're beyond hostile.
		if(GetReputationRankFromStanding(itr->second->standing) <= STANDING_HOSTILE && !AtWar(itr->second->flag))
			SetFlagAtWar(itr->second->flag);

		if(IsInWorld() && Visible(itr->second->flag))
		{
			WorldPacket data(SMSG_SET_FACTION_STANDING, 12);
			data << uint32(1) << dbc->RepListId << itr->second->CalcStanding();
			m_session->SendPacket(&data);
		}
   }
}

void Player::SetAtWar(uint32 Faction, bool Set)
{
	FactionReputation * rep = reputationByListId[Faction];
	if(Faction > 128 || !rep) return;
	
	if(GetReputationRankFromStanding(rep->standing) <= STANDING_HOSTILE && !Set) // At this point we have to be at war.
		return;

	if(Set)
	{
		if(!AtWar(rep->flag))
			SetFlagAtWar(rep->flag);

		UpdateInrangeSetsBasedOnReputation();
	}
	else
	{
		if(AtWar(rep->flag))
			UnsetFlagAtWar(rep->flag);

		UpdateInrangeSetsBasedOnReputation();
	}
}

void WorldSession::HandleSetAtWarOpcode(WorldPacket& recv_data)
{
	uint32 id;
	uint8 state;
	recv_data >> id >> state;

	/*uint32 faction_id = (id >= 128) ? 0 : INITIALIZE_FACTIONS[id];
	if(faction_id == 0) return;

	if(state & FACTION_FLAG_AT_WAR)
		_player->SetAtWar(faction_id, true);
	else
		_player->SetAtWar(faction_id, false);*/

	if(state == 1)
		_player->SetAtWar(id, true);
	else
		_player->SetAtWar(id, false);
}

void Player::UpdateInrangeSetsBasedOnReputation()
{
	// This function assumes that the opp faction set for player = the opp faction set for the unit.
	InRangeSet::iterator itr;
	Unit * pUnit;
	bool rep_value;
	bool enemy_current;
	for(itr = m_objectsInRange.begin(); itr != m_objectsInRange.end(); ++itr)
	{
		if((*itr)->GetTypeId() != TYPEID_UNIT)
			continue;

		pUnit = ((Unit*)*itr);
		if(pUnit->m_factionDBC->RepListId < 0)
			continue;

		rep_value = IsHostileBasedOnReputation(pUnit->m_factionDBC);
		enemy_current = IsInRangeOppFactSet(pUnit);

		if(rep_value && !enemy_current) // We are now enemies.
			m_oppFactsInRange.insert(pUnit);
		else if(!rep_value && enemy_current)
			m_oppFactsInRange.erase(pUnit);
	}
}

void Player::Reputation_OnKilledUnit(Unit * pUnit, bool InnerLoop)
{
	// add rep for on kill
	if(pUnit->GetTypeId() != TYPEID_UNIT || pUnit->IsPet())
		return;

	if(!InnerLoop && m_Group)
	{
		/* loop the rep for group members */
		m_Group->getLock().Acquire();
		GroupMembersSet::iterator it;
		for(uint32 i = 0; i < m_Group->GetSubGroupCount(); ++i)
		{
			for(it = m_Group->GetSubGroup(i)->GetGroupMembersBegin(); it != m_Group->GetSubGroup(i)->GetGroupMembersEnd(); ++it)
			{
				if(it->player)
					it->player->Reputation_OnKilledUnit(pUnit, true);
			}
		}
		m_Group->getLock().Release();
		return;
	}

	int team = GetTeam();
	ReputationModifier * modifier = objmgr.GetReputationModifier(pUnit->GetEntry(), pUnit->m_factionDBC->ID);
	if(modifier != 0)
	{
		// Apply this data.
		for(vector<ReputationMod>::iterator itr = modifier->mods.begin(); itr != modifier->mods.end(); ++itr)
		{
			if(!(*itr).faction[team])
				continue;

			/* rep limit? */
			if((*itr).replimit)
			{
				if(GetStanding((*itr).faction[team]) >= (int32)(*itr).replimit)
					continue;
			}

			int32 value = int32(float(itr->value) * sWorld.getRate(RATE_KILLREPUTATION));
			//value *= sWorld.getRate(RATE_KILLREPUTATION);
			ModStanding(itr->faction[team], value);
		}
	}
	else
	{
		if(IS_INSTANCE(GetMapId()) && objmgr.HandleInstanceReputationModifiers(this, pUnit))
			return;

		if(pUnit->m_factionDBC->RepListId < 0)
			return;

		// decrease rep by 5.
		int change = -5;
		change = int32((float(change) * sWorld.getRate(RATE_KILLREPUTATION)));
		ModStanding(pUnit->m_factionDBC->ID, change);
	}
}

void Player::Reputation_OnTalk(FactionDBC * dbc)
{
	// set faction visible if not visible
	if(!dbc || dbc->RepListId < 0)
		return;

	FactionReputation * rep = reputationByListId[dbc->RepListId];
	if(!rep)
		return;

	if(!Visible(rep->flag))
	{
		SetFlagVisible(rep->flag);
		if(IsInWorld())
			m_session->OutPacket(SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId);
	}
}
