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
        //puts("Executing batch ...");
        batch_exe(argn, args);
    }

    //otherwise, get command from user
    else {
        //puts("Running shell normally ..");
        run_shell();
    }
    return 0;
}