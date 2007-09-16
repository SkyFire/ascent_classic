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

class Arena : public CBattleground
{
	set<GameObject*> m_gates;
	bool m_started;
public:
	Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side);
	virtual ~Arena();

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
			return true;
	}

	bool CreateCorpse(Player * plr) { return false; }

	/* dummy stuff */
	void HookOnMount(Player * plr) {}
	void HookFlagDrop(Player * plr, GameObject * obj) {}
	void HookFlagStand(Player * plr, GameObject * obj) {}
	void HookOnAreaTrigger(Player * plr, uint32 id);

	int32 GetFreeTeam()
	{
		uint32 c0 = m_players[0].size() + m_pendPlayers[0].size();
		uint32 c1 = m_players[1].size() + m_pendPlayers[1].size();
		if(m_started) return -1;

		/* Grab the team with the least players. */
		if(c0 >= m_playerCountPerTeam)
		{
			/* Team 0 is full, try team 1 */
			if(c1 >= m_playerCountPerTeam)
				return -1;
			else
				return 1;
		}
		else if(c1 >= m_playerCountPerTeam)
		{
			/* Team 1 is full, try team 0 */
			if(c0 >= m_playerCountPerTeam)
				return -1;
			else
				return 0;
		}
		else
		{
			/* Neither team is full. Pick the one with the least slots */
			if(c0 > c1)
			{
				/* 0 has more players than 1 */
				return 1;
			}
			else
			{
				/* 1 has more players than 0 */
				return 0;
			}
		}
		/* We shouldn't reach here. */
	}
};
