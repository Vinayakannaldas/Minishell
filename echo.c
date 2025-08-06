#include "main.h"

void echo(char *input_string, int status)
{
    // echo $?
    if (strncmp("echo $?", input_string, 7) == 0)
    {
        printf("%d\n", status);
    }
    // echo $$ --> PID of the minishell project
    else if (strncmp("echo $$", input_string, 7) == 0)
    {
        printf("%d\n", getpid());
    }
    // echo $SHELL
    else if (strncmp("echo $SHELL", input_string, 11) == 0)
    {
       
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s/msh\n", cwd); 
        } else {
            perror("getcwd");
        }
    }
   
    else if (strncmp("echo ", input_string, 5) == 0)
    {
        printf("%s\n", input_string + 5); 
        
    }
}
