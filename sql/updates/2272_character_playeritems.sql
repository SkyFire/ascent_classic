alter table playeritems add column wrapped_item_id int(30) not null default 0 after entry;
alter table playeritems_external add column wrapped_item_id int(30) not null default 0 after entry;

alter table playeritems add column wrapped_creator int(30) not null default 0 after wrapped_item_id;
alter table playeritems_external add column wrapped_creator int(30) not null default 0 after wrapped_item_id;