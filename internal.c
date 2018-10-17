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
    /*
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    */
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
    /*
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    */
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
    printf("\n================================================================= HELP MANUAL ==============================================================\n"
           "\nFUNCTIONS\n"
           "echo <text>: \t\t%s\n"
           "cd [path]: \t\t%s\n"
           "dir [path]: \t\t%s\n"
           "environ: \t\t%s\n"
           "clear: \t\t\t%s\n"
           "pause: \t\t\t%s\n"
           "quit: \t\t\t%s\n\n",
           "Prints any typed input into the terminal log",
           "Lists current directory, or changes the  directory to [path]",
           "Lists the contents of the current directory or the contents of the directory in [path]",
           "Prints all directories associated with this shell",
           "Wipes the terminal log",
           "Pauses the terminal until character is pressed",
           "Exits the terminal"
   );

    printf("\nCOMMAND SYNTAX\n"
           "<command> [-arg1]: \t\t\t\t%s\n"
           "<command> [-arg1] &: \t\t\t\t%s\n"
           "<command> [-arg1] > <outputfile>: \t\t%s\n"
           "<command> [-arg1] >> <outputfile>: \t\t%s\n"
           "<command> [-arg1] | <command> [-arg2]: \t\t%s\n"
           "<command> < <inputfile>: \t\t\t%s\n"
           "<command> < <inputfile> > <outputfile>: \t%s\n",
           "Execute command with optional arguments",
           "Execute command without printing to console and immediately prompting for next input",
           "Execute command and write the output to a specified file, creating it if it doesn't exist",
           "Execute command and write the output to a specified file, appending to it if it already exists",
           "Execute first command and use its output as the input for another command",
           "Execute the command using a specified file as the arguments",
           "Execute command using specified input and then writing its output to another file"
           "\n=========================================================================================================================================\n"
    );
}

void pause_cmd() {
    printf("%s\n", "Shell paused. Press 'c' to continue operation.");

    while (getchar() != 'c' && getchar() != 'C' && getchar() != EOF) {
        getchar();
    }
    printf("%s", "Shell resuming ...\n");
}