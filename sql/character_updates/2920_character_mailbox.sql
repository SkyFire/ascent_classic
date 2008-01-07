alter table mailbox drop column external_attached_item_guid;
alter table mailbox change column attached_item_guid attached_item_guids varchar(200) not null default "";
drop table playeritems_external;