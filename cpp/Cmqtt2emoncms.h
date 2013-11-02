bool CNHmqtt::debug_mode = false;
bool CNHmqtt::daemonized = false;

#include "CNHDBAccess.h"

class Cmqtt2emoncms : public CNHmqtt
{

  public:
    Cmqtt2emoncms(int argc, char *argv[]);
    ~Cmqtt2emoncms();

    void process_message(std::string topic, std::string message);
    int dblist_subscribe();
    int go();
    int emoncms_update(int node, std::string name, std::string reading);

  private:
    CNHDBAccess *_db;
    dbrows _mappings;
    std::string _apikey;
    std::string _url;
};
