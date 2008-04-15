alter table characters drop column honorPointsToAdd;
alter table characters add column honorRolloverTime INT(30) NOT NULL DEFAULT 0 AFTER finished_quests;