#include "shell.h"

void run_shell() {
    //get user & cwd
    struct passwd *CURR_USR = getpwuid(getuid());
    char CURR_PATH[1024];
    getcwd(CURR_PATH, sizeof(CURR_PATH));

    //var for command line
    char line[BUFFER];

    //array to hold tokens
    char *argv[BUFFER];

    //counter for arguments
    int argc = 0;

    while(1) {
        //clear stdin
        fflush(stdin);

        //prompt
        printf("%s@%s%s ", CURR_PATH, CURR_USR->pw_name, PROMPT);

        //get user input
        fgets(line, BUFFER, stdin);

        //parse it
        parse(line, &argc, argv);

        break;
    }
}

void readline(int argc, char *tokens[]) {

}