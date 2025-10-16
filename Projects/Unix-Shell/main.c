#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PATHS 10
#define MAX_ARGS 64
#define MAX_PARALLEL_CMDS 10

// Global path array for searching executables
char *search_paths[MAX_PATHS];
int num_paths = 0;

// Error message function
void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Function to initialize default paths
void init_paths() {
    search_paths[num_paths++] = strdup("/bin");
}

// Function to find the executable in the search paths
char* find_executable(char *command) {
    static char full_path[256];
    struct stat st;
    
    // Try each path in search_paths
    for (int i = 0; i < num_paths; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", search_paths[i], command);
        
        // Check if file exists and is executable
        if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR)) {
            return full_path;
        }
    }
    
    return NULL;  // Not found
}

// Function to normalize redirection by adding spaces around '>'
// Returns a newly allocated string that must be freed by caller
char* normalize_redirection(char *cmd_str) {
    static char normalized[4096];
    int j = 0;
    
    for (int i = 0; cmd_str[i] != '\0' && j < 4095; i++) {
        if (cmd_str[i] == '>') {
            // Add space before '>' if not already there
            if (j > 0 && normalized[j-1] != ' ' && normalized[j-1] != '\t') {
                normalized[j++] = ' ';
            }
            normalized[j++] = '>';
            // Add space after '>' if next char is not space
            if (cmd_str[i+1] != ' ' && cmd_str[i+1] != '\t' && cmd_str[i+1] != '\0') {
                normalized[j++] = ' ';
            }
        } else {
            normalized[j++] = cmd_str[i];
        }
    }
    normalized[j] = '\0';
    return normalized;
}

// Function to parse a single command into arguments and handle redirection
// Returns: arg_count, and sets redirect_file if redirection is found
int parse_single_command(char *cmd_str, char **args, char **redirect_file) {
    int argc = 0;
    char *token;
    int redirect_count = 0;
    
    // Initialize redirect_file
    *redirect_file = NULL;
    
    // Normalize redirection (add spaces around '>')
    cmd_str = normalize_redirection(cmd_str);
    
    // Skip leading whitespace
    while (*cmd_str == ' ' || *cmd_str == '\t') {
        cmd_str++;
    }
    
    // If empty command, return 0
    if (*cmd_str == '\0') {
        return 0;
    }
    
    // Tokenize the command
    token = strtok(cmd_str, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        // Check for redirection operator
        if (strcmp(token, ">") == 0) {
            redirect_count++;
            
            // Check if there's a command before >
            if (argc == 0) {
                print_error();
                return -1;  // Error: no command before >
            }
            
            // Get the next token as the redirect file
            token = strtok(NULL, " \t");
            if (token == NULL || redirect_count > 1) {
                // Error: no file specified after > or multiple redirections
                print_error();
                return -1;  // Error
            }
            *redirect_file = token;
            
            // Check if there are more tokens after redirect file
            token = strtok(NULL, " \t");
            if (token != NULL) {
                // Error: extra tokens after redirect file
                print_error();
                return -1;
            }
            break;  // No more tokens to process
        }
        
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;  // NULL-terminate the array
    
    return argc;
}

// Function to split input by '&' and return array of command strings
int split_parallel_commands(char *input, char **commands) {
    int cmd_count = 0;
    char *token;
    
    // Remove trailing newline if present
    input[strcspn(input, "\n")] = '\0';
    
    // Skip leading whitespace
    while (*input == ' ' || *input == '\t') {
        input++;
    }
    
    // If empty line, return 0
    if (*input == '\0') {
        return 0;
    }
    
    // Split by '&'
    token = strtok(input, "&");
    while (token != NULL && cmd_count < MAX_PARALLEL_CMDS) {
        commands[cmd_count++] = token;
        token = strtok(NULL, "&");
    }
    
    return cmd_count;
}

// Function to handle the built-in 'exit' command
void builtin_exit(char **args, int arg_count) {
    // Exit should have no arguments
    if (arg_count > 1) {
        print_error();
        return;
    }
    exit(0);
}

// Function to handle the built-in 'cd' command
void builtin_cd(char **args, int arg_count) {
    // cd should have exactly one argument (the directory)
    if (arg_count != 2) {
        print_error();
        return;
    }
    
    // Change directory
    if (chdir(args[1]) != 0) {
        print_error();
    }
}

// Function to handle the built-in 'path' command
void builtin_path(char **args, int arg_count) {
    // Clear existing paths
    for (int i = 0; i < num_paths; i++) {
        free(search_paths[i]);
        search_paths[i] = NULL;
    }
    num_paths = 0;
    
    // Add new paths from arguments
    for (int i = 1; i < arg_count && num_paths < MAX_PATHS; i++) {
        search_paths[num_paths++] = strdup(args[i]);
    }
}

// Function to check if command is a built-in and execute it
int handle_builtin(char **args, int arg_count) {
    if (strcmp(args[0], "exit") == 0) {
        builtin_exit(args, arg_count);
        return 1;  // Built-in handled
    } else if (strcmp(args[0], "cd") == 0) {
        builtin_cd(args, arg_count);
        return 1;  // Built-in handled
    } else if (strcmp(args[0], "path") == 0) {
        builtin_path(args, arg_count);
        return 1;  // Built-in handled
    }
    
    return 0;  // Not a built-in command
}

// Function to execute a command with optional redirection
// Returns PID of child process, or -1 on error, or 0 if command not found
pid_t execute_command_async(char **args, char *redirect_file) {
    char *executable_path;
    pid_t pid;
    
    // Find the executable
    executable_path = find_executable(args[0]);
    
    if (executable_path == NULL) {
        // Command not found in any search path
        print_error();
        return 0;
    }
    
    // Fork a child process
    pid = fork();
    
    if (pid < 0) {
        // Fork failed
        print_error();
        exit(1);
    } else if (pid == 0) {
        // Child process: set up redirection if needed
        if (redirect_file != NULL) {
            // Open the output file
            int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                print_error();
                exit(1);
            }
            
            // Redirect stdout to the file
            if (dup2(fd, STDOUT_FILENO) < 0) {
                print_error();
                close(fd);
                exit(1);
            }
            
            // Redirect stderr to the file (as per spec)
            if (dup2(fd, STDERR_FILENO) < 0) {
                print_error();
                close(fd);
                exit(1);
            }
            
            close(fd);
        }
        
        // Execute the command
        if (execv(executable_path, args) == -1) {
            print_error();
            exit(1);
        }
    }
    
    // Parent process: return child PID
    return pid;
}

// Function to process commands from an input stream
void process_commands(FILE *input_stream, int interactive) {
    char *input = NULL;
    size_t input_size = 0;
    ssize_t chars_read;
    char *parallel_cmds[MAX_PARALLEL_CMDS];
    char *cmd_copy;
    int num_cmds;
    
    while (1) {
        // Print the prompt only in interactive mode
        if (interactive) {
            printf("wish> ");
            fflush(stdout);
        }
        
        // Read user input using getline
        chars_read = getline(&input, &input_size, input_stream);
        
        // Check for EOF
        if (chars_read == -1) {
            if (interactive) {
                printf("\n");
            }
            break;
        }
        
        // Create a copy of input for processing (since strtok modifies it)
        cmd_copy = strdup(input);
        
        // Split input into parallel commands separated by '&'
        num_cmds = split_parallel_commands(cmd_copy, parallel_cmds);
        
        // If no commands, continue to next iteration
        if (num_cmds == 0) {
            free(cmd_copy);
            continue;
        }
        
        // Check if any command is a built-in
        // Built-ins cannot be run in parallel with other commands
        int has_builtin = 0;
        int has_parse_error = 0;
        char *args[MAX_ARGS];
        char *redirect_file;
        int arg_count;
        
        for (int i = 0; i < num_cmds; i++) {
            // Make a copy for parsing
            char *temp_cmd = strdup(parallel_cmds[i]);
            arg_count = parse_single_command(temp_cmd, args, &redirect_file);
            
            if (arg_count < 0) {
                has_parse_error = 1;
                free(temp_cmd);
                break;
            }
            
            if (arg_count > 0) {
                if (strcmp(args[0], "exit") == 0 || 
                    strcmp(args[0], "cd") == 0 || 
                    strcmp(args[0], "path") == 0) {
                    has_builtin = 1;
                    free(temp_cmd);
                    break;
                }
            }
            free(temp_cmd);
        }
        
        // If there was a parse error, skip this line
        if (has_parse_error) {
            free(cmd_copy);
            continue;
        }
        
        // If there's a built-in with parallel commands, it's an error
        if (has_builtin && num_cmds > 1) {
            print_error();
            free(cmd_copy);
            continue;
        }
        
        // If single built-in command, execute it
        if (has_builtin && num_cmds == 1) {
            char *temp_cmd = strdup(parallel_cmds[0]);
            arg_count = parse_single_command(temp_cmd, args, &redirect_file);
            
            if (arg_count > 0) {
                handle_builtin(args, arg_count);
            }
            
            free(temp_cmd);
            free(cmd_copy);
            continue;
        }
        
        // Execute parallel commands
        pid_t pids[MAX_PARALLEL_CMDS];
        int valid_pids = 0;
        
        for (int i = 0; i < num_cmds; i++) {
            char *temp_cmd = strdup(parallel_cmds[i]);
            arg_count = parse_single_command(temp_cmd, args, &redirect_file);
            
            if (arg_count > 0) {
                pid_t pid = execute_command_async(args, redirect_file);
                if (pid > 0) {
                    pids[valid_pids++] = pid;
                }
            }
            
            free(temp_cmd);
        }
        
        // Wait for all child processes to complete
        for (int i = 0; i < valid_pids; i++) {
            waitpid(pids[i], NULL, 0);
        }
        
        free(cmd_copy);
    }
    
    // Cleanup
    free(input);
}

int main(int argc, char *argv[]) {
    FILE *batch_file = NULL;
    
    // Initialize the search paths
    init_paths();
    
    // Check if a batch file was provided
    if (argc == 1) {
        // Interactive mode - read from stdin
        process_commands(stdin, 1);
    } else if (argc == 2) {
        // Batch mode - read from file
        batch_file = fopen(argv[1], "r");
        if (batch_file == NULL) {
            print_error();
            exit(1);
        }
        process_commands(batch_file, 0);
        fclose(batch_file);
    } else {
        // Too many arguments
        print_error();
        exit(1);
    }
    
    // Cleanup
    for (int i = 0; i < num_paths; i++) {
        free(search_paths[i]);
    }
    
    return 0;
}
