#include "shell.h"

int main(int argn, char **args) {
    /*
    //print arguments
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    */
    //if there's arguments, run as batch file
    if (argn > 1) {
        batch_exe(argn, args);
    }

    //otherwise, get command from user
    else {
        run_shell();
    }
    return 0;
}