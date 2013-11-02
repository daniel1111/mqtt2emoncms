drop procedure if exists sp_get_mqtt_mappings;

/* Get a list of mqtt topics and the corresponding node/input name. */

DELIMITER //
CREATE PROCEDURE sp_get_mqtt_mappings
(
  IN p_topic  varchar(255)
)
SQL SECURITY DEFINER
BEGIN

  select i.nodeid, i.name, m.mqtt_topic
  from mqtt_mappings m
  inner join input i
     on m.input_id = i.id
  where m.mqtt_topic = p_topic or length(ifnull(p_topic, '')) = 0;
    
END //
DELIMITER ;
