#include "shell.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void run_shell() {
    //set shell path in the environment
    char pwd[BUFFER];
    getcwd(pwd, sizeof(pwd));
    strcat(pwd, "/myshell");
    setenv("SHELL_PATH", pwd, 1);

    //var for command line
    char line[BUFFER];

    //array to hold arg tokens
    char *argv[BUFFER];

    //counter for # of arguments
    int argc = 0;

    while(1) {
        //clear stdin
        fflush(stdin);

        //prompt
        printf("%s@%s%s ", getcwd(pwd, sizeof(pwd)), getenv("USER"), PROMPT);

        //get user input
        fgets(line, BUFFER, stdin);

        //get rid of '\n' first
        size_t len = strlen(line);
        if(line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        //variable to hold state, assume normal input
        int state = normal;

        //parse it
        parse(line, &argc, argv, &state);

        //if not internal, run as external
        if (!(run_shell_cmd(argc, argv, state))) {
            run_ext_exe(argc, argv, state);
        }
    }
}
#pragma clang diagnostic pop

int run_shell_cmd(const int argc, char **argv, const int state) {
    if (strcmp(argv[0], "environ") == 0) {
        environ();
        //output redir
        return 1;
    } else if(strcmp(argv[0], "echo") == 0) {
        echo(argv);
        //
        return 1;
    }else if (strcmp(argv[0], "help") == 0) {
        help();
        //
        return 1;
    }else if(strcmp(argv[0], "dir") == 0) {
        dir(argc, argv);
        //
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

void run_ext_exe(const int argc, char **argv, const int state) {
    if (state == normal) {
        //fork process
        pid_t rc = fork();
        if (rc < 0) {
            perror("Fork() failure");
            exit(EXIT_FAILURE);
        }
        //parent
        else if (rc > 0) {
            int status = 0;
            wait(&status);
        }
        //child
        else {
            execvp(argv[0], argv);
            perror("Forking problem");
            exit(EXIT_FAILURE);
        }
    }

    else if (state == red_in) {
        //do stuff
    }

    else if (state == red_out) {
        //yep
    }

    else if (state == dub_red_in) {
        //test
    }

    else if (state == dub_red_out) {
        //hello
        int i = 0;
    }

    else if (state == piping){
        //piping fx
        int o = 4;
    }

    else {
        //backgrounding, disable stdout
    }
}

void redirects() {
    int fd;
    //int fd = fopen("output", O_CREAT|O_WRONLY, S_IRWXU);
    printf("%d\n", fd);
    dup2(fd, 1); //Change standard output(1) to be the value of fd.
    printf("This text is not on the terminal, but in the output file.\n");
    close(fd);
}

void pipes() {
    /*
     * shellcmdhandler handler1 = findhandler(cmdlist, argv1[0])
     * shellcmdhandler handler2 = findhandler(cmdlist, argv2[0])
     */

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