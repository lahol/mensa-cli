/** @defgroup commands Command handling.
 *  @ingroup mensacli
 *  @file
 *  @ingroup commands
 *  Header file for the main command handling.
 */
#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/** @typedef CMD_CALLBACK 
 * Callback for a subcommand of mensa.
 */
typedef int (*CMD_CALLBACK)(int, char**);
/** @typedef CMD_HELP_CALLBACK
 *  Callback to print help text for a command.
 */
typedef void (*CMD_HELP_CALLBACK)(void);

int command_add(char *name, CMD_CALLBACK cb, 
  CMD_HELP_CALLBACK help_cb);
  
int command_call(int argc, char **argv);
int command_help(char *name);

#endif
