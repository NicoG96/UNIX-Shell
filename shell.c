#include "shell.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void run_shell() {
    //set shell path in the environment
    char pwd[BUFFER];
    getcwd(pwd, sizeof(pwd));
    strcat(pwd, "/myshell");
    setenv("SHELL_PATH", pwd, 1);
    /*
    for (int i = 0; i < argc; i++) {
        printf("%s\n", args[i]);
    }
    printf("%s", getenv("SHELL_PATH"));
    printf("%d", argn);
    */

    //buffer for command line
    char line[BUFFER];

    //array of strings to hold arguments
    char *argv[BUFFER];

    //counter for # of arguments
    int argc;

    /*
    //array1 to hold arguments, arrray2 for possible redirections/piping
    int* argc;
    int argc1 = 0;
    int argc2 = 0;

    char *argv1[BUFFER];
    char *argv2[BUFFER];

    char **argv = argv1;
    argc = &argc1;
    */

    while(1) {
        //clear stdin
        fflush(stdin);

        //reset buffers
        memset(line, '\0', BUFFER * sizeof(char));
        memset(argv, -1, BUFFER * sizeof(char *));

        //prompt
        printf("%s@%s%s ", getcwd(pwd, sizeof(pwd)), getenv("USER"), PROMPT);

        //get keyboard input
        fgets(line, BUFFER, stdin);
        //printf("%s\n", line);

        //get rid of '\n' first
        size_t len = strlen(line);
        if(line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        //variable to hold state, assume normal input
        int state = normal;

        //parse it
        parse(line, &argc, argv, &state);

        /*
        for (int i = 0; i < argc; i++) {
            printf("arg[%d]:\t%s\n", i, argv[i]);
        }
        printf("State:\t%d\n", state);
        printf("argc:\t%d\n", argc);
        */

        //if not internal, run as external
        if (!(run_shell_cmd(argc, argv, state))) {
            run_ext_exe(argv, state);
        }
    }
}
#pragma clang diagnostic pop

void batch_exe(int argc, char** argv) {
    char line[BUFFER];
    FILE *file = fopen(argv[1], "r");

    //if file can't be opened
    if (!file) {
        perror("fopen() error");
        exit(EXIT_FAILURE);
    }

    //puts("Shell opened file successfully.");
    while (fgets(line, BUFFER, file) != NULL) {
        //printf("%s", line);

        //get rid of '\n' first
        size_t len = strlen(line);
        if(line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        //printf("%s\n", line);

        //variable to hold state, assume normal input
        int state = normal;

        //parse it
        parse(line, &argc, argv, &state);

        /*
        for (int i = 0; i < argc; i++) {
            printf("arg[%d]:\t%s\n", i, argv[i]);
        }
        printf("argc:\t%d\n", argc);
        printf("State:\t%d\n", state);
        puts("===== NEXT COMMAND =====");
        */

        //if not internal, run as external
        if (!(run_shell_cmd(argc, argv, state))) {
            run_ext_exe(argv, state);
        }
    }
    fclose(file);
    exit(EXIT_SUCCESS);
}

int run_shell_cmd(const int argc, char **argv, const int state) {
    if (strcmp(argv[0], "environ") == 0) {
        //if it's not normal, then it's a redirection (only supported for environ, echo, help, dir)
        if (state != normal) {
            redirects(argv, state);
            return 1;
        }
        environ();
        return 1;

    } else if(strcmp(argv[0], "echo") == 0) {
        if (state != normal) {
            redirects(argv, state);
            return 1;
        }
        echo(argv);
        return 1;

    }else if (strcmp(argv[0], "help") == 0) {
        if (state != normal) {
            redirects(argv, state);
            return 1;
        }
        help();
        return 1;

    }else if(strcmp(argv[0], "dir") == 0) {
        if (state != normal) {
            redirects(argv, state);
            return 1;
        }
        dir(argc, argv);
        return 1;

    }else if(strcmp(argv[0], "cd") == 0) {
        cd(argc, argv);
        return 1;

    }else if(strcmp(argv[0], "clear") == 0) {
        clear();
        return 1;

    }else if(strcmp(argv[0], "pause") == 0) {
        pause_cmd();
        return 1;

    }else if(strcmp(argv[0], "quit") == 0) {
        quit_cmd();
        return 1;
    }
    return 0;
}

void run_ext_exe(char **argv, const int state) {
    //if no redirection, run command normally
    if (state == normal) {
        //puts("Normal");
        //fork process
        pid_t rc = fork();
        if (rc < 0) {
            perror("Fork() failure");
            exit(EXIT_FAILURE);
        }
        //parent
        else if (rc > 0) {
            wait(&rc);
        }
        //child
        else {
            execvp(argv[0], argv);
            perror("Command not found");
            exit(EXIT_FAILURE);
        }
    }

    //if it's redirection, send to respective function
    else if (state == red_in || state == red_out || state == dub_red_out || state == dub_red_in || state == in_out) {
        //puts("Redirection");
        redirects(argv, state);
    }

    //if it's piping, do same
    else if(state == piping) {
        //puts("Piping");
        pipes(argv);
    }

    //otherwise its backgrounding
    else {
        //puts("Background");
        backgrounding(argv);
    }
}

void redirects(char **argv, const int state) {
    int fd;
    //int fd = fopen("output", O_CREAT|O_WRONLY, S_IRWXU);
    printf("%d\n", fd);
    dup2(fd, 1); //Change standard output(1) to be the value of fd.
    printf("This text is not on the terminal, but in the output file.\n");
    close(fd);
}

void pipes(char **argv) {
    //create file descriptor
    int pfds[2];

    //create pipe b4 fork, pipe shared by both processes
    if (pipe(pfds) == 0) {
        //child
        if (fork() == 0) {
            //close stdout
            close(1);

            //set stdout at write end
            dup2(pfds[1], 1);

            //close read end
            close(pfds[0]);

            /*
            if (handler1 != NULL) {
                handler1(argv1[0], argv1);
            }
             */
        }
        //parent
        else {
            //close stdin
            close(0);

            //set stdin as read end
            dup2(pfds[0], 0);

            //close write end
            close(pfds[1]);

            /*
            if (handler2 != NULL) {
                handler2(argv2[0], argv2);
            }
             */
        }
    }
}

void backgrounding(char** argv) {
    //fork process
    pid_t rc = fork();
    if (rc < 0) {
        perror("Fork() failure");
        exit(EXIT_FAILURE);
    }
        //parent
    else if (rc > 0) {
        //close stdout
        close(1);
    }

        //child
    else {
        //close stdout
        close(1);
        execvp(argv[0], argv);
        perror("Unkown command");
        exit(EXIT_FAILURE);
    }
};