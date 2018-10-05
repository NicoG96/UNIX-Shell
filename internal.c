#include "shell.h"

void echo(char **argv) {
    int i = 0;

    while(argv[i] != NULL) {
        printf("%s ", argv[i]);
        i++;
    }
}

void cd(const char* cmd, char **argv) {
    if (*(argv++) == NULL) {
        char buf[80];
        getcwd(buf, sizeof(buf));
        printf("%s\n", buf);
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
    fflush(stdin);
    wait(5);
}

void quit_cmd(const char* cmd, char **argv) {
    puts("Exiting shell.");
    exit(0);
}