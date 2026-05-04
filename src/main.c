#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"

int main(int argc, char *argv[]){
  //Arguments:
  //-h, --help: description and flags available
  //-t, --title
  //-a, --artist
  //-d, --date YYYY-MM-DD
  
  if(argc < 2){
    fprintf(stderr, "Missing comand.\n");
    usage_info(argv[0]);
    return 1;
  }

  printf("\n");
  if(strcmp(argv[1], "add") == 0){
    cmd_add(argc, argv);
  }else if(strcmp(argv[1], "list") == 0){
    cmd_list(argc, argv);
  }else if(strcmp(argv[1], "init") == 0){
    if(argv[2]){
      cmd_init(argv[2]);
    }else{
      cmd_init("coda.db");
    }
  }else if(strcmp(argv[1], "edit") == 0){
    cmd_edit(argc, argv);
  }else if(strcmp(argv[1], "search") == 0){
    printf("search not yet implemented");
  }else if(strcmp(argv[1], "remove") == 0){
    cmd_remove(atoi(argv[2]));
  }else{
    usage_info(argv[0]);
  }

  printf("\n");
  return 0;
}
