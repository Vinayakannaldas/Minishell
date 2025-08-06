#ifndef MAIN_H
#define MAIN_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdio_ext.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
#include<unistd.h>
#include<errno.h> // For errno

#define BUILDIN    1
#define EXTERNAL   2
#define NO_COMMAND 3
#define BACKGROUND 4 // New define for background commands
#define PIPED      5 // New define for piped commands

#define ANSI_COLOUR_RED     "\x1b[31m"
#define ANSI_COLOUR_GREEN   "\x1b[32m"
#define ANSI_COLOUR_YELLOW  "\x1b[33m"
#define ANSI_COLOUR_BLUE    "\x1b[34m"
#define ANSI_COLOUR_MAGENTA "\x1b[35m"
#define ANSI_COLOUR_CYAN    "\x1b[36m"
#define ANSI_COLOUR_RESET   "\x1b[0m"

// Structure to hold job information for background processes
typedef struct Job {
    pid_t pid;
    char command[256];
    int status; // 0: Running, 1: Stopped, 2: Done
    int job_id;
    struct Job *next;
} Job;

// Global variables 
extern Job *head_job;
extern int next_job_id;
extern pid_t foreground_pid; // to track the foreground process for signal handling

// Function declarations
void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
void copy_change(char *prompt, char *input_string); 
int check_command_type(char *input_string, char **command_args, int *is_background, int *num_pipes);
void echo(char *input_string, int status);
void execute_internal_command(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
void execute_external_command(char **command_args, int is_background);
void add_job(pid_t pid, const char *command, int status);
void remove_job(pid_t pid);
void print_jobs();
void handle_fg(char *input_string);
void handle_bg(char *input_string);
void execute_piped_commands(char *input_string, int num_pipes);

#endif
