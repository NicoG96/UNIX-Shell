#include "shell.h"

void echo(char **argv) {
    int i = 0;
    if((strcmp(argv[i], "echo") == 0)) {
        i++;
    }

    while(argv[i] != NULL) {
        printf("%s ", argv[i]);
        i++;
    }
}

void cd(char **argv) {
    if (*(argv+1) == NULL) {
        char pwd[BUFFER];
        getcwd(pwd, sizeof(pwd));
        printf("%s%s\n", "Current directory: ", pwd);
    } else {
        //change directory
        //test .., /, /home, etc
    }
}

void clear(const char* cmd, char **argv) {
    printf("\033[H\033[2J");
}

void dir() {

}

void help() {

}

void environment() {
    static const char* SHELL = "N-Shell 0.1b";

    char CURR_PATH[1024];
    getcwd(CURR_PATH, sizeof(CURR_PATH));

    static char HOSTNAME[1024];
    gethostname(HOSTNAME, sizeof(HOSTNAME));

    struct passwd *CURR_USR = getpwuid(getuid());

    printf("\n========================================== ENVIRONMENT VARIABLES ==========================================\n"
           "Current User: \t%s\n"
           "Shell Name: \t%s\n"
           "OS Type: \t\t%s\n"
           "Hostname: \t\t%s\n"
           "Current Path: \t%s\n"
           "===========================================================================================================\n",
           CURR_USR->pw_name, SHELL, "UNIX", HOSTNAME, CURR_PATH);
}

void pause_cmd() {
    printf("%s\n", "Shell paused. Press 'c' to continue operation.");
    char ch;
    scanf("%c", &ch);

    while(ch != 'c' && ch != 'C') {
        scanf("%c", &ch);
    }
    printf("%s", "Shell resuming ...\n");
}

void quit_cmd() {
    puts("Exiting shell...");
    exit(0);
}