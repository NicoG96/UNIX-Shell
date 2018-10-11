#include "shell.h"

int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Too many arguments.\n");
        return 1;

    //batch command
    }else if (argc == 2) {
        FILE* batch;
        batch = fopen(argv[1], "r");

        if (!batch) {
            fprintf(stderr, "Empty batch file.");
        } else{
            //run batch commands
        }
        fclose(batch);

    }else{
        run_shell();
    }
    return 0;
}