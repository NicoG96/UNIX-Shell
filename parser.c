#include "shell.h"

int parse(char* line, int* argc1, char** argv1, char** argv2) {
    /*
    printf("%s\n", line);
    printf("%d\n", *argc1);
    char *token = strtok(line, " ");
    while (token != NULL) {
        printf("%s\n", argv1[1]);
        token = strtok(NULL, " ");
    }
*/
    char *token = strtok(line, " ");
    int state = normal;
    int argc2 = 0;
    char **argv = argv1;
    int *argc = argc1;

    while(token != NULL) {
        //check for redirection, piping, backgrounding
        if (strcmp(token, "<") == 0) {
            state = red_in;
            argv = argv2;
            argc = &argc2;
            if(!(token = strtok(NULL, " "))) {
                perror("Invalid command");
                return -1;
            }
        }
        else if (strcmp(token, ">") == 0) {
            //if there was an input redirection earlier
            if (state == red_in) {
                //it's a special type
                state = in_out;
                if(!(token = strtok(NULL, " "))) {
                    perror("Invalid command");
                    return -1;
                }

                //have argv2 hold the 2 redirections
                argv[*argc] = calloc(1, strlen(token));
                strcpy(argv[*argc], token);
                (*argc)++;
                argv[*argc] = NULL;
                return state;
            }
            state = red_out;
            argv = argv2;
            argc = &argc2;
            if(!(token = strtok(NULL, " "))) {
                perror("Invalid command");
                return -1;
            }
        }
        else if ((strcmp(token, ">>") == 0)) {
            state = dub_red_out;
            argv = argv2;
            argc = &argc2;
            if(!(token = strtok(NULL, " "))) {
                perror("Invalid command");
                return -1;
            }
        }
        else if ((strcmp(token, "|") == 0)) {
            state = piping;
            argv = argv2;
            argc = &argc2;
            if(!(token = strtok(NULL, " "))) {
                perror("Invalid command");
                return -1;
            }
        }
        else if ((strcmp(token, "&") == 0)) {
            state = background;

            //& always end of command, so we can exit
            return state;
        }
        argv[*argc] = calloc(1, strlen(token));
        strcpy(argv[*argc], token);
        (*argc)++;
        argv[*argc] = NULL;
        token = strtok(NULL, " ");
    }
    //printf("%d\n", state);
    return state;
}