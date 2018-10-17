#include "shell.h"

int main(int argc, char **argv) {
    /*
    //print arguments
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    */
    //sandbox();
    run_shell(argc, argv);
    return 0;
}