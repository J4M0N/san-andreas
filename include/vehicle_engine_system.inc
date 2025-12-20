/*
    Sistema de Motor de Vehículos
    
    Descripción:
    - Los vehículos requieren llave equipada para encender
    - Sistema de motor ON/OFF
    - Verificación de propiedad del vehículo
*/

#if defined _vehicle_engine_system_included
    #endinput
#endif
#define _vehicle_engine_system_included

// Constantes
#define MAX_VEHICLES_ENGINE 2000

// Variables globales
new VehicleEngine[MAX_VEHICLES_ENGINE]; // 0 = apagado, 1 = encendido
new VehicleDBID[MAX_VEHICLES_ENGINE]; // ID del vehículo en la base de datos
new bool:PlayerStartingEngine[MAX_PLAYERS]; // Estado de encendido en progreso
new PlayerStartingVehicle[MAX_PLAYERS]; // Vehículo que está encendiendo

// Funciones principales
stock GetVehicleDBID(vehicleid)
{
    if(vehicleid < 1 || vehicleid >= MAX_VEHICLES_ENGINE) return 0;
    return VehicleDBID[vehicleid];
}

stock SetVehicleDBID(vehicleid, dbid)
{
    if(vehicleid < 1 || vehicleid >= MAX_VEHICLES_ENGINE) return 0;
    VehicleDBID[vehicleid] = dbid;
    return 1;
}

stock GetVehicleEngineState(vehicleid)
{
    if(vehicleid < 1 || vehicleid >= MAX_VEHICLES_ENGINE) return 0;
    return VehicleEngine[vehicleid];
}

stock SetVehicleEngineState(vehicleid, enginestate)
{
    if(vehicleid < 1 || vehicleid >= MAX_VEHICLES_ENGINE) return 0;
    VehicleEngine[vehicleid] = enginestate;
    
    new v_engine, v_lights, v_alarm, v_doors, v_bonnet, v_boot, v_objective;
    GetVehicleParamsEx(vehicleid, v_engine, v_lights, v_alarm, v_doors, v_bonnet, v_boot, v_objective);
    SetVehicleParamsEx(vehicleid, enginestate, v_lights, v_alarm, v_doors, v_bonnet, v_boot, v_objective);
    
    return 1;
}

stock ToggleVehicleEngine(playerid, vehicleid)
{
    // Verificar que el jugador esté en el vehículo
    if(GetPlayerVehicleID(playerid) != vehicleid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}Debes estar dentro del vehículo.");
        return 0;
    }
    
    // Verificar que sea el conductor
    if(GetPlayerVehicleSeat(playerid) != 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}Debes ser el conductor.");
        return 0;
    }
    
    // Obtener ID del vehículo en la base de datos
    new dbVehicleID = GetVehicleDBID(vehicleid);
    if(dbVehicleID == 0)
    {
        // Vehículo sin registro (vehículos del servidor)
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}Este vehículo no requiere llave.");
        return 0;
    }
    
    // Verificar que el jugador tenga la llave equipada en cualquier mano
    new bool:hasKey = false;
    new heldSlotRight = PlayerHeldItemSlotRight[playerid];
    new heldSlotLeft = PlayerHeldItemSlotLeft[playerid];
    
    printf("[DEBUG] Motor - Player: %d, VehicleID: %d, DBID: %d", playerid, vehicleid, dbVehicleID);
    printf("[DEBUG] SlotRight: %d, SlotLeft: %d", heldSlotRight, heldSlotLeft);
    
    // Revisar mano derecha
    if(heldSlotRight != -1)
    {
        new itemID = PlayerInventory[playerid][heldSlotRight][invItemID];
        printf("[DEBUG] Mano derecha - ItemID: %d (Master=%d, Normal=%d)", itemID, ITEM_VEHICLE_KEY_MASTER, ITEM_VEHICLE_KEY);
        
        if(itemID == ITEM_VEHICLE_KEY_MASTER || itemID == ITEM_VEHICLE_KEY)
        {
            new metadata[128];
            format(metadata, sizeof(metadata), "%s", PlayerInventory[playerid][heldSlotRight][invMetadata]);
            printf("[DEBUG] Metadata: %s", metadata);
            
            // Parsear manualmente el metadata
            new keyVehicleID = -1;
            new pos = strfind(metadata, "vehicleid:");
            if(pos != -1)
            {
                keyVehicleID = strval(metadata[pos + 10]); // "vehicleid:" = 10 caracteres
                printf("[DEBUG] Parse manual OK - KeyVehicleID: %d", keyVehicleID);
                
                if(keyVehicleID == dbVehicleID)
                {
                    hasKey = true;
                    printf("[DEBUG] Llave valida!");
                }
            }
            else
            {
                printf("[DEBUG] No se encontro 'vehicleid:' en metadata");
            }
        }
    }
    
    // Revisar mano izquierda
    if(!hasKey && heldSlotLeft != -1)
    {
        new itemID = PlayerInventory[playerid][heldSlotLeft][invItemID];
        printf("[DEBUG] Mano izquierda - ItemID: %d", itemID);
        
        if(itemID == ITEM_VEHICLE_KEY_MASTER || itemID == ITEM_VEHICLE_KEY)
        {
            new metadata[128];
            format(metadata, sizeof(metadata), "%s", PlayerInventory[playerid][heldSlotLeft][invMetadata]);
            printf("[DEBUG] Metadata izq: %s", metadata);
            
            // Parsear manualmente el metadata
            new keyVehicleID = -1;
            new pos = strfind(metadata, "vehicleid:");
            if(pos != -1)
            {
                keyVehicleID = strval(metadata[pos + 10]);
                printf("[DEBUG] Parse manual izq OK - KeyVehicleID: %d", keyVehicleID);
                
                if(keyVehicleID == dbVehicleID)
                {
                    hasKey = true;
                    printf("[DEBUG] Llave valida en mano izquierda!");
                }
            }
        }
    }
    
    printf("[DEBUG] HasKey final: %d", hasKey);
    
    if(!hasKey)
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}Necesitas la llave de este vehiculo en tu mano.");
        SendClientMessage(playerid, COLOR_INFO, "{FFFF00}Tip: {FFFFFF}Usa /inventario y selecciona la llave para equiparla.");
        return 0;
    }
    
    // Verificar si ya está encendiendo
    if(PlayerStartingEngine[playerid])
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Ya estas encendiendo el motor...");
        return 0;
    }
    
    // Verificar estado actual
    new currentState = GetVehicleEngineState(vehicleid);
    
    // Si quiere apagar, apagar inmediatamente
    if(currentState)
    {
        SetVehicleEngineState(vehicleid, 0);
        SendClientMessage(playerid, COLOR_SUCCESS, "{FF0000}Motor apagado. {FFFFFF}Usa /motor para encender.");
        GameTextForPlayer(playerid, "~r~Motor: OFF", 3000, 3);
        PlayerPlaySound(playerid, 1146, 0.0, 0.0, 0.0);
        
        printf("[Engine] %s apago el motor del vehiculo %d (DBID: %d)",
            CharacterData[playerid][cName], vehicleid, dbVehicleID);
        return 1;
    }
    
    // Iniciar proceso de encendido (2 segundos)
    PlayerStartingEngine[playerid] = true;
    PlayerStartingVehicle[playerid] = vehicleid;
    
    GameTextForPlayer(playerid, "~y~Encendiendo...", 2000, 3);
    SendClientMessage(playerid, COLOR_INFO, "{FFFF00}Encendiendo motor...");
    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0); // Sonido de encendido
    
    // Timer para encender después de 2 segundos
    SetTimerEx("OnVehicleEngineStart", 2000, false, "ddd", playerid, vehicleid, dbVehicleID);
    
    return 1;
}

// Callback cuando termina el cooldown de encendido
forward OnVehicleEngineStart(playerid, vehicleid, dbVehicleID);
public OnVehicleEngineStart(playerid, vehicleid, dbVehicleID)
{
    // Verificar que el jugador siga en el vehículo
    if(GetPlayerVehicleID(playerid) != vehicleid)
    {
        PlayerStartingEngine[playerid] = false;
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Has salido del vehiculo.");
        return 0;
    }
    
    // Verificar que siga siendo el conductor
    if(GetPlayerVehicleSeat(playerid) != 0)
    {
        PlayerStartingEngine[playerid] = false;
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Ya no eres el conductor.");
        return 0;
    }
    
    // Encender motor
    SetVehicleEngineState(vehicleid, 1);
    PlayerStartingEngine[playerid] = false;
    
    SendClientMessage(playerid, COLOR_SUCCESS, "{00FF00}Motor encendido. {FFFFFF}Usa /motor para apagar.");
    GameTextForPlayer(playerid, "~g~Motor: ON", 3000, 3);
    PlayerPlaySound(playerid, 1145, 0.0, 0.0, 0.0);
    
    printf("[Engine] %s encendio el motor del vehiculo %d (DBID: %d)",
        CharacterData[playerid][cName], vehicleid, dbVehicleID);
    
    return 1;
}

// Comando: /motor
CMD:motor(playerid, params[])
{
    if(!CharacterData[playerid][cID])
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}No has seleccionado un personaje.");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Error: {FFFFFF}No estás en un vehículo.");
        return 1;
    }
    
    ToggleVehicleEngine(playerid, vehicleid);
    return 1;
}

// Alias
CMD:engine(playerid, params[])
{
    return cmd_motor(playerid, params);
}

CMD:m(playerid, params[])
{
    return cmd_motor(playerid, params);
}

// Hook: OnPlayerEnterVehicle - Apagar motor si no tiene llave
public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
    if(!ispassenger)
    {
        new dbVehicleID = GetVehicleDBID(vehicleid);
        if(dbVehicleID > 0)
        {
            // Apagar motor al entrar (requiere encenderlo manualmente)
            SetVehicleEngineState(vehicleid, 0);
        }
    }
    
    #if defined Engine_OnPlayerEnterVehicle
        Engine_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
    #endif
    return 1;
}

#if defined _ALS_OnPlayerEnterVehicle
    #undef OnPlayerEnterVehicle
#else
    #define _ALS_OnPlayerEnterVehicle
#endif
#define OnPlayerEnterVehicle Engine_OnPlayerEnterVehicle
#if defined Engine_OnPlayerEnterVehicle
    forward Engine_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
#endif

// Hook: OnVehicleSpawn - Apagar motor al spawnear vehículo
public OnVehicleSpawn(vehicleid)
{
    SetVehicleEngineState(vehicleid, 0);
    
    #if defined Engine_OnVehicleSpawn
        Engine_OnVehicleSpawn(vehicleid);
    #endif
    return 1;
}

#if defined _ALS_OnVehicleSpawn
    #undef OnVehicleSpawn
#else
    #define _ALS_OnVehicleSpawn
#endif
#define OnVehicleSpawn Engine_OnVehicleSpawn
#if defined Engine_OnVehicleSpawn
    forward Engine_OnVehicleSpawn(vehicleid);
#endif
