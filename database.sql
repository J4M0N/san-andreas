-- San Andreas Roleplay Database Setup
-- Ejecutar estas queries en tu servidor MySQL

-- Crear la base de datos
CREATE DATABASE IF NOT EXISTS `san-andreas` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- Usar la base de datos
USE `san-andreas`;

-- Tabla de usuarios (cuentas principales con nickname)
CREATE TABLE IF NOT EXISTS `users` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `nickname` VARCHAR(24) NOT NULL,
    `password` VARCHAR(64) NOT NULL,
    `salt` VARCHAR(32) NOT NULL,
    `email` VARCHAR(100) DEFAULT NULL,
    `admin_level` INT DEFAULT 0,
    `registered` DATETIME DEFAULT CURRENT_TIMESTAMP,
    `last_login` DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`id`),
    UNIQUE KEY `nickname` (`nickname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Tabla de personajes (cada usuario puede tener múltiples personajes)
CREATE TABLE IF NOT EXISTS `characters` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `user_id` INT NOT NULL,
    `name` VARCHAR(24) NOT NULL,
    `money` INT DEFAULT 5000,
    `bank` INT DEFAULT 10000,
    `pos_x` FLOAT DEFAULT 1759.0189,
    `pos_y` FLOAT DEFAULT -1898.1260,
    `pos_z` FLOAT DEFAULT 13.5622,
    `pos_a` FLOAT DEFAULT 266.4503,
    `interior` INT DEFAULT 0,
    `virtual_world` INT DEFAULT 0,
    `health` FLOAT DEFAULT 100.0,
    `armour` FLOAT DEFAULT 0.0,
    `skin` INT DEFAULT 0,
    `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP,
    `last_played` DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`id`),
    UNIQUE KEY `name` (`name`),
    FOREIGN KEY (`user_id`) REFERENCES `users`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Tabla de vehículos (opcional para futuro)
CREATE TABLE IF NOT EXISTS `vehicles` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `character_id` INT NOT NULL,
    `model` INT NOT NULL,
    `color1` INT DEFAULT -1,
    `color2` INT DEFAULT -1,
    `pos_x` FLOAT DEFAULT 0.0,
    `pos_y` FLOAT DEFAULT 0.0,
    `pos_z` FLOAT DEFAULT 0.0,
    `pos_a` FLOAT DEFAULT 0.0,
    `fuel` FLOAT DEFAULT 100.0,
    `locked` TINYINT DEFAULT 0,
    PRIMARY KEY (`id`),
    FOREIGN KEY (`character_id`) REFERENCES `characters`(`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Confirmación
SELECT 'Base de datos actualizada exitosamente!' as STATUS;
