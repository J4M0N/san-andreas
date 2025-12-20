/*
    Sistema de Inventario - Items y Objetos
*/

#if defined _inventory_system_included
    #endinput
#endif
#define _inventory_system_included

#define MAX_INVENTORY_SLOTS 20
#define ITEM_TYPE_KEY 1
#define ITEM_TYPE_WEAPON 2

// IDs de items (deben coincidir con la BD)
#define ITEM_VEHICLE_KEY_MASTER 1  // Llave maestra
#define ITEM_VEHICLE_KEY 2          // Llave normal
#define ITEM_PISTOL_9MM 3
#define ITEM_DESERT_EAGLE 4
#define ITEM_PHONE 5
#define ITEM_REPAIR_KIT 6
#define ITEM_BURGER 7
#define ITEM_WATER 8

// Enumeración de items
enum E_INVENTORY_ITEM
{
    invItemID,
    invCharacterID,
    invSlot,
    invQuantity,
    invMetadata[128]
}

new PlayerInventory[MAX_PLAYERS][MAX_INVENTORY_SLOTS][E_INVENTORY_ITEM];
new PlayerInventoryCount[MAX_PLAYERS];

// Objetos equipados en manos
new PlayerHeldObjectLeft[MAX_PLAYERS] = {INVALID_OBJECT_ID, ...};
new PlayerHeldObjectRight[MAX_PLAYERS] = {INVALID_OBJECT_ID, ...};
new PlayerHeldItemSlotLeft[MAX_PLAYERS] = {-1, ...};
new PlayerHeldItemSlotRight[MAX_PLAYERS] = {-1, ...};

// Mantener compatibilidad
#define PlayerHeldObject PlayerHeldObjectRight
#define PlayerHeldItemSlot PlayerHeldItemSlotRight

// Cargar inventario del personaje
stock LoadPlayerInventory(playerid)
{
    new query[256];
    mysql_format(g_MySQL, query, sizeof(query),
        "SELECT ci.id, ci.item_id, ci.quantity, ci.slot, ci.metadata \
        FROM character_inventory ci WHERE ci.character_id = %d ORDER BY ci.slot",
        CharacterData[playerid][cID]);
    
    new Cache:result = mysql_query(g_MySQL, query);
    new rows = cache_num_rows();
    
    PlayerInventoryCount[playerid] = 0;
    
    for(new i = 0; i < rows && i < MAX_INVENTORY_SLOTS; i++)
    {
        cache_get_value_name_int(i, "id", PlayerInventory[playerid][i][invItemID]);
        cache_get_value_name_int(i, "character_id", PlayerInventory[playerid][i][invCharacterID]);
        cache_get_value_name_int(i, "slot", PlayerInventory[playerid][i][invSlot]);
        cache_get_value_name_int(i, "quantity", PlayerInventory[playerid][i][invQuantity]);
        cache_get_value_name(i, "metadata", PlayerInventory[playerid][i][invMetadata], 128);
        
        PlayerInventoryCount[playerid]++;
    }
    
    cache_delete(result);
    
    printf("[Inventory] Cargados %d items para %s", PlayerInventoryCount[playerid], CharacterData[playerid][cName]);
    return 1;
}

// Dar item al jugador
stock GivePlayerItem(playerid, itemid, quantity = 1, const metadata[] = "")
{
    // Buscar slot libre
    new slot = -1;
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++)
    {
        if(PlayerInventory[playerid][i][invItemID] == 0)
        {
            slot = i;
            break;
        }
    }
    
    if(slot == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Tu inventario está lleno.");
        return 0;
    }
    
    // Insertar en base de datos
    new query[512];
    mysql_format(g_MySQL, query, sizeof(query),
        "INSERT INTO character_inventory (character_id, item_id, quantity, slot, metadata) \
        VALUES (%d, %d, %d, %d, '%e')",
        CharacterData[playerid][cID], itemid, quantity, slot, metadata);
    
    printf("[Inventory] Ejecutando query: %s", query);
    
    new Cache:result = mysql_query(g_MySQL, query);
    new insertid = cache_insert_id();
    cache_delete(result);
    
    printf("[Inventory] Insert ID: %d, MySQL errno: %d", insertid, mysql_errno(g_MySQL));
    
    if(insertid > 0)
    {
        PlayerInventory[playerid][slot][invItemID] = itemid;
        PlayerInventory[playerid][slot][invCharacterID] = CharacterData[playerid][cID];
        PlayerInventory[playerid][slot][invSlot] = slot;
        PlayerInventory[playerid][slot][invQuantity] = quantity;
        format(PlayerInventory[playerid][slot][invMetadata], 128, "%s", metadata);
        PlayerInventoryCount[playerid]++;
        
        printf("[Inventory] Item %d dado a %s en slot %d (cantidad: %d, metadata: %s)", 
            itemid, CharacterData[playerid][cName], slot, quantity, metadata);
        
        return 1;
    }
    
    printf("[Inventory] ERROR: No se pudo insertar item en BD");
    
    return 0;
}

// Remover item del inventario
stock RemovePlayerItem(playerid, slot)
{
    if(slot < 0 || slot >= MAX_INVENTORY_SLOTS)
        return 0;
    
    if(PlayerInventory[playerid][slot][invItemID] == 0)
        return 0;
    
    // Eliminar de BD
    new query[128];
    mysql_format(g_MySQL, query, sizeof(query),
        "DELETE FROM character_inventory WHERE character_id = %d AND slot = %d",
        CharacterData[playerid][cID], slot);
    mysql_query(g_MySQL, query);
    
    // Limpiar memoria
    PlayerInventory[playerid][slot][invItemID] = 0;
    PlayerInventory[playerid][slot][invQuantity] = 0;
    PlayerInventory[playerid][slot][invMetadata][0] = 0;
    PlayerInventoryCount[playerid]--;
    
    return 1;
}

// Equipar objeto en mano izquierda
stock HoldItemLeft(playerid, slot)
{
    if(slot < 0 || slot >= MAX_INVENTORY_SLOTS)
        return 0;
    
    new itemid = PlayerInventory[playerid][slot][invItemID];
    if(itemid == 0)
        return 0;
    
    // Remover objeto actual si existe
    if(PlayerHeldObjectLeft[playerid] != INVALID_OBJECT_ID)
    {
        RemovePlayerAttachedObject(playerid, 1);
        PlayerHeldObjectLeft[playerid] = INVALID_OBJECT_ID;
        PlayerHeldItemSlotLeft[playerid] = -1;
    }
    
    new modelid;
    switch(itemid)
    {
        case ITEM_VEHICLE_KEY_MASTER: modelid = 11746; // DoorKey1
        case ITEM_VEHICLE_KEY: modelid = 11746; // DoorKey1
        case ITEM_PHONE: modelid = 330;
        case ITEM_REPAIR_KIT: modelid = 1242;
        default: return 0;
    }
    
    // Attachar objeto a la mano izquierda (bone 5)
    SetPlayerAttachedObject(playerid, 1, modelid, 5,
        -0.08, 0.03, 0.0,
        0.0, -90.0, -120.0,
        1.0, 1.0, 1.0);
    
    PlayerHeldObjectLeft[playerid] = modelid;
    PlayerHeldItemSlotLeft[playerid] = slot;
    
    new itemName[64];
    GetItemName(itemid, itemName);
    
    new string[128];
    format(string, sizeof(string), "{00FF00}Has equipado en mano izquierda: %s", itemName);
    SendClientMessage(playerid, COLOR_SUCCESS, string);
    
    return 1;
}

// Equipar objeto en mano derecha
stock HoldItem(playerid, slot)
{
    if(slot < 0 || slot >= MAX_INVENTORY_SLOTS)
        return 0;
    
    new itemid = PlayerInventory[playerid][slot][invItemID];
    if(itemid == 0)
        return 0;
    
    // Remover objeto actual si existe
    if(PlayerHeldObjectRight[playerid] != INVALID_OBJECT_ID)
    {
        RemovePlayerAttachedObject(playerid, 0);
        PlayerHeldObjectRight[playerid] = INVALID_OBJECT_ID;
        PlayerHeldItemSlotRight[playerid] = -1;
    }
    
    new modelid;
    switch(itemid)
    {
        case ITEM_VEHICLE_KEY_MASTER: modelid = 11746; // DoorKey1 - Llave maestra
        case ITEM_VEHICLE_KEY: modelid = 11746; // DoorKey1 - Llave normal
        case ITEM_PHONE: modelid = 330; // Teléfono
        case ITEM_REPAIR_KIT: modelid = 1242; // Caja de herramientas
        default: return 0;
    }
    
    // Attachar objeto a la mano derecha (bone 6)
    SetPlayerAttachedObject(playerid, 0, modelid, 6,
        0.08, 0.03, 0.0,
        0.0, 90.0, 120.0,
        1.0, 1.0, 1.0);
    
    PlayerHeldObjectRight[playerid] = modelid;
    PlayerHeldItemSlotRight[playerid] = slot;
    
    new itemName[64];
    GetItemName(itemid, itemName);
    
    new string[128];
    format(string, sizeof(string), "{00FF00}Has equipado en mano derecha: %s", itemName);
    SendClientMessage(playerid, COLOR_SUCCESS, string);
    
    return 1;
}

// Desequipar objeto
stock UnholdItem(playerid)
{
    if(PlayerHeldObjectRight[playerid] == INVALID_OBJECT_ID)
        return 0;
    
    RemovePlayerAttachedObject(playerid, 0);
    PlayerHeldObjectRight[playerid] = INVALID_OBJECT_ID;
    PlayerHeldItemSlotRight[playerid] = -1;
    
    SendClientMessage(playerid, COLOR_INFO, "Has guardado el objeto de la mano derecha.");
    return 1;
}

// Obtener nombre del item
stock GetItemName(itemid, name[], size = sizeof(name))
{
    switch(itemid)
    {
        case ITEM_VEHICLE_KEY_MASTER: format(name, size, "Llave Maestra");
        case ITEM_VEHICLE_KEY: format(name, size, "Llave Normal");
        case ITEM_PISTOL_9MM: format(name, size, "Pistola 9mm");
        case ITEM_DESERT_EAGLE: format(name, size, "Desert Eagle");
        case ITEM_PHONE: format(name, size, "Telefono Celular");
        case ITEM_REPAIR_KIT: format(name, size, "Kit de Reparacion");
        case ITEM_BURGER: format(name, size, "Hamburguesa");
        case ITEM_WATER: format(name, size, "Agua");
        default: format(name, size, "Item Desconocido");
    }
}

// Verificar si tiene llave del vehículo
stock PlayerHasVehicleKey(playerid, vehicleid)
{
    new metadata[128];
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++)
    {
        if(PlayerInventory[playerid][i][invItemID] == ITEM_VEHICLE_KEY)
        {
            format(metadata, sizeof(metadata), "%s", PlayerInventory[playerid][i][invMetadata]);
            
            // Metadata contiene "vehicleid:X"
            new vid;
            if(sscanf(metadata, "vehicleid:%d", vid))
                continue;
            
            if(vid == vehicleid)
                return 1;
        }
    }
    return 0;
}

// ============================================================================
// COMANDOS
// ============================================================================

CMD:inventario(playerid, params[])
{
    new dialogStr[2048], itemName[64], line[128];
    strcat(dialogStr, "Ubicacion\tItem\tCantidad\n");
    
    // Mano izquierda
    if(PlayerHeldItemSlotLeft[playerid] != -1)
    {
        new slot = PlayerHeldItemSlotLeft[playerid];
        GetItemName(PlayerInventory[playerid][slot][invItemID], itemName);
        format(line, sizeof(line), "{00FF00}[Mano Izquierda]\t%s\t%d\n",
            itemName, PlayerInventory[playerid][slot][invQuantity]);
        strcat(dialogStr, line);
    }
    else
    {
        strcat(dialogStr, "{808080}[Mano Izquierda]\t{808080}Vacia\t-\n");
    }
    
    // Mano derecha
    if(PlayerHeldItemSlotRight[playerid] != -1)
    {
        new slot = PlayerHeldItemSlotRight[playerid];
        GetItemName(PlayerInventory[playerid][slot][invItemID], itemName);
        format(line, sizeof(line), "{00FF00}[Mano Derecha]\t%s\t%d\n",
            itemName, PlayerInventory[playerid][slot][invQuantity]);
        strcat(dialogStr, line);
    }
    else
    {
        strcat(dialogStr, "{808080}[Mano Derecha]\t{808080}Vacia\t-\n");
    }
    
    // Items en bolsillos
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++)
    {
        if(PlayerInventory[playerid][i][invItemID] > 0)
        {
            // Omitir si ya esta equipado
            if(i == PlayerHeldItemSlotLeft[playerid] || i == PlayerHeldItemSlotRight[playerid])
                continue;
            
            GetItemName(PlayerInventory[playerid][i][invItemID], itemName);
            format(line, sizeof(line), "Bolsillo %d\t%s\t%d\n",
                i + 1, itemName, PlayerInventory[playerid][i][invQuantity]);
            strcat(dialogStr, line);
        }
    }
    
    ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS,
        "Inventario", dialogStr, "Equipar/Usar", "Cerrar");
    
    return 1;
}
CMD:inv(playerid, params[]) return cmd_inventario(playerid, params);

// Handler del dialog de inventario
stock OnDialogInventory(playerid, response, listitem)
{
    if(!response) return 1;
    
    // Listitem 0 = Mano izquierda, 1 = Mano derecha, 2+ = bolsillos
    if(listitem == 0) // Mano izquierda
    {
        if(PlayerHeldItemSlotLeft[playerid] != -1)
        {
            // Desequipar mano izquierda
            if(PlayerHeldObjectLeft[playerid] != INVALID_OBJECT_ID)
            {
                RemovePlayerAttachedObject(playerid, 1);
                PlayerHeldObjectLeft[playerid] = INVALID_OBJECT_ID;
                PlayerHeldItemSlotLeft[playerid] = -1;
                SendClientMessage(playerid, COLOR_INFO, "Has guardado el objeto de la mano izquierda.");
            }
        }
        return 1;
    }
    
    if(listitem == 1) // Mano derecha
    {
        if(PlayerHeldItemSlotRight[playerid] != -1)
        {
            // Desequipar mano derecha
            UnholdItem(playerid);
        }
        return 1;
    }
    
    // Contar items en bolsillos
    new bolsilloIndex = 0;
    new targetSlot = -1;
    
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++)
    {
        if(PlayerInventory[playerid][i][invItemID] > 0)
        {
            if(i == PlayerHeldItemSlotLeft[playerid] || i == PlayerHeldItemSlotRight[playerid])
                continue;
            
            if(bolsilloIndex == (listitem - 2))
            {
                targetSlot = i;
                break;
            }
            bolsilloIndex++;
        }
    }
    
    if(targetSlot == -1) return 1;
    
    // Equipar en mano derecha si esta vacia, sino en izquierda
    if(PlayerHeldItemSlotRight[playerid] == -1)
    {
        HoldItem(playerid, targetSlot);
    }
    else if(PlayerHeldItemSlotLeft[playerid] == -1)
    {
        HoldItemLeft(playerid, targetSlot);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "{FF0000}Tus manos estan llenas. Guarda algo primero.");
    }
    
    return 1;
}

CMD:usar(playerid, params[])
{
    new slot;
    if(sscanf(params, "d", slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Uso: /usar [slot]");
    
    slot--; // Ajustar (usuario usa 1-20, sistema usa 0-19)
    
    if(slot < 0 || slot >= MAX_INVENTORY_SLOTS)
        return SendClientMessage(playerid, COLOR_ERROR, "Slot inválido (1-20)");
    
    new itemid = PlayerInventory[playerid][slot][invItemID];
    if(itemid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "No tienes nada en ese slot.");
    
    switch(itemid)
    {
        case ITEM_VEHICLE_KEY_MASTER:
        {
            HoldItem(playerid, slot);
            SendClientMessage(playerid, COLOR_SUCCESS, "{FFD700}Llave maestra equipada. {FFFFFF}Permite crear duplicados.");
        }
        case ITEM_VEHICLE_KEY:
        {
            HoldItem(playerid, slot);
            SendClientMessage(playerid, COLOR_SUCCESS, "{00FF00}Llave del vehículo equipada.");
        }
        case ITEM_PHONE:
        {
            HoldItem(playerid, slot);
        }
        case ITEM_REPAIR_KIT:
        {
            new vehicleid = GetPlayerVehicleID(playerid);
            if(vehicleid == 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Debes estar en un vehículo.");
            
            SetVehicleHealth(vehicleid, 1000.0);
            RemovePlayerItem(playerid, slot);
            SendClientMessage(playerid, COLOR_SUCCESS, "Has reparado el vehículo.");
            GameTextForPlayer(playerid, "~g~Vehiculo reparado!", 3000, 3);
        }
        case ITEM_BURGER:
        {
            new Float:health;
            GetPlayerHealth(playerid, health);
            SetPlayerHealth(playerid, health + 20.0);
            RemovePlayerItem(playerid, slot);
            SendClientMessage(playerid, COLOR_SUCCESS, "Has comido una hamburguesa. +20 HP");
            ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0);
        }
        case ITEM_WATER:
        {
            new Float:health;
            GetPlayerHealth(playerid, health);
            SetPlayerHealth(playerid, health + 10.0);
            RemovePlayerItem(playerid, slot);
            SendClientMessage(playerid, COLOR_SUCCESS, "Has bebido agua. +10 HP");
            ApplyAnimation(playerid, "VENDING", "VEND_Drink2_P", 4.1, false, false, false, false, 0);
        }
        default:
        {
            SendClientMessage(playerid, COLOR_ERROR, "Este item no se puede usar así.");
        }
    }
    
    return 1;
}

CMD:guardar(playerid, params[])
{
    if(PlayerHeldObject[playerid] == INVALID_OBJECT_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "No tienes nada equipado.");
    
    UnholdItem(playerid);
    return 1;
}

// Dar item a jugador (admin)
CMD:giveitem(playerid, params[])
{
    if(!IsAdministrator(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "No tienes permisos.");
    
    new targetid, itemid, quantity;
    if(sscanf(params, "ddd", targetid, itemid, quantity))
        return SendClientMessage(playerid, COLOR_ERROR, "Uso: /giveitem [playerid] [itemid] [cantidad]");
    
    if(!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, COLOR_ERROR, "Jugador no conectado.");
    
    if(itemid < 1 || itemid > 7)
        return SendClientMessage(playerid, COLOR_ERROR, "Item ID inválido (1-7)");
    
    if(GivePlayerItem(targetid, itemid, quantity, ""))
    {
        new itemName[64];
        GetItemName(itemid, itemName);
        
        new string[144];
        format(string, sizeof(string), "{00FF00}Le has dado %d x %s a %s",
            quantity, itemName, CharacterData[targetid][cName]);
        SendClientMessage(playerid, COLOR_SUCCESS, string);
        
        format(string, sizeof(string), "{00FF00}Has recibido %d x %s",
            quantity, itemName);
        SendClientMessage(targetid, COLOR_SUCCESS, string);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "Error al dar el item.");
    }
    
    return 1;
}
