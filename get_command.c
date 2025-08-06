#include "main.h"

char *get_command(char *input_string)
{
    // var declaration
    static char command[256] = {'\0'}; // Increased size
    int i = 0;

    // for skip leading spaces
    while (*input_string == ' ' || *input_string == '\t') 
    {
        input_string++;
    }

    while (1)
    {
        if (*input_string == ' ' || *input_string == '\t' || *input_string == '\0' || *input_string == '|')
        {
            break;
        }
        command[i++] = *input_string;
        input_string++;
    }
    command[i] = '\0';
    return command;
}
