#include "commands.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <getopt.h>

#define CURRENT_CONFIG_FILE ".codaconf"
#define TEMP_CONFIG_FILE ".codaconf.tmp"
#define BUFFER_SIZE 512

//struct used by getopt_long to have long and short flags
static struct option long_options[] = {
  {"album", required_argument, 0, 'a'},
  {"artist", required_argument, 0, 'r'},
  {"date", required_argument, 0, 'd'},
  {0, 0, 0, 0}
};

/** usage_info
 *  Prints out a generic message about how to use the 
 *  program, this is used as the default message
 *  when an unknown argument is used.
 */
void usage_info(const char* prog){
  fprintf(stderr,
    "Usage:\n"
    " %s init <database name>.db\n"
    " %s add {-a|--album} <name> {-r|--artist} <name> [{-d|--date} <date>]\n"
    " %s edit <id> [{-a|--album} <name>] [{-r|--artist} <name>] [{-d|--date} <date>]\n"
    " %s search {-a|--album} <name> | {-r|--artist} <name>\n"
    " %s list [{-a|--album}] [{-r|--artist}] [{-d|--date}]\n"
    " %s remove <id>\n",
    prog, prog, prog, prog, prog, prog
  );
}

/** save_current_db_name 
  * Saves a given name for a database into
  * a local, prenamed, configuration file.
  * Used to reaccess a specifically named database later.
  */
int save_current_db_name(const char* dbName){
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
int load_current_db_name(char* buffer, size_t size){
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
int open_current_db(sqlite3** db){
  char dbName[BUFFER_SIZE];
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

/** cmd_init
  * takes in a string for a database name, currently should
  * end with .db. Init will attempt to open the database, if 
  * it doesn't exist yet a new one will be created. It will then
  * initiatiate a table for the Albums, this is only 
  * done if the table doesn't already exist.
  *
  * init also uses the save_current_db() function to
  * save the database name to the local .codaconf file
  * which is later referenced by open_current_db()
  * 
*/
int cmd_init(char* dbName){
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

/** cmd_add
  * This function is used to add new entries into the albums table
  * Currently, only title and artist are mandatory fields, 
  * the rest can be left blank and will be NULL in the table.
  */
int cmd_add(int argc, char *argv[]){
  sqlite3* db = NULL;
  if(open_current_db(&db) != SQLITE_OK){
    return 1;
  }

  char* title = NULL; //mandatory
  char* artist = NULL; //mandatory
  char* date = NULL; //optional
  int opt;
  int rc;
  //processing flags.
  while ((opt = getopt_long(argc, argv, "a:r:d:",long_options, NULL)) != -1){
    switch(opt){
      case 'a':
        title = optarg;
        break;

      case 'r':
        artist = optarg;
        break;

      case 'd':
        date = optarg;
        break;

      default:
        fprintf(stderr, 
                "Usage: %s {-a|--album} <name> {-r|--artist} <name> [{-d|--date} <date>]\n",
                argv[0]);
        return 1;
    }
  }
  if (title == NULL || artist == NULL) {
    fprintf(stderr, "Error: album and artist are required.\n");
    sqlite3_close(db);
    return 1;
  }

  //template sql command to be populated by sqlite3_bind_text()
  const char *sql = "INSERT INTO ALBUMS (Title, Artist, Date) VALUES (?, ?, ?);";
  sqlite3_stmt *stmt = NULL;
  
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); //make a statement using template
  if(rc != SQLITE_OK){
    fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  sqlite3_bind_text(stmt, 1, title, -1, SQLITE_TRANSIENT); //fill slot 1
  sqlite3_bind_text(stmt, 2, artist, -1, SQLITE_TRANSIENT); //fill slot 2

  if(date){
    sqlite3_bind_text(stmt, 3, date, -1, SQLITE_TRANSIENT); //if we have date fill 3
  }else{
    sqlite3_bind_null(stmt, 3); //if not set it to null
  }

  rc = sqlite3_step(stmt); //run the filled statement
  if (rc != SQLITE_DONE){
    fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt); //clean and free the statement
    sqlite3_close(db);
    return 1;
  }

  sqlite3_finalize(stmt);
  printf("Successfully added %s to collection!\n", title);
  sqlite3_close(db);
  
  return 0;
}

//sample function from the sqlite documentation
static int callback(void *NotUsed, int argc, char *argv[], char *azColName[]){
  for (int i = 0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

/** cmd_list
  * this function retrieves the albums table and uses the callback() function
  * in order to print out every entry. 
  *
  * TODO: implement sorting by column and improve the printout.
  */
int cmd_list(){
  sqlite3* db = NULL;
  if(open_current_db(&db) != SQLITE_OK){
    return 1;
  }

  const char* sqlCommand = "SELECT * FROM Albums";

  char* errMsg = 0;
  int rc = sqlite3_exec(db, sqlCommand, callback, 0, &errMsg);
  if( rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
  }
  sqlite3_close(db);

  return 0;

}
