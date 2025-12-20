// Este archivo contiene las funciones auxiliares para el sistema de personajes
// Copia y pega estas funciones al final de derby.pwn

// ==================== Callbacks MySQL ====================

forward OnUserCheckAccount(playerid);
public OnUserCheckAccount(playerid)
{
    if(cache_num_rows() > 0)
    {
        // Usuario existe - cargar password y salt
        cache_get_value_name(0, "password", UserData[playerid][uPassword], 65);
        cache_get_value_name(0, "salt", UserData[playerid][uSalt], 32);
        ShowLoginDialog(playerid);
    }
    else
    {
        // Usuario nuevo - mostrar registro
        ShowRegisterDialog(playerid);
    }
    return true;
}

forward OnUserRegister(playerid);
public OnUserRegister(playerid)
{
    UserData[playerid][uID] = cache_insert_id();
    UserData[playerid][uLogged] = true;
    UserData[playerid][uAdminLevel] = 0;
    
    new string[128];
    format(string, sizeof(string), "{00FF00}%s{FFFFFF} se ha registrado en el servidor!", UserData[playerid][uNickname]);
    SendClientMessageToAll(COLOR_WHITE, string);
    
    SendClientMessage(playerid, COLOR_GREEN, "¡Cuenta creada exitosamente!");
    SendClientMessage(playerid, COLOR_YELLOW, "Ahora debes crear tu primer personaje...");
    
    ShowCharacterCreate(playerid);
    return true;
}

forward OnUserLogin(playerid);
public OnUserLogin(playerid)
{
    cache_get_value_name_int(0, "id", UserData[playerid][uID]);
    cache_get_value_name_int(0, "admin_level", UserData[playerid][uAdminLevel]);
    UserData[playerid][uLogged] = true;
    
    SendClientMessage(playerid, COLOR_GREEN, "¡Sesión iniciada correctamente!");
    
    // Mostrar lista de personajes
    ShowCharacterList(playerid);
    return true;
}

forward OnLoadCharacterList(playerid);
public OnLoadCharacterList(playerid)
{
    new rows = cache_num_rows();
    
    if(rows == 0)
    {
        // No tiene personajes, crear uno nuevo
        SendClientMessage(playerid, COLOR_YELLOW, "No tienes personajes creados. Crea tu primer personaje:");
        ShowCharacterCreate(playerid);
        return true;
    }
    
    new dialogString[512], charName[MAX_PLAYER_NAME], lastPlayed[32];
    format(dialogString, sizeof(dialogString), "{FFFFFF}Personaje\t{AAAAAA}Última sesión\n");
    
    for(new i = 0; i < rows && i < MAX_CHARACTERS_PER_USER; i++)
    {
        cache_get_value_name(i, "name", charName, MAX_PLAYER_NAME);
        cache_get_value_name(i, "last_played", lastPlayed, 32);
        format(dialogString, sizeof(dialogString), "%s{FFFFFF}%s\t{AAAAAA}%s\n", dialogString, charName, lastPlayed);
    }
    
    if(rows < MAX_CHARACTERS_PER_USER)
    {
        format(dialogString, sizeof(dialogString), "%s{00FF00}[+] Crear nuevo personaje", dialogString);
    }
    
    ShowPlayerDialog(playerid, DIALOG_CHARACTER_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Seleccionar Personaje", dialogString, "Seleccionar", "");
    return true;
}

forward OnCharacterCreate(playerid);
public OnCharacterCreate(playerid)
{
    printf("[DEBUG] OnCharacterCreate llamada para playerid: %d", playerid);
    
    if(!IsPlayerConnected(playerid))
    {
        printf("[DEBUG] ERROR: Jugador %d no esta conectado!", playerid);
        return false;
    }
    
    new insertid = cache_insert_id();
    printf("[DEBUG] cache_insert_id() = %d", insertid);
    
    if(insertid == 0)
    {
        printf("[DEBUG] ERROR: INSERT falló, insert_id = 0");
        printf("[DEBUG] MySQL errno: %d", mysql_errno(g_MySQL));
        SendClientMessage(playerid, COLOR_ERROR, "Error al crear el personaje en la base de datos.");
        return false;
    }
    
    CharacterData[playerid][cID] = insertid;
    CharacterData[playerid][cUserID] = UserData[playerid][uID];
    CharacterData[playerid][cMoney] = 5000;
    CharacterData[playerid][cBank] = 10000;
    CharacterData[playerid][cPosX] = 1759.0189;
    CharacterData[playerid][cPosY] = -1898.1260;
    CharacterData[playerid][cPosZ] = 13.5622;
    CharacterData[playerid][cPosA] = 266.4503;
    CharacterData[playerid][cInterior] = 0;
    CharacterData[playerid][cVirtualWorld] = 0;
    CharacterData[playerid][cHealth] = 100.0;
    CharacterData[playerid][cArmour] = 0.0;
    CharacterData[playerid][cSelected] = true;
    
    printf("[DEBUG] Personaje creado con ID %d, skin %d", insertid, CharacterData[playerid][cSkin]);
    
    new string[128];
    format(string, sizeof(string), "{00FF00}%s{FFFFFF} ha creado el personaje {FFFF00}%s", UserData[playerid][uNickname], CharacterData[playerid][cName]);
    SendClientMessageToAll(COLOR_WHITE, string);
    
    SetSpawnInfo(playerid, 0, CharacterData[playerid][cSkin], 
        CharacterData[playerid][cPosX], CharacterData[playerid][cPosY], CharacterData[playerid][cPosZ], 
        CharacterData[playerid][cPosA], WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    
    printf("[DEBUG] SetSpawnInfo configurado, spawneando jugador...");
    
    TogglePlayerSpectating(playerid, false);
    SpawnPlayer(playerid);
    
    printf("[DEBUG] SpawnPlayer ejecutado exitosamente");
    
    return true;
}

forward OnCharacterSelect(playerid);
public OnCharacterSelect(playerid)
{
    if(cache_num_rows() == 0)
    {
        SendClientMessage(playerid, COLOR_RED, "Error al cargar el personaje!");
        ShowCharacterList(playerid);
        return true;
    }
    
    LoadCharacterData(playerid);
    return true;
}

// ==================== Character Functions ====================

LoadCharacterData(playerid)
{
    cache_get_value_name_int(0, "id", CharacterData[playerid][cID]);
    cache_get_value_name_int(0, "user_id", CharacterData[playerid][cUserID]);
    cache_get_value_name(0, "name", CharacterData[playerid][cName], MAX_PLAYER_NAME);
    cache_get_value_name_int(0, "age", CharacterData[playerid][cAge]);
    cache_get_value_name_int(0, "gender", CharacterData[playerid][cGender]);
    cache_get_value_name_int(0, "money", CharacterData[playerid][cMoney]);
    cache_get_value_name_int(0, "bank", CharacterData[playerid][cBank]);
    cache_get_value_name_float(0, "pos_x", CharacterData[playerid][cPosX]);
    cache_get_value_name_float(0, "pos_y", CharacterData[playerid][cPosY]);
    cache_get_value_name_float(0, "pos_z", CharacterData[playerid][cPosZ]);
    cache_get_value_name_float(0, "pos_a", CharacterData[playerid][cPosA]);
    cache_get_value_name_int(0, "interior", CharacterData[playerid][cInterior]);
    cache_get_value_name_int(0, "virtual_world", CharacterData[playerid][cVirtualWorld]);
    cache_get_value_name_float(0, "health", CharacterData[playerid][cHealth]);
    cache_get_value_name_float(0, "armour", CharacterData[playerid][cArmour]);
    cache_get_value_name_int(0, "skin", CharacterData[playerid][cSkin]);
    CharacterData[playerid][cSelected] = true;
    
    new string[128];
    format(string, sizeof(string), "{FFFF00}%s{FFFFFF} ({00FF00}%s{FFFFFF}) se ha conectado al servidor.", CharacterData[playerid][cName], UserData[playerid][uNickname]);
    SendClientMessageToAll(COLOR_WHITE, string);
    
    SendClientMessage(playerid, COLOR_YELLOW, "Usa /auto para spawnear un vehículo | /stats para ver tus estadísticas | /personajes para cambiar de personaje");
    
    SpawnPlayer(playerid);
    return true;
}

SaveCharacterData(playerid)
{
    GetPlayerPos(playerid, CharacterData[playerid][cPosX], CharacterData[playerid][cPosY], CharacterData[playerid][cPosZ]);
    GetPlayerFacingAngle(playerid, CharacterData[playerid][cPosA]);
    GetPlayerHealth(playerid, CharacterData[playerid][cHealth]);
    GetPlayerArmour(playerid, CharacterData[playerid][cArmour]);
    CharacterData[playerid][cInterior] = GetPlayerInterior(playerid);
    CharacterData[playerid][cVirtualWorld] = GetPlayerVirtualWorld(playerid);
    CharacterData[playerid][cSkin] = GetPlayerSkin(playerid);
    CharacterData[playerid][cMoney] = GetPlayerMoney(playerid);
    
    new query[512];
    mysql_format(g_MySQL, query, sizeof(query),
        "UPDATE `characters` SET \
        `money` = %d, `bank` = %d, `pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `pos_a` = %f, \
        `interior` = %d, `virtual_world` = %d, `health` = %f, `armour` = %f, `skin` = %d \
        WHERE `id` = %d",
        CharacterData[playerid][cMoney], CharacterData[playerid][cBank],
        CharacterData[playerid][cPosX], CharacterData[playerid][cPosY], CharacterData[playerid][cPosZ], CharacterData[playerid][cPosA],
        CharacterData[playerid][cInterior], CharacterData[playerid][cVirtualWorld],
        CharacterData[playerid][cHealth], CharacterData[playerid][cArmour], CharacterData[playerid][cSkin],
        CharacterData[playerid][cID]
    );
    mysql_tquery(g_MySQL, query);
    return true;
}

// ==================== Dialog Functions ====================

ShowRegisterDialog(playerid)
{
    new string[256];
    format(string, sizeof(string), 
        "{FFFFFF}Bienvenido a {00FF00}San Andreas Roleplay{FFFFFF}!\n\n\
        El nickname {FFFF00}%s{FFFFFF} no está registrado.\n\
        Por favor, ingresa una contraseña para crear tu cuenta:\n\n\
        {AAAAAA}La contraseña debe tener entre 6 y 32 caracteres.",
        UserData[playerid][uNickname]
    );
    ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registro de Cuenta", string, "Registrar", "Salir");
}

ShowLoginDialog(playerid)
{
    new string[256];
    format(string, sizeof(string), 
        "{FFFFFF}Bienvenido de vuelta a {00FF00}San Andreas Roleplay{FFFFFF}!\n\n\
        Nickname: {FFFF00}%s{FFFFFF}\n\
        Por favor, ingresa tu contraseña para iniciar sesión:",
        UserData[playerid][uNickname]
    );
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Iniciar Sesión", string, "Login", "Salir");
}

ShowCharacterList(playerid)
{
    new query[256];
    mysql_format(g_MySQL, query, sizeof(query), "SELECT * FROM `characters` WHERE `user_id` = %d ORDER BY `last_played` DESC LIMIT %d", UserData[playerid][uID], MAX_CHARACTERS_PER_USER);
    mysql_tquery(g_MySQL, query, "OnLoadCharacterList", "d", playerid);
}

ShowCharacterCreate(playerid)
{
    new string[256];
    format(string, sizeof(string), 
        "{FFFFFF}Crear Nuevo Personaje\n\n\
        {AAAAAA}Ingresa el nombre de tu personaje.\n\
        {AAAAAA}Debe ser un nombre RP realista:\n\
        {00FF00}Correcto: Pablo Prieto, Sebastian Mena\n\
        {FF0000}Incorrecto: xXKillerXx, PrO_GaMeR\n\n\
        {FFFFFF}Formato: {FFFF00}Nombre Apellido"
    );
    ShowPlayerDialog(playerid, DIALOG_CHARACTER_CREATE, DIALOG_STYLE_INPUT, "Crear Personaje", string, "Crear", "Cancelar");
}

ShowCharacterStatsDialog(playerid)
{
    new string[512];
    new ageStr[32], genderStr[32], skinStr[32];
    
    // Edad
    if(CharacterData[playerid][cAge] == 0)
        format(ageStr, sizeof(ageStr), "{FF0000}No configurado");
    else
        format(ageStr, sizeof(ageStr), "{00FF00}%d a\xF1os", CharacterData[playerid][cAge]);
    
    // Genero
    if(CharacterData[playerid][cGender] == 0)
        format(genderStr, sizeof(genderStr), "{FF0000}No configurado");
    else if(CharacterData[playerid][cGender] == 1)
        format(genderStr, sizeof(genderStr), "{00FF00}Masculino");
    else
        format(genderStr, sizeof(genderStr), "{00FF00}Femenino");
    
    // Skin
    if(CharacterData[playerid][cSkin] == 0)
        format(skinStr, sizeof(skinStr), "{FF0000}No configurado (0 - CJ)");
    else
        format(skinStr, sizeof(skinStr), "{00FF00}ID: %d", CharacterData[playerid][cSkin]);
    
    new dialogTitle[128];
    format(dialogTitle, sizeof(dialogTitle), "Configuracion: %s", CharacterData[playerid][cName]);
    
    format(string, sizeof(string),
        "Edad\t%s\n\
        Genero\t%s\n\
        Skin\t%s\n\
        {00FF00}Finalizar creacion",
        ageStr, genderStr, skinStr
    );
    
    ShowPlayerDialog(playerid, DIALOG_CHARACTER_STATS, DIALOG_STYLE_TABLIST,
        dialogTitle, string, "Seleccionar", "Atras");
}

// Validar nombre RP (Nombre_Apellido)
ValidateRPName(const name[])
{
    new len = strlen(name);
    if(len < 5 || len > 23) return false; // Muy corto o muy largo
    
    new spaces = 0;
    for(new i = 0; i < len; i++)
    {
        if(name[i] == ' ')
        {
            spaces++;
            // No permitir espacios al inicio, final o consecutivos
            if(i == 0 || i == len-1 || name[i+1] == ' ') return false;
        }
        else if(!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z')))
        {
            // Solo letras permitidas
            return false;
        }
    }
    
    // Debe tener exactamente un espacio (Nombre Apellido)
    if(spaces != 1) return false;
    
    // Primera letra debe ser mayúscula
    if(!(name[0] >= 'A' && name[0] <= 'Z')) return false;
    
    return true;
}
