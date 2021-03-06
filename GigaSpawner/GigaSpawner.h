#pragma once
#include "includes.h"

using namespace plugin;

class GigaSpawner {
private:
	static int entityCount;
	static float entityOffset;
	static bool IAEnabled;
	static char msg[255];

	static std::list<int>* specialPedsC, *specialPedsV, *specialPedsB, *specialPedsN, *specialPedsM;
	static std::list<CPed*>* peds;
	static std::list<CVehicle*>* vehicles;

	static void fillPedList(char* strIds, std::list<int>* list);
	
	static CVehicle* spawnVehicle(unsigned int modelIndex, CVector position, float orientation);
	static void spawnVehicle(unsigned int modelIndex, CVector pos);
	static void spawnNPC(CVector pos);
	static void spawnSpecificNPC(int modelID, CVector pos);
	static void spawnLine(unsigned int id);
	static void spawnMatrix(unsigned int id);
	
	static void incrementEntities();
	static void decrementEntities();
	
	static void countPeds();
	static void deletePeds();
	static void countVehicles();
	static void deleteVehicles();

	static void incrementOffset();
	static void decrementOffset();

	static void enableIA();
	static void disableIA();
public:
	GigaSpawner();
} gigaSpawner;

char GigaSpawner::msg[255];
std::list<CPed*>* GigaSpawner::peds;
std::list<CVehicle*>* GigaSpawner::vehicles;
std::list<int>* GigaSpawner::specialPedsC;
std::list<int>* GigaSpawner::specialPedsV;
std::list<int>* GigaSpawner::specialPedsB;
std::list<int>* GigaSpawner::specialPedsN;
std::list<int>* GigaSpawner::specialPedsM;