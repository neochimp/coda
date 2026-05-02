#ifndef COMMANDS_H
#define COMMANDS_H

#include <sqlite3.h>
#include <getopt.h>

void usage_info(const char *prog);
int cmd_add(int argc, char *argv[]);
int cmd_search(int argc, char *argv[]);
int cmd_edit(int argc, char *argv[]);
int cmd_list();
int cmd_remove(int id);
int cmd_init(char *db_name);

int open_current_db(sqlite3 **db);
#endif
