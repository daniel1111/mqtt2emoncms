#include <stdio.h>
#include "CNHDBAccess.h"
#include "CLogging.h"


using namespace std;

int main()
{

  CNHDBAccess *db;
  CLogging *log;
  int a=-99;
  dbrows rs;
  
  log = new CLogging();
        
  db = new CNHDBAccess("localhost", "eve_run", "123", "eve", log);   

  db->dbConnect();
  
  db->sp_get_devices(a, &rs);
  

  for (dbrows::const_iterator iterator = rs.begin(), end = rs.end(); iterator != end; ++iterator) 
  {
    dbrow row = *iterator;
    
    
    cout << row["device_id"].asStr() + "\t";
    cout << row["device_description"].asStr() + "\t";
    cout << row["mqtt_topic"].asStr() + "\t\n";
  }  
  
  printf("Done, a=[%d]\n", a);
    db->sp_get_devices(a, &rs);
  
  delete db;
  delete log;

  return 0;
}





