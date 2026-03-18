#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include "commands.h"

static int callback(void *NotUsed, int argc, char *argv[], char *azColName[]){
  for (int i = 0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int main(int argc, char *argv[]){
  //Arguments:
  //-h, --help: description and flags available
  //-a, --album
  //-A, --artist
  //-d, --date YYYY-MM-DD
  
 // if(argc < 2){
 //   fprintf(stderr, "Missing comand.\n");
 //   usage_info(argv[0]);
 //   return 1;
 // }
  

  //example quickstart from documentation
/*
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if( argc!= 3) {
    fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
    return(1);
  }
  rc = sqlite3_open(argv[1], &db);
  if (rc){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }
  rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
  if( rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);
*/

  return 0;
}
