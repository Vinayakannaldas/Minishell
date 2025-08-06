/*
Name : Vinayak Annaldas
P1 :- Minishell
*/
#include "main.h"

// global variables 
Job *head_job = NULL;
int next_job_id = 1;
pid_t foreground_pid = 0; // initialize to 0, no foreground process

int main()
{
    // clear the screen
    system("clear");

    // set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler); // Handle child process termination

    // Variable declaration
    
    char input_string[256]; // array to store the input
    
    char prompt[25] = "msh";// array to store the prompt

    // Scan the input
    scan_input(prompt, input_string);

    return 0;
}

// signal handler function
void signal_handler(int sig_num)
{
    if (sig_num == SIGINT) // Ctrl-C
    {
        if (foreground_pid > 0)
        {
            kill(foreground_pid, SIGINT); // send SIGINT to foreground process
            foreground_pid = 0; // reset foreground PID
        }
        else
        {
            printf("\n"); 
            fflush(stdout); // flush output buffer
        }
    }
    else if (sig_num == SIGTSTP) // Ctrl-Z
    {
        if (foreground_pid > 0)
        {
            kill(foreground_pid, SIGTSTP); 
            // send SIGTSTP to foreground process
            // mark job as stopped and add to background jobs
            

            printf("\nStopped process with PID: %d\n", foreground_pid);
            // find the job and update its status to stopped
            Job *current = head_job;
            while (current != NULL) {
                if (current->pid == foreground_pid) {
                    current->status = 1; // 1: Stopped
                    break;
                }
                current = current->next;
            }
            foreground_pid = 0; // Reset foreground PID
        }
        else
        {
            printf("\n"); // Newline to clear current line
            fflush(stdout); // Flush output buffer
        }
    }
    else if (sig_num == SIGCHLD) // SIGCHLD for background process cleanup
    {
        pid_t child_pid;
        int status;
        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            // child process has terminated
            Job *current = head_job;
            while (current != NULL)
            {
                if (current->pid == child_pid)
                {
                    printf("\n[%d] Done %s\n", current->job_id, current->command);
                    remove_job(child_pid);
                    break;
                }
                current = current->next;
            }
        }
    }
}
