#include "commands.h"
#include <stdio.h>
#include <sqlite3.h>

void usage_info(const char *prog){
  fprintf(stderr,
    "Usage:\n"
    " %s add [-a album] [-A artist] [-d date]\n"
    " %s edit <id> [-a album] [-A artist] [-d date]\n"
    " %s search [-a album] [-A artist]\n"
    " %s remove <id>\n",
    prog, prog, prog, prog
  );
}

int cmd_init(char *dbName){
  sqlite3* db;

  const char *sql_create_table =  "CREATE TABLE IF NOT EXISTS Albums(" \
                                  "AlbumID INT PRIMARY KEY AUTOINCREMENT," \
                                  "Title TEXT NOT NULL," \
                                  "Artist TEXT NOT NULL," \
                                  "Date DATE);";

  int exit = 0;
  exit = sqlite3_open(dbName, &db);
  char* messageError;
  exit = sqlite3_exec(db, sql_create_table, NULL, 0, &messageError);

  if(exit != SQLITE_OK){
    fprintf(stderr, "Error Creating Table");
    sqlite3_free(messageError);
  }else{
    printf("Table created successfully");
    sqlite3_close(db);
  }
  return 0;
}
