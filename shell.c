#include "shell.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void run_shell() {
    //environ vars
    extern char *environment[];

    //get user & cwd
    struct passwd *CURR_USR = getpwuid(getuid());
    char CURR_PATH[1024];

    //var for command line
    char line[BUFFER];

    //array to hold tokens
    char *argv[BUFFER];

    //counter for arguments
    int argc = 0;

    while(1) {
        //clear stdin
        fflush(stdin);

        //prompt
        printf("%s@%s%s ", getcwd(CURR_PATH, sizeof(CURR_PATH)), CURR_USR->pw_name, PROMPT);

        //get user input
        fgets(line, BUFFER, stdin);

        //get rid of '\n' first
        size_t len = strlen(line);
        if(line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        //parse it
        parse(line, &argc, argv);

        //attempt to run command
        run_cmd(argc, argv);
    }
}
#pragma clang diagnostic pop

void run_cmd(int argc, char **argv) {
    /* INTERNALS */
    if (strcmp(argv[0], "environ") == 0) {
        environ();
    } else if(strcmp(argv[0], "echo") == 0) {
        echo(argv);
    }else if(strcmp(argv[0], "cd") == 0) {
        cd(argc, argv);
    }else if(strcmp(argv[0], "clear") == 0) {
        clear();
    }else if(strcmp(argv[0], "dir") == 0) {
        dir(argc, argv);
    }else if(strcmp(argv[0], "pause") == 0) {
        pause_cmd();
    }else if(strcmp(argv[0], "quit") == 0) {
        quit_cmd();
    }else if (strcmp(argv[0], "help") == 0) {
        help();


    /* EXTERNALS */
    }else {
        ext_exe(argc, argv);
    }
}

void ext_exe(int argc, char **argv) {
    //get cwd
    char CURR_PATH[1024];
    getcwd(CURR_PATH, sizeof(CURR_PATH));

    //begin fork
    int rc = fork();

    if (rc < 0) {
        perror("Fork failure.");
        exit(EXIT_FAILURE);
    }
    //parent
    if (rc > 0) {
        wait(NULL);
        //unless backgrounded

    //child
    } else{
        execvp(argv[0], argv);
        /*
         * 1. look in curr dir
         * 2. look at specified dir
         * 3. look at environ dir
         * 4. fork should b executed w parent path
         * test externals:
         *  ls -la | wc -l
         *  ls -la >> output.txt
         *  sort < input.txt
         */


        /*
         * BG EXEC
         * execute(char** argv){
            if((pid = fork()) == 0){
            is redirection?
            is piping?
            exec(argv[0],argv);
            }
            else if(is’&’<0) { // "&" means no wait for parent process,
                waitpid(pid,&status,0);
            }
         */
    }
}