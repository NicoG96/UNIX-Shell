#include "shell.h"

void run_shell() {
    //get user & cwd
    struct passwd *CURR_USR = getpwuid(getuid());
    char CURR_PATH[1024];
    getcwd(CURR_PATH, sizeof(CURR_PATH));

    //var for # of args
    int argc;

    //var for command line
    char line[BUFFER];

    //array to hold tokens
    char *argv[BUFFER];

    while(1) {
        //clear stdin
        fflush(stdin);

        //prompt
        printf("%s@%s%s ", CURR_PATH, CURR_USR->pw_name, PROMPT);

        //get user input
        fgets(line, BUFFER, stdin);

        //tokenize input
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL) {
            argv[i++] = token;
            token = strtok(NULL, " ");
        }

        //append NULL to signify end of cmd
        argv[i] = NULL;

        //set argc = to # of args
        argc = i;

        //run through readline
        readline(argc, argv);

        break;
    }
}

void readline(int argc, char *tokens[]) {

}