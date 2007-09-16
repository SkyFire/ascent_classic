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

#ifndef __TRANSPORTERHANDLER_H
#define __TRANSPORTERHANDLER_H

class TransportPath
{
public:
	struct PathNode
	{
		uint32 mapid;
		float x,y,z;
		uint32 actionFlag;
		uint32 delay;
	};

	inline void SetLength(const unsigned int sz)
	{
		i_nodes.resize( sz );
	}

	inline unsigned int Size(void) const { return i_nodes.size(); }
	inline void Resize(unsigned int sz) { i_nodes.resize(sz); }
	inline void Clear(void) { i_nodes.clear(); }
	inline PathNode* GetNodes(void) { return static_cast<PathNode *>(&i_nodes[0]); }
	float GetTotalLength(void)
	{
		float len = 0, xd, yd, zd;
		for(unsigned int idx=1; idx < i_nodes.size(); ++idx)
		{
			xd = i_nodes[ idx ].x - i_nodes[ idx-1 ].x;
			yd = i_nodes[ idx ].y - i_nodes[ idx-1 ].y;
			zd = i_nodes[ idx ].z - i_nodes[ idx-1 ].z;
			len += (float)sqrt( xd * xd + yd*yd + zd*zd );
		}
		return len;
	}

	PathNode& operator[](const unsigned int idx) { return i_nodes[idx]; }
	const PathNode& operator()(const unsigned int idx) const { return i_nodes[idx]; }

protected:
	std::vector<PathNode> i_nodes;
};

struct keyFrame {
	keyFrame(float _x, float _y, float _z, uint32 _mapid, int _actionflag, int _delay)
	{ x = _x; y = _y; z = _z; mapid = _mapid; actionflag = _actionflag; delay = _delay; distFromPrev = -1; distSinceStop = -1; distUntilStop = -1; 
	tFrom = 0; tTo = 0;}

	float x;
	float y;
	float z;
	uint32 mapid;
	int actionflag;
	int delay;
	float distSinceStop;
	float distUntilStop;
	float distFromPrev;
	float tFrom, tTo;
};

struct TWayPoint {
	TWayPoint() : mapid(0), x(0), y(0), z(0), teleport(0) {}
	TWayPoint(uint32 _mapid, float _x, float _y, float _z, bool _teleport) : 
	mapid(_mapid), x(_x), y(_y), z(_z), teleport(_teleport) {}
	uint32 mapid;
	float x;
	float y;
	float z;
	float o;
	bool teleport;
};

typedef std::map<uint32, TWayPoint> WaypointMap;
typedef std::map<uint32, TWayPoint>::iterator WaypointIterator;
typedef std::map<uint64, Player*> PassengerMap;
typedef std::map<uint64, Player*>::iterator PassengerIterator;

bool FillTransporterPathVector(uint32 PathID, TransportPath & Path);

class Transporter : public GameObject
{
public:
	Transporter(uint32 guidlow, uint32 guidhigh);
	~Transporter();

	bool CreateAsTransporter(uint32 EntryID, const char* Name, uint32 Time);
	void UpdatePosition();
	void TransportPassengers(uint32 mapid, uint32 oldmap, float x, float y, float z);

	bool GenerateWaypoints();

	inline void AddPlayer(Player *pPlayer) { mPassengers[pPlayer->GetGUID()] = pPlayer; }
	inline void RemovePlayer(Player *pPlayer) {mPassengers.erase(pPlayer->GetGUID()); }
	inline bool HasPlayer(Player* pPlayer) { return mPassengers.find(pPlayer->GetGUID()) != mPassengers.end(); }
	inline void SetPeriod(uint32 val) { m_period = val; }

	uint32 m_pathTime;
	uint32 m_timer;

	WaypointIterator mCurrentWaypoint;
	WaypointIterator mNextWaypoint;

	void OnPushToWorld();

private:
	
	WaypointMap m_WayPoints;
	PassengerMap mPassengers;

	uint32 m_period;

	WaypointIterator GetNextWaypoint();
};

#endif
