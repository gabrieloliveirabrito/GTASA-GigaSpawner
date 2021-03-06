#include "GigaSpawner.h"

using namespace plugin;

int pedModelIds[];
//int vehModelIds[];
//const char* vehNames[];

const float playerOffset = 2.5f;
int GigaSpawner::entityCount = 3;
float GigaSpawner::entityOffset = 1.0f;
bool GigaSpawner::IAEnabled = false;

GigaSpawner::GigaSpawner() {
    static int keyPressTime = 0;
    strcpy(GigaSpawner::msg, "GigaSpawner");

    GigaSpawner::peds = new std::list<CPed*>();
    GigaSpawner::vehicles = new std::list<CVehicle*>();

    GigaSpawner::specialPedsC = new std::list<int>();
    GigaSpawner::specialPedsV = new std::list<int>();
    GigaSpawner::specialPedsB = new std::list<int>();
    GigaSpawner::specialPedsN = new std::list<int>();
	GigaSpawner::specialPedsM = new std::list<int>();

    //LPCTSTR cfg_IniName = _T(".\\scripts\\GigaSpawner.ini");
    //CIniReader config(cfg_IniName);

    TCHAR sAbsolutePathBuffer[2048] = _T("");
    if (GetFullPathName(TEXT(".\\GigaSpawner.ini"), sizeof(sAbsolutePathBuffer) / sizeof(TCHAR), sAbsolutePathBuffer, NULL))
    {
        CIniReader config(sAbsolutePathBuffer);
        GigaSpawner::entityCount = config.ReadInteger("Settings", "InitialEntityCount", 3);
        GigaSpawner::entityOffset = config.ReadFloat("Settings", "InitialEntityOFfset", 1.5f);
        GigaSpawner::IAEnabled = config.ReadBoolean("Settings", "IAEnabled", false);

        char* specialPedsCstr = config.ReadString("SpecialPeds", "C", "");
        GigaSpawner::fillPedList(specialPedsCstr, GigaSpawner::specialPedsC);

        char* specialPedsVstr = config.ReadString("SpecialPeds", "V", "");
        GigaSpawner::fillPedList(specialPedsVstr, GigaSpawner::specialPedsV);

        char* specialPedsBstr = config.ReadString("SpecialPeds", "B", "");
        GigaSpawner::fillPedList(specialPedsBstr, GigaSpawner::specialPedsB);

        char* specialPedsNstr = config.ReadString("SpecialPeds", "N", "");
        GigaSpawner::fillPedList(specialPedsNstr, GigaSpawner::specialPedsN);

        char* specialPedsMstr = config.ReadString("SpecialPeds", "M", "");
        GigaSpawner::fillPedList(specialPedsMstr, GigaSpawner::specialPedsM);
    }
    Events::gameProcessEvent += [] {
        CPlayerPed* player = FindPlayerPed();

        if (player && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
#ifdef GTASA
            keyPressTime = CTimer::m_snTimeInMilliseconds;

            if (KeyPressed(0x31) || KeyPressed(VK_RSHIFT) || KeyPressed(VK_OEM_2)) { //1
                if (KeyPressed(0x5A)) { //Z
                    GigaSpawner::spawnLine(0);
                }
                else if (KeyPressed(0x58)) { //X
                    GigaSpawner::spawnMatrix(0);
                }
                else if (KeyPressed(0x43)) { //C
                    GigaSpawner::spawnLine(1);
                }
                else if (KeyPressed(0x56)) { //V
                    GigaSpawner::spawnLine(2);
                }
                else if (KeyPressed(0x42)) { //B
                    GigaSpawner::spawnLine(3);
                }
                else if (KeyPressed(0x4E)) { //N
                    GigaSpawner::spawnLine(4);
                }
                else if (KeyPressed(0x4D)) { //M
                    GigaSpawner::spawnLine(5);
                }
                else if (KeyPressed(VK_SPACE)) {
                    std::list<CPed*>::iterator it;
                    for (it = GigaSpawner::peds->begin(); it != GigaSpawner::peds->end(); ++it) {
                        CPed* ped = *it;
                        if (ped->IsAlive()) {
                            ped->SetLook(player);
                            ped->Look();

                            ped->m_pIntelligence->m_TaskMgr.SetTaskSecondary(new CTaskComplexPlayHandSignalAnim(320 + rand() % 4, 10.0f), 4);
                            ped->EnablePedSpeech();
                        }
                    }
                }
            }
            else if (KeyPressed(0x32)) { //2
                if (KeyPressed(0x5A)) { //Z
                    GigaSpawner::incrementEntities();
                }
                else if (KeyPressed(0x58)) { //X
                    GigaSpawner::decrementEntities();
                }
                else if (KeyPressed(0x43)) { //C
                    GigaSpawner::spawnMatrix(1);
                }
                else if (KeyPressed(0x56)) { //V
                    GigaSpawner::spawnMatrix(2);
                }
                else if (KeyPressed(0x42)) { //B
                    GigaSpawner::spawnMatrix(3);
                }
                else if (KeyPressed(0x4E)) { //N
                    GigaSpawner::spawnMatrix(4);
                }
                else if (KeyPressed(0x4D)) { //M
                    GigaSpawner::spawnMatrix(5);
                }
                else if (KeyPressed(VK_SPACE)) {
                    std::list<CPed*>::iterator it;
                    for (it = GigaSpawner::peds->begin(); it != GigaSpawner::peds->end(); ++it) {
                        CPed* ped = *it;
                        if (ped->IsAlive()) {
                            ped->SetLook(player);
                            ped->Look();

                            ped->SetPedState(ePedState::PEDSTATE_AI_CONTROL);
                            ped->m_pIntelligence->m_TaskMgr.SetTask(new CTaskSimpleUseGun(player, player->GetPosition(), COMMAND_TASK_AIM_GUN_AT_CHAR), 3, false);
                        }
                    }
                }
            }
            else if (KeyPressed(0x33)) { //3
                if (KeyPressed(0x5A)) { //Z
                    GigaSpawner::countPeds();
                }
                else if (KeyPressed(0x58)) { //X
                    GigaSpawner::deletePeds();
                }
                else if (KeyPressed(0x43)) { //C
                    CVector pos = player->GetPosition();
                    
                    CWorld::ClearPedsFromArea(pos.x - 50.0f, pos.y - 50.0f, pos.z - 50.0f, pos.x + 50.0f, pos.y + 50.0f, pos.z + 50.0f);
                    CHud::SetHelpMessage("GigaSpawner - Cleared all vehicles in 100 meters radius!", true, false, false);
                }
                else if (KeyPressed(VK_SPACE)) {
                    std::list<CPed*>::iterator it;
                    for (it = GigaSpawner::peds->begin(); it != GigaSpawner::peds->end(); ++it) {
                        CPed* ped = *it;
                        if (ped->IsAlive()) {
                            ped->SetLook(player);
                            ped->Look();

                            ped->m_pIntelligence->m_TaskMgr.SetTask(new CTaskComplexJump(0), 3, false);
                            ped->SetPedState(ePedState::PEDSTATE_JUMP);
                        }
                    }
                }
            }
            else if (KeyPressed(0x34)) { //4
                if (KeyPressed(0x5A)) { //Z
                    GigaSpawner::incrementOffset();
                }
                else if (KeyPressed(0x58)) { //X
                    GigaSpawner::decrementOffset();
                }
            }
            else if (KeyPressed(0x35)) { //5
                if (KeyPressed(0x5A)) { //Z
                    GigaSpawner::enableIA();
                }
                else if (KeyPressed(0x58)) { //X
                    GigaSpawner::disableIA();
                }
            }
            else if (KeyPressed(0x36)) { //6
                if (KeyPressed(0x4A)) {
                    CCheat::JetpackCheat();
                }
        }
#else
            CHud::SetHelpMessage(L"Not done yet!", true, false);
#endif
        }
    };
}

void GigaSpawner::fillPedList(char* strIds, std::list<int>* list) {
    std::stringstream ss(strIds);

    for (int i; ss >> i;) {
        list->push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
}

CVehicle* GigaSpawner::spawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
    /*unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelIndex].m_nFlags;
    CStreaming::RequestModel(modelIndex, GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) {
        if (!(oldFlags & GAME_REQUIRED)) {
            CStreaming::SetModelIsDeletable(modelIndex);
            CStreaming::SetModelTxdIsDeletable(modelIndex);
        }

        CVehicle* vehicle = nullptr;
        switch (reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType) {
        case VEHICLE_MTRUCK:
            vehicle = new CMonsterTruck(modelIndex, 1);
            break;
        case VEHICLE_QUAD:
            vehicle = new CQuadBike(modelIndex, 1);
            break;
        case VEHICLE_HELI:
            vehicle = new CHeli(modelIndex, 1);
            break;
        case VEHICLE_PLANE:
            vehicle = new CPlane(modelIndex, 1);
            break;
        case VEHICLE_BIKE:
            vehicle = new CBike(modelIndex, 1);
            reinterpret_cast<CBike*>(vehicle)->m_nDamageFlags |= 0x10;
            break;
        case VEHICLE_BMX:
            vehicle = new CBmx(modelIndex, 1);
            reinterpret_cast<CBmx*>(vehicle)->m_nDamageFlags |= 0x10;
            break;
        case VEHICLE_TRAILER:
            vehicle = new CTrailer(modelIndex, 1);
            break;
        case VEHICLE_BOAT:
            vehicle = new CBoat(modelIndex, 1);
            break;
        default:
            vehicle = new CAutomobile(modelIndex, 1, true);
            break;
        }
        if (vehicle) {
            vehicle->SetOrientation(0.0f, 0.0f, orientation);
            vehicle->m_nStatus = 4;
            vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
            CWorld::Add(vehicle);
            CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
            if (vehicle->m_nVehicleClass == VEHICLE_BIKE)
                reinterpret_cast<CBike*>(vehicle)->PlaceOnRoadProperly();
            else if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
                reinterpret_cast<CAutomobile*>(vehicle)->PlaceOnRoadProperly();

            sprintf(GigaSpawner::msg, "GigaSpawner - Spawned vehicle %s", vehNames[modelIndex - 400]);
            CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
            return vehicle;
        }
    }*/
    return nullptr;
}

void GigaSpawner::spawnVehicle(unsigned int modelIndex, CVector pos) {
    CPlayerPed* player = FindPlayerPed(-1);
    float orientation = player->m_fCurrentRotation + 1.5707964f;

    CVehicle* vehicle = GigaSpawner::spawnVehicle(modelIndex, player->TransformFromObjectSpace(pos), orientation);
    GigaSpawner::vehicles->insert(GigaSpawner::vehicles->end(), vehicle);
}

void GigaSpawner::spawnNPC(CVector pos) {
    int modelID = pedModelIds[rand() % 250];
    GigaSpawner::spawnSpecificNPC(modelID, pos);
}

void GigaSpawner::spawnSpecificNPC(int modelID, CVector pos)
{
    CStreaming::RequestModel(modelID, 0);
    CStreaming::LoadAllRequestedModels(false);
    CPed* ped = new CCivilianPed(CPopulation::IsFemale(modelID) ? PED_TYPE_CIVFEMALE : PED_TYPE_CIVMALE, modelID);
    if (ped) {
        ped->SetPosn(FindPlayerPed()->TransformFromObjectSpace(pos));
        ped->SetOrientation(0.0f, 0.0f, 0.0f);
        CWorld::Add(ped);

        ped->PositionAnyPedOutOfCollision();
        ped->UpdatePosition();
        ped->GiveDelayedWeapon(eWeaponType::WEAPON_PISTOL_SILENCED, 500);
        ped->SetCurrentWeapon(ped->GetWeaponSlot(eWeaponType::WEAPON_PISTOL_SILENCED));
        ped->m_nMoneyCount = 1000;

        if (GigaSpawner::IAEnabled)
            ped->m_pIntelligence->m_TaskMgr.SetTask(new CTaskComplexWanderStandard(4, rand() % 8, true), 4, false);
    }

    GigaSpawner::peds->insert(GigaSpawner::peds->end(), ped);
}

void GigaSpawner::spawnLine(unsigned int id) {
    for (int i = 0; i < GigaSpawner::entityCount; i++) {
        CVector pos = CVector(playerOffset, playerOffset + GigaSpawner::entityOffset * (i + 1), 1.0f);
        std::list<int>::const_iterator it;

        switch (id) {
        case 0:
            GigaSpawner::spawnNPC(pos);
            break;
        case 1:
            if (GigaSpawner::specialPedsC->size() > 0) {
                it = specialPedsC->begin();
                std::advance(it, rand() % GigaSpawner::specialPedsC->size());

                GigaSpawner::spawnSpecificNPC(*it, pos);
            }
            else {
                CHud::SetHelpMessage("GigaSpawner - No SpecialPedsC on INI file!!", true, false, false);
                return;
            }
            break;
        case 2:
            if (GigaSpawner::specialPedsV->size() > 0) {
                it = specialPedsV->begin();
                std::advance(it, rand() % GigaSpawner::specialPedsV->size());

                GigaSpawner::spawnSpecificNPC(*it, pos);
            }
            else {
                CHud::SetHelpMessage("GigaSpawner - No SpecialPedsV on INI file!!", true, false, false);
                return;
            }
            break;
        case 3:
            if (GigaSpawner::specialPedsB->size() > 0) {
                it = specialPedsB->begin();
                std::advance(it, rand() % GigaSpawner::specialPedsB->size());

                GigaSpawner::spawnSpecificNPC(*it, pos);
            }
            else {
                CHud::SetHelpMessage("GigaSpawner - No SpecialPedsB on INI file!!", true, false, false);
                return;
            }
            break;
        case 4:
            if (GigaSpawner::specialPedsN->size() > 0) {
                it = specialPedsN->begin();
                std::advance(it, rand() % GigaSpawner::specialPedsN->size());

                GigaSpawner::spawnSpecificNPC(*it, pos);
            }
            else {
                CHud::SetHelpMessage("GigaSpawner - No SpecialPedsN on INI file!!", true, false, false);
                return;
            }
            break;
        case 5:
            if (GigaSpawner::specialPedsM->size() > 0) {
                it = specialPedsM->begin();
                std::advance(it, rand() % GigaSpawner::specialPedsM->size());

                GigaSpawner::spawnSpecificNPC(*it, pos);
            }
            else {
                CHud::SetHelpMessage("GigaSpawner - No SpecialPedsM on INI file!!", true, false, false);
                return;
            }
            break;
        }
    }

    sprintf(GigaSpawner::msg, "GigaSpawner - Spawned %d NPCs - Total of %d", GigaSpawner::entityCount, GigaSpawner::peds->size());
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::spawnMatrix(unsigned int id) {
    for (int x = 0; x < GigaSpawner::entityCount; x++) {
        for (int y = 0; y < GigaSpawner::entityCount; y++) {
            CVector pos = CVector(playerOffset + GigaSpawner::entityOffset * (y + 1), playerOffset + GigaSpawner::entityOffset * (x + 1), 1.0f);
            std::list<int>::const_iterator it;

            switch (id) {
            case 0:
                GigaSpawner::spawnNPC(pos);
                break;
            case 1:
                if (GigaSpawner::specialPedsC->size() > 0) {
                    it = specialPedsC->begin();
                    std::advance(it, rand() % GigaSpawner::specialPedsC->size());

                    GigaSpawner::spawnSpecificNPC(*it, pos);
                }
                else {
                    CHud::SetHelpMessage("GigaSpawner - No SpecialPedsC on INI file!!", true, false, false);
                    return;
                }
                break;
            case 2:
                if (GigaSpawner::specialPedsV->size() > 0) {
                    it = specialPedsV->begin();
                    std::advance(it, rand() % GigaSpawner::specialPedsV->size());

                    GigaSpawner::spawnSpecificNPC(*it, pos);
                }
                else {
                    CHud::SetHelpMessage("GigaSpawner - No SpecialPedsV on INI file!!", true, false, false);
                    return;
                }
                break;
            case 3:
                if (GigaSpawner::specialPedsB->size() > 0) {
                    it = specialPedsB->begin();
                    std::advance(it, rand() % GigaSpawner::specialPedsB->size());

                    GigaSpawner::spawnSpecificNPC(*it, pos);
                }
                else {
                    CHud::SetHelpMessage("GigaSpawner - No SpecialPedsB on INI file!!", true, false, false);
                    return;
                }
                break;
            case 4:
                if (GigaSpawner::specialPedsN->size() > 0) {
                    it = specialPedsN->begin();
                    std::advance(it, rand() % GigaSpawner::specialPedsN->size());

                    GigaSpawner::spawnSpecificNPC(*it, pos);
                }
                else {
                    CHud::SetHelpMessage("GigaSpawner - No SpecialPedsN on INI file!!", true, false, false);
                    return;
                }
                break;
            case 5:
                if (GigaSpawner::specialPedsM->size() > 0) {
                    it = specialPedsM->begin();
                    std::advance(it, rand() % GigaSpawner::specialPedsM->size());

                    GigaSpawner::spawnSpecificNPC(*it, pos);
                }
                else {
                    CHud::SetHelpMessage("GigaSpawner - No SpecialPedsM on INI file!!", true, false, false);
                    return;
                }
                break;
            }
        }
    }

    sprintf(GigaSpawner::msg, "GigaSpawner - Spawned %d NPCs - Total of %d", GigaSpawner::entityCount * GigaSpawner::entityCount, GigaSpawner::peds->size());
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::incrementEntities() {
    GigaSpawner::entityCount++;

    sprintf(GigaSpawner::msg, "GigaSpawner - Spawn NPC Count = %d", GigaSpawner::entityCount);
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::decrementEntities() {
    if (GigaSpawner::entityCount > 1) {
        GigaSpawner::entityCount--;

        sprintf(GigaSpawner::msg, "GigaSpawner - Spawn NPC Count = %d", GigaSpawner::entityCount);
        CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
    }
    else {
        CHud::SetHelpMessage("GigaSpawner - Minimum count (1) reached", true, false, false);
    }
}

void GigaSpawner::countPeds() {
    int count = GigaSpawner::peds->size();

    sprintf(GigaSpawner::msg, "GigaSpawner - Spawned NPCS = %d", count);
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::deletePeds() {
    CVector pos = FindPlayerPed(-1)->GetPosition();
    CWorld::ClearPedsFromArea(pos.x - 50.0f, pos.y - 50.0f, pos.z - 50.0f, pos.x + 50.0f, pos.y + 50.0f, pos.z + 50.0f);

    GigaSpawner::peds->clear();
    CHud::SetHelpMessage("GigaSpawner - Cleared all peds", true, false, false);
}

void GigaSpawner::countVehicles() {
    int count = GigaSpawner::vehicles->size();

    sprintf(GigaSpawner::msg, "GigaSpawner - Spawned Vehicles = %d", count);
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::deleteVehicles() {
    std::list<CVehicle*>::iterator it;
    for (it = GigaSpawner::vehicles->begin(); it != GigaSpawner::vehicles->end(); ++it) {
        CVehicle* vehicle = *it;
        CWorld::Remove(vehicle);

        delete vehicle;
    }

    GigaSpawner::vehicles->clear();
    CHud::SetHelpMessage("GigaSpawner - Cleared all vehicles", true, false, false);
}

void GigaSpawner::incrementOffset() {
    FindPlayerPed()->m_pIntelligence->m_TaskMgr.SetTaskSecondary(new CTaskComplexPlayHandSignalAnim(320, 4.0f), 4);

    GigaSpawner::entityOffset += 0.5f;
    sprintf(GigaSpawner::msg, "GigaSpawner - Spawn NPC Offset = %.2f", GigaSpawner::entityOffset);
    CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
}

void GigaSpawner::decrementOffset() {
    if (GigaSpawner::entityOffset > 0.5f) {
        GigaSpawner::entityOffset -= 0.5f;
        sprintf(GigaSpawner::msg, "GigaSpawner - Spawn NPC Offset = %.2f", GigaSpawner::entityOffset);
        CHud::SetHelpMessage(GigaSpawner::msg, true, false, false);
    }
    else {
        CHud::SetHelpMessage("GigaSpawner - Minimum offset (0.5) reached", true, false, false);
    }
}

void GigaSpawner::enableIA() {
    GigaSpawner::IAEnabled = true;
    CHud::SetHelpMessage("GigaSpawner - IA enabled, NPCs can walk after spawn", true, false, false);
}

void GigaSpawner::disableIA() {
    GigaSpawner::IAEnabled = false;
    CHud::SetHelpMessage("GigaSpawner - IA disabled, NPCs can't walk but run if CJ shot", true, false, false);
}