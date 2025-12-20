-- Sistema de Tiendas (24/7, Armerias, Tiendas de Ropa, Restaurantes)

-- Modificar tabla shops para permitir property_id NULL (shops standalone)
ALTER TABLE `shops` 
MODIFY COLUMN `property_id` INT NULL,
ADD COLUMN `id` INT NOT NULL AUTO_INCREMENT FIRST,
ADD PRIMARY KEY (`id`),
DROP PRIMARY KEY,
ADD KEY `property_id_idx` (`property_id`);

-- Modificar tabla shops existente para agregar columnas de posiciones
ALTER TABLE `businesses` 
ADD COLUMN `name` VARCHAR(64) NULL AFTER `property_id`,
ADD COLUMN `pos_x` FLOAT NULL COMMENT 'Posicion del pickup (entrada)' AFTER `reputation`,
ADD COLUMN `pos_y` FLOAT NULL AFTER `pos_x`,
ADD COLUMN `pos_z` FLOAT NULL AFTER `pos_y`,
ADD COLUMN `interior_x` FLOAT NOT NULL DEFAULT 0.0 COMMENT 'Posicion interior (punto de venta)' AFTER `pos_z`,
ADD COLUMN `interior_y` FLOAT NOT NULL DEFAULT 0.0 AFTER `interior_x`,
ADD COLUMN `interior_z` FLOAT NOT NULL DEFAULT 0.0 AFTER `interior_y`,
ADD COLUMN `interior` INT NOT NULL DEFAULT 0 AFTER `interior_z`,
ADD COLUMN `virtual_world` INT NOT NULL DEFAULT 0 AFTER `interior`,
ADD COLUMN `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP AFTER `virtual_world`;

-- Modificar tabla shops existente para agregar columnas de posiciones
ALTER TABLE `shops` 
ADD COLUMN `name` VARCHAR(64) NULL AFTER `shop_category`,
ADD COLUMN `pos_x` FLOAT NULL COMMENT 'Posicion del pickup (entrada)' AFTER `name`,
ADD COLUMN `pos_y` FLOAT NULL AFTER `pos_x`,
ADD COLUMN `pos_z` FLOAT NULL AFTER `pos_y`,
ADD COLUMN `interior_x` FLOAT NOT NULL DEFAULT 0.0 COMMENT 'Posicion interior (punto de venta)' AFTER `pos_z`,
ADD COLUMN `interior_y` FLOAT NOT NULL DEFAULT 0.0 AFTER `interior_x`,
ADD COLUMN `interior_z` FLOAT NOT NULL DEFAULT 0.0 AFTER `interior_y`,
ADD COLUMN `interior` INT NOT NULL DEFAULT 0 AFTER `interior_z`,
ADD COLUMN `virtual_world` INT NOT NULL DEFAULT 0 AFTER `interior`,
ADD COLUMN `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP AFTER `virtual_world`;

-- Tabla de stock de tiendas
CREATE TABLE IF NOT EXISTS `shop_stock` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `shop_id` INT NOT NULL,
    `item_id` INT NOT NULL,
    `quantity` INT NOT NULL DEFAULT 0,
    `price` INT NOT NULL DEFAULT 0,
    `updated_at` DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`id`),
    UNIQUE KEY `shop_item` (`shop_id`, `item_id`),
    FOREIGN KEY (`shop_id`) REFERENCES `shops`(`property_id`) ON DELETE CASCADE,
    FOREIGN KEY (`item_id`) REFERENCES `items`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- NOTA: Para agregar shops y stock, usar los comandos en el juego:
-- /creartienda [tipo] [nombre] - Crear tienda en tu posicion
-- /agregarstock [shop_id] [item_id] [quantity] [price] - Agregar items al stock
-- /configurarbusiness [shop_id] - Configurar posiciones e interior

-- Ejemplo comentado (requiere property_id valido):
-- INSERT INTO `shops` (`property_id`, `shop_category`, `name`, `pos_x`, `pos_y`, `pos_z`, `interior`, `virtual_world`) 
-- VALUES (1, 'general_store', '24/7 Unity Station', 1833.0, -1842.5, 13.5, 0, 0);
-- 
-- INSERT INTO `shop_stock` (`shop_id`, `item_id`, `quantity`, `price`) VALUES
-- (1, 5, 50, 500),   -- Telefono x50 a $500
-- (1, 6, 30, 1000),  -- Kit de Reparacion x30 a $1000
-- (1, 7, 100, 50),   -- Hamburguesa x100 a $50
-- (1, 8, 150, 25);   -- Agua x150 a $25
