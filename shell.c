#include "shell.h"

int run_shell(int argn, char** args) {
    /*
    printf("argc: %d\n", argn);
    for (int i = 0; i < argn; i++) {
        printf("argv[%d]:\t%s\n", i, args[i]);
    }
     */

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

    //arrays for arguments, accounting for potential redirection or piping
    char *argv1[BUFFER];
    char *argv2[BUFFER];

    int argc1 = 0;
    int * argc = &argc1;

    //var to hold type of command
    int state;

    int newstdin;

    if (argn > 1) {
        //puts("batch file");
        newstdin = open(args[1], O_RDONLY);
        if (newstdin < 0) {
            perror("Batch file couldn't be found");
            return 1;
        }
        close(0);
        dup(newstdin);
        close(newstdin);
    }

    while(1) {
        //reset buffers
        memset(line, '\0', BUFFER * sizeof(char));
        memset(argv1, -1, BUFFER * sizeof(char *));
        memset(argv2, -1, BUFFER * sizeof(char *));
        argc1 = 0;

        //clear stdi/o
        fflush(stdin);
        fflush(stdout);

        //prompt
        if (argn == 1) {
            printf("%s@%s%s ", getcwd(pwd, sizeof(pwd)), getenv("USER"), PROMPT);
        }

        if (fgets(line, BUFFER, stdin) == NULL) {
            break;
        }
        //printf("getline: %s\n", line);

        //catch any newlines still in stream
        if (line[0] == '\n') {
            continue;
        }

        //get rid of '\n' first
        size_t len = strlen(line);
        if(line[len - 1] == '\n' && len > 1) {
            line[len - 1] = '\0';
        }

        //parse it
        if ((state = parse(line, &argc1, argv1, argv2)) < 0) {
            continue;
        }
        /*
        for (int i = 0; i < *argc; i++) {
            printf("after parse: argv1[%d]:\t%s\n", i, argv1[i]);
        }
        printf("argv2[%d]:\t%s\n", 0, argv2[0]);
        printf("argc:\t%d\n", *argc);
        */

        //if not internal, run as external
        if (run_shell_cmd(*argc, argv1, argv2, state)) {
            run_ext_exe(*argc, argv1, argv2, state);
        }
    }
}

int run_shell_cmd(const int argc, char **argv1, char ** argv2, const int state) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv1[i]);
    }
    for(int i = 0; i < 2; i++) {
        printf("%s\", argv2[i]);
    }
    printf("argc:\t%d", argc);
    printf("state:\t%d", state);
    */
    if (strcmp(argv1[0], "environ") == 0) {
        //if it's not normal, then it's a redirection (only supported for environ, echo, help, dir)
        if (state != normal && state != background) {
            redirects(argc, argv1, argv2, state);
            return 0;
        }

        if (state == background) {
            backgrounding(argc, argv1);
            return 0;
        }
        environ();
        return 0;
    }

    else if(strcmp(argv1[0], "echo") == 0) {
        if (state != normal & state != background) {
            redirects(argc, argv1, argv2, state);
            return 0;
        }

        if (state == background) {
            backgrounding(argc, argv1);
            return 0;
        }
        echo(argv1);
        return 0;
    }

    else if (strcmp(argv1[0], "help") == 0) {
        if (state != normal & state != background) {
            redirects(argc, argv1, argv2, state);
            return 0;
        }

        if (state == background) {
            backgrounding(argc, argv1);
            return 0;
        }
        help();
        return 0;
    }

    else if(strcmp(argv1[0], "dir") == 0) {
        if (state != normal & state != background) {
            redirects(argc, argv1, argv2, state);
            return 0;
        }

        if (state == background) {
            backgrounding(argc, argv1);
            return 0;
        }
        dir(argc, argv1);
        return 0;
    }

    else if(strcmp(argv1[0], "cd") == 0) {
        cd(argc, argv1);
        return 0;
    }

    else if(strcmp(argv1[0], "clear") == 0) {
        clear();
        return 0;
    }

    else if(strcmp(argv1[0], "pause") == 0) {
        pause_cmd();
        return 0;
    }

    else if(strcmp(argv1[0], "quit") == 0) {
        quit_cmd();
        return 0;
    }
    return 1;
}

int run_ext_exe(int argc, char **argv1, char ** argv2, const int state) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv1[i]);
    }
    for(int i = 0; i < 2; i++) {
        printf("%s\", argv2[i]);
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
            //printf("Child executing: %s\n", argv1[0]);

            //launch exe from current dir
            execvp(argv1[0], argv1);

            //else, search the bin
            //puts("Searching bin ...");
            char *binpath = getenv("PATH");
            strcat(binpath, "/");
            strcat(binpath, argv1[0]);
            execvp(binpath, argv1);

            //finally, search the shell's directory
            //puts("Searching shell directory ...");
            char *shellpath = getenv("SHELL_PATH");
            strcat(shellpath, "/");
            strcat(shellpath, argv1[0]);
            execvp(shellpath, argv1);

            //command doesn't exist otherwise
            perror("[run_ext_exe] Command not found");
            exit(0);
        }
    }

    //if it's redirection, send to respective function
    else if (state == red_in || state == red_out || state == dub_red_out || state == in_out) {
        //puts("Redirection");
        redirects(argc, argv1, argv2, state);
        return 0;
    }

    //if it's piping, do same
    else if(state == piping) {
        //puts("Piping");
        pipes(argv1, argv2);
        return 0;
    }

    //otherwise its backgrounding
    else {
        //puts("Background");
        backgrounding(argc, argv1);
        return 0;
    }
    return 1;
}

int redirects(int argc, char **argv1, char **argv2, const int state) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv1[i]);
    }
    printf("argc:\t%d", argc);
    printf("state:\t%d", state);
    */
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
        //puts("Child forked.");
        if (state == red_in) {
            //printf("Input Redirection:\t%s\n", argv2[0]);
            newstdin = open(argv2[0], O_RDONLY);
            if (newstdin < 0) {
                perror("Input couldn't be found");
                return 1;
            }
            //replace stdin with specified file
            close(0);
            dup(newstdin);
            close(newstdin);
        }

        else if (state == red_out) {
            //printf("Output Redirection:\t%s\n", argv2[0]);
            newstdout = open(argv2[0], O_WRONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
            if (newstdout < 0) {
                perror("Output creation failed");
                return 1;
            }
            //replace stdout with specified file
            close(1);
            dup(newstdout);
            close(newstdout);
        }

        else if(state == dub_red_out) {
            newstdout = open(argv2[0], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
            if (newstdout < 0) {
                perror("Output creation failed");
                return 1;
            }
            //replace stdout with specified file
            close(1);
            dup(newstdout);
            close(newstdout);
        }

        //input && output redirection
        else if (state == in_out){
            newstdin = open(argv2[0], O_RDONLY);
            newstdout = open(argv2[1], O_WRONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);

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

        //printf("%s\n", argv1[0]);
        //check if it's an internal command first
        if (strcmp(argv1[0], "dir") == 0) {
            dir(argc, argv1);
            exit(0);
        }
        else if (strcmp(argv1[0], "environ") == 0) {
            environ();
            exit(0);
        }

        else if (strcmp(argv1[0], "echo") == 0) {
            echo(argv1);
            exit(0);
        }
        else if (strcmp(argv1[0], "help") == 0) {
            help();
            exit(0);
        }

        //if not internal, must be external
        execvp(argv1[0], argv1);

        //else, search the bin
        //puts("Searching bin ...");
        char *binpath = getenv("PATH");
        strcat(binpath, "/");
        strcat(binpath, argv1[0]);
        execvp(binpath, argv1);

        //finally, search the shell's directory
        //puts("Searching shell directory ...");
        char *shellpath = getenv("SHELL_PATH");
        strcat(shellpath, "/");
        strcat(shellpath, argv1[0]);
        execvp(shellpath, argv1);

        //command doesn't exist otherwise
        perror("[Redirection] Command not found");
        return 1;
    }
    return 0;
}

int pipes(char **argv1, char **argv2) {
    /* print passed arguments
    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv1[i]);
    }
    for(int i = 0; i < 2; i++) {
        printf("%s\", argv2[i]);
    }
    printf("argc:\t%d", argc);
    */
    //create file descriptor
    int pfds[2];

    //create pipe before fork, pipe shared by both processes
    int pip = pipe(pfds);

    if (pip < 0) {
        perror("Piping error");
        return 1;
    }

    //fork
    pid_t rc = fork();

    if (rc < 0) {
        perror("[pipes] forking error:");
        return 1;
    }

    //parent
    else if (rc > 0) {
        wait(&rc);

        //close stdin
        close(0);

        //set stdin to the read pipe
        dup2(pfds[0], 0);

        //close write end
        close(pfds[1]);

        execvp(argv2[0], argv2);

        //else, search the bin
        //puts("Searching bin ...");
        char *binpath = getenv("PATH");
        strcat(binpath, "/");
        strcat(binpath, argv2[0]);
        execvp(binpath, argv2);

        //finally, search the shell's directory
        //puts("Searching shell directory ...");
        char *shellpath = getenv("SHELL_PATH");
        strcat(shellpath, "/");
        strcat(shellpath, argv2[0]);
        execvp(shellpath, argv2);

        //command doesn't exist otherwise
        perror("[Piping] Command not found");
        return 1;
    }

    //child
    else {
        //close stdout
        close(1);

        //set stdout to the write pipe
        dup2(pfds[1], 1);

        //close read end
        close(pfds[0]);

        execvp(argv1[0], argv1);

        //else, search the bin
        //puts("Searching bin ...");
        char *binpath = getenv("PATH");
        strcat(binpath, "/");
        strcat(binpath, argv1[0]);
        execvp(binpath, argv1);

        //finally, search the shell's directory
        //puts("Searching shell directory ...");
        char *shellpath = getenv("SHELL_PATH");
        strcat(shellpath, "/");
        strcat(shellpath, argv1[0]);
        execvp(shellpath, argv1);

        //command doesn't exist otherwise
        perror("[Piping] Command not found");
        return 1;
    }
    return 0;
}

int backgrounding(int argc, char** argv1) {
    /*
    char *token = strtok(*argv1, " ");
    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
     */
    //fork process
    pid_t rc = fork();
    if (rc < 0) {
        perror("Fork() failure");
        return 1;
    }
    //parent
    else if (rc > 0) {
        //no need to wait
    }

    //child
    else {
        //close stdout
        close(1);

        //check if it's an internal command first
        if (strcmp(argv1[0], "dir") == 0) {
            dir(argc, argv1);
            exit(0);
        }
        else if (strcmp(argv1[0], "environ") == 0) {
            environ();
            exit(0);
        }

        else if (strcmp(argv1[0], "echo") == 0) {
            echo(argv1);
            exit(0);
        }
        else if (strcmp(argv1[0], "help") == 0) {
            help();
            exit(0);
        }

        execvp(argv1[0], argv1);

        //else, search the bin
        //puts("Searching bin ...");
        char *binpath = getenv("PATH");
        strcat(binpath, "/");
        strcat(binpath, argv1[0]);
        execvp(binpath, argv1);

        //finally, search the shell's directory
        //puts("Searching shell directory ...");
        char *shellpath = getenv("SHELL_PATH");
        strcat(shellpath, "/");
        strcat(shellpath, argv1[0]);
        execvp(shellpath, argv1);

        //command doesn't exist otherwise
        perror("[Piping] Command not found");
        return 1;
    }
    return 0;
}