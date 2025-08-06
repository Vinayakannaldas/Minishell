#include "main.h"

void extract_external_commands(char **external_commands)
{
    // var declaration
    char ch;
    char buffer[256] = {'\0'}; // buffer
    int i = 0, j = 0;

    // open file
    int fd = open("external_commands.txt", O_RDONLY);

    if (fd == -1)
    { //check for external command file
        perror("open external_commands.txt");
        return;
    }
    // read the commands from the file
    while (read(fd, &ch, 1) > 0)
    {
        if (ch != '\n')
        {
            if (i < sizeof(buffer) - 1) 
            { 
                buffer[i++] = ch;
            }
        }
        else
        {
            buffer[i] = '\0'; 

            // allocate the memory
            external_commands[j] = calloc(strlen(buffer) + 1, sizeof(char));
            if (external_commands[j] == NULL) 
            {
                perror("calloc failed");
                close(fd);
                return;
            }

            // copy the contents
            strcpy(external_commands[j++], buffer);

           
            if (j >= 154)
            { // max 155 elements
                break;
            }

            // clear the buffer
            memset(buffer, '\0', sizeof(buffer));
            i = 0;
        }
    }
    external_commands[j] = NULL; // null-terminate the array of strings
    close(fd);
}
