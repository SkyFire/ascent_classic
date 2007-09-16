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

#ifndef ASCENT_ARENATEAMS_H
#define ASCENT_ARENATEAMS_H

enum ArenaTeamCommandTypes
{
    ERR_ARENA_TEAM_CREATE_S                 = 0x00,
    ERR_ARENA_TEAM_INVITE_SS                = 0x01,
    //ERR_ARENA_TEAM_QUIT_S                   = 0x02,
    ERR_ARENA_TEAM_QUIT_S                   = 0x03,
    ERR_ARENA_TEAM_FOUNDER_S                = 0x0C  // need check, probably wrong...
};

enum ArenaTeamCommandErrors
{
    //ARENA_TEAM_PLAYER_NO_MORE_IN_ARENA_TEAM = 0x00,
    ERR_ARENA_TEAM_INTERNAL                 = 0x01,
    ERR_ALREADY_IN_ARENA_TEAM               = 0x02,
    ERR_ALREADY_IN_ARENA_TEAM_S             = 0x03,
    ERR_INVITED_TO_ARENA_TEAM               = 0x04,
    ERR_ALREADY_INVITED_TO_ARENA_TEAM_S     = 0x05,
    ERR_ARENA_TEAM_NAME_INVALID             = 0x06,
    ERR_ARENA_TEAM_NAME_EXISTS_S            = 0x07,
    ERR_ARENA_TEAM_LEADER_LEAVE_S           = 0x08,
    ERR_ARENA_TEAM_PERMISSIONS              = 0x08,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM       = 0x09,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS    = 0x0A,
    ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S       = 0x0B,
    ERR_ARENA_TEAM_NOT_ALLIED               = 0x0C
};

enum ArenaTeamEvents
{
    ERR_ARENA_TEAM_JOIN_SS                  = 3,    // player name  arena team name
    ERR_ARENA_TEAM_LEAVE_SS                 = 4,    // player name  arena team name
    ERR_ARENA_TEAM_REMOVE_SSS               = 5,    // player name  arena team name  captain name
    ERR_ARENA_TEAM_LEADER_IS_SS             = 6,    // player name  arena team name
    ERR_ARENA_TEAM_LEADER_CHANGED_SSS       = 7,    // old captain  new captain  arena team name
    ERR_ARENA_TEAM_DISBANDED_S              = 8     // captain name  arena team name
};

/*
need info how to send these ones:
ERR_ARENA_TEAM_YOU_JOIN_S - client show it automatically when accept invite
ERR_ARENA_TEAM_TARGET_TOO_LOW_S
ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S
ERR_ARENA_TEAM_LEVEL_TOO_LOW_I
*/

enum ArenaTeamStatTypes
{
    STAT_TYPE_RATING    = 0,
    STAT_TYPE_GAMES     = 1,
    STAT_TYPE_WINS      = 2,
    STAT_TYPE_PLAYED    = 3,
    STAT_TYPE_WINS2     = 4,
    STAT_TYPE_RANK      = 5
};

struct ArenaTeamMember
{
	PlayerInfo * Info;
	uint32 Played_ThisWeek;
	uint32 Won_ThisWeek;
	uint32 Played_ThisSeason;
	uint32 Won_ThisSeason;
};

class ArenaTeam
{
	void AllocateSlots(uint32 Type)
	{
		uint32 Slots = 0;
		if(Type == ARENA_TEAM_TYPE_2V2)
			Slots=2;
		else if(Type == ARENA_TEAM_TYPE_3V3)
			Slots=3;
		else if(Type == ARENA_TEAM_TYPE_5V5)
			Slots=5;
		ASSERT(Slots);
		m_members = new ArenaTeamMember[Slots];
		memset(m_members,0,sizeof(ArenaTeamMember)*Slots);
		m_slots = Slots;
		m_memberCount=0;
	}

public:

	uint32 m_id;
	uint32 m_type;
	uint32 m_leader;
	uint32 m_slots;
	string m_name;
	uint32 m_memberCount;
	ArenaTeamMember * m_members;

	uint32 m_emblemStyle;
	uint32 m_emblemColour;
	uint32 m_borderStyle;
	uint32 m_borderColour;
	uint32 m_backgroundColour;

	uint32 m_stat_rating;
	uint32 m_stat_gamesplayedweek;
	uint32 m_stat_gameswonweek;
	uint32 m_stat_gamesplayedseason;
	uint32 m_stat_gameswonseason;
	uint32 m_stat_ranking;

	ArenaTeam(uint32 Type, uint32 Id)
	{
		m_id = Id;
		m_type = Type;
		AllocateSlots(Type);
		m_leader = 0;
		m_emblemStyle = 0;
		m_emblemColour = 0;
		m_borderColour = 0;
		m_borderStyle = 0;
		m_backgroundColour = 0;
		m_stat_rating = 0;
		m_stat_gamesplayedweek = 0;
		m_stat_gamesplayedseason = 0;
		m_stat_gameswonseason = 0;
		m_stat_gameswonweek = 0;
		m_stat_ranking = 0;
	}

	ArenaTeam(Field * f);
	~ArenaTeam()
	{
		delete [] m_members;
	}

	void SendPacket(WorldPacket * data);
	void Query(WorldPacket & data);
	void Stat(WorldPacket & data);
	void Roster(WorldPacket & data);
	void Inspect(WorldPacket & data);
	void Destroy();
	void SaveToDB();

	bool AddMember(PlayerInfo * info);
	bool RemoveMember(PlayerInfo * info);
	bool HasMember(uint32 guid);

	inline uint32 GetPlayersPerTeam()
	{
		switch(m_type)
		{
		case ARENA_TEAM_TYPE_2V2:
			return 2;

		case ARENA_TEAM_TYPE_3V3:
			return 3;

		case ARENA_TEAM_TYPE_5V5:
			return 5;
		}
	}
};

#endif		// ASCENT_ARENATEAMS_H
