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

void cd(const int argc, char **argv) {
    if (argc == 1) {
        char pwd[BUFFER];
        getcwd(pwd, sizeof(pwd));
        printf("%s%s\n", "Current directory: ", pwd);

    } else {
        char * dir = argv[1];
        int valid = chdir(dir);

        if (valid < 0) {
            perror("Error");
        }
    }
}

void dir(const int argc, char **argv) {
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
        perror("Error");
    } else {
        while ((folder = readdir(dir)) != NULL) {
            printf(">> %s\n", folder->d_name);
        }
        closedir(dir);
    }
}

void environ() {
    //set hostname
    static char HOSTNAME[1024];
    gethostname(HOSTNAME, sizeof(HOSTNAME));

    //set CWD buf
    char pwd[BUFFER];

    printf("\n=============================================== ENVIRONMENT VARIABLES ===============================================\n"
           "Current User: \t%s\n"
           "Shell Name: \t%s\n"
           "Hostname: \t\t%s\n"
           "Current Path: \t%s\n"
           "Shell Path: \t%s\n"
           "Bin Path: \t\t%s\n"
           "Home Path: \t\t%s\n"
           "=====================================================================================================================\n",
           getenv("USER"), getenv("SHELL"), HOSTNAME, getcwd(pwd, sizeof(pwd)), getenv("SHELL_PATH"), getenv("PATH"), getenv("HOME"));
}

void quit_cmd() {
    puts("Exiting shell...");
    exit(EXIT_SUCCESS);
}

void clear() {
    printf("\033[H\033[2J");
}

void help() {
    //Display the user manual using the more filter.
}

void pause_cmd() {
    printf("%s\n", "Shell paused. Press 'c' to continue operation.");

    while (getchar() != 'c' && getchar() != 'C' && getchar() != EOF) {
        getchar();
    }
    printf("%s", "Shell resuming ...\n");
}