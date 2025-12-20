/*
    Sistema de Persistencia de Vehiculos
    
    - Guarda la posicion del vehiculo al salir o desconectar
    - Carga vehiculos spawneados al conectarse
    - No duplica vehiculos si ya estan spawneados
*/

#if defined _vehicle_persistence_included
    #endinput
#endif
#define _vehicle_persistence_included

// Inicializar sistema
stock InitVehiclePersistence()
{
    printf("[Vehicle Persistence] Sistema iniciado");
}

// Guardar todas las posiciones de vehículos spawneados
forward SaveAllVehiclePositions();
public SaveAllVehiclePositions()
{
    new query[256], Float:x, Float:y, Float:z, Float:a;
    new count = 0;
    
    for(new vehicleid = 1; vehicleid < MAX_VEHICLES_ENGINE; vehicleid++)
    {
        new dbid = GetVehicleDBID(vehicleid);
        if(dbid > 0)
        {
            GetVehiclePos(vehicleid, x, y, z);
            GetVehicleZAngle(vehicleid, a);
            
            // Actualizar posición en la BD
            mysql_format(g_MySQL, query, sizeof(query),
                "UPDATE vehicles SET pos_x = %.4f, pos_y = %.4f, pos_z = %.4f, pos_a = %.4f WHERE id = %d",
                x, y, z, a, dbid);
            mysql_tquery(g_MySQL, query);
            count++;
        }
    }
    
    if(count > 0)
        printf("[Vehicle Persistence] %d vehículos guardados", count);
}

// Guardar posición de un vehículo específico
stock SaveVehiclePosition(vehicleid)
{
    new dbid = GetVehicleDBID(vehicleid);
    if(dbid == 0) return 0;
    
    new Float:x, Float:y, Float:z, Float:a;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    
    // Agregar offset de altura para evitar que se hundan
    z += 1.0;
    
    new query[256];
    mysql_format(g_MySQL, query, sizeof(query),
        "UPDATE vehicles SET pos_x = %.4f, pos_y = %.4f, pos_z = %.4f, pos_a = %.4f WHERE id = %d",
        x, y, z, a, dbid);
    mysql_tquery(g_MySQL, query);
    
    return 1;
}

// Cargar vehículos del personaje
stock LoadCharacterVehicles(playerid)
{
    if(!CharacterData[playerid][cID]) return 0;
    
    new query[512];
    mysql_format(g_MySQL, query, sizeof(query),
        "SELECT id, model, pos_x, pos_y, pos_z, pos_a, color1, color2 \
        FROM vehicles \
        WHERE owner_type = 'character' AND owner_id = %d AND is_spawned = 1",
        CharacterData[playerid][cID]);
    
    mysql_tquery(g_MySQL, query, "OnLoadCharacterVehicles", "d", playerid);
    return 1;
}

forward OnLoadCharacterVehicles(playerid);
public OnLoadCharacterVehicles(playerid)
{
    new rows = cache_num_rows();
    if(rows == 0) return 0;
    
    new dbid, model, Float:x, Float:y, Float:z, Float:a, color1, color2;
    new count = 0;
    
    for(new i = 0; i < rows; i++)
    {
        cache_get_value_name_int(i, "id", dbid);
        cache_get_value_name_int(i, "model", model);
        cache_get_value_name_float(i, "pos_x", x);
        cache_get_value_name_float(i, "pos_y", y);
        cache_get_value_name_float(i, "pos_z", z);
        cache_get_value_name_float(i, "pos_a", a);
        cache_get_value_name_int(i, "color1", color1);
        cache_get_value_name_int(i, "color2", color2);
        
        // Verificar si el vehículo ya está spawneado
        new bool:alreadySpawned = false;
        for(new v = 1; v < MAX_VEHICLES_ENGINE; v++)
        {
            if(GetVehicleDBID(v) == dbid)
            {
                alreadySpawned = true;
                break;
            }
        }
        
        if(!alreadySpawned)
        {
            // Spawnear vehículo
            new vehicleid = CreateVehicle(model, x, y, z, a, color1, color2, 300000);
            SetVehicleDBID(vehicleid, dbid);
            SetVehicleEngineState(vehicleid, 0); // Motor apagado
            count++;
            
            printf("[Vehicle Persistence] Spawneado vehículo ID %d (DB: %d) para %s", 
                vehicleid, dbid, CharacterData[playerid][cName]);
        }
    }
    
    if(count > 0)
    {
        new string[128];
        format(string, sizeof(string), "{00FF00}Se han cargado %d vehículo(s) de tu propiedad.", count);
        SendClientMessage(playerid, COLOR_SUCCESS, string);
    }
    
    return 1;
}

// Despawnear vehículos del personaje (opcional)
stock DespawnCharacterVehicles(playerid)
{
    if(!CharacterData[playerid][cID]) return 0;
    
    new count = 0;
    for(new vehicleid = 1; vehicleid < MAX_VEHICLES_ENGINE; vehicleid++)
    {
        new dbid = GetVehicleDBID(vehicleid);
        if(dbid > 0)
        {
            // Verificar si es del jugador (necesitaríamos un array para esto)
            // Por ahora solo guardamos la posición
            SaveVehiclePosition(vehicleid);
            count++;
        }
    }
    
    return count;
}

// Guardar vehículos al desconectarse (llamar desde OnPlayerDisconnect)
stock SavePlayerVehiclesOnDisconnect(playerid)
{
    if(!CharacterData[playerid][cID]) return 0;
    
    // Si esta en un vehiculo, guardarlo inmediatamente
    new currentVehicle = GetPlayerVehicleID(playerid);
    if(currentVehicle != 0)
    {
        new dbid = GetVehicleDBID(currentVehicle);
        if(dbid != 0)
        {
            SaveVehiclePosition(currentVehicle);
            printf("[Vehicle Persistence] Guardado vehiculo %d al desconectar %s (estaba dentro)", 
                dbid, CharacterData[playerid][cName]);
        }
    }
    
    new count = 0;
    for(new vehicleid = 1; vehicleid < MAX_VEHICLES_ENGINE; vehicleid++)
    {
        if(vehicleid == currentVehicle) continue; // Ya guardado arriba
        
        new dbid = GetVehicleDBID(vehicleid);
        if(dbid > 0)
        {
            SaveVehiclePosition(vehicleid);
            count++;
        }
    }
    
    if(count > 0)
    {
        printf("[Vehicle Persistence] %d vehiculos guardados al desconectarse %s", 
            count, CharacterData[playerid][cName]);
    }
    
    return count;
}

// Cerrar sistema (llamar desde OnGameModeExit)
stock CloseVehiclePersistence()
{
    SaveAllVehiclePositions();
    printf("[Vehicle Persistence] Sistema cerrado - todas las posiciones guardadas");
    return 1;
}
