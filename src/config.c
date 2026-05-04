#include "config.h"

/** load_config_value
  * generic helper for reading config file
  */
int load_config_value(const char* wanted_key, char* buffer, size_t size) {
  FILE *f = fopen(CURRENT_CONFIG_FILE, "r");
  char line[BUFFER_SIZE];
  char key[KEY_SIZE];
  char value[BUFFER_SIZE];

  if (!f) {
    return 1;
  }

  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%253s %511s", key, value) == 2) {
      if (strcmp(key, wanted_key) == 0) {
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

int load_current_db_name(char* buffer, size_t size) {
  return load_config_value("CURRENT_DB", buffer, size);
}

int load_db_directory(char* buffer, size_t size) {
  return load_config_value("DB_DIRECTORY", buffer, size);
}

/** save_config_value 
  * Saves a new config value
  * a local, prenamed, configuration file.
  * Used to reaccess a database later.
  */
int save_config_value(const char* target_key, const char* target_value) {
  FILE *in = fopen(CURRENT_CONFIG_FILE, "r");
  FILE *out = fopen(TEMP_CONFIG_FILE, "w");
  char line[BUFFER_SIZE];
  char key[KEY_SIZE];
  int found = 0;

  if (!out) {
    perror("fopen temp file");
    if (in) fclose(in);
    return 1;
  }

  if (in) {
    while (fgets(line, sizeof(line), in)) {
      if (sscanf(line, "%253s", key) == 1 && strcmp(key, target_key) == 0) {
        fprintf(out, "%s %s\n", target_key, target_value);
        found = 1;
      } else {
        fputs(line, out);
      }
    }
    fclose(in);
  }

  if (!found) {
    fprintf(out, "%s %s\n", target_key, target_value);
  }

  fclose(out);

  remove(CURRENT_CONFIG_FILE);

  if (rename(TEMP_CONFIG_FILE, CURRENT_CONFIG_FILE)  != 0) {
    perror("rename");
    return 1;
  }

  return 0;
}

/** save_current_db_name
  * save the database name called by init to the config file
  */ 
int save_current_db_name(const char* db_name) {
  return save_config_value("CURRENT_DB", db_name);
}

/** open_current_db
 *  takes in double pointer for a database and assigns it
 *  the database specified by the name saved by cmd_init();
 *  This should be used at the start of every database command
 *  in order to ensure you are working in the correct database.
  */
int open_current_db(sqlite3** db) {
  char db_dir[BUFFER_SIZE];
  char db_name[BUFFER_SIZE];
  char filepath[DB_PATH_SIZE];

  if (load_db_directory(db_dir, sizeof(db_dir)) != 0) {
    fprintf(stderr, "No database directory configured.\n");
    return 1;
  }
  if (load_current_db_name(db_name, sizeof(db_name)) != 0) {
    fprintf(stderr, "No database selected. Run init first.\n");
    return 1;
  }

  snprintf(filepath, sizeof(filepath), "%s/%s", db_dir, db_name);

  int rc = sqlite3_open(filepath, db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database '%s': %s\n", filepath, sqlite3_errmsg(*db));
    if (*db) sqlite3_close(*db);
    return rc;
  }
  return SQLITE_OK;

}
