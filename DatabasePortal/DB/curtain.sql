CREATE DATABASE `curtain`;

USE `curtain`;

DROP TABLE IF EXISTS `curtains`;
CREATE TABLE IF NOT EXISTS `curtains` (
	`curtain_id` int(11) AUTO_INCREMENT NOT NULL PRIMARY KEY,
	`curtain_activated` BOOLEAN DEFAULT FALSE,
	`curtain_direction` BOOLEAN DEFAULT FALSE,
	`curtain_last_connection` DATETIME DEFAULT CURRENT_TIMESTAMP,
	`curtain_length` BIGINT UNSIGNED NOT NULL,
	`curtain_name` varchar(25) DEFAULT NULL,
	`curtain_position` BIGINT UNSIGNED NOT NULL
);


INSERT INTO `curtains` (`curtain_length`, `curtain_position`, `curtain_direction`, `curtain_name`) VALUES
(1000, 500, FALSE, 'Office');



DROP TABLE IF EXISTS `error_log`;
CREATE TABLE IF NOT EXISTS `error_log` (
	`error_log_key` int(11) AUTO_INCREMENT NOT NULL PRIMARY KEY,
	`curtain_id` int(11) NOT NULL,
	`curtain_position` BIGINT UNSIGNED NOT NULL,
	`event_position` BIGINT UNSIGNED NOT NULL,
	`error` text DEFAULT NULL,
	`path` text DEFAULT NULL,
	`time` datetime DEFAULT CURRENT_TIMESTAMP
) CHARSET=utf8;


DROP TABLE IF EXISTS `events`;
CREATE TABLE IF NOT EXISTS `events` (
	`event_id` int(11) AUTO_INCREMENT NOT NULL PRIMARY KEY,
	`curtain_id` int(11) NOT NULL,
	`event_position` BIGINT UNSIGNED NOT NULL,
	`event_activated` BOOLEAN DEFAULT TRUE,
	`event_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) CHARSET=utf8;


DROP TABLE IF EXISTS `options`;
CREATE TABLE IF NOT EXISTS `options` (
	`curtain_id` int(11) AUTO_INCREMENT NOT NULL PRIMARY KEY,
	`adafruit_feed_close` varchar(35) DEFAULT NULL,
	`adafruit_feed_open` varchar(35) DEFAULT NULL,
	`auto_calibration` BOOLEAN DEFAULT FALSE,
	`auto_correct` BOOLEAN DEFAULT FALSE,
	`event_predictor` BOOLEAN DEFAULT FALSE,
	`sunrise_open` BOOLEAN DEFAULT FALSE,
	`sunset_close` BOOLEAN DEFAULT FALSE
);


INSERT INTO `options` (`adafruit_feed_open`, `adafruit_feed_close`, `auto_calibration`, 
		`auto_correct`, `event_predictor`, `sunrise_open`, `sunset_close`) VALUES
(NULL, NULL, TRUE, TRUE, FALSE, FALSE, TRUE);