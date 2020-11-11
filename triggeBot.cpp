#include <Windows.h>
#include <iostream>
#include <cmath>
#include "MemoryManger.h"
#include "Offsets.h"

MemoryManger a;

struct variables {
	DWORD localplayer;
	DWORD gameModule;
	DWORD m_ItemIndex;
	DWORD activeWeapon;
	int myTeam;
	float delay;
	int weaponID;
}b;



/*bool areTheyScoped() {
	return a.readMem<DWORD>(b.gameModule + hazedumper::netvars::m_bIsScoped);
}*/

struct vector {
	float x, y, z;
};

/*void getWeaponID() {
	int weapon = a.readMem<int>(b.localplayer + hazedumper::netvars::m_hActiveWeapon);
	int weaponEntity = a.readMem<int>(b.gameModule + hazedumper::signatures::dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);
	if (weaponEntity != NULL) {
		b.weaponID = a.readMem<int>(weaponEntity + hazedumper::netvars::m_iItemIDHigh);
		std::cout << b.weaponID << std::endl;
	}
}*/


void getEntityPosition(DWORD entity) {
	vector lposition = a.readMem<vector>(b.localplayer + hazedumper::netvars::m_vecOrigin);
	vector ePosition = a.readMem<vector>(entity + hazedumper::netvars::m_vecOrigin);

	float distance = sqrtf(pow(lposition.x - ePosition.x, 2) + (lposition.y - ePosition.y, 2) + (lposition.z - ePosition.z)) * 0.256 ;
	b.delay = distance * 3.3;

}

bool checkIfEnemy()
{
	int cross = a.readMem<int>(b.localplayer + hazedumper::netvars::m_iCrosshairId);
	b.myTeam = a.readMem<int>(b.localplayer + hazedumper::netvars::m_iTeamNum);
	if (cross != 0 && cross < 64) 
	{
		
		DWORD entity = a.readMem<DWORD>(b.gameModule + hazedumper::signatures::dwEntityList + ((cross - 1) * 0x10));
		int eTeam = a.readMem<int>(entity + hazedumper::netvars::m_iTeamNum);
		int eHealth = a.readMem<int>(entity + hazedumper::netvars::m_iHealth);
		if (eTeam != b.myTeam && eHealth > 0)
		{
			//getWeaponID();
			//if (b.weaponID == 9 || b.weaponID == 40 || b.weaponID == 38) {
				//return areTheyScoped();
		//	}

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void shoot() { // force attacks
	a.writeMem<int>(b.gameModule + hazedumper::signatures::dwForceAttack, 5);
	Sleep(20);
	a.writeMem<int>(b.gameModule + hazedumper::signatures::dwForceAttack, 4);
}

void yesShootyTime() { // checks if enemy is in your crosshair, if it is shoot them 
	if (checkIfEnemy())
		shoot();
}



int main() {
	//getWeaponID();
	
	bool startShoot = false;
	int proc = a.getProcID("csgo.exe");
	b.gameModule = a.getModID(proc, "client.dll");
	b.localplayer = a.readMem<DWORD>(b.gameModule + hazedumper::signatures::dwLocalPlayer);

	while (true) {
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			b.myTeam = a.readMem<int>(b.localplayer + hazedumper::netvars::m_iTeamNum);
			startShoot = !startShoot;
		}
			if (startShoot)
				yesShootyTime();
			Sleep(20);
	}

}
	
