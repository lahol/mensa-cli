#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "defaults.h"

typedef struct _CmdList CmdList;
struct _CmdList {
  char cmd_name[32];
  CMD_CALLBACK cmd_callback;
  CMD_HELP_CALLBACK cmd_help;
  CmdList *next;
};

CmdList *_cmd_list = NULL;

CmdList* _command_exists(char *name);

int command_add(char *name, CMD_CALLBACK cb, CMD_HELP_CALLBACK help_cb) {
  CmdList *ins = NULL;
  if (!name || _command_exists(name)) {
    return 1;
  }
  ins = malloc(sizeof(CmdList));
  if (!ins) {
    exit(1);
  }
  memset(ins, 0, sizeof(CmdList));
  ins->next = _cmd_list;
  _cmd_list = ins;
  strcpy(ins->cmd_name, name);
  ins->cmd_callback = cb;
  ins->cmd_help = help_cb;
  
  return 0;
}

CmdList* _command_exists(char *name) {
  CmdList *tmp = _cmd_list;
  if (!name) return NULL;
  while (tmp) {
    if (!strcmp(tmp->cmd_name, name)) {
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}

int command_call(int argc, char **argv) {
  char cmd_name[32];
  CmdList *cmd = NULL;
  if (argc == 1) {
    defaults_get("cmd.default", cmd_name, 32);
    cmd = _command_exists(cmd_name);
  }
  else {
    cmd = _command_exists(argv[1]);
  }
  if (!cmd) {
    fprintf(stderr, "Command not found.\n");
    return 1;
  }
  
  if (cmd->cmd_callback) {
    return (*(cmd->cmd_callback))(argc, argv);
  }
  return 1;
}

int command_help(char *name) {
  CmdList *cmd = _command_exists(name);
  if (!cmd) {
    fprintf(stderr, "Command %s not found.\n", name);
    return 1;
  }
  if (cmd->cmd_help) {
    (*(cmd->cmd_help))();
    return 0;
  }
  else {
    fprintf(stderr, "No help entry for command %s.\n", name);
    return 0;
  }
}
