ALTER TABLE creature_names CHANGE mouseFlags info_str VARCHAR(500) NOT NULL;
UPDATE creature_names set info_str=""