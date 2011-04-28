#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/*typedef struct _CmdParams {
  int clear_cache_flag;
  char date[32];
  int 
} CmdParams;*/

typedef int (*CMD_CALLBACK)(int, char**);
typedef void (*CMD_HELP_CALLBACK)(void);
int command_add(char *name, CMD_CALLBACK cb, 
  CMD_HELP_CALLBACK help_cb);
  
int command_call(int argc, char **argv);
int command_help(char *name);

/*void command_set_defaults*/

#endif