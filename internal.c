#include "main.h"

void execute_internal_command(char *input_string)
{
    // exit --> terminate the program
    if (strncmp("exit", input_string, 4) == 0)
    {
        exit(0); // Exit with success status
    }
    // pwd -> print the current location
    else if (strncmp("pwd", input_string, 3) == 0)
    {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
        }
    }
    // cd --> change the directory
    // input_string -> cd abc
    else if (strncmp("cd", input_string, 2) == 0)
    {
        
        // variable declaration
        int i, count = 0;
        for (i = 2; input_string[i] == ' '; i++)
        {
            count++;
        }
        char *path = &input_string[2 + count];

        if (strlen(path) == 0 || strcmp(path, "~") == 0) 
        {
            
            char *home_dir = getenv("HOME");
            if (home_dir != NULL) {
                if (chdir(home_dir) != 0) 
                {
                    perror("cd");
                }
            } else {
                fprintf(stderr, "cd: HOME environment variable not set\n");
            }
        } else {
            if (chdir(path) != 0)
            {
                perror("cd");
            }
        }
    }
}
