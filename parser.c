#include "shell.h"

void parse(char* line, int* argc, char** argv, int* state) {
    char *token = strtok(line, " ");
    int i = 0;

    while(token != NULL) {
        //check for redirection, piping, backgrounding
        if (strcmp(token, "<") == 0) {
            *state = red_in;
        }
        else if (strcmp(token, ">") == 0) {
            *state == red_in ? (*state = in_out) : (*state = red_out);
        }
        else if (strcmp(token, "<<") == 0) {
            *state = dub_red_in;
        }
        else if ((strcmp(token, ">>") == 0)) {
            *state = dub_red_out;
        }
        else if ((strcmp(token, "|") == 0)) {
            *state = piping;
        }
        else if ((strcmp(token, "&") == 0)) {
            *state = background;

            //if it's an '&' then it's the end of command and can break out of loop
            break;
        }
        argv[i++] = token;
        token = strtok(NULL, " ");
    }

    //append NULL to signify end of cmd
    argv[i] = NULL;

    //record arg # into argc
    *argc = i;
}