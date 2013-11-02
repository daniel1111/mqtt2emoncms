drop table if exists mqtt_mappings;

create table mqtt_mappings
(
  mapping_id      int not null auto_increment,
  mqtt_topic      varchar(255) not null,
  input_id        int,
  primary key (mapping_id)
);
