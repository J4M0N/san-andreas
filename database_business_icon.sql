-- Agregar columna para icono personalizado del pickup
ALTER TABLE shops ADD COLUMN pickup_icon INT DEFAULT 1274 AFTER virtual_world;

-- Actualizar iconos por defecto según tipo de tienda
UPDATE shops SET pickup_icon = 1239 WHERE shop_category = 'general_store';
UPDATE shops SET pickup_icon = 1242 WHERE shop_category = 'ammunation';
UPDATE shops SET pickup_icon = 1275 WHERE shop_category = 'clothing';
UPDATE shops SET pickup_icon = 1559 WHERE shop_category = 'hardware';
