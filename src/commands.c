/** @file
 *  @ingroup commands
 *  Implementation of the command handling.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "commands.h"
#include "defaults.h"

/** List of known commands.
 */
/* @{ */
typedef struct _CmdList CmdList;
struct _CmdList {
  char cmd_name[32];              /**< The name of the command. */
  CMD_CALLBACK cmd_callback;      /**< Callback function to run the command. */
  CMD_HELP_CALLBACK cmd_help;     /**< Callback function to print help for that command. */
  CmdList *next;                  /**< Next command in list. */
};
/* @} */

CmdList *_cmd_list = NULL;

CmdList* _command_exists(char *name);

/** Add a command to the list of known commands.
 *  @param[in] name The name for that command.
 *  @param[in] cb The callback function to run the command or NULL.
 *  @param[in] help_cb The callback function to print help for the command or NULL.
 *  @return 0 on success, 1 if an error occured.
 */
int command_add(char *name, CMD_CALLBACK cb, CMD_HELP_CALLBACK help_cb) {
  CmdList *ins = NULL;
  if (!name || _command_exists(name)) {
    return 1;
  }
  ins = malloc(sizeof(CmdList));
  assert(ins);
  
  memset(ins, 0, sizeof(CmdList));
  ins->next = _cmd_list;
  _cmd_list = ins;
  strcpy(ins->cmd_name, name);
  ins->cmd_callback = cb;
  ins->cmd_help = help_cb;
  
  return 0;
}

/** Check whether a given command exists.
 *  @param[in] name The name of the command.
 *  @return The list element of the command if found or NULL if
 *          the command has not been added yet.
 *  @internal
 */
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

/** Call a command as specified in the command line.
 *  @param[in] argc The number of command line arguments.
 *  @param[in] argv The command line arguments.
 *  @return 1 if an error occured, otherwise the return value of
 *          the called command.
 */
int command_call(int argc, char **argv) {
  unsigned char *cmd_name;
  CmdList *cmd = NULL;
  if (argc == 1) {
    defaults_get("cmd.default", &cmd_name);
    if (cmd_name) {
      cmd = _command_exists(cmd_name);
      free(cmd_name);
    }
    else {
      fprintf(stderr, "No default command specified.\n");
    }
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

/** Invoke the helper function for a command if one exists.
 *  @param[in] name The name of the command.
 *  @return 0 on success 1 if an error occured.
 */
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
