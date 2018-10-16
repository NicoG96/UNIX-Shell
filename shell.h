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
#include <fcntl.h>

//macros
#define BUFFER 128
#define PROMPT "$"

//functions
void run_shell(int argc, char** argv);
void parse(char* line, int* argc, char** argv, int* state);
void echo(char **argv);
void environ();
void cd(int argc, char **argv);
void clear();
void dir(int argc, char **argv);
void help();
void pause_cmd();
void quit_cmd();
int run_shell_cmd(int argc, char** argv, int state);
void run_ext_exe(int argc, char** argv, int state);
void pipes(char** argv);
void redirects(char **argv, int state);
void backgrounding(char** argv);
void sandbox();

//enum
enum state {
    normal,
    red_in,
    red_out,
    dub_red_in,
    dub_red_out,
    in_out,
    piping,
    background
};

#endif