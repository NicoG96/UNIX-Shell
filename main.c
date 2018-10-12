#include "shell.h"

int main(int argc, char **argv) {
    //if batch file
    if (argc >= 2) {
        FILE* batch;
        batch = fopen(argv[1], "r");

        if (!batch) {
            fprintf(stderr, "Empty batch file.\n");
        } else{
            //run batch commands
            fclose(batch);
        }

    //get user input otherwise
    }else{
        run_shell();
    }
    return 0;
}