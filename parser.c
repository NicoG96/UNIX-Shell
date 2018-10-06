#include "shell.h"

void parse(char* line, int* argc, char* argv[]) {
    char *token = strtok(line, " ");
    int i = 0;
    while(token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }

    //append NULL to signify end of cmd
    argv[i] = NULL;

    //record arg # into argc
    *argc = i;
}