#include "main.h"

pid_t pid; // global pid for fork

void scan_input(char *prompt, char *input_string)
{
    // var declaration
    char *command_name = NULL;
    char *command_args[256]; // array to store parsed command arguments
    int command_type;
    int status;
    int is_background = 0; // flag for background process
    int num_pipes = 0;     // count for pipes

    while (1)
    {
        // clear memory
        memset(input_string, '\0', 256);
        printf(ANSI_COLOUR_BLUE "%s$" ANSI_COLOUR_GREEN, prompt);

        // clear the output buffer
        fflush(stdout);

        // Read input
        if (scanf("%[^\n]s", input_string) == EOF) {
            printf("\nexit\n");
            exit(0);
        }

        // clear the input buffer
        getchar();

        // change name
        if (!strncmp("PS1=", input_string, 4))
        {
            if (input_string[4] != ' ')
            {
                strcpy(prompt, &input_string[4]);
                // clear the input string
                memset(input_string, '\0', 256);
                continue;
            }
            printf("msh: PS1: Invalid syntax. No whitespace allowed after '='\n");
            continue;
        }

        // check for empty command
        if (strlen(input_string) == 0)
        {
            continue;
        }

        // reset flags and arrays 
        is_background = 0;
        num_pipes = 0;
        for (int i = 0; i < 256; i++) {
            command_args[i] = NULL;
        }

        // check for background command 
        if (input_string[strlen(input_string) - 1] == '&')
        {
            is_background = 1;
            input_string[strlen(input_string) - 1] = '\0'; 
          
            while (strlen(input_string) > 0 && input_string[strlen(input_string) - 1] == ' ') 
            {
                input_string[strlen(input_string) - 1] = '\0';
            }
        }

        // check for pipes
        for (int i = 0; input_string[i] != '\0'; i++) {
            if (input_string[i] == '|') {
                num_pipes++;
            }
        }

        // get the first command name and parse arguments
        char temp_input[256];
        strcpy(temp_input, input_string);
        char *token = strtok(temp_input, " \t\n");
        int i = 0;
        while (token != NULL) {
            command_args[i] = strdup(token); 
            if (i == 0) {
                command_name = command_args[0]; 
            }
            token = strtok(NULL, " \t\n");
            i++;
        }
        command_args[i] = NULL; 
        if (command_name == NULL) 
        { 
            continue;
        }

        // determine command type
        command_type = check_command_type(input_string, command_args, &is_background, &num_pipes);

        if (command_type == PIPED) {
            execute_piped_commands(input_string, num_pipes);
        }
        else if (command_type == EXTERNAL || is_background) // external or background command
        {
            execute_external_command(command_args, is_background);
        }
        else if (command_type == BUILDIN) // Built-in commands
        {
            
            echo(input_string, status); // status here is from the last external command
            // internal commands
            execute_internal_command(input_string);

            // handle fg, bg, jobs built-ins
            if (strcmp(command_name, "jobs") == 0) {
                print_jobs();
            } else if (strcmp(command_name, "fg") == 0) {
                handle_fg(input_string);
            } else if (strcmp(command_name, "bg") == 0) {
                handle_bg(input_string);
            }
        }
        else if (command_type == NO_COMMAND)
        {
            
        }
        else
        {
            printf("msh: command not found: %s\n", command_name);
        }

        // free duplicated arguments
        for (int j = 0; command_args[j] != NULL; j++) 
        {
            free(command_args[j]);
        }
    }
}

// external commands
void execute_external_command(char **command_args, int is_background)
{
    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child process
    {
        // reset signal handlers for child process to default
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        if (is_background) 
        {
            setsid();
            int fd0 = open("/dev/null", O_RDWR);
            dup2(fd0, STDIN_FILENO);
            dup2(fd0, STDOUT_FILENO);
            dup2(fd0, STDERR_FILENO);
            if (fd0 > 2) close(fd0);
        }

        // execute the command
        execvp(command_args[0], command_args);
        // ef execvp returns, an error occurred
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    else // parent process
    {
        if (is_background)
        {
            add_job(pid, command_args[0], 0); // 0: Running
            printf("[%d] %d\n", next_job_id - 1, pid); // job ID and PID
        }
        else
        {
            foreground_pid = pid; //foreground PID
            int status;
            waitpid(pid, &status, WUNTRACED); // wait for foreground process, WUNTRACED to catch stopped children
            foreground_pid = 0; // reset foreground PID after child stops

            if (WIFEXITED(status))
            {
                 //printf("Child process terminated with status %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
              //  printf("Child process terminated by signal %d\n", WTERMSIG(status));
            }
            else if (WIFSTOPPED(status))
            {
                printf("\nStopped: %s (PID: %d)\n", command_args[0], pid);
                add_job(pid, command_args[0], 1); // 1: Stopped
            }
        }
    }
}

void add_job(pid_t pid, const char *command, int status) {
    Job *new_job = (Job *)malloc(sizeof(Job));
    if (new_job == NULL) {
        perror("malloc failed");
        return;
    }
    new_job->pid = pid;
    strncpy(new_job->command, command, sizeof(new_job->command) - 1);
    new_job->command[sizeof(new_job->command) - 1] = '\0';
    new_job->status = status;
    new_job->job_id = next_job_id++;
    new_job->next = head_job;
    head_job = new_job;
}

void remove_job(pid_t pid) {
    Job *current = head_job;
    Job *prev = NULL;

    while (current != NULL && current->pid != pid) {
        prev = current;
        current = current->next;
    }

    if (current != NULL) {
        if (prev == NULL) {
            head_job = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
    }
}

void print_jobs() {
    Job *current = head_job;
    if (current == NULL) {
        printf("No jobs.\n");
        return;
    }
    while (current != NULL) {
        printf("[%d] %s %s\n", current->job_id,
               (current->status == 0) ? "Running" : "Stopped",
               current->command);
        current = current->next;
    }
}

void handle_fg(char *input_string) {
    int job_id_or_pid = -1;
    char *token = strtok(input_string, " \t\n"); // "fg"
    token = strtok(NULL, " \t\n"); // argument

    if (token != NULL) {
        job_id_or_pid = atoi(token);
    }

    Job *target_job = NULL;
    if (job_id_or_pid != -1) {
        Job *current = head_job;
        while (current != NULL) {
            if (current->job_id == job_id_or_pid || current->pid == job_id_or_pid) {
                target_job = current;
                break;
            }
            current = current->next;
        }
    } else 
    { 
        target_job = head_job; //head is the most recently added 
    }

    if (target_job == NULL) 
    {
        printf("fg: no such job\n");
        return;
    }

    printf("Bringing %s to foreground...\n", target_job->command);
    kill(target_job->pid, SIGCONT); // continue the process if stopped
    foreground_pid = target_job->pid;
    int status;
    waitpid(target_job->pid, &status, WUNTRACED); // wait for it to finish or stop
    foreground_pid = 0;

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        remove_job(target_job->pid);
    } else if (WIFSTOPPED(status)) {
        target_job->status = 1; // mark as stopped
    }
}

void handle_bg(char *input_string) {
    int job_id_or_pid = -1;
    char *token = strtok(input_string, " \t\n"); // "bg"
    token = strtok(NULL, " \t\n"); // argument

    if (token != NULL) {
        job_id_or_pid = atoi(token);
    }

    Job *target_job = NULL;
    if (job_id_or_pid != -1) {
        Job *current = head_job;
        while (current != NULL) {
            if (current->job_id == job_id_or_pid || current->pid == job_id_or_pid) {
                target_job = current;
                break;
            }
            current = current->next;
        }
    } else 
    { 
        Job *current = head_job;
        while (current != NULL) {
            if (current->status == 1)
             {                           // find a stopped job
                target_job = current;
                break;
            }
            current = current->next;
        }
    }

    if (target_job == NULL || target_job->status != 1) {
        printf("bg: no such job or job not stopped\n");
        return;
    }

    printf("Moving %s to background...\n", target_job->command);
    kill(target_job->pid, SIGCONT); // continue the process
    target_job->status = 0; // mark as running
}

//  piped commands
void execute_piped_commands(char *input_string, int num_pipes) 
{
    char *commands[num_pipes + 1];
    int i = 0;
    char *token = strtok(input_string, "|");
    while (token != NULL && i <= num_pipes) 
    {
        
        while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') end--;
        *(end + 1) = '\0';

        commands[i++] = token;
        token = strtok(NULL, "|");
    }

    int pipefds[2 * num_pipes]; 

    for (int j = 0; j < num_pipes; j++) 
    {
        if (pipe(pipefds + j * 2) < 0) 
        {
            perror("pipe failed");
            return;
        }
    }

    pid_t pids[num_pipes + 1];
    int in_fd = 0; 

    for (int k = 0; k <= num_pipes; k++) 
    {
        pids[k] = fork();

        if (pids[k] < 0) {
            perror("fork failed");
            return;
        }
        else if (pids[k] == 0) 
        { // child process
            // reset signal handlers for child process to default
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);

            if (in_fd != STDIN_FILENO) 
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (k < num_pipes) 
            {
                dup2(pipefds[k * 2 + 1], STDOUT_FILENO); // write to the next pipe
            }

            // close all pipe file descriptors in the child
            for (int j = 0; j < 2 * num_pipes; j++)
             {
                close(pipefds[j]);
            }

            // command arguments for execvp
            char *cmd_args[256];
            int arg_idx = 0;
            char *cmd_token = strtok(commands[k], " \t\n");
            while (cmd_token != NULL) 
            {
                cmd_args[arg_idx++] = cmd_token;
                cmd_token = strtok(NULL, " \t\n");
            }
            cmd_args[arg_idx] = NULL;

            if (cmd_args[0] == NULL) 
            {
                exit(EXIT_FAILURE); // empty command in pipe
            }

            execvp(cmd_args[0], cmd_args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else { // parent process
            if (k > 0) 
            { // close the read end of the previous pipe
                close(pipefds[(k - 1) * 2]);
            }
            if (k < num_pipes)
             { 
                in_fd = pipefds[k * 2];
                close(pipefds[k * 2 + 1]); 
            }
        }
    }

    for (int j = 0; j < 2 * num_pipes; j++) 
    {
        close(pipefds[j]);
    }


    for (int k = 0; k <= num_pipes; k++) {
        waitpid(pids[k], NULL, 0);
    }
}
