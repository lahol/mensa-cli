/** @ingroup commands
 *  @file
 *  Header file for the implementation of the supported commands and
 *  shared features.
 */
#ifndef __CMD_H__
#define __CMD_H__

int cmd_config(int argc, char **argv);
int cmd_config_set(char *arg);

int cmd_show(int argc, char **argv);
int cmd_help(int argc, char **argv);
int cmd_info(int argc, char **argv);

#endif