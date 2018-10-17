#include "shell.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void run_shell() {
    //set shell path in the environment
    char pwd[BUFFER];
    getcwd(pwd, sizeof(pwd));
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
        if (run_shell_cmd(argc, argv, state)) {
            run_ext_exe(argc, argv, state);
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
            run_ext_exe(argc, argv, state);
        }
    }
    fclose(file);
    exit(EXIT_SUCCESS);
}

int run_shell_cmd(const int argc, char **argv, const int state) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("argc:\t%d", argc);
    printf("state:\t%d", state);
    */
    if (strcmp(argv[0], "environ") == 0) {
        //if it's not normal, then it's a redirection (only supported for environ, echo, help, dir)
        if (state != normal) {
            redirects(argc, argv, state);
            return 0;
        }
        environ();
        return 0;

    } else if(strcmp(argv[0], "echo") == 0) {
        if (state != normal) {
            redirects(argc, argv, state);
            return 0;
        }
        echo(argv);
        return 0;

    }else if (strcmp(argv[0], "help") == 0) {
        if (state != normal) {
            redirects(argc, argv, state);
            return 0;
        }
        help();
        return 0;

    }else if(strcmp(argv[0], "dir") == 0) {
        if (state != normal) {
            redirects(argc, argv, state);
            return 0;
        }
        dir(argc, argv);
        return 0;

    }else if(strcmp(argv[0], "cd") == 0) {
        cd(argc, argv);
        return 0;

    }else if(strcmp(argv[0], "clear") == 0) {
        clear();
        return 0;

    }else if(strcmp(argv[0], "pause") == 0) {
        pause_cmd();
        return 0;

    }else if(strcmp(argv[0], "quit") == 0) {
        quit_cmd();
        return 0;
    }
    return 1;
}

int run_ext_exe(int argc, char **argv, const int state) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("argc:\t%d", argc);
    printf("state:\t%d", state);
    */
    //if no redirection, run command normally
    if (state == normal) {
        //puts("Normal");

        //fork process
        pid_t rc = fork();
        if (rc < 0) {
            perror("Fork() failure");
            return 1;
        }
        //parent
        else if (rc > 0) {
            wait(&rc);
        }
        //child
        else {
            //printf("Child executing: %s\n", argv[0]);

            //launch exe from current dir
            execvp(argv[0], argv);

            //else, search the bin
            //puts("Searching bin ...");
            char *binpath = getenv("PATH");
            strcat(binpath, "/");
            strcat(binpath, argv[0]);
            execvp(binpath, argv);

            //finally, search the shell's directory
            //puts("Searching shell directory ...");
            char *shellpath = getenv("SHELL_PATH");
            strcat(shellpath, "/");
            strcat(shellpath, argv[0]);
            execvp(shellpath, argv);

            //command doesn't exist otherwise
            perror("Command not found");
            return 1;
        }
    }

    //if it's redirection, send to respective function
    else if (state == red_in || state == red_out || state == dub_red_out || state == dub_red_in || state == in_out) {
        //puts("Redirection");
        redirects(argc, argv, state);
        return 0;
    }

    //if it's piping, do same
    else if(state == piping) {
        //puts("Piping");
        pipes(argc, argv);
        return 0;
    }

    //otherwise its backgrounding
    else {
        //puts("Background");
        backgrounding(argv);
        return 0;
    }
    return 1;
}

int redirects(int argc, char **argv, const int state) {
    int newstdin;
    int newstdout;

    //create new fork
    pid_t rc = fork();
    if (rc < 0) {
        perror("Fork() redirect failure");
        return 1;
    }

    //parent
    else if (rc > 0) {
        wait(&rc);
    }

    //child
    else {
        if (state == red_in) {
            //printf("Input Redirection:\t%s\n", argv[argc - 1]);
            newstdin = open(argv[argc - 1], O_RDONLY);
            if (newstdin < 0) {
                perror("Input couldn't be found");
                return 1;
            }
            close(0);

            //replace stdin with specified file
            dup(newstdin);
            close(newstdin);
        }

        else if (state == red_out) {
            //printf("Output Redirection:\t%s\n", argv[argc - 1]);
            newstdout = open(argv[argc - 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
            if (newstdout < 0) {
                perror("Output couldn't be found");
                return 1;
            }
            close(1);

            //replace stdout with specified file
            dup(newstdout);
            close(newstdout);
        }

        else if(state == dub_red_out) {
            newstdout = open(argv[argc - 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO); /*change these parameters*/
            if (newstdout < 0) {
                perror("Output couldn't be found");
                return 1;
            }
            close(1);

            //replace stdout with specified file
            dup(newstdout);
            close(newstdout);
        }

        else if (state == dub_red_in) { /* delete???? */
            newstdout = open(argv[argc - 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //input && output redirection
        else {
            newstdin = open(argv[argc - 3], O_RDONLY);
            newstdout = open(argv[argc - 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

            if (newstdout < 0 || newstdin < 0) {
                perror("File couldn't be found");
                return 1;
            }
            //replace stdin with specified file
            close(0);
            dup(newstdin);

            //replace stdout with specified file
            close(1);
            dup(newstdout);

            close(newstdin);
            close(newstdout);

        }
        /*
        //printf("Child executing: %s\n", argv[0]);

        //dup2(stdin, 1);
        //close(stdin) || stdout;

        //launch exe from current dir
        execvp(argv[0], argv);

        //else, search the bin
        //puts("Searching bin ...");
        char *binpath = getenv("PATH");
        strcat(binpath, "/");
        strcat(binpath, argv[0]);
        execvp(binpath, argv);

        //finally, search the shell's directory
        //puts("Searching shell directory ...");
        char *shellpath = getenv("SHELL_PATH");
        strcat(shellpath, "/");
        strcat(shellpath, argv[0]);
        execvp(shellpath, argv);

        //command doesn't exist otherwise
        perror("Command not found");
        return 1;
         */
    }
    return 0;
}

int pipes(int argc, char **argv) {
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
    return 0;
}

int backgrounding(char** argv) {
    //fork process
    pid_t rc = fork();
    if (rc < 0) {
        perror("Fork() failure");
        return 1;
    }
    //parent
    else if (rc > 0) {
        //close stdout
        close(1);
        //waitpid(pid,&status,0);
    }

    //child
    else {
        //close stdout
        close(1);
        execvp(argv[0], argv);
        perror("Unkown command");
        return 1;
    }
    return 0;
}