#include "shell.h"

int main(int argc, char **argv) {
    /*
    //print arguments
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    */

    //if batch file
    if (argc == 2) {
        run_batch(argc, argv);

    //otherwise, get input from user
    }else{
        run_shell();
    }
    return 0;
}