#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "mensa-output.h"
#include "utils.h"

int cmd_help(int argc, char **argv) {
  if (argc <= 2) {
    printf("Usage: %s [cmd [args]]\n", argv[0]);
    printf(
      "\nCommands: \n"
      "    help            Show this help. \n"
      "    info            Show information about this program. \n"
      "    show            Show meals. \n"
      "    config          Show configuration or set values. \n"
      "\n"    
      "Use %s help cmd to show help about this command.\n", argv[0]);
    return 0;
  }
  else if (argc > 2) {
    return command_help(argv[1]);
  } 
}
