#include "main.h"

// to find out the type command -> internal, external, no command, background, piped
int check_command_type(char *input_string, char **command_args, int *is_background, int *num_pipes)
{
    char *command = command_args[0]; // the first argument is the command name

    // check pipes 
    if (*num_pipes > 0) {
        return PIPED;
    }

    // check built-in commands
    char *builtins[] = {"echo", "cd", "pwd", "exit", "jobs", "fg", "bg", NULL};

    for (int i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(command, builtins[i]) == 0)
        {
            return BUILDIN;
        }
    }

    // check empty command
    if (command == NULL || strcmp(command, "\0") == 0)      
    {
        return NO_COMMAND;
    }

    // check external commands 
    char *external_commands[155] = {NULL}; 
    extract_external_commands(external_commands);

    for (int i = 0; external_commands[i] != NULL; i++)
    {
        if (strcmp(command, external_commands[i]) == 0)
        {
            // free memory allocated by extract_external_commands
            for (int k = 0; external_commands[k] != NULL; k++) {
                free(external_commands[k]);
            }
            return EXTERNAL;
        }
    }

    for (int k = 0; external_commands[k] != NULL; k++) {
        free(external_commands[k]);
    }
    return EXTERNAL;
}
