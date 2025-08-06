# Minishell Project

## Project Brief
This project implements a basic Unix-like shell, named `msh`, capable of executing built-in commands, external programs, handling background processes, and supporting command piping. It provides a fundamental command-line interface for interacting with the operating system, demonstrating core concepts of shell development such as process management, inter-process communication, and signal handling.

## Technologies Used
*   **C Language**: The primary programming language used for development.
*   **GNU Compiler Collection (GCC)**: Used for compiling the C source code.
*   **Standard Unix Libraries**: Utilizes various system calls and libraries for process creation (`fork`, `execvp`), inter-process communication (`pipe`), signal handling (`signal`, `kill`), and directory manipulation (`chdir`, `getcwd`).

## How to Compile and Run

1.  **Clone the repository:**
    ```bash
    git clone <repository_url>
    cd Minishell
    ```

2.  **Compile the source code:**
    Use the `gcc` compiler to compile all `.c` files into an executable.
    ```bash
    gcc -o msh *.c
    ```

3.  **Run the minishell:**
    ```bash
    ./msh
    ```

    You will see a prompt like `msh$`. You can then type commands.

    **Example Commands:**
    *   `ls`
    *   `pwd`
    *   `cd ..`
    *   `echo Hello World`
    *   `ls -l &` (for background process)
    *   `ps aux | grep msh` (for piping)
    *   `jobs`
    *   `fg %1` (to bring job 1 to foreground)
    *   `bg %1` (to move stopped job 1 to background)
    *   `exit` (to exit the shell)

## Key Learnings / Challenges

*   **Process Management**: Gained a deep understanding of `fork()` and `execvp()` for creating and executing new processes.
*   **Inter-Process Communication (IPC)**: Implemented pipes for communication between processes, enabling complex command pipelines.
*   **Signal Handling**: Developed robust signal handlers for `SIGINT` (Ctrl-C), `SIGTSTP` (Ctrl-Z), and `SIGCHLD` to manage foreground/background processes and clean up terminated children.
*   **Job Control**: Implemented basic job control features (`jobs`, `fg`, `bg`) to manage background and stopped processes.
*   **Memory Management**: Practiced dynamic memory allocation and deallocation, especially when parsing commands and managing job lists.
*   **Command Parsing**: Handled parsing of user input, including arguments, background indicators, and pipe symbols.
*   **Error Handling**: Integrated error checking for system calls to provide informative feedback to the user.

## Author
Vinayak Annaldas
