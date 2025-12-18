/*
    San Andreas Roleplay by pabvlov 2024
*/
#include <open.mp>
#include <a_samp>
#include <a_mysql>

// MySQL Configuration  
#define MYSQL_HOST      "143.198.232.23"
#define MYSQL_USER      "root"
#define MYSQL_PASSWORD  "sitehefalladotepidoperdon"
#define MYSQL_DATABASE  "san-andreas"
#define MYSQL_PORT      3307

// Colors
#define COLOR_WHITE     (0xFFFFFFFF)
#define COLOR_GRAY      (0xAAAAAAFF)
#define COLOR_RED       (0xFF0000FF)
#define COLOR_GREEN     (0x00FF00FF)
#define COLOR_YELLOW    (0xFFFF00FF)

// Dialogs
#define DIALOG_AUTO             1000
#define DIALOG_REGISTER         1001
#define DIALOG_LOGIN            1002
#define DIALOG_CHARACTER_LIST   1003
#define DIALOG_CHARACTER_CREATE 1004
#define DIALOG_CHARACTER_DELETE 1005

#define MAX_CHARACTERS_PER_USER 3

// MySQL Handle
new MySQL:g_MySQL;

// User Data (Cuenta principal)
enum E_USER_DATA
{
    uID,
    uNickname[MAX_PLAYER_NAME],
    uPassword[65],
    uSalt[32],
    uAdminLevel,
    bool:uLogged
}

new UserData[MAX_PLAYERS][E_USER_DATA];

// Character Data (Personaje activo)
enum E_CHARACTER_DATA
{
    cID,
    cUserID,
    cName[MAX_PLAYER_NAME],
    cMoney,
    cBank,
    Float:cPosX,
    Float:cPosY,
    Float:cPosZ,
    Float:cPosA,
    cInterior,
    cVirtualWorld,
    Float:cHealth,
    Float:cArmour,
    cSkin,
    bool:cSelected
}

new CharacterData[MAX_PLAYERS][E_CHARACTER_DATA];

// Vehicle list for /auto command
new const VehicleNames[][] = {
    "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perennial", "Sentinel", "Dumper", "Firetruck", "Trashmaster", "Stretch",
    "Manana", "Infernus", "Voodoo", "Pony", "Mule", "Cheetah", "Ambulance", "Leviathan", "Moonbeam", "Esperanto",
    "Taxi", "Washington", "Bobcat", "Mr Whoopee", "BF Injection", "Hunter", "Premier", "Enforcer", "Securicar", "Banshee",
    "Predator", "Bus", "Rhino", "Barracks", "Hotknife", "Trailer", "Previon", "Coach", "Cabbie", "Stallion",
    "Rumpo", "RC Bandit", "Romero", "Packer", "Monster", "Admiral", "Squalo", "Seasparrow", "Pizzaboy", "Tram",
    "Trailer", "Turismo", "Speeder", "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair", "Berkley's RC Van",
    "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider", "Glendale", "Oceanic", "Sanchez", "Sparrow",
    "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton", "Regina",
    "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher",
    "FBI Rancher", "Virgo", "Greenwood", "Jetmax", "Hotring", "Sandking", "Blista Compact", "Police Maverick", "Boxville", "Benson",
    "Mesa", "RC Goblin", "Hotring Racer A", "Hotring Racer B", "Bloodring Banger", "Rancher", "Super GT", "Elegant", "Journey", "Bike",
    "Mountain Bike", "Beagle", "Cropdust", "Stunt", "Tanker", "RoadTrain", "Nebula", "Majestic", "Buccaneer", "Shamal",
    "Hydra", "FCR-900", "NRG-500", "HPV1000", "Cement Truck", "Tow Truck", "Fortune", "Cadrona", "FBI Truck", "Willard",
    "Forklift", "Tractor", "Combine", "Feltzer", "Remington", "Slamvan", "Blade", "Freight", "Streak", "Vortex",
    "Vincent", "Bullet", "Clover", "Sadler", "Firetruck LA", "Hustler", "Intruder", "Primo", "Cargobob", "Tampa",
    "Sunrise", "Merit", "Utility", "Nevada", "Yosemite", "Windsor", "Monster A", "Monster B", "Uranus", "Jester",
    "Sultan", "Stratum", "Elegy", "Raindance", "RC Tiger", "Flash", "Tahoma", "Savanna", "Bandito", "Freight",
    "Trailer", "Kart", "Mower", "Duneride", "Sweeper", "Broadway", "Tornado", "AT-400", "DFT-30", "Huntley",
    "Stafford", "BF-400", "Newsvan", "Tug", "Trailer A", "Emperor", "Wayfarer", "Euros", "Hotdog", "Club",
    "Trailer B", "Trailer C", "Andromada", "Dodo", "RC Cam", "Launch", "Police Car (LSPD)", "Police Car (SFPD)", "Police Car (LVPD)", "Police Ranger",
    "Picador", "S.W.A.T. Van", "Alpha", "Phoenix", "Glendale", "Sadler", "Luggage Trailer A", "Luggage Trailer B", "Stair Trailer", "Boxville",
    "Farm Plow", "Utility Trailer"
};

#define DIALOG_AUTO 1000

main()
{
    print("=====================================");
    print("  San Andreas Roleplay - pabvlov");
    print("=====================================");
}

public OnGameModeInit()
{
    SetGameModeText("San Andreas Roleplay");
    EnableStuntBonusForAll(false);
    
    SetWeather(10);
    SetWorldTime(12);
    
    // Conectar a MySQL
    MySQL_Connect();
    
    // Unity Station spawn
    AddPlayerClass(0, 1759.0189, -1898.1260, 13.5622, 266.4503, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    
    return true;
}

public OnGameModeExit()
{
    // Cerrar conexión MySQL
    if(g_MySQL != MYSQL_INVALID_HANDLE)
    {
        mysql_close(g_MySQL);
        print("[MySQL] Conexión cerrada correctamente.");
    }
    return true;
}

public OnPlayerConnect(playerid)
{
    // Resetear datos
    UserData[playerid][uID] = 0;
    UserData[playerid][uLogged] = false;
    UserData[playerid][uAdminLevel] = 0;
    
    CharacterData[playerid][cID] = 0;
    CharacterData[playerid][cSelected] = false;
    
    GetPlayerName(playerid, UserData[playerid][uNickname], MAX_PLAYER_NAME);
    
    // Verificar si el usuario está registrado
    new query[256];
    mysql_format(g_MySQL, query, sizeof(query), "SELECT * FROM `users` WHERE `nickname` = '%e' LIMIT 1", UserData[playerid][uNickname]);
    mysql_tquery(g_MySQL, query, "OnUserCheckAccount", "d", playerid);
    
    return true;
}

public OnPlayerDisconnect(playerid, reason)
{
    if(CharacterData[playerid][cSelected])
    {
        SaveCharacterData(playerid);
    }
    
    new string[128];
    
    new reasonText[32];
    switch(reason)
    {
        case 0: reasonText = "Timeout/Crash";
        case 1: reasonText = "Salió";
        case 2: reasonText = "Kick/Ban";
    }
    
    if(CharacterData[playerid][cSelected])
    {
        format(string, sizeof(string), "%s se ha desconectado. [%s]", CharacterData[playerid][cName], reasonText);
    }
    else
    {
        format(string, sizeof(string), "%s se ha desconectado. [%s]", UserData[playerid][uNickname], reasonText);
    }
    SendClientMessageToAll(COLOR_GRAY, string);
    return true;
}

public OnPlayerSpawn(playerid)
{
    if(!CharacterData[playerid][cSelected])
    {
        Kick(playerid);
        return true;
    }
    
    SetPlayerPos(playerid, CharacterData[playerid][cPosX], CharacterData[playerid][cPosY], CharacterData[playerid][cPosZ]);
    SetPlayerFacingAngle(playerid, CharacterData[playerid][cPosA]);
    SetPlayerInterior(playerid, CharacterData[playerid][cInterior]);
    SetPlayerVirtualWorld(playerid, CharacterData[playerid][cVirtualWorld]);
    SetPlayerHealth(playerid, CharacterData[playerid][cHealth]);
    SetPlayerArmour(playerid, CharacterData[playerid][cArmour]);
    SetPlayerSkin(playerid, CharacterData[playerid][cSkin]);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, CharacterData[playerid][cMoney]);
    SetCameraBehindPlayer(playerid);
    
    SendClientMessage(playerid, COLOR_GREEN, "Has spawneado correctamente!");
    return true;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!CharacterData[playerid][cSelected])
    {
        SendClientMessage(playerid, COLOR_RED, "Debes seleccionar un personaje primero!");
        return 1;
    }
    
    if(strcmp("/auto", cmdtext, true, 5) == 0)
    {
        new dialogString[3000];
        for(new i = 0; i < sizeof(VehicleNames); i++)
        {
            format(dialogString, sizeof(dialogString), "%s%d. %s\n", dialogString, 400 + i, VehicleNames[i]);
        }
        ShowPlayerDialog(playerid, DIALOG_AUTO, DIALOG_STYLE_LIST, "Selecciona un vehículo", dialogString, "Seleccionar", "Cancelar");
        return 1;
    }
    
    if(strcmp("/stats", cmdtext, true, 6) == 0)
    {
        new string[512];
        format(string, sizeof(string), 
            "{FFFFFF}Cuenta: {FFFF00}%s {AAAAAA}(ID: %d)\n\
            {FFFFFF}Personaje: {00FF00}%s {AAAAAA}(ID: %d)\n\
            {FFFFFF}Dinero: {00FF00}$%d\n\
            {FFFFFF}Banco: {00FF00}$%d\n\
            {FFFFFF}Nivel Admin: {FF0000}%d\n\
            {FFFFFF}Skin: {FFFF00}%d",
            UserData[playerid][uNickname], UserData[playerid][uID],
            CharacterData[playerid][cName], CharacterData[playerid][cID],
            CharacterData[playerid][cMoney],
            CharacterData[playerid][cBank],
            UserData[playerid][uAdminLevel],
            CharacterData[playerid][cSkin]
        );
        ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, "Estadísticas", string, "Cerrar", "");
        return 1;
    }
    
    if(strcmp("/personajes", cmdtext, true, 11) == 0 || strcmp("/chars", cmdtext, true, 6) == 0)
    {
        ShowCharacterList(playerid);
        return 1;
    }
    
    SendClientMessage(playerid, COLOR_RED, "Comando desconocido. Usa /auto | /stats | /personajes");
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_REGISTER:
        {
            if(!response) return Kick(playerid);
            
            if(strlen(inputtext) < 6 || strlen(inputtext) > 32)
            {
                SendClientMessage(playerid, COLOR_RED, "La contraseña debe tener entre 6 y 32 caracteres.");
                ShowRegisterDialog(playerid);
                return true;
            }
            
            // Generar salt aleatorio único
            new salt[32];
            format(salt, sizeof(salt), "%d%d%d", random(99999), gettime(), playerid);
            format(UserData[playerid][uSalt], 32, "%s", salt);
            
            // Hashear con SHA256
            new query[512], hash[65];
            SHA256_PassHash(inputtext, UserData[playerid][uSalt], hash, sizeof(hash));
            
            mysql_format(g_MySQL, query, sizeof(query), 
                "INSERT INTO `users` (`nickname`, `password`, `salt`) VALUES ('%e', '%e', '%e')",
                UserData[playerid][uNickname], hash, UserData[playerid][uSalt]
            );
            mysql_tquery(g_MySQL, query, "OnUserRegister", "d", playerid);
            
            SendClientMessage(playerid, COLOR_GREEN, "Cuenta creada exitosamente!");
            return true;
        }
        
        case DIALOG_LOGIN:
        {
            if(!response) return Kick(playerid);
            
            // Hashear con SHA256
            new hash[65];
            SHA256_PassHash(inputtext, UserData[playerid][uSalt], hash, sizeof(hash));
            
            if(strcmp(hash, UserData[playerid][uPassword], false) != 0)
            {
                SendClientMessage(playerid, COLOR_RED, "Contraseña incorrecta!");
                ShowLoginDialog(playerid);
                return true;
            }
            
            new query[256];
            mysql_format(g_MySQL, query, sizeof(query), "SELECT * FROM `users` WHERE `nickname` = '%e' LIMIT 1", UserData[playerid][uNickname]);
            mysql_tquery(g_MySQL, query, "OnUserLogin", "d", playerid);
            return true;
        }
        
        case DIALOG_CHARACTER_LIST:
        {
            if(!response) return true;
            
            new query[256];
            mysql_format(g_MySQL, query, sizeof(query), "SELECT * FROM `characters` WHERE `user_id` = %d ORDER BY `last_played` DESC", UserData[playerid][uID]);
            new Cache:result = mysql_query(g_MySQL, query);
            new rows = cache_num_rows();
            
            if(listitem >= rows)
            {
                // Crear nuevo personaje
                cache_delete(result);
                ShowCharacterCreate(playerid);
            }
            else
            {
                // Seleccionar personaje existente
                new charID;
                cache_get_value_name_int(listitem, "id", charID);
                cache_delete(result);
                
                mysql_format(g_MySQL, query, sizeof(query), "SELECT * FROM `characters` WHERE `id` = %d LIMIT 1", charID);
                mysql_tquery(g_MySQL, query, "OnCharacterSelect", "d", playerid);
            }
            
            return true;
        }
        
        case DIALOG_CHARACTER_CREATE:
        {
            if(!response)
            {
                ShowCharacterList(playerid);
                return true;
            }
            
            // Validar nombre RP
            if(!ValidateRPName(inputtext))
            {
                SendClientMessage(playerid, COLOR_RED, "Nombre inválido! Debe ser Nombre_Apellido con formato RP");
                SendClientMessage(playerid, COLOR_YELLOW, "Ejemplos: Pablo Prieto, Maria Garcia, John Smith");
                ShowCharacterCreate(playerid);
                return true;
            }
            
            // Verificar que el nombre no esté en uso
            format(CharacterData[playerid][cName], MAX_PLAYER_NAME, "%s", inputtext);
            
            new query[256];
            mysql_format(g_MySQL, query, sizeof(query), 
                "INSERT INTO `characters` (`user_id`, `name`) VALUES (%d, '%e')",
                UserData[playerid][uID], CharacterData[playerid][cName]
            );
            mysql_tquery(g_MySQL, query, "OnCharacterCreate", "d", playerid);
            
            SendClientMessage(playerid, COLOR_GREEN, "Personaje creado exitosamente!");
            return true;
        }
        
        case DIALOG_AUTO:
        {
            if(response)
            {
                new Float:x, Float:y, Float:z, Float:angle;
                GetPlayerPos(playerid, x, y, z);
                GetPlayerFacingAngle(playerid, angle);
                
                new vehicleid = CreateVehicle(400 + listitem, x + 3.0, y, z, angle, -1, -1, 60000);
                
                new message[128];
                format(message, sizeof(message), "Has spawneado un %s (ID: %d)", VehicleNames[listitem], 400 + listitem);
                SendClientMessage(playerid, COLOR_WHITE, message);
                
                PutPlayerInVehicle(playerid, vehicleid, 0);
            }
            return true;
        }
    }
    return false;
}

// ==================== MySQL Functions ====================

MySQL_Connect()
{
    print("[MySQL] Iniciando conexión...");
    printf("[MySQL] Host: %s:%d", MYSQL_HOST, MYSQL_PORT);
    printf("[MySQL] User: %s", MYSQL_USER);
    printf("[MySQL] Database: %s", MYSQL_DATABASE);
    
    // Usar MySQLOpt para especificar puerto personalizado
    new MySQLOpt:options = mysql_init_options();
    mysql_set_option(options, SERVER_PORT, MYSQL_PORT);
    mysql_set_option(options, AUTO_RECONNECT, true);
    
    g_MySQL = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE, options);
    
    printf("[MySQL] Handle obtenido: %d", _:g_MySQL);
    
    if(g_MySQL == MYSQL_INVALID_HANDLE || _:g_MySQL == 0)
    {
        print("====================================");
        print("[MySQL] ERROR: No se pudo conectar!");
        print("====================================");
        return false;
    }
    
    new errno = mysql_errno(g_MySQL);
    printf("[MySQL] Error code: %d", errno);
    
    if(errno == 2019)
    {
        print("[MySQL] Error 2019: Charset incompatible");
        return false;
    }
    
    if(errno != 0)
    {
        printf("[MySQL] ERROR: Código de error %d", errno);
        return false;
    }
    
    print("====================================");
    print("[MySQL] ¡Conexión exitosa a MySQL 5.7!");
    printf("[MySQL] Base de datos: %s", MYSQL_DATABASE);
    print("====================================");
    
    MySQL_CreateTables();
    return true;
}

MySQL_CreateTables()
{
    mysql_tquery(g_MySQL, "CREATE TABLE IF NOT EXISTS `users` (\
        `id` INT NOT NULL AUTO_INCREMENT,\
        `nickname` VARCHAR(24) NOT NULL,\
        `password` VARCHAR(64) NOT NULL,\
        `salt` VARCHAR(32) NOT NULL,\
        `email` VARCHAR(100) DEFAULT NULL,\
        `admin_level` INT DEFAULT 0,\
        `registered` DATETIME DEFAULT CURRENT_TIMESTAMP,\
        `last_login` DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
        PRIMARY KEY (`id`),\
        UNIQUE KEY `nickname` (`nickname`)\
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
    
    mysql_tquery(g_MySQL, "CREATE TABLE IF NOT EXISTS `characters` (\
        `id` INT NOT NULL AUTO_INCREMENT,\
        `user_id` INT NOT NULL,\
        `name` VARCHAR(24) NOT NULL,\
        `money` INT DEFAULT 5000,\
        `bank` INT DEFAULT 10000,\
        `pos_x` FLOAT DEFAULT 1759.0189,\
        `pos_y` FLOAT DEFAULT -1898.1260,\
        `pos_z` FLOAT DEFAULT 13.5622,\
        `pos_a` FLOAT DEFAULT 266.4503,\
        `interior` INT DEFAULT 0,\
        `virtual_world` INT DEFAULT 0,\
        `health` FLOAT DEFAULT 100.0,\
        `armour` FLOAT DEFAULT 0.0,\
        `skin` INT DEFAULT 0,\
        `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP,\
        `last_played` DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
        PRIMARY KEY (`id`),\
        UNIQUE KEY `name` (`name`)\
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
    
    print("[MySQL] Tablas verificadas/creadas correctamente.");
}

#include "../include/character_system.inc"



