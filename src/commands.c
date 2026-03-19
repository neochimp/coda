#include "commands.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#define CURRENT_CONFIG_FILE ".codaconf"
#define TEMP_CONFIG_FILE ".codaconf.tmp"

/** usage_info
 *  Prints out a generic message about how to use the 
 *  program, this is used as the default message
 *  when an unknown argument is used.
 */
void usage_info(const char *prog){
  fprintf(stderr,
    "Usage:\n"
    " %s init <database name>.db\n"
    " %s add [-a album] [-A artist] [-d date]\n"
    " %s edit <id> [-a album] [-A artist] [-d date]\n"
    " %s search [-a album] [-A artist]\n"
    " %s list [-a, --album] [-A, --artist] [-d, --Date] \n"
    " %s remove <id>\n",
    prog, prog, prog, prog, prog, prog
  );
}

/** save_current_db_name 
  * Saves a given name for a database into
  * a local, prenamed, configuration file.
  * Used to reaccess a specifically named database later.
  */
int save_current_db_name(const char *dbName){
  FILE *in = fopen(CURRENT_CONFIG_FILE, "r");
  FILE *out = fopen(TEMP_CONFIG_FILE, "w");
  char line[1024];
  int found = 0;

  if (!out) {
    perror("fopen temp file");
    if(in) fclose(in);
    return 1;
  }

  if(in){
    while(fgets(line, sizeof(line), in)){
      if(strncmp(line, "CURRENT_DB", 10) == 0){
        fprintf(out, "CURRENT_DB %s\n", dbName);
        found = 1;
      }else{
        fputs(line, out);
      }
    }
    fclose(in);
  }

  if(!found){
    fprintf(out, "CURRENT_DB %s\n", dbName);
  }

  fclose(out);

  if(remove(CURRENT_CONFIG_FILE) != 0){
    //ignore error if file doesn't exist
  }

  if(rename(TEMP_CONFIG_FILE, CURRENT_CONFIG_FILE) != 0){
    perror("rename");
    return 1;
  }

  return 0;
}

/** load_current_db_name
  * Reads the prenamed config file for
  * an existing database name. 
  * Returns 1 if none exists.
  */
int load_current_db_name(char *buffer, size_t size){
  FILE *f = fopen(CURRENT_CONFIG_FILE, "r");
  char line[1024];
  char key[256];
  char value[768];

  if (!f){
    return 1;
  }

  while(fgets(line, sizeof(line), f)){
    if(sscanf(line, "%255s %767s", key, value) == 2){
      if(strcmp(key, "CURRENT_DB") == 0){
        strncpy(buffer, value, size - 1);
        buffer[size - 1] = '\0';
        fclose(f);
        return 0;
      }
    }
  }

  fclose(f);
  return 1;
}

/** open_current_db
 *  takes in double pointer for a database and assigns it
 *  the database specified by the name saved by cmd_init();
 *  This should be used at the start of every database command
 *  in order to ensure you are working in the correct database.
  */
int open_current_db(sqlite3 **db){
  char dbName[512];
  if(load_current_db_name(dbName, sizeof(dbName)) != 0){
    fprintf(stderr, "No database selected. Run init first.\n");
    return 1;
  }

  int rc = sqlite3_open(dbName, db);
  if(rc != SQLITE_OK){
    fprintf(stderr, "Can't open database '%s': %s\n", dbName, sqlite3_errmsg(*db));
    if (*db) sqlite3_close(*db);
    return rc;
  }
  return SQLITE_OK;

}

int cmd_init(char *dbName){
  sqlite3* db = NULL;
  char* errMsg = NULL;

  int rc = sqlite3_open(dbName, &db);
  if(rc != SQLITE_OK){
    fprintf(stderr, "Cannot open database '%s': %s\n", dbName, sqlite3_errmsg(db));
    if(db) sqlite3_close(db);
    return rc;
  }

  const char *sql_create_table =  "CREATE TABLE IF NOT EXISTS Albums("
                                  "AlbumID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "Title TEXT NOT NULL, "
                                  "Artist TEXT NOT NULL, "
                                  "Date DATE);";

  rc = sqlite3_exec(db, sql_create_table, NULL, 0, &errMsg);
  if(rc != SQLITE_OK){
    fprintf(stderr, "Error Creating Table %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);
    return rc;
  }

  sqlite3_close(db);

  if(save_current_db_name(dbName) != 0){
    fprintf(stderr, "Failed to save current database name.\n");
    return 1;
  }

  printf("Initialized and selected database: %s\n", dbName);
  return 0;
}


