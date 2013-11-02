
SRC_DIR = cpp/
DATABASE = database/
BUILD_DIR = build/
OBJ = Cmqtt2emoncms.o CLogging.o CNHmqtt.o ini.o INIReader.o CNHDBAccess.o CDBValue.o
OBJS   := $(addprefix $(BUILD_DIR),$(OBJ))
CC_OUT = -o $(BUILD_DIR)$(notdir $@)

CFLAGS = -Wall -c -g
LFLAGS = -Wall -g
CC = g++

all: mqtt2emoncms

dbupdate: 
	php database/db_load.php INSTALL

mqtt2emoncms : $(OBJS)
	$(CC) $(LFLAGS) -lmosquitto -lrt -lmysqlclient -lcurl $(OBJS) -o mqtt2emoncms


$(BUILD_DIR)Cmqtt2emoncms.o: $(SRC_DIR)Cmqtt2emoncms.cpp $(SRC_DIR)Cmqtt2emoncms.h $(DATABASE)lib/CNHDBAccess.cpp $(DATABASE)lib/CNHDBAccess.h
	echo $@
	$(CC) $(CFLAGS) $(SRC_DIR)Cmqtt2emoncms.cpp $(CC_OUT)

$(BUILD_DIR)CLogging.o: $(SRC_DIR)CLogging.cpp $(SRC_DIR)CLogging.h
	$(CC) $(CFLAGS) $(SRC_DIR)CLogging.cpp $(CC_OUT)

$(BUILD_DIR)CNHmqtt.o: $(SRC_DIR)CNHmqtt.cpp $(SRC_DIR)CNHmqtt.h
	$(CC) $(CFLAGS) $(SRC_DIR)CNHmqtt.cpp $(CC_OUT)

$(BUILD_DIR)ini.o: $(SRC_DIR)inireader/ini.c $(SRC_DIR)inireader/ini.h
	gcc -c $(SRC_DIR)inireader/ini.c $(CC_OUT)

$(BUILD_DIR)INIReader.o: $(SRC_DIR)inireader/INIReader.cpp $(SRC_DIR)inireader/INIReader.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)inireader/INIReader.cpp $(CC_OUT)



# Database library
$(DATABASE)lib/gen_dblib: $(DATABASE)lib/gen_dblib.c
	gcc $(LFLAGS) $(DATABASE)lib/gen_dblib.c -o $(DATABASE)lib/gen_dblib

$(DATABASE)lib/CNHDBAccess.cpp: $(DATABASE)lib/gen_dblib $(DATABASE)lib/CNHDBAccess_template.cpp $(wildcard $(DATABASE)sp/sp_*.sql) 
	$(DATABASE)lib/gen_dblib $(DATABASE)lib $(wildcard $(DATABASE)sp/sp_*.sql)

$(DATABASE)lib/CNHDBAccess.h: $(DATABASE)lib/gen_dblib $(DATABASE)/lib/CNHDBAccess_template.h $(wildcard $(DATABASE)sp/sp_*.sql)
	$(DATABASE)lib/gen_dblib $(DATABASE)lib $(wildcard $(DATABASE)sp/sp_*.sql)

$(DATABASE)lib/CNHDBAccess.php: $(DATABASE)lib/gen_dblib $(DATABASE)lib/CNHDBAccess_template.php $(wildcard $(DATABASE)sp/sp_*.sql)
	$(DATABASE)lib/gen_dblib $(DATABASE)lib $(wildcard $(DATABASE)sp/sp_*.sql)

$(BUILD_DIR)CNHDBAccess.o: $(DATABASE)lib/CNHDBAccess.cpp $(DATABASE)lib/CNHDBAccess.h $(DATABASE)lib/CDBValue.h
	g++ $(CFLAGS) $(DATABASE)lib/CNHDBAccess.cpp $(CC_OUT)

$(BUILD_DIR)CDBValue.o: $(DATABASE)lib/CDBValue.cpp $(DATABASE)lib/CDBValue.h
	g++ $(CFLAGS) $(DATABASE)lib/CDBValue.cpp $(CC_OUT)


# db test app
dbtest : $(BUILD_DIR)dbtest.o $(BUILD_DIR)CNHDBAccess.o $(BUILD_DIR)CLogging.o $(BUILD_DIR)CDBValue.o 
	$(CC) $(LFLAGS) -lmysqlclient $(BUILD_DIR)dbtest.o $(BUILD_DIR)CLogging.o $(BUILD_DIR)CNHDBAccess.o $(BUILD_DIR)CDBValue.o -o dbtest

$(BUILD_DIR)dbtest.o: $(SRC_DIR)dbtest.cpp $(DATABASE)lib/CNHDBAccess.cpp $(DATABASE)lib/CNHDBAccess.h
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(SRC_DIR)dbtest.cpp $(CC_OUT)


clean:
	rm -fv mqtt2emoncms $(BUILD_DIR)*.o
	rm -fv $(DATABASE)lib/*.o $(DATABASE)lib/gen_dblib $(DATABASE)lib/CNHDBAccess.*