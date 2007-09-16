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

ArenaTeam::ArenaTeam(Field * f)
{
	uint32 z = 0, i, guid;
	const char * data;
	m_id = f[z++].GetUInt32();
	m_type = f[z++].GetUInt32();
	m_leader = f[z++].GetUInt32();
	m_name = f[z++].GetString();
	m_emblemStyle = f[z++].GetUInt32();
	m_emblemColour = f[z++].GetUInt32();
	m_borderStyle = f[z++].GetUInt32();
	m_borderColour = f[z++].GetUInt32();
	m_backgroundColour = f[z++].GetUInt32();
	m_stat_rating = f[z++].GetUInt32();

	m_stat_gamesplayedweek = 0;
	m_stat_gamesplayedseason = 0;
	m_stat_gameswonseason = 0;
	m_stat_gameswonweek = 0;
	sscanf(f[z++].GetString(), "%u %u %u %u", &m_stat_gamesplayedweek, &m_stat_gameswonweek, &m_stat_gamesplayedseason, &m_stat_gameswonseason);

	m_stat_ranking = f[z++].GetUInt32();
	for(i = 0; i < m_slots; ++i)
	{
		data = f[z++].GetString();
		if(sscanf(data, "%u %u %u %u %u", &guid, &m_members[i].Played_ThisWeek, &m_members[i].Won_ThisWeek,
			&m_members[i].Played_ThisSeason, &m_members[i].Won_ThisSeason) == 5)
		{
			m_members[i].Info = objmgr.GetPlayerInfo(guid);
		}
		else
			m_members[i].Info = NULL;
	}	
}

void ArenaTeam::SendPacket(WorldPacket * data)
{
	PlayerInfo * info;
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		info = m_members[i].Info;
		if(info && info->m_loggedInPlayer)
			info->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
}

void ArenaTeam::Destroy()
{

}

bool ArenaTeam::AddMember(PlayerInfo * info)
{
	uint32 base_field;
	Player * plr = info->m_loggedInPlayer;
	if(m_memberCount >= m_slots)
		return false;

	memset(&m_members[m_memberCount], 0, sizeof(ArenaTeamMember));
	m_members[m_memberCount++].Info = info;
	SaveToDB();

	if(plr)
	{
		base_field = (m_type*5) + PLAYER_FIELD_ARENA_TEAM_INFO_1_1;
		plr->SetUInt32Value(base_field, m_id);
		if(info->guid == m_leader)
			plr->SetUInt32Value(base_field+1,0);
		else
			plr->SetUInt32Value(base_field+1,1);


	}
	return true;
}

bool ArenaTeam::RemoveMember(PlayerInfo * info)
{
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info == info)
		{
			/* memcpy all the blocks in front of him back (so we only loop O(members) instead of O(slots) */
			for(uint32 j = (i+1); j < m_memberCount; ++j)
				memcpy(&m_members[j-1], &m_members[j], sizeof(ArenaTeamMember));

			--m_memberCount;
			SaveToDB();
			return true;
		}
	}

	return false;
}

void ArenaTeam::Stat(WorldPacket & data)
{
	data.Initialize(SMSG_ARENA_TEAM_STATS);
	data << m_id;
	data << m_stat_rating;
	data << m_stat_gamesplayedweek;
	data << m_stat_gameswonweek;
	data << m_stat_gamesplayedseason;
	data << m_stat_gameswonseason;
	data << m_stat_ranking;
}

void ArenaTeam::Query(WorldPacket & data)
{
	data.Initialize(SMSG_ARENA_TEAM_QUERY_RESPONSE);
	data << m_id;
	data << m_name;
	data << GetPlayersPerTeam();
	data << m_emblemStyle;
	data << m_emblemColour;
	data << m_borderStyle;
	data << m_borderColour;
	data << m_backgroundColour;
}

void ArenaTeam::Roster(WorldPacket & data)
{
	data.Initialize(SMSG_ARENA_TEAM_ROSTER);
	data.reserve(m_memberCount * 81 + 9);
	data << uint8(m_type);
	data << m_memberCount;
	data << uint32(0);			// unk

	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		PlayerInfo * info = m_members[i].Info;
		data << uint64(info->guid);
		data << uint8( (info->m_loggedInPlayer != NULL) ? 1 : 0 );
		data << info->name;
		data << info->lastZone;
		data << uint8( info->lastLevel );
		data << uint8( info->cl );
		data << m_members[i].Played_ThisWeek;
		data << m_members[i].Won_ThisWeek;
		data << m_members[i].Played_ThisSeason;
		data << m_members[i].Won_ThisSeason;
	}
}

void ArenaTeam::SaveToDB()
{
	std::stringstream ss;
	uint32 i;
	ss << "REPLACE INTO arenateams VALUES("
		<< m_id << ","
		<< m_type << ","
		<< m_leader << ",'"
		<< m_name << "',"
		<< m_emblemStyle << ","
		<< m_emblemColour << ","
		<< m_borderStyle << ","
		<< m_borderColour << ","
		<< m_backgroundColour << ","
		<< m_stat_rating << ",'"
		<< m_stat_gamesplayedweek << " " << m_stat_gameswonweek << " "
		<< m_stat_gamesplayedseason << " " << m_stat_gameswonseason << "',"
		<< m_stat_ranking;
    
	for(i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info)
		{
			ss << ",'" << m_members[i].Info->guid << " " << m_members[i].Played_ThisWeek << " "
				<< m_members[i].Won_ThisWeek << " " << m_members[i].Played_ThisSeason << " "
				<< m_members[i].Won_ThisSeason << "'";
		}
		else
		{
			ss << ",'0 0 0 0 0'";
		}
	}

	for(; i < 5; ++i)
	{
		ss << ",'0 0 0 0 0'";
	}

	ss << ")";
	CharacterDatabase.Execute(ss.str().c_str());
}

bool ArenaTeam::HasMember(uint32 guid)
{
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info && m_members[i].Info->guid == guid)
			return true;
	}
	return false;
}

void WorldSession::HandleArenaTeamRosterOpcode(WorldPacket & recv_data)
{
	uint8 slot;
	ArenaTeam * team;
	recv_data >> slot;
	if(slot < 3)
	{
		team = _player->m_arenaTeams[slot];
		if(team != NULL)
		{
			WorldPacket data(1000);
			team->Roster(data);
			SendPacket(&data);
		}
	}
}

void WorldSession::HandleArenaTeamQueryOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	uint32 team_id;
	recv_data >> team_id;

	team = objmgr.GetArenaTeamById(team_id);
	if(team != NULL)
	{
		WorldPacket data(1000);
		team->Query(data);
		SendPacket(&data);

		team->Stat(data);
		SendPacket(&data);
	}
}

void WorldSession::HandleArenaTeamAddMemberOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamRemoveMemberOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamInviteAcceptOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamInviteDenyOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamLeaveOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamDisbandOpcode(WorldPacket & recv_data) {}
void WorldSession::HandleArenaTeamPromoteOpcode(WorldPacket & recv_data) {}