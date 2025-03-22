# Simple-Shell-Development
This project is a simple command-line shell implemented in C. It allows users to execute system commands, handle input and output redirection, and run processes in the background. The shell is designed to provide basic functionality similar to a Unix shell, making it a useful project for learning process management and system calls in Linux.

The shell supports executing external commands found in /bin/ and /usr/bin/, such as ls, pwd, and cat. It also includes built-in commands like cd for changing directories and exit to close the shell. Additionally, users can redirect input using < and output using >, as well as run processes in the background using &.

Features

Support for executing system commands using execvp()

Built-in commands: cd (change directory) and exit (terminate the shell)

Input redirection using < to read from a file

Output redirection using > to write to a file

Background process execution using &

Error handling for invalid commands and file operations

Installation and Compilation
Prerequisites

To run this shell, you need a Linux-based operating system (such as Ubuntu) and a GCC compiler installed. The shell uses standard system calls, so it may not work in Windows without a Unix-like environment such as WSL (Windows Subsystem for Linux).
