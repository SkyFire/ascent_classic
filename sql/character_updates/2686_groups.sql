alter table groups add column difficulty int(30) not null default 0 after loot_threshold;
alter table groups add column assistant_leader int(30) not null default 0 after difficulty;
alter table groups add column main_tank int(30) not null default 0 after assistant_leader;
alter table groups add column main_assist int(30) not null default 0 after main_tank;
