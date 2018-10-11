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
    } else {
        char * dir = argv[1];
        int valid = chdir(dir);

        if (valid < 0) {
            puts("Invalid directory. Please try again.");
        }
    }
}

void dir(int argc, char **argv) {
    //list folders in current dir
    if(argc == 1) {
        DIR * dir;
        struct dirent * folder;

        //open current dir
        dir = opendir(".");

        if(dir == NULL) {
            printf("%s\n", "Invalid directory. Please try again.");
        } else {
            while((folder = readdir(dir)) != NULL) {
                printf(">> %s\n", folder->d_name);
            }
        }
        closedir(dir);

    //list folders in specified dir
    } else{
        DIR * dir;
        struct dirent * folder;

        //open relative/absolute dir
        dir = opendir(argv[1]);

        if(dir == NULL) {
            printf("%s\n", "Invalid directory. Please try again.");
        } else {
            while((folder = readdir(dir)) != NULL) {
                printf(">> %s\n", folder->d_name);
            }
        }
        closedir(dir);
    }
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

void quit_cmd() {
    puts("Exiting shell...");
    exit(0);
}

void clear(const char* cmd, char **argv) {
    printf("\033[H\033[J");
}

void help() {

}

void pause_cmd() {
    printf("%s\n", "Shell paused. Press 'c' to continue operation.");
    char ch;
    scanf(" %c", &ch);

    while(ch != 'c' && ch != 'C') {
        scanf(" %c", &ch);
    }
    printf("%s", "Shell resuming ...\n");
}