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
    puts("");
}

void cd(int argc, char **argv) {
    if (argc == 1) {
        char pwd[BUFFER];
        getcwd(pwd, sizeof(pwd));
        printf("%s%s\n", "Current directory: ", pwd);
        /*get enviro var instead */

    } else {
        char * dir = argv[1];
        int valid = chdir(dir);

        if (valid < 0) {
            puts("Invalid directory. Please try again.");
        }
    }
}

void dir(int argc, char **argv) {
    //open dir
    DIR *dir;
    struct dirent *folder;

    //open current dir or specified dir
    if (argc == 1) {
        dir = opendir(".");
    } else {
        dir = opendir(argv[1]);
    }

    //check if entry is valid
    if (dir == NULL) {
        printf("%s\n", "Invalid directory. Please try again.");
    } else {
        while ((folder = readdir(dir)) != NULL) {
            printf(">> %s\n", folder->d_name);
        }
        closedir(dir);
    }
}

void environ() {
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

void quit_cmd() {
    puts("Exiting shell...");
    exit(0);
}

void clear() {
    printf("\033[H\033[J");
}

void help() {

}

void pause_cmd() {
    printf("%s\n", "Shell paused. Press 'c' to continue operation.");

    while (getchar() != 'c' && getchar() != 'C') {
        getchar();
    }
    printf("%s", "Shell resuming ...\n");
}