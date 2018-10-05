#ifndef SHELL_H
#define SHELL_H

//libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/utsname.h>

//macros
#define BUFFER 128
#define PROMPT "$"
#define BIN "/bin/"

//functions
void run_shell();
void readline(int argc, char *tokens[]);
void echo(char **argv);
void environment();
void cd(const char* cmd, char **argv);
void clear(const char* cmd, char **argv);
void dir();
void help();
void pause_cmd();
void quit_cmd();

//enum
static const char* shell_cmds[] = {
    "clear", "cd", "echo", "dir", "environ", "help", "pause", "quit"
};


#endif
