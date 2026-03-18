#include "commands.h"
#include <stdio.h>

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
