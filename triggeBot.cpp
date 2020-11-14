#include <Windows.h>
#include <iostream>
#include <cmath>
#include "MemoryManger.h"
#include "Offsets.h"

MemoryManger a;

struct variables { // this is a struct to hold important variables
	DWORD localplayer; // local player variable
	DWORD gameModule; // this is the game module (client.dll)
	DWORD activeWeapon;// this is the weapion you are holding
	int myTeam; // this the local players team
	float delay; // this is the delay we will have from when you see them to when you shoot
 	int weaponID; // this is just your weapon ID
}var;


//-----------------------------------------------------------------------------
/*bool areTheyScoped() {
	return a.readMem<DWORD>(b.gameModule + hazedumper::netvars::m_bIsScoped);  // This function simply checks if a entity is in the scope of the weapon
}*/
//-----------------------------------------------------------------------------

//----------------------------
struct vector {
	float x, y, z;                // This is just a struct that will be used to get the distance of the local player to the entity player
};
//---------------------------

//----------------------------------------------------------------------------------------------------------------------
/*void getWeaponID() { //this functions purpose is to get the weaponID of what you are currently holding
	int weapon = a.readMem<int>(b.localplayer + hazedumper::netvars::m_hActiveWeapon);
	int weaponEntity = a.readMem<int>(b.gameModule + hazedumper::signatures::dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);  
	                                                                                                                        
	if (weaponEntity != NULL) {                                                                                            
		var.weaponID = a.readMem<int>(weaponEntity + hazedumper::netvars::m_iItemIDHigh);
		std::cout << var.weaponID << std::endl;
	}
}*/
//-----------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------
void getEntityPosition(DWORD entity) { // This function will get the entity postion
	vector lposition = a.readMem<vector>(var.localplayer + hazedumper::netvars::m_vecOrigin);   
	vector ePosition = a.readMem<vector>(entity + hazedumper::netvars::m_vecOrigin);// this reads from the game memory of where the enemy position is          
	                                                                                // in the local players crosshair
	float distance = sqrtf(pow(lposition.x - ePosition.x, 2) + 
		(lposition.y - ePosition.y, 2) + (lposition.z - ePosition.z)) * 0.256 ; // this gets the distance from the local player to the entity

	var.delay = distance * 3.3; // this sets a delay from when you see the entity player to when you actually shoot

}
//--------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
bool checkIfEnemy() // this is a function to check to see if the entity in the players crosshair is an enemey player
{
	int cross = a.readMem<int>(var.localplayer + hazedumper::netvars::m_iCrosshairId); // this reads from game memory of the local players crosshair ID
	var.myTeam = a.readMem<int>(var.localplayer + hazedumper::netvars::m_iTeamNum); // this assigns the myTeam variabel with the games memory for your team
	if (cross != 0 && cross < 64) // this will loop through every entity in the game (max is 64)
	{
		
		DWORD entity = a.readMem<DWORD>(var.gameModule + hazedumper::signatures::dwEntityList + 
			((cross - 1) * 0x10)); // the reads from memory if a entity is in the players crosshair

		int eTeam = a.readMem<int>(entity + hazedumper::netvars::m_iTeamNum); // this creates the enemy team, adding entities with each iteration
		int eHealth = a.readMem<int>(entity + hazedumper::netvars::m_iHealth); // this reads from memory the entity of the iteration its on health

		if (eTeam != var.myTeam && eHealth > 0) // needs to check if the enemy team is not equal to the players team, and that the enemys health is greater than zero
		{
			//getWeaponID();
			//if (b.weaponID == 9 || b.weaponID == 40 || b.weaponID == 38) {
				//return areTheyScoped();
		//	}

			return true; // for each iteration, if the entity it read is alive, and not equal to a team player, it must mean its a enemy player, so return true
		}
		else
			return false;
	}
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
void shoot() { // force attacks
	a.writeMem<int>(var.gameModule + hazedumper::signatures::dwForceAttack, 5); // writes to memeory to shoot
	Sleep(20);
	a.writeMem<int>(var.gameModule + hazedumper::signatures::dwForceAttack, 4);
}
//---------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
void shootTime() { // checks if enemy is in your crosshair, if it is shoot them 
	if (checkIfEnemy())
		Sleep(var.delay);
		shoot();
}
//------------------------------------------------------------------------------------------


int main() {
	//getWeaponID();
	
	bool startShoot = false;

	int proc = a.getProcID("csgo.exe"); // this gets the process ID of csgo.exe
	var.gameModule = a.getModID(proc, "client.dll"); // this gets the game module which we want to be client.dll
	var.localplayer = a.readMem<DWORD>(var.gameModule + hazedumper::signatures::dwLocalPlayer); // this assigns the local player variable

	while (true) { // infinite while loop so the program will always continously run
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			var.myTeam = a.readMem<int>(var.localplayer + hazedumper::netvars::m_iTeamNum); // forces it to assign the players team
			startShoot = !startShoot; // this way you can turn on and off the triggerbot
		}
			if (startShoot)
				shootTime();
			Sleep(20);
	}

}
