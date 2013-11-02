/* 
 * Copyright (c) 2013, Daniel Swann <hs@dswann.co.uk>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "CNHmqtt.h"
#include "Cmqtt2emoncms.h"
#include <curl/curl.h>

CLogging *log;

using namespace std;

Cmqtt2emoncms::Cmqtt2emoncms(int argc, char *argv[]) : CNHmqtt(argc, argv)
{
  /* Read config, with some defaults if config missing */
  _db = new CNHDBAccess(get_str_option("mysql", "server", "localhost"), 
                        get_str_option("mysql_runtime", "username", "root"), 
                        get_str_option("mysql_runtime", "password", "root"), 
                        get_str_option("mysql", "database", "emoncms"), 
                        log);

  _apikey = get_str_option("emoncms", "apikey", "(nokey)");
  _url    = get_str_option("emoncms", "url"   , "http://127.0.0.1/emoncms/");

}

Cmqtt2emoncms::~Cmqtt2emoncms()
{
  if (_db != NULL)
    delete _db;
}

void Cmqtt2emoncms::process_message(string topic, string message)
/* Process incoming MQTT message */
{
  /* Loop through topics from database, and update emoncms on match */
  for (dbrows::const_iterator iterator = _mappings.begin(), end = _mappings.end(); iterator != end; ++iterator) 
  {
    dbrow row = *iterator;
    if (row["mqtt_topic"].asStr() == topic)
      emoncms_update(row["nodeid"].asInt(), row["name"], message);
  }

    CNHmqtt::process_message(topic, message);
}

int Cmqtt2emoncms::emoncms_update(int node, string name, string reading)
/* Update emoncms with reading for input <name> on <node> */
{
  CURL *curl;
  CURLcode res;
  string url;

  log->dbg("emoncms_update> node=[" + itos(node) + "], name=[" + name + "], reading=[" + reading + "]");

  url =  _url + "input/post.json?";
  url += "node=" + itos(node);
  url += "&json={" + name + ":" + reading + "}";
  url += "&apikey=" + _apikey;

  curl = curl_easy_init();
  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
      log->dbg("emoncms_update> curl error:  " + (string)curl_easy_strerror(res));

    curl_easy_cleanup(curl);
  } else
  {
    log->dbg("emoncms_update> curl_easy_init failed");
  }

  return 0;
}

int Cmqtt2emoncms::go()
/* Connect to mosquitto, MySQL, daemonize, then start processing incoming messages */
{
  mosq_connect();
  Cmqtt2emoncms::daemonize();
  _db->dbConnect();

  dblist_subscribe();

  //process any mqtt messages
  while (!message_loop(50));

  return 0;
}

int Cmqtt2emoncms::dblist_subscribe()
{
  if (_db->sp_get_mqtt_mappings("", &_mappings))
  {
    log->dbg("Failed to read topics to subscribe to from database!");
    return -1;
  }

  /* Subscribe to mapped topics in database */
  for (dbrows::const_iterator iterator = _mappings.begin(), end = _mappings.end(); iterator != end; ++iterator) 
  {
    dbrow row = *iterator;
    subscribe(row["mqtt_topic"]);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int ret;
  log = new CLogging();  
  Cmqtt2emoncms mqtt2emoncms = Cmqtt2emoncms(argc, argv);
  ret = mqtt2emoncms.go();
  delete log;
  return ret;
}