mqtt2emoncms
============
mqtt2emoncms is designed to allow sensor readings published via MQTT to be pushed into Emoncms v6 for graphing.

Prerequisites
-------------
* Emoncms installed and working
* Usual build tools installed (gcc, g++, etc)
* libmosquitto (tested with 1.2-0mosquitto1)
* libcurl
* libmysqlclient-dev

Installation
------------
1) Edit mqtt2emoncms.conf, filling in the MQTT broker, MySQL details and Emoncms URL and API key
2) Build by running `make all`
3) Run `make updatedb` to update the Emoncms database to include the required extra table + stored procedure.

Configuration
-------------
First, create the input in Emoncms, and add to feeds as required.
Then, using phpmyadmin (or the MySQL CLI client, whatever), find the id of the input from the `input` table, and add a mapping for it to the new mqtt_mappings table, e.g. for input.id=1:
`insert into mqtt_mappings (mqtt_topic, input_id) values ('mqtt/topic/sensor', 1);`.

Run
---
Start mqtt2emoncms by running `./mqtt2emoncms -d -c mqtt2emoncms.conf`. It should connect to MySQL & Mosquitto, and start forwarding readings received via MQTT to Emoncms. The -d flag is debug - run without it to daemonize.

Also see
--------
* Emoncms - http://emoncms.org/
* Mosquitto MQTT broker - http://mosquitto.org/
