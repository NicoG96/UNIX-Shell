#include "shell.h"

int main(int argc, char **argv) {
    /*
    //print arguments
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]:\t%s\n", i, argv[i]);
    }
     */

    //sandbox();

    run_shell(argc, argv);
    return 0;
}