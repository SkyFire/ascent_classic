/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

class ArenaTeam;
struct PlayerInfo;

class Arena : public CBattleground
{
	set<GameObject*> m_gates;
	set<ArenaTeam*> doneteams;
	uint32 m_arenateamtype;

	uint32 m_pcWorldStates[2];

public:
	bool rated_match;
	Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side);
	virtual ~Arena();

	void RecalculateArenaTeams(ArenaTeam * pWinner, ArenaTeam * pLoser);

	bool HookHandleRepop(Player * plr);
	void OnAddPlayer(Player * plr);
	void OnRemovePlayer(Player * plr);
	void OnCreate();
	void HookOnPlayerDeath(Player * plr);
	void HookOnPlayerKill(Player * plr, Unit * pVictim);
	void HookOnHK(Player * plr);
	void UpdatePlayerCounts();
	LocationVector GetStartingCoords(uint32 Team);
	virtual const char * GetName() { return "Arena"; }
	void OnStart();
	bool CanPlayerJoin(Player * plr)
	{
		if(m_started)
			return false;
		else
			return CBattleground::CanPlayerJoin(plr);
	}

	bool CreateCorpse(Player * plr) { return false; }

	/* dummy stuff */
	void HookOnMount(Player * plr) {}
	void HookFlagDrop(Player * plr, GameObject * obj) {}
	void HookFlagStand(Player * plr, GameObject * obj) {}
	void HookOnAreaTrigger(Player * plr, uint32 id);

	void BuffRespawn(uint32 buffId);

	int32 GetFreeTeam()
	{
		size_t c0 = m_players[0].size() + m_pendPlayers[0].size();
		size_t c1 = m_players[1].size() + m_pendPlayers[1].size();
		if(m_started) return -1;

		// Check if there is free room, if yes, return team with less members
		return ((c0 + c1 >= m_playerCountPerTeam * 2) ? -1 : (c0 > c1));

		/* We shouldn't reach here. */
	}

	//ArenaTeam * m_RatedTeams[2];
	uint32 m_ratedTeams[2];
	std::set<PlayerInfo*> m_RatedPlayers[2];

	GameObject * mBuffs[2];

	void Finish();
	uint8 Rated() { return rated_match; }
	ASCENT_INLINE uint32 GetArenaTeamType() { return m_arenateamtype; }

	/* looooooot */
	bool SupportsPlayerLoot() { return false; }
	void HookGenerateLoot(Player *plr, Corpse *pCorpse) {}
};
