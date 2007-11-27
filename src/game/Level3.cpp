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

/////////////////////////////////////////////////
//  Admin Chat Commands
//

#include "StdAfx.h"
#include "ObjectMgr.h"
#include "../ascent/Master.h"

bool ChatHandler::HandleWorldPortCommand(const char* args, WorldSession *m_session)
{
	float x, y, z;
	uint32 mapid;
	if(sscanf(args, "%u %f %f %f", (unsigned int*)&mapid, &x, &y, &z) != 4)
		return false;

	if(x >= _maxX || x <= _minX || y <= _minY || y >= _maxY)
		return false;

	LocationVector vec(x, y, z);
	m_session->GetPlayer()->SafeTeleport(mapid, 0, vec);
	return true;
}


bool ChatHandler::HandleClearCooldownsCommand(const char *args, WorldSession *m_session)
{
	uint32 guid = (uint32)m_session->GetPlayer()->GetSelection();
	Player *plr = getSelectedChar(m_session, true);
	
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}
	if(!plr) return false;

	if(plr->getClass()==WARRIOR)
	{
		plr->ClearCooldownsOnLine(26, guid);
		plr->ClearCooldownsOnLine(256, guid);
		plr->ClearCooldownsOnLine(257 , guid);
		BlueSystemMessage(m_session, "Cleared all Warrior cooldowns.");
		return true;
	}
	if(plr->getClass()==PALADIN)
	{
		plr->ClearCooldownsOnLine(594, guid);
		plr->ClearCooldownsOnLine(267, guid);
		plr->ClearCooldownsOnLine(184, guid);
		BlueSystemMessage(m_session, "Cleared all Paladin cooldowns.");
		return true;
	}
	if(plr->getClass()==HUNTER)
	{
		plr->ClearCooldownsOnLine(50, guid);
		plr->ClearCooldownsOnLine(51, guid);
		plr->ClearCooldownsOnLine(163, guid);
		BlueSystemMessage(m_session, "Cleared all Hunter cooldowns.");
		return true;
	}
	if(plr->getClass()==ROGUE)
	{
		plr->ClearCooldownsOnLine(253, guid);
		plr->ClearCooldownsOnLine(38, guid);
		plr->ClearCooldownsOnLine(39, guid);
		BlueSystemMessage(m_session, "Cleared all Rogue cooldowns.");
		return true;
	}
	if(plr->getClass()==PRIEST)
	{
		plr->ClearCooldownsOnLine(56, guid);
		plr->ClearCooldownsOnLine(78, guid);
		plr->ClearCooldownsOnLine(613, guid);
		BlueSystemMessage(m_session, "Cleared all Priest cooldowns.");
		return true;
	}
	if(plr->getClass()==SHAMAN)
	{
		plr->ClearCooldownsOnLine(373, guid);
		plr->ClearCooldownsOnLine(374, guid);
		plr->ClearCooldownsOnLine(375, guid);
		BlueSystemMessage(m_session, "Cleared all Shaman cooldowns.");
		return true;
	}
	if(plr->getClass()==MAGE)
	{
		plr->ClearCooldownsOnLine(6, guid);
		plr->ClearCooldownsOnLine(8, guid);
		plr->ClearCooldownsOnLine(237, guid);
		BlueSystemMessage(m_session, "Cleared all Mage cooldowns.");
		return true;
	}
	if(plr->getClass()==WARLOCK)
	{
		plr->ClearCooldownsOnLine(355, guid);
		plr->ClearCooldownsOnLine(354, guid);
		plr->ClearCooldownsOnLine(593, guid);
		BlueSystemMessage(m_session, "Cleared all Warlock cooldowns.");
		return true;
	}
	if(plr->getClass()==DRUID)
	{
		plr->ClearCooldownsOnLine(573, guid);
		plr->ClearCooldownsOnLine(574, guid);
		plr->ClearCooldownsOnLine(134, guid);
		BlueSystemMessage(m_session, "Cleared all Druid cooldowns.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleLearnCommand(const char* args, WorldSession *m_session)
{
	if (!*args)
		return false;

	uint32 spell = atol((char*)args);
	
	Player *plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}
	if(!plr) return false;

	sGMLog.writefromsession(m_session, "taught %s spell %u", plr->GetName(), spell);

	SpellEntry * sp = dbcSpell.LookupEntry(spell);
	if(!plr->GetSession()->HasGMPermissions() && (sp->Effect[0]==SPELL_EFFECT_INSTANT_KILL||sp->Effect[1]==SPELL_EFFECT_INSTANT_KILL||sp->Effect[2]==SPELL_EFFECT_INSTANT_KILL))
	{
		SystemMessage(m_session, "don't be an idiot and teach players instakill spells. this action has been logged.");
		return true;
	}

	if (plr->HasSpell(spell)) // check to see if char already knows
	{
		std::string OutStr = plr->GetName();
		OutStr += " already knows that spell.";

		SystemMessage(m_session, OutStr.c_str());
		return true;
	}

	plr->addSpell(spell);
	BlueSystemMessageToPlr(plr, "%s taught you Spell %d", m_session->GetPlayer()->GetName(), spell);

	return true;
}

bool ChatHandler::HandleReviveCommand(const char* args, WorldSession *m_session)
{
	Player* SelectedPlayer = getSelectedChar(m_session, true);
	if(!SelectedPlayer) return true;

	
	SelectedPlayer->SetMovement(MOVE_UNROOT, 1);
	SelectedPlayer->ResurrectPlayer();
	SelectedPlayer->SetUInt32Value(UNIT_FIELD_HEALTH, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
	return true;
}

bool ChatHandler::HandleExploreCheatCommand(const char* args, WorldSession *m_session)
{
	if (!*args)
		return false;

	int flag = atoi((char*)args);

	Player *chr = getSelectedChar(m_session);
	if (chr == NULL)
	{
		SystemMessage(m_session, "No character selected.");
		return true;
	}

	char buf[256];

	// send message to user
	if (flag != 0)
	{
		snprintf((char*)buf,256,"%s has explored all zones now.", chr->GetName());
	}
	else
	{
		snprintf((char*)buf,256,"%s has no more explored zones.", chr->GetName());
	}
	SystemMessage(m_session, buf);

	// send message to player
	if (flag != 0)
	{
		snprintf((char*)buf,256,"%s has explored all zones for you.",
			m_session->GetPlayer()->GetName());
	}
	else
	{
		snprintf((char*)buf,256,"%s has hidden all zones from you.", 
			m_session->GetPlayer()->GetName());
	}
	SystemMessage(m_session,  buf);


	for (uint8 i=0; i<64; i++)
	{
		if (flag != 0)
		{
			chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0xFFFFFFFF);
		}
		else
		{
			chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0);
		}
	}

	return true;
}

bool ChatHandler::HandleBanCharacterCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	// this is rather complicated due to ban reasons being able to have spaces. so we'll have to some c string magic
	// rather than just sscanf'ing it.
	char * pCharacter = (char*)args;
	char * pBanDuration = strchr(pCharacter, ' ');
	if(pBanDuration == NULL)
		return false;

	char * pReason = strchr(pBanDuration+1, ' ');
	if(pReason == NULL)
		return false;

	// zero them out to create sepearate strings.
	*pBanDuration = 0;
	++pBanDuration;
	*pReason = 0;
	++pReason;

	int32 BanTime = GetTimePeriodFromString(pBanDuration);
	if(BanTime < 1)
		return false;

	Player * pPlayer = objmgr.GetPlayer(pCharacter, false);
	if(pPlayer == NULL)
	{
		string sCharacter = string(pCharacter);
		PlayerInfo * pInfo = objmgr.GetPlayerInfoByName(sCharacter);
		if(pInfo == NULL)
		{
			SystemMessage(m_session, "Player not found.");
			return true;
		}

		SystemMessage(m_session, "Banning player '%s' in database for '%s'.", pCharacter, pReason);
		string escaped_reason = CharacterDatabase.EscapeString(string(pReason));
		
		CharacterDatabase.Execute("UPDATE characters SET banned = %u, banReason = '%s' WHERE guid = %u",
			BanTime ? BanTime+(uint32)UNIXTIME : 1, escaped_reason.c_str(), pInfo->guid);
	}
	else
	{
		SystemMessage(m_session, "Banning player '%s' ingame for '%s'.", pCharacter, pReason);
		string sReason = string(pReason);
		uint32 uBanTime = BanTime ? BanTime+(uint32)UNIXTIME : 1;
		pPlayer->SetBanned(uBanTime, sReason);
	}

	SystemMessage(m_session, "This ban is due to expire %s%s.", BanTime ? "on " : "", BanTime ? ConvertTimeStampToDataTime(BanTime+(uint32)UNIXTIME).c_str() : "Never");
	if(pPlayer)
	{
		SystemMessage(m_session, "Kicking %s.", pPlayer->GetName());
		pPlayer->Kick();
	}

	sGMLog.writefromsession(m_session, "used ban character on %s reason %s for %s", pCharacter, pReason, BanTime ? ConvertTimeStampToString(BanTime).c_str() : "ever");
	return true;
}

bool ChatHandler::HandleUnBanCharacterCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	char Character[255];
	if(sscanf(args, "%s", Character) == 0)
	{
		RedSystemMessage(m_session, "A character name and reason is required.");
		return true;
	}

	// Check if player is in world.
	Player * pPlayer = ObjectMgr::getSingleton( ).GetPlayer(Character, false);
	if(pPlayer != 0)
	{
		GreenSystemMessage(m_session, "Unbanned player %s ingame.",pPlayer->GetName());
		pPlayer->UnSetBanned();
	}
	else
	{
		GreenSystemMessage(m_session, "Player %s not found ingame.", Character);
	}

	// Ban in database
	CharacterDatabase.Execute("UPDATE characters SET banned = 0 WHERE name = '%s'", CharacterDatabase.EscapeString(string(Character)).c_str());

	SystemMessage(m_session, "Unbanned character %s in database.", Character);
	sGMLog.writefromsession(m_session, "used unban character on %s", Character);
	return true;
}

bool ChatHandler::HandleGMTicketGetAllCommand(const char* args, WorldSession *m_session)
{
	Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
	if(!chn)
		return false;

	chn->Say(m_session->GetPlayer(), "GmTicket 2", m_session->GetPlayer(), true);
	for(GmTicketList::iterator itr = objmgr.GM_TicketList.begin(); itr != objmgr.GM_TicketList.end(); itr++)
	{
		uint32 cont = 0;
		uint32 zone = 0;
		Player* plr = objmgr.GetPlayer((uint32)(*itr)->guid);
		if(plr)
			if(plr->IsInWorld())
			{
				zone = plr->GetZoneId();
				cont = plr->GetMapId();
			}

			std::stringstream str;
			str << "GmTicket 0,";
			str << (*itr)->name.c_str() << ","  << (*itr)->level << ","  << (*itr)->type << ",";
			str << zone;

			chn->Say(m_session->GetPlayer(),str.str().c_str(), m_session->GetPlayer(), true);
	}

	return true;
}

bool ChatHandler::HandleGMTicketGetByIdCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;


	GmTicketList::iterator i;
	for(i = objmgr.GM_TicketList.begin(); i != objmgr.GM_TicketList.end(); i++)
	{
		if(strcmp((*i)->name.c_str(), args) == 0)
		{
			Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
			if(!chn)
				return false;

			std::stringstream str;
			str << "GmTicket 3,";
			str << (*i)->name.c_str() << "," << (*i)->message;
			chn->Say(m_session->GetPlayer(),str.str().c_str(), m_session->GetPlayer(), true);
		}
	}
	return true;
}

bool ChatHandler::HandleGMTicketDelByIdCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	GmTicketList::iterator i;
	int64 guid = -1;
	for(i = objmgr.GM_TicketList.begin(); i != objmgr.GM_TicketList.end(); i++)
	{
		if(strcmp((*i)->name.c_str(), args) == 0)
		{
			guid = (int64)(*i)->guid;
			break;
		}
	}
	if(guid != -1)
	{
		objmgr.remGMTicket(guid);

		std::stringstream str;
		str << "GmTicket 1," << args;

		Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
		if(!chn)
			return false;

		chn->Say(m_session->GetPlayer(), str.str().c_str(), NULL, true);


		Player* plr = objmgr.GetPlayer((uint32)guid);
		if(!plr)
			return true;
		if(!plr->IsInWorld())
			return true;

		WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
		data << uint32(9);

		plr->GetSession()->SendPacket( &data );
	}

	return true;
}

bool ChatHandler::HandleAddSkillCommand(const char* args, WorldSession *m_session)
{
	char buf[256];
	Player* target = objmgr.GetPlayer((uint32)m_session->GetPlayer()->GetSelection());

	if(!target) {
		SystemMessage(m_session, "Select A Player first.");
		return true;
	}

	uint32 skillline;
	uint16 cur, max;

	char* pSkillline = strtok((char*)args, " ");
	if (!pSkillline)
		return false;

	char* pCurrent = strtok(NULL, " ");
	if (!pCurrent)
		return false;

	char* pMax = strtok(NULL, " ");
	if (!pMax)
		return false;

	skillline = (uint32)atol(pSkillline);
	cur = (uint16)atol(pCurrent);
	max = (uint16)atol(pMax);

	target->_AddSkillLine(skillline,cur,max);

	snprintf(buf,256,"SkillLine: %u CurrentValue %u Max Value %u Added.",(unsigned int)skillline,(unsigned int)cur,(unsigned int)max);
	sGMLog.writefromsession(m_session, "added skill line %u (%u/%u) to %s", skillline, cur, max, target->GetName());
	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleNpcInfoCommand(const char *args, WorldSession *m_session)
{
	char msg[512];
	uint32 guid = GUID_LOPART(m_session->GetPlayer()->GetSelection());
	Creature *crt = getSelectedCreature(m_session);
	if(!crt) return false;
	if(crt->GetCreatureName())
		BlueSystemMessage(m_session, "Showing creature info for %s", crt->GetCreatureName()->Name);
	snprintf(msg,512,"GUID: %d\nFaction: %d\nNPCFlags: %d\nDisplayID: %d", (int)guid, (int)crt->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE), (int)crt->GetUInt32Value(UNIT_NPC_FLAGS), (int)crt->GetUInt32Value(UNIT_FIELD_DISPLAYID));
	SystemMessage(m_session, msg);
	if(crt->m_faction)
		GreenSystemMessage(m_session, "Combat Support: 0x%.3X", crt->m_faction->FriendlyMask);
	GreenSystemMessage(m_session, "Base Health: %d", crt->GetUInt32Value(UNIT_FIELD_BASE_HEALTH));
	GreenSystemMessage(m_session, "Base Armor: %d", crt->GetUInt32Value(UNIT_FIELD_STAT1)*2);
	GreenSystemMessage(m_session, "Base Mana: %d", crt->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
	GreenSystemMessage(m_session, "Base Holy: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_01));
	GreenSystemMessage(m_session, "Base Fire: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_02));
	GreenSystemMessage(m_session, "Base Nature: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_03));
	GreenSystemMessage(m_session, "Base Frost: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_04));
	GreenSystemMessage(m_session, "Base Shadow: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_05));
	GreenSystemMessage(m_session, "Base Arcane: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_06));
	GreenSystemMessage(m_session, "Damage min/max: %f/%f", crt->GetFloatValue(UNIT_FIELD_MINDAMAGE),crt->GetFloatValue(UNIT_FIELD_MAXDAMAGE));
	
	ColorSystemMessage(m_session, MSG_COLOR_RED, "Entry ID: %d", crt->GetUInt32Value(OBJECT_FIELD_ENTRY));
	ColorSystemMessage(m_session, MSG_COLOR_RED, "SQL Entry ID: %d", crt->GetSQL_id());
	// show byte
	std::stringstream sstext;
	uint32 theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_0);
	sstext << "UNIT_FIELD_BYTES_0 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';
	BlueSystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleIncreaseWeaponSkill(const char *args, WorldSession *m_session)
{
	char *pMin = strtok((char*)args, " ");
	uint32 cnt = 0;
	if(!pMin)
		cnt = 1;
	else
		cnt = atol(pMin);

	Player *pr = getSelectedChar(m_session, true);
	
	uint32 SubClassSkill = 0;
	if(!pr) pr = m_session->GetPlayer();
	if(!pr) return false;
	Item *it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
	ItemPrototype* proto = NULL;
	if (!it)
		it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
	if (it)
		proto = it->GetProto();
	if (proto)
	{
		switch(proto->SubClass)
		{
			// Weapons
		case 0:	// 1 handed axes
			SubClassSkill = SKILL_AXES;
			break;
		case 1:	// 2 handed axes
			SubClassSkill = SKILL_2H_AXES;
			break;		
		case 2:	// bows
			SubClassSkill = SKILL_BOWS;
			break;
		case 3:	// guns
			SubClassSkill = SKILL_GUNS;
			break;		
		case 4:	// 1 handed mace
			SubClassSkill = SKILL_MACES;
			break;
		case 5:	// 2 handed mace
			SubClassSkill = SKILL_2H_MACES;
			break;		
		case 6:	// polearms
			SubClassSkill = SKILL_POLEARMS;
			break;
		case 7: // 1 handed sword
			SubClassSkill = SKILL_SWORDS;
			break;
		case 8: // 2 handed sword
			SubClassSkill = SKILL_2H_SWORDS;
			break;
		case 9: // obsolete
			SubClassSkill = 136;
			break;
		case 10: //1 handed exotic
			SubClassSkill = 136;
			break;
		case 11: // 2 handed exotic
			SubClassSkill = 0;
			break;
		case 12: // fist
			SubClassSkill = SKILL_FIST_WEAPONS;
			break;
		case 13: // misc
			SubClassSkill = 0;
			break;
		case 15: // daggers
			SubClassSkill = SKILL_DAGGERS;
			break;
		case 16: // thrown
			SubClassSkill = SKILL_THROWN;
			break;
		case 17: // spears
			SubClassSkill = SKILL_SPEARS;
			break;
		case 18: // crossbows
			SubClassSkill = SKILL_CROSSBOWS;
			break;
		case 19: // wands
			SubClassSkill = SKILL_WANDS;
			break;
		case 20: // fishing
			SubClassSkill = SKILL_FISHING;
			break;
		}
	} 
	else
	{
		SubClassSkill = 162;
	}

	if(!SubClassSkill)
	{
		RedSystemMessage(m_session, "Can't find skill ID :-/");
		return false;
	}

	uint32 skill = SubClassSkill;

	BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);
	sGMLog.writefromsession(m_session, "increased weapon skill of %s by %u", pr->GetName(), cnt);

	if(!pr->_HasSkillLine(skill))
	{
		SystemMessage(m_session, "Does not have skill line, adding.");
		pr->_AddSkillLine(skill, 1, 300);   
	} 
	else 
	{
		pr->_AdvanceSkillLine(skill,cnt);
	}	   
	return true;	
}


bool ChatHandler::HandleResetTalentsCommand(const char* args, WorldSession *m_session)
{
	Player *plr = this->getSelectedChar(m_session);
	if(!plr) return true;

	plr->Reset_Talents();

	SystemMessage(m_session, "Reset talents of %s.", plr->GetName());;
	BlueSystemMessageToPlr(plr, "%s reset all your talents.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset talents of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSpellsCommand(const char* args, WorldSession *m_session)
{
	Player *plr = this->getSelectedChar(m_session);
	if(!plr) return true;

	plr->Reset_Spells();
	
	SystemMessage(m_session, "Reset spells of %s to level 1.", plr->GetName());;
	BlueSystemMessage(m_session, "%s reset all your spells to starting values.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset spells of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleAccountLevelCommand(const char * args, WorldSession * m_session)
{
    if(!*args) return false;

	char account[100];
	char gmlevel[100];
	int argc = sscanf(args, "%s %s", account, gmlevel);
	if(argc != 2)
		return false;

	std::stringstream my_sql;
	my_sql << "UPDATE accounts SET gm = '" << gmlevel << "' WHERE login = '" << account << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());

	GreenSystemMessage(m_session, "Account '%s' level has been updated to '%s'. The change will be effective with the next reload cycle.", account, gmlevel);
	sGMLog.writefromsession(m_session, "set account %s flags to %s", account, gmlevel);

	return true;
}

bool ChatHandler::HandleAccountPasswordCommand(const char * args, WorldSession * m_session)
{
   if(!*args) return false;

	char account[100];
	char password[100];
	int argc = sscanf(args, "%s %s", account, password);
	if(argc != 2)
		return false;

	std::stringstream my_sql;
	my_sql << "UPDATE accounts SET password = '" << password << "' WHERE login = '" << account << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());

	GreenSystemMessage(m_session, "Account '%s' password has been changed to '%s'. The change will be effective with the next reload cycle.", account, password);

	return true;
}

bool ChatHandler::HandleAccountUnbanCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;
	char * pAccount = (char*)args;
	string escaped_account = CharacterDatabase.EscapeString(string(pAccount));
	GreenSystemMessage(m_session, "Account '%s' has been unbanned. This change will be effective with the next reload cycle.", pAccount);
	sLogonCommHandler.LogonDatabaseSQLExecute("UPDATE accounts SET banned = 0 WHERE login = '%s'", escaped_account.c_str());
	sGMLog.writefromsession(m_session, "unbanned account %s", pAccount);
	return true;
}

bool ChatHandler::HandleAccountBannedCommand(const char * args, WorldSession * m_session)
{
    if(!*args) return false;

	/*char account[100];
	uint32 banned;
	int argc = sscanf(args, "%s %u", account, (unsigned int*)&banned);
	if(argc != 2)
		return false;*/

	char * pAccount = (char*)args;
	char * pDuration = strchr(pAccount, ' ');
	if(pDuration == NULL)
		return false;
	*pDuration = 0;
	++pDuration;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if(timeperiod < 0)
		return NULL;

	uint32 banned = (timeperiod ? (uint32)UNIXTIME+timeperiod : 1);

	stringstream my_sql;
	my_sql << "UPDATE accounts SET banned = " << banned << " WHERE login = '" << CharacterDatabase.EscapeString(string(pAccount)) << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());
	sLogonCommHandler.LogonDatabaseReloadAccounts();

	GreenSystemMessage(m_session, "Account '%s' has been banned %s%s. The change will be effective with the next reload cycle.", pAccount, 
		timeperiod ? "until " : "forever", timeperiod ? ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str() : "");

	sGMLog.writefromsession(m_session, "banned account %s until %s", pAccount, timeperiod ? ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str() : "permanant");
	return true;
}

bool ChatHandler::HandleAccountFlagsCommand(const char * args, WorldSession * m_session)
{
    if(!*args) return false;

	char account[100];
	unsigned int flags;
	int argc = sscanf(args, "%s %u", account, &flags);
	if(argc != 2)
		return false;

	stringstream my_sql;
	my_sql << "UPDATE accounts SET flags = " << flags << " WHERE login = '" << account << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());

	GreenSystemMessage(m_session, "Account '%s' flags have been updated. The change will be effective with the next reload cycle.", account);
	sGMLog.writefromsession(m_session, "set account %s flags to %u", account, flags);
	return true;
}

bool ChatHandler::HandleAccountEmailCommand(const char * args, WorldSession * m_session)
{
    if(!*args) return false;

	char account[100];
	char email[100];
	int argc = sscanf(args, "%s %s", account, email);
	if(argc != 2)
		return false;

	stringstream my_sql;
	my_sql << "UPDATE accounts SET email = '" << email << "' WHERE login = '" << account << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());

	GreenSystemMessage(m_session, "Account '%s' email has been updated to '%s'. The change will be effective with the next reload cycle.", account, email);

	return true;
}

bool ChatHandler::HandleCreateAccountCommand(const char* args, WorldSession *m_session)
{
	char *user = strtok((char *)args, " ");
	if(!user) return false;
	char *pass = strtok(NULL, " ");
	if(!pass) return false;
	char *email = strtok(NULL, "\n");
	if(!email) return false;

	std::stringstream ss;
	sLog.outString("%s creating account: %s %s %s.", m_session->GetPlayer()->GetName(), user, pass, email);
	BlueSystemMessage(m_session, "Attempting to create account: %s, %s (Email: %s)...", user, pass, email);

	ss << "INSERT INTO accounts (login, password, email) VALUES(\""
		<< WorldDatabase.EscapeString(user) << "\",\""
		<< WorldDatabase.EscapeString(pass) << "\",\""
		<< WorldDatabase.EscapeString(email) << "\");";

	sLogonCommHandler.LogonDatabaseSQLExecute(ss.str().c_str());
	/*if(sLogonDatabase.Execute(ss.str().c_str()))
	{*/
		GreenSystemMessage(m_session, "Account creation successful. The account will be active with the next reload cycle.");
		return true;
	/*} else {
		RedSystemMessage(m_session, "Creation failed. The player may or may not be able to log in.");
		return true;
	}*/
}

bool ChatHandler::HandleGetTransporterTime(const char* args, WorldSession* m_session)
{
	Player *plyr = m_session->GetPlayer();
	if(plyr->m_TransporterGUID == 0) return false;

	Transporter *trans = objmgr.GetTransporter(plyr->m_TransporterGUID);
	if(!trans) return false;

//	BlueSystemMessage(m_session, "Transporter Time: %d", trans->m_TravelTime);
	return true;
}

bool ChatHandler::HandleRemoveAurasCommand(const char *args, WorldSession *m_session)
{
	Player *plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	BlueSystemMessage(m_session, "Removing all auras...");
	for(uint32 i = 0; i < MAX_AURAS; ++i)
	{
		if(plr->m_auras[i] != 0) plr->m_auras[i]->Remove();
	}
	return true;
}

bool ChatHandler::HandleParalyzeCommand(const char* args, WorldSession *m_session)
{
	//Player *plr = getSelectedChar(m_session, true);
	//if(!plr) return false;
	Unit *plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || plr->GetTypeId() != TYPEID_PLAYER)
	{
		RedSystemMessage(m_session, "Invalid target.");
		return true;
	}

	BlueSystemMessage(m_session, "Rooting target.");
	BlueSystemMessageToPlr(((Player*)plr), "You have been rooted by %s.", m_session->GetPlayer()->GetName());
	WorldPacket data;
	data.Initialize(SMSG_FORCE_MOVE_ROOT);
	data << plr->GetNewGUID();
	data << uint32(1);

	plr->SendMessageToSet(&data, true);
	return true;
}

bool ChatHandler::HandleUnParalyzeCommand(const char* args, WorldSession *m_session)
{
	//Player *plr = getSelectedChar(m_session, true);
	//if(!plr) return false;
	Unit *plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || plr->GetTypeId() != TYPEID_PLAYER)
	{
		RedSystemMessage(m_session, "Invalid target.");
		return true;
	}
	
	BlueSystemMessage(m_session, "Unrooting target.");
	BlueSystemMessageToPlr(((Player*)plr), "You have been unrooted by %s.", m_session->GetPlayer()->GetName());
	WorldPacket data;
	data.Initialize(SMSG_FORCE_MOVE_UNROOT);
	data << plr->GetNewGUID();
	data << uint32(5);

	plr->SendMessageToSet(&data, true);
	return true;
}

bool ChatHandler::HandleSetMotdCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "You must specify a message.");
		return true;
	}

	GreenSystemMessage(m_session, "Motd has been set to: %s", args);
	World::getSingleton().SetMotd(args);
	sGMLog.writefromsession(m_session, "Set MOTD to %s", args);
	return true;
}

bool ChatHandler::HandleAddItemSetCommand(const char* args, WorldSession* m_session)
{
	uint32 setid = (args ? atoi(args) : 0);
	if(!setid)
	{
		RedSystemMessage(m_session, "You must specify a setid.");
		return true;
	}

	Player *chr = getSelectedChar(m_session);
	if (chr == NULL) {
	RedSystemMessage(m_session, "Unable to select character.");
	return true;
	}

	ItemSetEntry *entry = dbcItemSet.LookupEntry(setid);
	std::list<ItemPrototype*>* l = objmgr.GetListForItemSet(setid);
	if(!entry || !l)
	{
		RedSystemMessage(m_session, "Invalid item set.");
		return true;
	}
	//const char* setname = sItemSetStore.LookupString(entry->name);
	BlueSystemMessage(m_session, "Searching item set %u...", setid);
	uint32 start = getMSTime();
	sGMLog.writefromsession(m_session, "used add item set command, set %u, target %s", setid, chr->GetName());
	for(std::list<ItemPrototype*>::iterator itr = l->begin(); itr != l->end(); ++itr)
	{
		Item *itm = objmgr.CreateItem((*itr)->ItemId, m_session->GetPlayer());
		if(!itm) continue;
		if(!chr->GetItemInterface()->AddItemToFreeSlot(itm))
		{
			m_session->SendNotification("No free slots left!");
			delete itm;
			return true;
		} else {
			//SystemMessage(m_session, "Added item: %s [%u]", (*itr)->Name1, (*itr)->ItemId);
			SlotResult * le = chr->GetItemInterface()->LastSearchResult();
			chr->GetSession()->SendItemPushResult(itm,false,true,false,true,le->ContainerSlot,le->Slot,1);
		}
	}
	GreenSystemMessage(m_session, "Added set to inventory complete. Time: %u ms", getMSTime() - start);
	return true;
}

bool ChatHandler::HandleExitInstanceCommand(const char* args, WorldSession* m_session)
{
	BlueSystemMessage(m_session, "Attempting to exit from instance...");
	bool result = m_session->GetPlayer()->ExitInstance();
	if(!result)
	{
		RedSystemMessage(m_session, "Entry points not found.");
		return true;
	} else {
		GreenSystemMessage(m_session, "Removal successful.");
		return true;
	}
}

bool ChatHandler::HandleCastTimeCheatCommand(const char* args, WorldSession* m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->CastTimeCheat;
	BlueSystemMessage(m_session, "%s cast time cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a cast time cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->CastTimeCheat = !val;
	sGMLog.writefromsession(m_session, "%s cast time cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandleCooldownCheatCommand(const char* args, WorldSession* m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->CooldownCheat;
	BlueSystemMessage(m_session, "%s cooldown cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a cooldown cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->CooldownCheat = !val;
	sGMLog.writefromsession(m_session, "%s cooldown cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandleGodModeCommand(const char* args, WorldSession* m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->GodModeCheat;
	BlueSystemMessage(m_session, "%s godmode cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a godmode cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->GodModeCheat = !val;
	sGMLog.writefromsession(m_session, "%s godmode cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandlePowerCheatCommand(const char* args, WorldSession* m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->PowerCheat;
	BlueSystemMessage(m_session, "%s power cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a power cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->PowerCheat = !val;
	sGMLog.writefromsession(m_session, "%s powertime cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandleShowCheatsCommand(const char* args, WorldSession* m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	uint32 active = 0, inactive = 0;
#define print_cheat_status(CheatName, CheatVariable) SystemMessage(m_session, "%s%s: %s%s", MSG_COLOR_LIGHTBLUE, CheatName, \
		CheatVariable ? MSG_COLOR_LIGHTRED : MSG_COLOR_GREEN, CheatVariable ? "Active" : "Inactive");  \
		if(CheatVariable) \
		active++; \
		else \
		inactive++; 

	GreenSystemMessage(m_session, "Showing cheat status for: %s", plyr->GetName());
	print_cheat_status("Cooldown", plyr->CooldownCheat);
	print_cheat_status("CastTime", plyr->CastTimeCheat);
	print_cheat_status("GodMode", plyr->GodModeCheat);
	print_cheat_status("Power", plyr->PowerCheat);
	print_cheat_status("Fly", plyr->FlyCheat);
	print_cheat_status("AuraStack", plyr->stack_cheat);
	SystemMessage(m_session, "%u cheats active, %u inactive.", active, inactive);

#undef print_cheat_status

	return true;
}

bool ChatHandler::HandleFlyCommand(const char* args, WorldSession* m_session)
{
	WorldPacket fly(835, 13);
	
	Player *chr = getSelectedChar(m_session);
	
	if(!chr)
		chr = m_session->GetPlayer();
	
	chr->FlyCheat = true;
	fly << chr->GetNewGUID();
	fly << uint32(2);
	chr->SendMessageToSet(&fly, true);
	BlueSystemMessage(chr->GetSession(), "Flying mode enabled.");
	return 1;
}

bool ChatHandler::HandleLandCommand(const char* args, WorldSession* m_session)
{
	WorldPacket fly(836, 13);
	
	Player *chr = getSelectedChar(m_session);
	
	if(!chr)
		chr = m_session->GetPlayer();
	
	chr->FlyCheat = false;
	fly << chr->GetNewGUID();
	fly << uint32(5);
	chr->SendMessageToSet(&fly, true);
	BlueSystemMessage(chr->GetSession(), "Flying mode disabled.");
	return 1;
}

bool ChatHandler::HandleDBReloadCommand(const char* args, WorldSession* m_session)
{
	char str[200];
	if(!*args || strlen(args) < 3)
		return false;

	uint32 mstime = getMSTime();
	snprintf(str, 200, "%s%s initiated server-side reload of table `%s`. The server may experience some lag while this occurs.",
		MSG_COLOR_LIGHTRED, m_session->GetPlayer()->GetName(), args);
	sWorld.SendWorldText(str, 0);
	if(!Storage_ReloadTable(args))
		snprintf(str, 256, "%sDatabase reload failed.", MSG_COLOR_LIGHTRED);
	else
		snprintf(str, 256, "%sDatabase reload completed in %u ms.", MSG_COLOR_LIGHTBLUE, (unsigned int)(getMSTime() - mstime));
	sWorld.SendWorldText(str, 0);
	sGMLog.writefromsession(m_session, "reloaded table %s", args);
	return true;
}

bool ChatHandler::HandleFlySpeedCheatCommand(const char* args, WorldSession* m_session)
{
	float Speed = (float)atof(args);
	if(Speed == 0)
		Speed = 20;

	Player * plr = getSelectedChar(m_session);
	if(plr == 0)
		return true;

	BlueSystemMessage(m_session, "Setting the fly speed of %s to %f.", plr->GetName(), Speed);
	GreenSystemMessage(plr->GetSession(), "%s set your fly speed to %f.", m_session->GetPlayer()->GetName(), Speed);
	
	WorldPacket data(SMSG_FORCE_MOVE_SET_FLY_SPEED, 16);
	data << plr->GetNewGUID();
	data << uint32(0) << Speed;
	plr->SendMessageToSet(&data, true);
	
	plr->m_flySpeed = Speed;
	
	return true;
}

bool ChatHandler::HandleModifyLevelCommand(const char* args, WorldSession* m_session)
{
	Player * plr = getSelectedChar(m_session, true);
	if(plr == 0) return true;

	uint32 Level = args ? atol(args) : 0;
	if(Level == 0 || Level > sWorld.m_levelCap)
	{
		RedSystemMessage(m_session, "A level (numeric) is required to be specified after this command.");
		return true;
	}

	// Set level message
	BlueSystemMessage(m_session, "Setting the level of %s to %u.", plr->GetName(), Level);
	GreenSystemMessageToPlr(plr, "%s set your level to %u.", m_session->GetPlayer()->GetName(), Level);

	sGMLog.writefromsession(m_session, "used modify level on %s, level %u", plr->GetName(), Level);

	// lookup level information
	LevelInfo * Info = objmgr.GetLevelInfo(plr->getRace(), plr->getClass(), Level);
	if(Info == 0)
	{
		RedSystemMessage(m_session, "Levelup information not found.");
		return true;
	}

	plr->ApplyLevelInfo(Info, Level);
	return true;
}

bool ChatHandler::HandleCreatePetCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
		return false;

	uint32 Entry = atol(args);
	if(!Entry)
		return false;
	CreatureProto * pTemplate = CreatureProtoStorage.LookupEntry(Entry);
	CreatureInfo * pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
	if(!pTemplate || !pCreatureInfo)
	{
		RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
		return true;
	}
	Player * plr = m_session->GetPlayer();

	// spawn a creature of this id to create from
	Creature * pCreature = new Creature(HIGHGUID_UNIT ,1);//no need in guid
	CreatureSpawn * sp = new CreatureSpawn;
	sp->id = 1;
	sp->bytes = 0;
	sp->bytes2 = 0;
	sp->displayid = pCreatureInfo->Male_DisplayID;
	sp->emote_state = 0;
	sp->entry = pCreatureInfo->Id;
	sp->factionid = pTemplate->Faction;
	sp->flags = 0;
	sp->form = 0;
	sp->movetype = 0;
	sp->o = plr->GetOrientation();
	sp->x = plr->GetPositionX();
	sp->y = plr->GetPositionY();
	sp->respawnNpcLink = 0;
	sp->channel_spell=sp->channel_target_creature=sp->channel_target_go=0;
	pCreature->Load(sp, (uint32)NULL, NULL);

	Pet *old_tame = plr->GetSummon();
	if(old_tame != NULL)
	{
		old_tame->Dismiss(true);
	}

	// create a pet from this creature
	Pet * pPet = objmgr.CreatePet();
	pPet->SetInstanceID(plr->GetInstanceID());
	pPet->SetMapId(plr->GetMapId());
	pPet->CreateAsSummon(Entry, pCreatureInfo, pCreature, plr, NULL, 0x2, 0);

	// remove the temp creature
	delete sp;
	delete pCreature;

	sGMLog.writefromsession(m_session, "used create pet entry %u", Entry);

	return true;
}

bool ChatHandler::HandleAddPetSpellCommand(const char* args, WorldSession* m_session)
{
	Player * plr = m_session->GetPlayer();
	Pet * pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry * spell = dbcSpell.LookupEntry(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	pPet->AddSpell(spell, true);
	GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
	return true;
}

bool ChatHandler::HandleRemovePetSpellCommand(const char* args, WorldSession* m_session)
{
	Player * plr = m_session->GetPlayer();
	Pet * pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry * spell = dbcSpell.LookupEntry(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	pPet->RemoveSpell(SpellId);
	GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
	return true;
}

bool ChatHandler::HandleRenamePetCommand(const char* args, WorldSession* m_session)
{
	Player * plr = m_session->GetPlayer();
	Pet * pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	if(strlen(args) < 1)
	{
		RedSystemMessage(m_session, "You must specify a name.");
		return true;
	}

	GreenSystemMessage(m_session, "Renamed your pet to %s.", args);
	pPet->Rename(args);
	return true;
}

bool ChatHandler::HandleShutdownCommand(const char* args, WorldSession* m_session)
{
	uint32 shutdowntime = atol(args);
	if(!args)
		shutdowntime = 5;

	
	char msg[500];
	snprintf(msg, 500, "%sServer shutdown initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
		m_session->GetPlayer()->GetName(), (unsigned int)shutdowntime);

	sWorld.SendWorldText(msg);
	sGMLog.writefromsession(m_session, "initiated server shutdown timer %u sec", shutdowntime);
	shutdowntime *= 1000;
	sMaster.m_ShutdownTimer = shutdowntime;
	sMaster.m_ShutdownEvent = true;
	sMaster.m_restartEvent = false;
	return true;
}

bool ChatHandler::HandleShutdownRestartCommand(const char* args, WorldSession* m_session)
{
	uint32 shutdowntime = atol(args);
	if(!args)
		shutdowntime = 5;

	char msg[500];
	snprintf(msg, 500, "%sServer restart initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
		m_session->GetPlayer()->GetName(), (unsigned int)shutdowntime);

	sGMLog.writefromsession(m_session, "initiated server restart timer %u sec", shutdowntime);
	sWorld.SendWorldText(msg);
		shutdowntime *= 1000;
	sMaster.m_ShutdownTimer = shutdowntime;
	sMaster.m_ShutdownEvent = true;
	sMaster.m_restartEvent = true;
	return true;
}

bool ChatHandler::HandleAllowWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	Player * plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->gmTargets.insert(plr);
	BlueSystemMessage(m_session, "Now accepting whispers from %s.", plr->GetName());
	return true;
}

bool ChatHandler::HandleBlockWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	Player * plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->gmTargets.erase(plr);
	BlueSystemMessage(m_session, "Now blocking whispers from %s.", plr->GetName());
	return true;
}

bool ChatHandler::HandleAdvanceAllSkillsCommand(const char* args, WorldSession* m_session)
{
	uint32 amt = args ? atol(args) : 0;
	if(!amt)
	{
		RedSystemMessage(m_session, "An amount to increment is required.");
		return true;
	}

	Player * plr = getSelectedChar(m_session);
	if(!plr)
		return true;


	plr->_AdvanceAllSkills(amt);
	GreenSystemMessageToPlr(plr, "Advanced all your skill lines by %u points.", amt);
	sGMLog.writefromsession(m_session, "advanced all skills by %u on %s", amt, plr->GetName());
	return true;
}

bool ChatHandler::HandleKillByPlayerCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	Player * plr = objmgr.GetPlayer(args, false);
	if(plr == 0)
	{
		RedSystemMessage(m_session, "Player %s not found.", args);
		return true;
	}

	GreenSystemMessage(m_session, "Disconnecting %s.", plr->GetName());
	sGMLog.writefromsession(m_session, "disconnected %s via .killbyplayer", plr->GetName());
	plr->GetSession()->Disconnect();
	return true;
}

bool ChatHandler::HandleKillBySessionCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	WorldSession * session = sWorld.FindSessionByName(args);
	if(session == 0)
	{
		RedSystemMessage(m_session, "Active session with name %s not found.", args);
		return true;
	}

	GreenSystemMessage(m_session, "Disconnecting %s.", session->GetAccountName().c_str());
	sGMLog.writefromsession(m_session, "disconnected %s via .killbysession", session->GetAccountName().c_str());
	session->Disconnect();
	return true;
}

bool ChatHandler::HandleMassSummonCommand(const char* args, WorldSession* m_session)
{
	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	Player * summoner = m_session->GetPlayer();
	Player * plr;
	uint32 c=0;
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			//plr->SafeTeleport(summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
			/* let's do this the blizz way */
			plr->SummonRequest(summoner->GetGUIDLow(), summoner->GetZoneId(), summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
			++c;
		}
	}
	sGMLog.writefromsession(m_session, "requested a mass summon of %u players.", c);
	objmgr._playerslock.ReleaseReadLock();
	return true;
}

bool ChatHandler::HandleCastAllCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "No spellid specified.");
		return true;
	}
	Player * plr;
	uint32 spellid = atol(args);
	SpellEntry * info = dbcSpell.LookupEntry(spellid);
	if(!info)
	{
		RedSystemMessage(m_session, "Invalid spell specified.");
		return true;
	}

	// this makes sure no moron casts a learn spell on everybody and wrecks the server
	for (int i = 0; i < 3; i++)
	{
		if (info->Effect[i] == 36) //SPELL_EFFECT_LEARN_SPELL - 36
		{
			sGMLog.writefromsession(m_session, "used wrong / learnall castall command, spellid %u", spellid);
			RedSystemMessage(m_session, "Learn spell specified.");
			return true;
		}
	}

	sGMLog.writefromsession(m_session, "used castall command, spellid %u", spellid);

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
			{
				sEventMgr.AddEvent(((Unit*)plr), &Unit::EventCastSpell, ((Unit*)plr), info, EVENT_PLAYER_CHECKFORCHEATS, 100, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			}
			else
			{
				Spell * sp = new Spell(plr, info, true, 0);
				SpellCastTargets targets(plr->GetGUID());
				sp->prepare(&targets);
			}
		}
	}
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Casted spell %u on all players!", spellid);
	return true;
}

bool ChatHandler::HandleNpcReturnCommand(const char* args, WorldSession* m_session)
{
	Creature * creature = getSelectedCreature(m_session);
	if(!creature || !creature->m_spawn) return true;

	// return to respawn coords
	float x = creature->m_spawn->x;
	float y = creature->m_spawn->y;
	float z = creature->m_spawn->z;
	float o = creature->m_spawn->o;
	
	// restart movement
	creature->GetAIInterface()->SetAIState(STATE_IDLE);
	creature->GetAIInterface()->WipeHateList();
	creature->GetAIInterface()->WipeTargetList();
	creature->GetAIInterface()->MoveTo(x, y, z, o);

	return true;
}

bool ChatHandler::HandleModPeriodCommand(const char* args, WorldSession * m_session)
{
	Transporter * trans = m_session->GetPlayer()->m_CurrentTransporter;
	if(trans == 0)
	{
		RedSystemMessage(m_session, "You must be on a transporter.");
		return true;
	}

	uint32 np = args ? atol(args) : 0;
	if(np == 0)
	{
		RedSystemMessage(m_session, "A time in ms must be specified.");
		return true;
	}

	trans->SetPeriod(np);
	BlueSystemMessage(m_session, "Period of %s set to %u.", trans->GetInfo()->Name, np);
	return true;
}

bool ChatHandler::HandleFormationLink1Command(const char* args, WorldSession * m_session)
{
	// set formation "master"
	Creature * pCreature = getSelectedCreature(m_session, true);
	if(pCreature == 0) return true;

	m_session->GetPlayer()->linkTarget = pCreature;
	BlueSystemMessage(m_session, "Linkup \"master\" set to %s.", pCreature->GetCreatureName()->Name);
	return true;
}

bool ChatHandler::HandleFormationLink2Command(const char* args, WorldSession * m_session)
{
	// set formation "slave" with distance and angle
	float ang, dist;
	if(*args == 0 || sscanf(args, "%f %f", &dist, &ang) != 2)
	{
		RedSystemMessage(m_session, "You must specify a distance and angle.");
		return true;
	}

	if(m_session->GetPlayer()->linkTarget == 0 || m_session->GetPlayer()->linkTarget->GetTypeId() != TYPEID_UNIT)
	{
		RedSystemMessage(m_session, "Master not selected. select the master, and use formationlink1.");
		return true;
	}

	Creature * slave = getSelectedCreature(m_session, true);
	if(slave == 0) return true;

	slave->GetAIInterface()->m_formationFollowDistance = dist;
	slave->GetAIInterface()->m_formationFollowAngle = ang;
	slave->GetAIInterface()->m_formationLinkTarget = ((Creature*)m_session->GetPlayer()->linkTarget);
	slave->GetAIInterface()->m_formationLinkSqlId = slave->GetAIInterface()->m_formationLinkTarget->GetSQL_id();
	slave->GetAIInterface()->SetUnitToFollowAngle(ang);
	
	// add to db
	WorldDatabase.Execute("INSERT INTO creature_formations VALUES(%u, %u, '%f', '%f')", 
		slave->GetSQL_id(), slave->GetAIInterface()->m_formationLinkSqlId, ang, dist);

	BlueSystemMessage(m_session, "%s linked up to %s with a distance of %f at %f radians.", slave->GetCreatureName()->Name, 
		((Creature*)m_session->GetPlayer()->linkTarget)->GetCreatureName()->Name, dist, ang);

	return true;
}

bool ChatHandler::HandleNpcFollowCommand(const char* args, WorldSession * m_session)
{
	Creature * creature = getSelectedCreature(m_session, true);
	if(!creature) return true;

	creature->GetAIInterface()->SetUnitToFollow(m_session->GetPlayer());
	return true;
}

bool ChatHandler::HandleFormationClearCommand(const char* args, WorldSession * m_session)
{
	Creature * c = getSelectedCreature(m_session, true);
	if(!c) return true;

	c->GetAIInterface()->m_formationLinkSqlId = 0;
	c->GetAIInterface()->m_formationLinkTarget = 0;
	c->GetAIInterface()->m_formationFollowAngle = 0.0f;
	c->GetAIInterface()->m_formationFollowDistance = 0.0f;
	c->GetAIInterface()->SetUnitToFollow(0);
	
	WorldDatabase.Execute("DELETE FROM creature_formations WHERE spawn_id=%u", c->GetSQL_id());
	return true;
}

bool ChatHandler::HandleNullFollowCommand(const char* args, WorldSession * m_session)
{
	Creature * c = getSelectedCreature(m_session, true);
	if(!c) return true;

	// restart movement
	c->GetAIInterface()->SetAIState(STATE_IDLE);
	c->GetAIInterface()->SetUnitToFollow(0);
	return true;
}

bool ChatHandler::HandleStackCheatCommand(const char* args, WorldSession * m_session)
{
	Player * plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->stack_cheat;
	BlueSystemMessage(m_session, "%s aura stack cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s an aura stack cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->stack_cheat = !val;
	sGMLog.writefromsession(m_session, "used stack cheat on %s", plyr->GetName());
	return true;
}

bool ChatHandler::HandleTriggerpassCheatCommand(const char* args, WorldSession * m_session)
{
	Player * plr = getSelectedChar(m_session, true);
	if (!plr)
		return true;

	bool val = plr->triggerpass_cheat;
	BlueSystemMessage(m_session, "%s areatrigger prerequisites immunity cheat on %s.", val ? "Deactivated" : "Activated", plr->GetName());
	GreenSystemMessageToPlr(plr, "%s %s areatrigger prerequisites immunity cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plr->triggerpass_cheat = !val;
	sGMLog.writefromsession(m_session, "used areatrigger cheat on %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSkillsCommand(const char* args, WorldSession * m_session)
{
	skilllineentry * se;
	Player * plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	plr->_RemoveAllSkills();

	// Load skills from create info.
	PlayerCreateInfo * info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
	if(!info) return true;

	for(std::list<CreateInfo_SkillStruct>::iterator ss = info->skills.begin(); ss!=info->skills.end(); ss++)
	{
		se = dbcSkillLine.LookupEntry(ss->skillid);
		if(se->type != SKILL_TYPE_LANGUAGE && ss->skillid && ss->currentval && ss->maxval)
			plr->_AddSkillLine(ss->skillid, ss->currentval, ss->maxval);		
	}
	//Chances depend on stats must be in this order!
	plr->UpdateStats();
	plr->UpdateChances();
	plr->_UpdateMaxSkillCounts();
	plr->_AddLanguages(false);
	BlueSystemMessage(m_session, "Reset skills to default.");
	sGMLog.writefromsession(m_session, "reset skills of %s to default", plr->GetName());
	return true;
}

bool ChatHandler::HandlePlayerInfo(const char* args, WorldSession * m_session)
{
	Player * plr;
	if(strlen(args) >= 2) // char name can be 2 letters
	{
		plr = objmgr.GetPlayer(args, false);
		if(!plr)
		{
			RedSystemMessage(m_session, "Unable to locate player %s.", args);		
			return true;
		}
	}
	else
		plr = getSelectedChar(m_session, true);
	
	if(!plr) return true;
	if(!plr->GetSession())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any session !");
		return true;
	}
	if(!plr->GetSession()->GetSocket())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any socket !");
		return true;
	}
	WorldSession* sess = plr->GetSession();

//	char* infos = new char[128];
	static const char* classes[12] =
	{"None","Warrior", "Paladin", "Hunter", "Rogue", "Priest", "None", "Shaman", "Mage", "Warlock", "None", "Druid"};
	static const char* races[12] =
	{"None","Human","Orc","Dwarf","Night Elf","Undead","Tauren","Gnome","Troll","None","Blood Elf","Draenei"};

	char playedLevel[64];
	char playedTotal[64];

	int seconds = (plr->GetPlayedtime())[0];
	int mins=0;
	int hours=0;
	int days=0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins*60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours*60;
					if(hours >= 24)
					{
						days = hours/24;
						if(days)
							hours -= days*24;
					}
				}
			}
		}
	}
	snprintf(playedLevel, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	seconds = (plr->GetPlayedtime())[1];
	mins=0;
	hours=0;
	days=0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins*60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours*60;
					if(hours >= 24)
					{
						days = hours/24;
						if(days)
							hours -= days*24;
					}
				}
			}
		}
	}
	snprintf(playedTotal, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	GreenSystemMessage(m_session, "%s is a %s %s %s", plr->GetName(),
		(plr->getGender()?"Female":"Male"), races[plr->getRace()], classes[plr->getClass()]);

	BlueSystemMessage(m_session, "%s has played %s at this level",(plr->getGender()?"She":"He"), playedLevel);
	BlueSystemMessage(m_session, "and %s overall", playedTotal);

	BlueSystemMessage(m_session, "%s is connecting from account '%s'[%u] with permissions '%s'",
		(plr->getGender()?"She":"He"), sess->GetAccountName().c_str(), sess->GetAccountId(), sess->GetPermissions());

	BlueSystemMessage(m_session, "%s uses %s (build %u)", (plr->getGender()?"She":"He"),
		(sess->HasFlag(ACCOUNT_FLAG_XPACK_01)?"WoW Burning Crusade":"WoW"), sess->GetClientBuild());

	BlueSystemMessage(m_session, "%s IP is '%s', and has a latency of %ums", (plr->getGender()?"Her":"His"),
		sess->GetSocket()->GetRemoteIP().c_str(), sess->GetLatency());

	return true;
}

bool ChatHandler::HandleGlobalPlaySoundCommand(const char* args, WorldSession * m_session)
{
	if(!*args) return false;
	uint32 sound = atoi(args);
	if(!sound) return false;

	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << sound;
	sWorld.SendGlobalMessage(&data, 0);
	BlueSystemMessage(m_session, "Broadcasted sound %u to server.", sound);
	sGMLog.writefromsession(m_session, "used play all command soundid %u", sound);
	return true;
}

bool ChatHandler::HandleIPBanCommand(const char * args, WorldSession * m_session)
{
	char * pIp = (char*)args;
	char * pDuration = strchr(pIp, ' ');
	if(pDuration == NULL)
		return false;
	*pDuration = 0;
	++pDuration;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if(timeperiod < 1)
		return false;

	uint32 o1, o2, o3, o4;
	if ( sscanf(pIp, "%3u.%3u.%3u.%3u", (unsigned int*)&o1, (unsigned int*)&o2, (unsigned int*)&o3, (unsigned int*)&o4) != 4
			|| o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255)
	{
		RedSystemMessage(m_session, "Invalid IPv4 address [%s]", pIp);
		return true;	// error in syntax, but we wont remind client of command usage
	}

	time_t expire_time;
	if ( timeperiod == 0)		// permanent ban
		expire_time = 0;
	else
		expire_time = UNIXTIME + (time_t)timeperiod;
	
	SystemMessage(m_session, "Adding [%s] to IP ban table, expires %s", pIp, (expire_time == 0)? "Never" : ctime( &expire_time ));
	sLogonCommHandler.LogonDatabaseSQLExecute("REPLACE INTO ipbans VALUES ('%s', %u);", WorldDatabase.EscapeString(pIp).c_str(), (uint32)expire_time);
	sLogonCommHandler.LogonDatabaseReloadAccounts();
	return true;
}

bool ChatHandler::HandleIPUnBanCommand(const char * args, WorldSession * m_session)
{
	char ip[16] = {0};		// IPv4 address

	// we require at least one argument, the network address to unban
	if ( sscanf(args, "%15s", ip) < 1)
		return false;

	/**
	 * We can afford to be less fussy with the validty of the IP address given since
	 * we are only attempting to remove it.
	 * Sadly, we can only blindly execute SQL statements on the logonserver so we have
	 * no idea if the address existed and so the account/IPBanner cache requires reloading.
	 */

	SystemMessage(m_session, "Removing [%s] from IP ban table if it exists", ip);
	sLogonCommHandler.LogonDatabaseSQLExecute("DELETE FROM ipbans WHERE ip = '%s';", WorldDatabase.EscapeString(ip).c_str());
	sLogonCommHandler.LogonDatabaseReloadAccounts();
	return true;
}

bool ChatHandler::HandleCreatureSpawnCommand(const char *args, WorldSession *m_session)
{
	uint32 entry = atol(args);
	if(entry == 0)
		return false;

	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(entry);
	if(proto == 0 || info == 0)
	{
		RedSystemMessage(m_session, "Invalid entry id.");
		return true;
	}

	CreatureSpawn * sp = new CreatureSpawn;
	//sp->displayid = info->DisplayID;
	info->GenerateModelId(&sp->displayid);
	sp->entry = entry;
	sp->form = 0;
	sp->id = objmgr.GenerateCreatureSpawnID();
	sp->movetype = 0;
	sp->x = m_session->GetPlayer()->GetPositionX();
	sp->y = m_session->GetPlayer()->GetPositionY();
	sp->z = m_session->GetPlayer()->GetPositionZ();
	sp->o = m_session->GetPlayer()->GetOrientation();
	sp->emote_state =0;
	sp->flags = 0;
	sp->factionid = proto->Faction;
	sp->bytes=0;
	sp->bytes2=0;
	sp->respawnNpcLink = 0;
	sp->channel_spell=sp->channel_target_creature=sp->channel_target_go=0;


	Creature * p = m_session->GetPlayer()->GetMapMgr()->CreateCreature();
	ASSERT(p);
	p->Load(sp, (uint32)NULL, NULL);
	p->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	
	uint32 x = m_session->GetPlayer()->GetMapMgr()->GetPosX(m_session->GetPlayer()->GetPositionX());
	uint32 y = m_session->GetPlayer()->GetMapMgr()->GetPosY(m_session->GetPlayer()->GetPositionY());

	// Add spawn to map
	m_session->GetPlayer()->GetMapMgr()->GetBaseMap()->GetSpawnsListAndCreate(
		x,
		y)->CreatureSpawns.push_back(sp);

	BlueSystemMessage(m_session, "Spawned a creature `%s` with entry %u at %f %f %f on map %u", info->Name, 
		entry, sp->x, sp->y, sp->z, m_session->GetPlayer()->GetMapId());

	// Save it to the database.
	p->SaveToDB();

	sGMLog.writefromsession(m_session, "spawned a %s at %u %f %f %f", info->Name, m_session->GetPlayer()->GetMapId(),sp->x,sp->y,sp->z);

	return true;
}

bool ChatHandler::HandleRemoveItemCommand(const char * args, WorldSession * m_session)
{
	uint32 item_id;
	int32 count, ocount;
	int argc = sscanf(args, "%u %u", (unsigned int*)&item_id, (unsigned int*)&count);
	if(argc == 1)
		count = 1;
	else if(argc != 2 || !count)
		return false;

	ocount = count;
	Player * plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	// loop until they're all gone.
	int32 loop_count = 0;
	int32 start_count = plr->GetItemInterface()->GetItemCount(item_id, true);
	int32 start_count2 = start_count;
	if(count > start_count)
		count = start_count;

	while(start_count >= count && (count > 0) && loop_count < 20)	 // Prevent a loop here.
	{
		plr->GetItemInterface()->RemoveItemAmt(item_id, count);
		start_count2 = plr->GetItemInterface()->GetItemCount(item_id, true);
		count -= (start_count - start_count2);
		start_count = start_count2;
		++loop_count;
	}

	sGMLog.writefromsession(m_session, "used remove item id %u count %u from %s", item_id, ocount, plr->GetName());
	BlueSystemMessage(m_session, "Removing %u copies of item %u from %s's inventory.", ocount, item_id, plr->GetName());
	BlueSystemMessage(plr->GetSession(), "%s removed %u copies of item %u from your inventory.", m_session->GetPlayer()->GetName(), ocount, item_id);
	return true;
}

bool ChatHandler::HandleForceRenameCommand(const char * args, WorldSession * m_session)
{
	// prevent buffer overflow
	if(strlen(args) > 100)
		return false;

	string tmp = string(args);
	PlayerInfo * pi = objmgr.GetPlayerInfoByName(tmp);
	if(pi == 0)
	{
		RedSystemMessage(m_session, "Player with that name not found.");
		return true;
	}

	Player * plr = objmgr.GetPlayer((uint32)pi->guid);
	if(plr == 0)
	{
		CharacterDatabase.Execute("UPDATE characters SET forced_rename_pending = 1 WHERE guid = %u", (uint32)pi->guid);
	}
	else
	{
		plr->rename_pending = true;
		plr->SaveToDB(false);
		BlueSystemMessageToPlr(plr, "%s forced your character to be renamed next logon.", m_session->GetPlayer()->GetName());
	}

	CharacterDatabase.Execute("INSERT INTO banned_names VALUES('%s')", CharacterDatabase.EscapeString(string(pi->name)).c_str());
	GreenSystemMessage(m_session, "Forcing %s to rename his character next logon.", args);
	return true;
}

bool ChatHandler::HandleGetStandingCommand(const char * args, WorldSession * m_session)
{
	uint32 faction = atoi(args);
	Player * plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	int32 standing = plr->GetStanding(faction);
	int32 bstanding = plr->GetBaseStanding(faction);

	GreenSystemMessage(m_session, "Reputation for faction %u:", faction);
	SystemMessage(m_session,	  "   Base Standing: %d", bstanding);
	BlueSystemMessage(m_session,  "   Standing: %d", standing);
	return true;
}

bool ChatHandler::HandleSetStandingCommand(const char * args, WorldSession * m_session)
{
	uint32 faction;
	int32 standing;
	if(sscanf(args, "%u %d", (unsigned int*)&faction, (unsigned int*)&standing) != 2)
		return false;
	Player * plr = getSelectedChar(m_session, true);
	if(!plr) return true;;

	BlueSystemMessage(m_session, "Setting standing of %u to %d on %s.", faction, standing, plr->GetName());
	plr->SetStanding(faction, standing);
	GreenSystemMessageToPlr(plr, "%s set your standing of faction %u to %d.", m_session->GetPlayer()->GetName(), faction, standing);
	sGMLog.writefromsession(m_session, "set standing of faction %u to %u for %s", faction,standing,plr->GetName());
	return true;
}

void SendHighlightedName(WorldSession * m_session, char* full_name, string& lowercase_name, string& highlight, uint32 id, bool item)
{
	char message[1024];
	char start[50];
	start[0] = message[0] = 0;

	snprintf(start, 50, "%s %u: %s", item ? "Item" : "Creature", (unsigned int)id, MSG_COLOR_WHITE);

	string::size_type hlen = highlight.length();
	string fullname = string(full_name);
	string::size_type offset = lowercase_name.find(highlight);
	string::size_type remaining = fullname.size() - offset - hlen;
	strcat(message, start);
	strncat(message, fullname.c_str(), offset);
	if(remaining > 0)
	{
		strcat(message, MSG_COLOR_LIGHTRED);
		strncat(message, (fullname.c_str() + offset), hlen);
		strcat(message, MSG_COLOR_WHITE);
		strncat(message, (fullname.c_str() + offset + hlen), remaining);
	}

	sChatHandler.SystemMessage(m_session, message);
}

bool ChatHandler::HandleLookupItemCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	transform(x.begin(), x.end(), x.begin(), towlower);
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 5 characters long.");
		return true;
	}

	StorageContainerIterator<ItemPrototype> * itr = ItemPrototypeStorage.MakeIterator();

	BlueSystemMessage(m_session, "Starting search of item `%s`...", x.c_str());
	uint32 t = getMSTime();
	ItemPrototype * it;
	uint32 count = 0;
	while(!itr->AtEnd())
	{
		it = itr->Get();
		if(FindXinYString(x, it->lowercase_name))
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, it->Name1, it->lowercase_name, x, it->ItemId, true);
			++count;
			if(count == 25)
			{
				RedSystemMessage(m_session, "More than 25 results returned. aborting.");
				break;
			}
		}
		
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupCreatureCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	transform(x.begin(), x.end(), x.begin(), towlower);
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 5 characters long.");
		return true;
	}

	StorageContainerIterator<CreatureInfo> * itr = CreatureNameStorage.MakeIterator();

	GreenSystemMessage(m_session, "Starting search of creature `%s`...", x.c_str());
	uint32 t = getMSTime();
	CreatureInfo * i;
	uint32 count = 0;
	while(!itr->AtEnd())
	{
		i = itr->Get();
		if(FindXinYString(x, i->lowercase_name))
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, i->Name, i->lowercase_name, x, i->Id, false);

			++count;
			if(count == 25)
			{
				RedSystemMessage(m_session, "More than 25 results returned. aborting.");
				break;
			}
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleGORotate(const char * args, WorldSession * m_session)
{
	GameObject *go = m_session->GetPlayer()->m_GM_SelectedGO;
	if( !go )
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	float deg = (float)atof(args);
	if(deg == 0.0f)
		return false;

	// Convert the argument to radians
	float rad = deg * (float(M_PI) / 180.0f);

	// let's try rotation_0
	go->ModFloatValue(GAMEOBJECT_ROTATION, rad);
	go->ModFloatValue(GAMEOBJECT_ROTATION_01, rad);
	go->ModFloatValue(GAMEOBJECT_ROTATION_02, rad);
	go->ModFloatValue(GAMEOBJECT_ROTATION_03, rad);
	go->SaveToDB();

	// despawn and respawn
	//go->Despawn(1000);
	go->RemoveFromWorld(true);
	go->SetNewGuid(m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid());
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	return true;
}

bool ChatHandler::HandleGOMove(const char * args, WorldSession * m_session)
{
	// move the go to player's coordinates
	GameObject *go = m_session->GetPlayer()->m_GM_SelectedGO;
	if( !go )
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	go->RemoveFromWorld(true);
	go->SetPosition(m_session->GetPlayer()->GetPosition());
	go->SetFloatValue(GAMEOBJECT_POS_X, m_session->GetPlayer()->GetPositionX());
	go->SetFloatValue(GAMEOBJECT_POS_Y, m_session->GetPlayer()->GetPositionY());
	go->SetFloatValue(GAMEOBJECT_POS_Z, m_session->GetPlayer()->GetPositionZ());
	go->SetFloatValue(GAMEOBJECT_FACING, m_session->GetPlayer()->GetOrientation());
	go->SetNewGuid(m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid());
	go->SaveToDB();
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	return true;
}

bool ChatHandler::HandleNpcPossessCommand(const char * args, WorldSession * m_session)
{
	Unit * pTarget = getSelectedChar(m_session, false);
	if(!pTarget)
	{
		pTarget = getSelectedCreature(m_session, false);
		if(pTarget && (pTarget->IsPet() || pTarget->GetUInt32Value(UNIT_FIELD_CREATEDBY) != 0))
			return false;
	}
		
	if(!pTarget)
	{
		RedSystemMessage(m_session, "You must select a player/creature.");
		return true;
	}

	m_session->GetPlayer()->Possess(pTarget);
	BlueSystemMessage(m_session, "Possessed "I64FMT, pTarget->GetGUID());
	return true;
}

bool ChatHandler::HandleNpcUnPossessCommand(const char * args, WorldSession * m_session)
{
	Creature * creature = getSelectedCreature(m_session);
 	m_session->GetPlayer()->UnPossess();

	if(creature)
	{
			// restart movement
			creature->GetAIInterface()->SetAIState(STATE_IDLE);
			creature->GetAIInterface()->WipeHateList();
			creature->GetAIInterface()->WipeTargetList();

			if(creature->m_spawn)
			{
				// return to respawn coords
				float x = creature->m_spawn->x;
				float y = creature->m_spawn->y;
				float z = creature->m_spawn->z;
				float o = creature->m_spawn->o;
				creature->GetAIInterface()->MoveTo(x, y, z, o);
			}
	}
	GreenSystemMessage(m_session, "Removed any possessed targets.");
	return true;
}

bool ChatHandler::HandleChangePasswordCommand(const char * args, WorldSession * m_session)
{
	char password[100];
	char username[100];
	int argc = sscanf("%s %s", password, username);
	if(argc == 2)
	{
		// username = password :P
		BlueSystemMessage(m_session, "Changed password of %s to %s.", password, username);
		sLogonCommHandler.LogonDatabaseSQLExecute("UPDATE accounts SET password = '%s' WHERE login = '%s'", WorldDatabase.EscapeString(password).c_str(), WorldDatabase.EscapeString(username).c_str());
		sLogonCommHandler.LogonDatabaseReloadAccounts();
		sGMLog.writefromsession(m_session, "used change password command, %s to %s.", password, username);
	}
	else if(argc == 1)
	{
		// changing our own username.
		BlueSystemMessage(m_session, "Changed your password to %s.", password);
		sLogonCommHandler.LogonDatabaseSQLExecute("UPDATE accounts SET password = '%s' WHERE login = '%s'", WorldDatabase.EscapeString(password).c_str(), WorldDatabase.EscapeString(m_session->GetAccountName()).c_str());
		sLogonCommHandler.LogonDatabaseReloadAccounts();
		sGMLog.writefromsession(m_session, "used change password command, self to %s.", password);
	}
	else
	{
		RedSystemMessage(m_session, "Use .changepassword <username> <password> or .changepassword <password>.");
		return false;
	}
	return true;
}

bool ChatHandler::HandleRehashCommand(const char * args, WorldSession * m_session)
{
	/* rehashes */
	char msg[250];
	snprintf(msg, 250, "%s is rehashing config file.", m_session->GetPlayer()->GetName());
	sWorld.SendWorldWideScreenText(msg, 0);
	sWorld.SendWorldText(msg, 0);
	sWorld.Rehash(true);
	return true;
}

struct spell_thingo
{
	uint32 type;
	uint32 target;
};

list<SpellEntry*> aiagent_spells;
map<uint32, spell_thingo> aiagent_extra;

bool ChatHandler::HandleAIAgentDebugBegin(const char * args, WorldSession * m_session)
{
	QueryResult * result = WorldDatabase.Query("SELECT DISTINCT spell FROM ai_agents");
	if(!result) return false;

	do 
	{
		SpellEntry * se = dbcSpell.LookupEntryForced(result->Fetch()[0].GetUInt32());
		if(se)
			aiagent_spells.push_back(se);
	} while(result->NextRow());
	delete result;

	for(list<SpellEntry*>::iterator itr = aiagent_spells.begin(); itr != aiagent_spells.end(); ++itr)
	{
		result = WorldDatabase.Query("SELECT * FROM ai_agents WHERE spell = %u", (*itr)->Id);
		ASSERT(result);
		spell_thingo t;
		t.type = result->Fetch()[6].GetUInt32();
		t.target = result->Fetch()[7].GetUInt32();
		delete result;
		aiagent_extra[(*itr)->Id] = t;
	}

	GreenSystemMessage(m_session, "Loaded %u spells for testing.", aiagent_spells.size());
	return true;
}

SpellCastTargets SetTargets(SpellEntry * sp, uint32 type, uint32 targettype, Unit * dst, Creature * src)
{
	SpellCastTargets targets;
	targets.m_unitTarget = 0;
	targets.m_itemTarget = 0;
	targets.m_srcX = 0;
	targets.m_srcY = 0;
	targets.m_srcZ = 0;
	targets.m_destX = 0;
	targets.m_destY = 0;
	targets.m_destZ = 0;

	if(targettype == TTYPE_SINGLETARGET)
	{
		targets.m_targetMask = 2;
		targets.m_unitTarget = dst->GetGUID();
	}
	else if(targettype == TTYPE_SOURCE)
	{
		targets.m_targetMask = 32;
		targets.m_srcX = src->GetPositionX();
		targets.m_srcY = src->GetPositionY();
		targets.m_srcZ = src->GetPositionZ();
	}
	else if(targettype == TTYPE_DESTINATION)
	{
		targets.m_targetMask = 64;
		targets.m_destX = dst->GetPositionX();
		targets.m_destY = dst->GetPositionY();
		targets.m_destZ = dst->GetPositionZ();
	}

	return targets;
};

bool ChatHandler::HandleAIAgentDebugContinue(const char * args, WorldSession * m_session)
{
	uint32 count = atoi(args);
	if(!count) return false;

	Creature * pCreature = getSelectedCreature(m_session, true);
	if(!pCreature) return true;

	Player * pPlayer = m_session->GetPlayer();

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		SpellEntry * sp = *aiagent_spells.begin();
		aiagent_spells.erase(aiagent_spells.begin());
		BlueSystemMessage(m_session, "Casting %u, "MSG_COLOR_SUBWHITE"%u remaining.", sp->Id, aiagent_spells.size());

		map<uint32, spell_thingo>::iterator it = aiagent_extra.find(sp->Id);
		ASSERT(it != aiagent_extra.end());

		SpellCastTargets targets;
		if(it->second.type == STYPE_BUFF)
			targets = SetTargets(sp, it->second.type, it->second.type, pCreature, pCreature );
		else
			targets = SetTargets(sp, it->second.type, it->second.type, pPlayer, pCreature );

		pCreature->GetAIInterface()->CastSpell(pCreature, sp, targets);
	}

	if(!aiagent_spells.size())
		RedSystemMessage(m_session, "Finished.");
	/*else
		BlueSystemMessage(m_session, "Got %u remaining.", aiagent_spells.size());*/
	return true;
}

bool ChatHandler::HandleAIAgentDebugSkip(const char * args, WorldSession * m_session)
{
	uint32 count = atoi(args);
	if(!count) return false;

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		aiagent_spells.erase(aiagent_spells.begin());
	}
	BlueSystemMessage(m_session, "Erased %u spells.", count);
	return true;
}

bool ChatHandler::HandleRenameGuildCommand(const char* args, WorldSession *m_session)
{
	Player * plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !args || !strlen(args)) return false;

	Guild * pGuild = objmgr.GetGuild(plr->GetGuildId());
	if(!pGuild) return true; // how the fuck?

	if(objmgr.GetGuildByGuildName(args))
	{
		RedSystemMessage(m_session, "Name already taken!");
		return true;
	}
	sGMLog.writefromsession(m_session, "renamed guild %s to %s", pGuild->GetGuildNameC(), args);
	pGuild->RenameGuild(args);
	GreenSystemMessage(m_session, "Guild renamed!");
	return true;
}

//People seem to get stuck in guilds from time to time. This should be helpfull. -DGM
bool ChatHandler::HandleGuildRemovePlayerCommand(const char* args, WorldSession *m_session)
{
	Player * plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId()) return false;
	bool result = false;

	Guild * pGuild = objmgr.GetGuild(plr->GetGuildId());
	if(!pGuild) return true;

	//To prevent gm's from accidently removing themselves..
	if(plr->GetGUID() == m_session->GetPlayer()->GetGUID())
	{
		RedSystemMessage(m_session, "GM self-protection. You can't force-remove yourself from a guild.");
		return true;
	}

	if( pGuild->GetGuildLeaderGuid() == plr->GetGUID())
	{
		RedSystemMessage(m_session, "Player %s is a Guild Master. Disband the guild instead.",plr->GetName());
		return true;
	}

	plr->SetGuildId(0);
	plr->SetGuildRank(0);
	result = pGuild->DeleteGuildMember(plr->GetGUID());

	if(result)
	{
		WorldPacket data(100);
		data.Initialize(SMSG_GUILD_EVENT);
		data << uint8(GUILD_EVENT_REMOVED);
		data << uint8(2);
		data << plr->GetName();
		data << m_session->GetPlayer()->GetName();
		pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
	}

	RedSystemMessage(plr->GetSession(), "You have been removed from your guild by a GM.");
	RedSystemMessage(m_session, "Player %s was removed from the guild.",plr->GetName());
	sGMLog.writefromsession(m_session, "removed player %s from his guild '%s'",plr->GetName(),pGuild->GetGuildNameC());
	return true;
}

//-DGM
bool ChatHandler::HandleGuildDisbandCommand(const char* args, WorldSession *m_session)
{
	Player * plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId() || !args || !strlen(args)) return false;

	Guild * pGuild = objmgr.GetGuild(plr->GetGuildId());
	if(!pGuild) return true;

	//To prevent gm's from accidentally disbanding their own guild..
	if(plr->GetGUID() == m_session->GetPlayer()->GetGUID())
	{
		RedSystemMessage(m_session, "GM self-protection. You can't disband your own guild with this command.");
		return true;
	}

	pGuild->DeleteGuildMembers();
	pGuild->RemoveFromDb();

	RedSystemMessage(plr->GetSession(), "Your guild was disbanded by a GM. Reason: %s",args);
	RedSystemMessage(m_session, "Guild %s was disbanded. Reason: %s",pGuild->GetGuildNameC(), args);
	sGMLog.writefromsession(m_session, "disbanded guild '%s' Reason: %s",pGuild->GetGuildNameC(),args);
	return true;
}

//-DGM
bool ChatHandler::HandleGuildMembersCommand(const char* args, WorldSession *m_session)
{
	Player * plr = getSelectedChar(m_session);
	if(!plr || !plr->GetGuildId()) return false;

	Guild * pGuild = objmgr.GetGuild(plr->GetGuildId());
	if(!pGuild) return true;

	size_t GuildSize = pGuild->GetGuildMembersCount();
	GreenSystemMessage(m_session, "Listing %u members.", GuildSize);

	std::list<PlayerInfo*>::iterator i;
	size_t Counter=0;
	for (i = pGuild->Begin(); i != pGuild->End();++i) 
	{
		Counter++;
		GreenSystemMessage(m_session, "%u: %s (Rank: %u,%s)",Counter, (*i)->name,(*i)->Rank,pGuild->GetRankName((*i)->Rank).c_str());
	}

	return true;
}

bool ChatHandler::HandleCreateArenaTeamCommands(const char * args, WorldSession * m_session)
{
	uint32 arena_team_type;
	char name[1000];
	uint32 real_type;
	Player * plr = getSelectedChar(m_session, true);
	if(sscanf(args, "%u %s", &arena_team_type, name) != 2)
	{
		SystemMessage(m_session, "Invalid syntax.");
		return true;
	}

	switch(arena_team_type)
	{
	case 2:
		real_type=0;
		break;
	case 3:
		real_type=1;
		break;
	case 5:
		real_type=2;
		break;
	default:
		SystemMessage(m_session, "Invalid arena team type specified.");
		return true;
	}

	if(!plr)
		return true;

	if(plr->m_arenaTeams[real_type] != NULL)
	{
		SystemMessage(m_session, "Already in an arena team of that type.");
		return true;
	}

	ArenaTeam * t = new ArenaTeam(real_type,objmgr.GenerateArenaTeamId());
	t->m_emblemStyle=22;
	t->m_emblemColour=4292133532UL;
	t->m_borderColour=4294931722UL;
	t->m_borderStyle=1;
	t->m_backgroundColour=4284906803UL;
	t->m_leader=plr->GetGUIDLow();
	t->m_name = string(name);
	t->AddMember(plr->m_playerInfo);
	objmgr.AddArenaTeam(t);
	SystemMessage(m_session, "created arena team.");
	return true;
}

bool ChatHandler::HandleWhisperBlockCommand(const char * args, WorldSession * m_session)
{
	if(m_session->GetPlayer()->bGMTagOn)
		return false;

	m_session->GetPlayer()->bGMTagOn = true;
	return true;
}

bool ChatHandler::HandleDispelAllCommand(const char * args, WorldSession * m_session)
{
	uint32 pos=0;
	if(*args)
		pos=atoi(args);

	Player * plr;

	sGMLog.writefromsession(m_session, "used dispelall command, pos %u", pos);

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
			{
				sEventMgr.AddEvent(((Unit*)plr), &Unit::DispelAll, pos?true:false, EVENT_PLAYER_CHECKFORCHEATS, 100, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			}
			else
			{
				plr->DispelAll(pos?true:false);
			}
		}
	}
	sGMLog.writefromsession(m_session, "used mass dispel");
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Dispel action done.");
	return true;
}
