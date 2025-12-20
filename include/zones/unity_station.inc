/*
    Unity Station - Los Santos
    Decoración y ambiente
*/

#if defined _unity_station_included
    #endinput
#endif
#define _unity_station_included

stock LoadUnityStation()
{
    // ========== Vehículos estacionados ==========
    
    // Motos en el estacionamiento
    AddStaticVehicle(462, 1744.3243, -1862.2318, 13.1930, 0.0000, 3, 1); // Faggio
    AddStaticVehicle(462, 1747.3243, -1862.2318, 13.1930, 0.0000, 6, 1); // Faggio
    
    // Autos en el estacionamiento
    AddStaticVehicle(400, 1735.8495, -1856.8773, 13.0938, 0.0000, 123, 1); // Landstalker
    AddStaticVehicle(562, 1728.5862, -1856.8773, 13.0938, 0.0000, 17, 1); // Elegy
    AddStaticVehicle(411, 1721.3230, -1856.8773, 13.0938, 0.0000, 112, 1); // Infernus
    
    // Taxis esperando pasajeros
    AddStaticVehicle(420, 1752.9861, -1894.1801, 13.0938, 90.0000, 6, 1); // Taxi
    AddStaticVehicle(420, 1752.9861, -1897.5801, 13.0938, 90.0000, 6, 1); // Taxi
    AddStaticVehicle(420, 1752.9861, -1901.0801, 13.0938, 90.0000, 6, 1); // Taxi
    
    // Bus de transporte público
    AddStaticVehicle(431, 1708.8234, -1918.0320, 13.3815, 90.0000, 79, 7); // Bus
    
    // ========== Mobiliario urbano ==========
    
    // Bancas
    CreateObject(1280, 1755.5000, -1898.5000, 13.0000, 0.0000, 0.0000, 0.0000);
    CreateObject(1280, 1755.5000, -1895.0000, 13.0000, 0.0000, 0.0000, 0.0000);
    CreateObject(1280, 1755.5000, -1901.0000, 13.0000, 0.0000, 0.0000, 0.0000);
    
    // Papeleras
    CreateObject(1337, 1754.0000, -1897.0000, 13.5000, 0.0000, 0.0000, 0.0000);
    CreateObject(1337, 1754.0000, -1900.0000, 13.5000, 0.0000, 0.0000, 0.0000);
    
    // Máquinas expendedoras
    CreateObject(955, 1761.0000, -1902.0000, 13.5000, 0.0000, 0.0000, 270.0000); // Bebidas
    CreateObject(956, 1761.0000, -1900.0000, 13.5000, 0.0000, 0.0000, 270.0000); // Snacks
    
    // Teléfonos públicos
    CreateObject(1361, 1763.0000, -1895.0000, 13.0000, 0.0000, 0.0000, 270.0000);
    CreateObject(1361, 1763.0000, -1897.0000, 13.0000, 0.0000, 0.0000, 270.0000);
    
    // Carteles informativos
    CreateObject(1215, 1757.0000, -1890.0000, 13.0000, 0.0000, 0.0000, 180.0000);
    CreateObject(1215, 1745.0000, -1890.0000, 13.0000, 0.0000, 0.0000, 180.0000);
    
    // Farolas adicionales
    CreateObject(1226, 1750.0000, -1885.0000, 13.0000, 0.0000, 0.0000, 0.0000);
    CreateObject(1226, 1740.0000, -1885.0000, 13.0000, 0.0000, 0.0000, 0.0000);
    CreateObject(1226, 1730.0000, -1885.0000, 13.0000, 0.0000, 0.0000, 0.0000);
    
    // Barreras de seguridad
    CreateObject(966, 1765.0000, -1905.0000, 13.0000, 0.0000, 0.0000, 90.0000);
    CreateObject(966, 1765.0000, -1890.0000, 13.0000, 0.0000, 0.0000, 90.0000);
    
    // Señales de tráfico
    CreateObject(1228, 1742.0000, -1905.0000, 13.0000, 0.0000, 0.0000, 0.0000); // STOP
    CreateObject(1229, 1738.0000, -1905.0000, 13.0000, 0.0000, 0.0000, 180.0000); // Dirección
    
    print("[Zones] Unity Station cargada - 9 vehículos, 19 objetos");
    return true;
}
