/*
trace for Question 5 (fork/execvp/wait):
$ ./miniShell 
mini-shell> echo "Hello Wilson"
"Hello Wilson"
mini-shell> ls
CMakeCache.txt  cmake_install.cmake  Makefile
CMakeFiles      combat               miniShell
mini-shell> quit
execvp failed: No such file or directory
mini-shell> exit
execvp failed: No such file or directory
mini-shell> whomai
execvp failed: No such file or directory
mini-shell> ^C

trace for Question 6 (Ctrl-C handling):
$ ./miniShell 
mini-shell> ls
CMakeCache.txt  cmake_install.cmake  Makefile
CMakeFiles      combat               miniShell
mini-shell> sleep 10
^Cmini-shell> ^C
Exiting on EOF (Ctrl-D).

trace for Question 7 (cadded a trace in sigint handler):
./miniShell 
mini-shell> ls
CMakeCache.txt  cmake_install.cmake  Makefile
CMakeFiles      combat               miniShell
mini-shell> sleep 10
^C
[Trace] Caught SIGINT!
mini-shell> ^C
[Trace] Caught SIGINT!

Exiting on EOF (Ctrl-D).

diagnostic for Question 8 (zombie processes):
./miniShell 
mini-shell> sleep 10
^C
[Trace] Caught SIGINT!
mini-shell> ps
    PID TTY          TIME CMD
  22152 pts/1    00:00:00 bash
  37577 pts/1    00:00:00 miniShell
  37610 pts/1    00:00:00 sleep
  37668 pts/1    00:00:00 ps
mini-shell> ^C
[Trace] Caught SIGINT!

Exiting on EOF (Ctrl-D).


trace for sol to Question 8 (no zombie processes in ps now):
./miniShell 
mini-shell> sleep 10
^C
[Trace] Caught SIGINT!
mini-shell> ps
    PID TTY          TIME CMD
  22152 pts/1    00:00:00 bash
  38415 pts/1    00:00:00 miniShell
  38466 pts/1    00:00:00 ps
mini-shell> ^C
[Trace] Caught SIGINT!

Exiting on EOF (Ctrl-D).

trace for Question 9 (diagnostics):
./miniShell 
mini-shell> ps   
    PID TTY          TIME CMD
  22152 pts/1    00:00:00 bash
  39698 pts/1    00:00:00 miniShell
  39722 pts/1    00:00:00 ps
Child exited normally with code 0
mini-shell> sleep 5
Child exited normally with code 0
mini-shell> sleep 10
^C
[Trace] Caught SIGINT!
Child terminated by signal 2
mini-shell> ^C
[Trace] Caught SIGINT!

Exiting on EOF (Ctrl-D).


trace for Question 10 (graceful exit and SIGTERM handling):
./miniShell 
mini-shell> ps
    PID TTY          TIME CMD
  22152 pts/1    00:00:00 bash
  42152 pts/1    00:00:00 miniShell
  42184 pts/1    00:00:00 ps
Child exited normally with code 0
mini-shell> kill -TERM 42152

[Trace] Caught SIGTERM! BYE!


Answer to last part of question 10:
external commands: require fork/exec/wait as implemented
built-in commands: handle directly in the shell without fork/exec/wait
examples of built-in commands: exit, quit, cd, help
example of external commands: ls, echo, ps, sleep, whomai

*/


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>

void handle_sigint(int sig) {
    const char* msg = "\n[Trace] Caught SIGINT!\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

void handle_exit(int sig) {
    const char* msg = "\n[Trace] Caught SIGTERM! BYE! \n";
    write(STDOUT_FILENO, msg, strlen(msg));
    _exit(0); 
}



// Custom mystrdup : allocates with new[], copies string (avoid strdup and free)
char* mystrdup(const char* src);

int main() {
    // ---------- q6: register sigint handler ------
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; 

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }
    // --------------------------------------------


    // ---------- q10: register sigterm handler -----
    struct sigaction sa_term;
    sa_term.sa_handler = handle_exit;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;

    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }
    // ---------------------------------------------

    std::string line;

    while (true) {
        std::cout << "mini-shell> " << std::flush;

        if (!std::getline(std::cin, line)) {
            std::cout << "\nExiting on EOF (Ctrl-D)." << std::endl;
            break;
        }

        if (line.empty()) continue;

        // Simple parsing: split by spaces using istringstream
        std::istringstream iss(line);
        std::vector<std::string> args;
        std::string token;
        while (iss >> token) {
            args.push_back(token);
        }
        if (args.empty()) continue;

        // Prepare C-style argv: allocate and mystrdup
        char** argv = new char*[args.size()];
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i] = mystrdup(args[i].c_str());
        }

        if (args[0] == "exit" || args[0] == "quit") {
            // cleanup argv allocations
            for (size_t i = 0; i < args.size(); ++i) {
                delete[] argv[i];
            }
            delete[] argv;
            break;
        }

        // Your code for fork/exec/wait/signals goes here...
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            // cleanup argv allocations
            for (size_t i = 0; i < args.size(); ++i) {
                delete[] argv[i];
            }
            delete[] argv;
            continue;
        }

        if (pid == 0) {
            // Child process
            // Prepare argv for execvp: add nullptr terminator
            char** exec_argv = new char*[args.size() + 1];
            for (size_t i = 0; i < args.size(); ++i) {
                exec_argv[i] = argv[i];
            }
            exec_argv[args.size()] = nullptr;

            execvp(exec_argv[0], exec_argv);
            perror("execvp failed");
            delete[] exec_argv; //cleanup exec_argv (but nott argv[i] since they are shared)
            exit(1);
        } else {
            // Parent process
            int status; 
            pid_t waited_pid;
            //-----------------Questio 8--------------------
            // If waitpid returns -1 because of a signal (Ctrl-C), we must call it again to ensure we collect the zombie child
            while ((waited_pid = waitpid(pid, &status, 0)) == -1) {
                if (errno != EINTR) {
                    perror("waitpid failed");
                    break;
                }
                // If errno == EINTR, we loop back and wait again
            }

            //--------------------Question 9(Diagnostics)-------------------
            if (waited_pid != -1) {
                if (WIFEXITED(status)) {
                    // The child finished voluntarily
                    std::cout << "Child exited normally with code " 
                            << WEXITSTATUS(status) << std::endl;
                } 
                else if (WIFSIGNALED(status)) {
                    // The child was killed by a signal (eg Ctrl-C)
                    std::cout << "Child terminated by signal " 
                            << WTERMSIG(status) << std::endl;
                }
            }
        }

        // cleanup argv allocations
        for (size_t i = 0; i < args.size(); ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
    return 0;
}


char* mystrdup(const char* src) {
    if (src == nullptr) return nullptr;
    size_t len = strlen(src) + 1;  // +1 for null terminator
    char* dest = new char[len];
    memcpy(dest, src, len);  // Or strcpy if preferred
    return dest;
}
