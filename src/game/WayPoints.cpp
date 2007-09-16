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

///////////////////////////////////////////////
//  Waypoint Commands
//

#include "StdAfx.h"

bool ChatHandler::HandleWPAddCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No Selection");
		return true;
	}
	AIInterface* ai = NULL;
	Creature * pCreature = NULL;
	Player* p = m_session->GetPlayer();
	if(p->waypointunit != NULL)
	{
		SystemMessage(m_session, "Using Previous Unit.");
		ai = p->waypointunit;
		if(!ai)
		{
			SystemMessage(m_session, "Invalid Creature, please select another one.");
			return true;
		}

		pCreature = (Creature*)ai->GetUnit();
		if(!pCreature)
		{
			SystemMessage(m_session, "Invalid Creature, please select another one.");
			return true;
		}
	}
	else
	{
		pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(guid);
		if(!pCreature)
		{
			SystemMessage(m_session, "You should select a creature.");
			return true;
		}
		ai = pCreature->GetAIInterface();
	}

	char* pWaitTime = strtok((char*)args, " ");
	uint32 WaitTime = (pWaitTime)? atoi(pWaitTime) : 10000;
	char* pFlags = strtok(NULL, " ");
	uint32 Flags = (pFlags)? atoi(pFlags) : 0 ;
	char* pForwardEmoteId = strtok(NULL, " ");
	uint32 ForwardEmoteId = (pForwardEmoteId)? atoi(pForwardEmoteId) : 0;
	char* pBackwardEmoteId = strtok(NULL, " ");
	uint32 BackwardEmoteId = (pBackwardEmoteId)? atoi(pBackwardEmoteId) : 0;
	char* pForwardSkinId = strtok(NULL, " ");
	uint32 ForwardSkinId = (pForwardSkinId)? atoi(pForwardSkinId) : pCreature->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID);
	char* pBackwardSkinId = strtok(NULL, " ");
	uint32 BackwardSkinId = (pBackwardSkinId)? atoi(pBackwardSkinId) : pCreature->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID);
	char* pForwardEmoteOneShot = strtok(NULL, " ");
	uint32 ForwardEmoteOneShot = (pForwardEmoteOneShot)? atoi(pForwardEmoteOneShot) : 1;
	char* pBackwardEmoteOneShot = strtok(NULL, " ");
	uint32 BackwardEmoteOneShot = (pBackwardEmoteOneShot)? atoi(pBackwardEmoteOneShot) : 1;

	WayPoint* wp = new WayPoint;
	wp->id = ai->GetWayPointsCount()+1;
	wp->x = p->GetPositionX();
	wp->y = p->GetPositionY();
	wp->z = p->GetPositionZ();
	wp->waittime = WaitTime;
	wp->flags = Flags;
	wp->forwardemoteoneshot = ForwardEmoteOneShot;
	wp->forwardemoteid = ForwardEmoteId;
	wp->backwardemoteoneshot = BackwardEmoteOneShot;
	wp->backwardemoteid = BackwardEmoteId;
	wp->forwardskinid = ForwardSkinId;
	wp->backwardskinid = BackwardSkinId;
	ai->addWayPoint(wp);
	ai->saveWayPoints(wp->id);
	if(ai->m_WayPointsShowing == true)
	{
		ai->showWayPoints(wp->id,p,ai->m_WayPointsShowBackwards);
	}

	SystemMessage(m_session, "Waypoint %u added.", wp->id);
	return true;
}

bool ChatHandler::HandleWPMoveTypeCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	uint32 option = atoi((char*)args);

	if (option != 0 && option != 1 && option != 2)
	{
		std::stringstream ss;
		ss << "Incorrect value." << endl;
		ss << "0 is Move from WP 1 ->  10 then 10 -> 1." << endl;
		ss << "1 is Move from WP to a random WP." << endl;
		ss << "2 is Move from WP 1 -> 10 then 1 -> 10." << endl;
		SendMultilineMessage(m_session, ss.str().c_str());
		return true;
	}

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature * pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(guid);
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	char sql[512];
	snprintf(sql, 512, "UPDATE creatures SET moverandom = '%u' WHERE id = '%u'", (unsigned int)option, (unsigned int)GUID_LOPART(guid));
	WorldDatabase.Execute( sql );

	pCreature->GetAIInterface()->setMoveType(option);

	SystemMessage(m_session, "Value saved to database.");
	return true;
}


bool ChatHandler::HandleWPShowCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature * pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(guid);
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a Creature.");
		return true;
	}

	char* pBackwards = strtok((char*)args, " ");
	bool Backwards = (pBackwards)? atoi(pBackwards) : false;

	AIInterface* ai = pCreature->GetAIInterface();
	Player* pPlayer = m_session->GetPlayer();


	if(pPlayer->waypointunit != ai)
	{
		if(ai->m_WayPointsShowing == true) 
		{
			SystemMessage(m_session, "Some one else is also Viewing this Creatures WayPoints.");
			SystemMessage(m_session, "Viewing WayPoints at the same time as some one else can cause undesireble results.");
		}

		if(pPlayer->waypointunit != NULL)
		{
			pPlayer->waypointunit->hideWayPoints(0,pPlayer);
		}
		pPlayer->waypointunit = ai;
		ai->showWayPoints(0,pPlayer,Backwards);
		ai->m_WayPointsShowBackwards = Backwards;
	}
	else
	{
		if(ai->m_WayPointsShowing == true) 
		{
			SystemMessage(m_session, "Waypoints Already Showing.");
		}
	}

	SystemMessage(m_session, "Showing waypoints for creature %u", pCreature->GetSQL_id());
	return true;
}

bool ChatHandler::HandleWPDeleteCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		//Refresh client
		//Hide all
		bool show = ai->m_WayPointsShowing;
		if(show == true)
		{
			ai->hideWayPoints(0,pPlayer);
		}
		ai->deleteWayPoint(wpid, true);
		//Show All again after delete
		if(show == true)
		{
			ai->showWayPoints(0,pPlayer,ai->m_WayPointsShowBackwards);
		}
		SystemMessage(m_session, "Waypoint %u deleted.", wpid);
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPChangeNoCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	std::stringstream ss;

	//get newid
	char* pNewID = strtok((char*)args, " ");
	uint32 NewID = (pNewID)? atoi(pNewID) : 0;

	uint32 wpid = GUID_LOPART(guid);
	if(NewID == wpid) return false;
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()) && (NewID > 0) && (NewID <= ai->GetWayPointsCount()))
	{
		//Refresh client
		//Hide all

		bool show = ai->m_WayPointsShowing;
		if(show == true)
		{
			ai->hideWayPoints(0,pPlayer);
		}

		//update to new id
		ai->changeWayPointID(wpid,NewID);

		//Show All again after update
		if(show == true)
		{
			ai->showWayPoints(0,pPlayer,ai->m_WayPointsShowBackwards);
		}

		ss << "Waypoint " << wpid << " changed to Waypoint " << NewID << ".";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPFlagsCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		uint32 flags = wp->flags;

		char* pNewFlags = strtok((char*)args, " ");
		uint32 NewFlags = (pNewFlags)? atoi(pNewFlags) : 0;

		wp->flags = NewFlags;

		//save wp
		ai->saveWayPoints(wpid);

		ss << "Waypoint " << wpid << " flags changed from " << flags << " to " << NewFlags;
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPMoveHereCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			wp->x = pPlayer->GetPositionX();
			wp->y = pPlayer->GetPositionY();
			wp->z = pPlayer->GetPositionZ();

			//save wp
			ai->saveWayPoints(wpid);
		}
		//Refresh client
		if(ai->m_WayPointsShowing == true)
		{
			ai->hideWayPoints(wpid,pPlayer);
			ai->showWayPoints(wpid,pPlayer,ai->m_WayPointsShowBackwards);
		}

		ss << "Waypoint " << wpid << " has been moved.";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPWaitCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}

	uint32 Wait = 10000;
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pWait = strtok((char*)args, " ");
			Wait = (pWait)? atoi(pWait) : 10000;

			if(Wait < 5000)
			{
				SystemMessage(m_session, "A Wait Time of less then 5000ms can cause lag, consider extending it.");
			}
			wp->waittime = Wait;

			//save wp
			ai->saveWayPoints(wpid);
		}

		ss << "Wait Time for Waypoint " << wpid << " is now " << Wait << "ms.";
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPEmoteCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	uint32 EmoteId = 0;
	bool OneShot = true;
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pBackwards = strtok((char*)args, " ");
			uint32 Backwards = (pBackwards)? atoi(pBackwards) : 0;
			char* pEmoteId = strtok(NULL, " ");
			EmoteId = (pEmoteId)? atoi(pEmoteId) : 0;
			char* pOneShot = strtok(NULL, " ");
			OneShot = (pOneShot)? atoi(pOneShot) : 1;
			if(Backwards)
			{
				wp->backwardemoteid = EmoteId;
				wp->backwardemoteoneshot = OneShot;
			}
			else
			{
				wp->forwardemoteid = EmoteId;
				wp->forwardemoteoneshot = OneShot;
			}

			//save wp
			ai->saveWayPoints(wpid);
		}

		ss << "EmoteID for Waypoint " << wpid << " is now " << EmoteId << " and oneshot is " << ((OneShot == true)? "Enabled." : "Disabled.");
		SystemMessage(m_session,  ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPSkinCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session,  "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session,  "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	uint32 SkinId = 0;
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			char* pBackwards = strtok((char*)args, " ");
			uint32 Backwards = (pBackwards)? atoi(pBackwards) : 0;
			char* pSkinId = strtok(NULL, " ");
			SkinId = (pSkinId)? atoi(pSkinId) : 0;
			if(Backwards)
			{
				wp->backwardskinid = SkinId;
			}
			else
			{
				wp->forwardskinid = SkinId;
			}

			//save wp
			ai->saveWayPoints(wpid);
		}

		ss << "SkinID for Waypoint " << wpid << " is now " << SkinId;
		SystemMessage(m_session, ss.str().c_str());
	}
	else
	{
		SystemMessage(m_session, "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPInfoCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	if(UINT32_LOPART(GUID_HIPART(guid)) != HIGHGUID_WAYPOINT)
	{
		SystemMessage(m_session, "You should select a Waypoint.");
		return true;
	}

	Player* pPlayer = m_session->GetPlayer();
	AIInterface* ai = pPlayer->waypointunit;
	if(!ai || !ai->GetUnit())
	{
		SystemMessage(m_session, "Invalid Creature, please select another one.");
		return true;
	}
	std::stringstream ss;

	uint32 wpid = GUID_LOPART(guid);
	if((wpid > 0) && (wpid <= ai->GetWayPointsCount()))
	{
		WayPoint* wp = ai->getWayPoint(wpid);
		if(wp)
		{
			ss << "Waypoint Number " << wp->id << ":\n";
			ss << "WaitTime: " << wp->waittime << "\n";
			ss << "Flags: " << wp->flags;
			if(wp->flags == 768)
				ss << " (Fly)\n";
			else if(wp->flags == 256)
				ss << " (Run)\n";
			else
				ss << " (Walk)\n";
			ss << "Backwards\n";
			ss << "   emoteid: " << wp->backwardemoteid << "\n";
			ss << "   oneshot: " << ((wp->backwardemoteoneshot == 1)? "Yes" : "No") << "\n";
			ss << "   skinid: " << wp->backwardskinid << "\n";
			ss << "Forwards\n";
			ss << "   emoteid: " << wp->forwardemoteid << "\n";
			ss << "   oneshot: " << ((wp->forwardemoteoneshot == 1)? "Yes" : "No") << "\n";
			ss << "   skinid: " << wp->forwardskinid << "\n";
			SendMultilineMessage(m_session, ss.str().c_str());
		}
	}
	else
	{
	   SystemMessage(m_session,  "Invalid Waypoint.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleWPHideCommand(const char* args, WorldSession *m_session)
{
	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid == 0)
	{
		SystemMessage(m_session, "No selection.");
		return true;
	}

	Creature * pCreature = m_session->GetPlayer()->GetMapMgr()->GetCreature(guid);
	if(!pCreature)
	{
		SystemMessage(m_session, "You should select a Creature.");
		return true;
	}

	AIInterface* ai = pCreature->GetAIInterface();
	Player* pPlayer = m_session->GetPlayer();


	if(pPlayer->waypointunit == ai)
	{
		if(ai->m_WayPointsShowing == true)
		{
			pPlayer->waypointunit->hideWayPoints(0,pPlayer);
			pPlayer->waypointunit = NULL;
		}
	}
	else
	{
		SystemMessage(m_session, "Waypoints for that Unit are not Visible.");
		return true;
	}

	std::stringstream ss;
	ss << "Hiding Waypoints for " << pCreature->GetGUIDLow();
	SystemMessage(m_session, ss.str().c_str());

	return true;
}

bool ChatHandler::HandleGenerateWaypoints(const char* args, WorldSession * m_session)
{
	Creature * cr = 
		m_session->GetPlayer()->GetMapMgr()->GetCreature(m_session->GetPlayer()->GetSelection());
	if(!cr)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}
	if(cr->GetAIInterface()->GetWayPointsCount())//ALREADY HAVE WAYPOINTS
	{	
		SystemMessage(m_session, "The creature already has waypoints");
		return false;
	}
	if(!cr->GetSQL_id())
		return false;
	char* pR = strtok((char*)args, " ");
	if(!pR)
	{
		SystemMessage(m_session, "Randomly generate wps params: range count");
		return true;
	}
	int r = atoi(pR);
	char *pC=strtok(NULL, " ");
	if(!pC)
	{
		SystemMessage(m_session, "Randomly generate wps params: range count");
		return true;
	}
	int n = atol(pC);

	for(int i=0;i<n;i++)
	{
		float ang = rand()/100.0;
		float ran = (rand()%(r*10))/10.0;
		while(ran<1)
		{
			ran = (rand()%(r*10))/10.0;
		}

		float x = cr->GetPositionX()+ran*sin(ang);
		float y = cr->GetPositionY()+ran*cos(ang);
		float z = cr->GetMapMgr()->GetBaseMap()->GetLandHeight(x,y);
		 
		WayPoint* wp = new WayPoint;
		wp->id = cr->GetAIInterface()->GetWayPointsCount()+1;
		wp->x = x;
		wp->y = y;
		wp->z = z;
		wp->waittime = 2000;
		wp->flags = 0;
		wp->forwardemoteoneshot = 0;
		wp->forwardemoteid = 0;
		wp->backwardemoteoneshot = 0;
		wp->backwardemoteid = 0;
		wp->forwardskinid = 0;
		wp->backwardskinid = 0;
		
		cr->GetAIInterface()->addWayPoint(wp);
		cr->GetAIInterface()->showWayPoints(wp->id,m_session->GetPlayer(),cr->GetAIInterface()->m_WayPointsShowBackwards);
	}
	
	return true;
}

bool ChatHandler::HandleSaveWaypoints(const char* args, WorldSession * m_session)
{
	Creature * cr = 
		m_session->GetPlayer()->GetMapMgr()->GetCreature(m_session->GetPlayer()->GetSelection());
	if(!cr)return false;
	if(!cr->GetSQL_id())
		return false;
	
	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE creatureid=%u",cr->GetSQL_id());

	
	for(uint32 x=1;x<=cr->GetAIInterface()->GetWayPointsCount();x++)
	{
		 WayPoint  *w =cr->GetAIInterface()->getWayPoint(x);
		 if(!w)continue;
		 WorldDatabase.Execute(
			 "INSERT INTO creature_waypoints VALUES(%u,%u,%f,%f,%f,%u,%u,%u,%u,%u,%u,%u,%u)",
				
				cr->GetSQL_id(),w->id,w->x,w->y,w->z,w->waittime,w->flags,w->forwardemoteoneshot,
				w->forwardemoteid,w->backwardemoteoneshot,w->backwardemoteid,
				w->forwardskinid,w->backwardskinid);

	}
	return true;
}


bool ChatHandler::HandleDeleteWaypoints(const char* args, WorldSession * m_session)
{
	Creature * cr = 
		m_session->GetPlayer()->GetMapMgr()->GetCreature(m_session->GetPlayer()->GetSelection());
	if(!cr)return false;
	if(!cr->GetSQL_id())
		return false;
	
	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE creatureid=%u",cr->GetSQL_id());

	
	while(cr->GetAIInterface()->GetWayPointsCount())
	{
		cr->GetAIInterface()->deleteWayPoint(1,false);
	}
	return true;
}
