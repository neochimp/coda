#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

//TODO: create a .config/coda directory and copy a "template codaconf there"
#define CURRENT_CONFIG_FILE "/home/soup/dev/repos/coda/.codaconf"
#define TEMP_CONFIG_FILE ".codaconf.tmp"
#define DB_DIRECTORY "databases/"

#define DB_PATH_SIZE 1024
#define BUFFER_SIZE 512
#define KEY_SIZE 254

int load_db_directory(char* buffer, size_t size);
int save_current_db_name(const char* db_name);
int open_current_db(sqlite3** db);

#endif
