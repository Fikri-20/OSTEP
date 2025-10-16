# WISH - Wisconsin Shell

A complete Unix shell implementation in C, built as part of the [OSTEP (Operating Systems: Three Easy Pieces)](https://pages.cs.wisc.edu/~remzi/OSTEP/) project series.

## 📋 Table of Contents

- [About](#about)
- [Program Specification](#program-specification)
- [Features](#features)
- [Building](#building)
- [Usage](#usage)
- [Demo](#demo)
- [Testing](#testing)
- [Implementation Details](#implementation-details)
- [Project Structure](#project-structure)

## 🎯 About

**WISH** (Wisconsin Shell) is a simple yet fully-functional Unix shell that demonstrates core operating system concepts including:
- Process creation and management
- System call usage (fork, exec, wait, chdir, etc.)
- I/O redirection
- Parallel command execution
- Command parsing and error handling

This implementation achieves **100% test pass rate (22/22 tests)** and fully complies with the OSTEP project specification.

## 📖 Program Specification

This project follows the official specification from the OSTEP projects repository:
- **Full Specification**: [OSTEP Processes Shell Project](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/processes-shell)

### Key Requirements
- Interactive and batch mode operation
- Built-in commands: `exit`, `cd`, `path`
- Output redirection with `>`
- Parallel command execution with `&`
- Proper error handling with standardized error messages
- Support for executables in configurable search paths

## ✨ Features

### 🔧 Core Shell Functionality
- **Interactive Mode**: Run the shell interactively with a `wish> ` prompt
- **Batch Mode**: Execute commands from a script file
- **Process Management**: Creates child processes using `fork()` and `execv()`
- **Command Parsing**: Uses `getline()` for robust input handling with spaces and newlines

### 🏠 Built-in Commands

#### `exit`
Exits the shell cleanly.
```bash
wish> exit
```
- Takes no arguments (error if arguments provided)
- Calls `exit(0)` system call

#### `cd` 
Changes the current working directory.
```bash
wish> cd /tmp
wish> cd ..
```
- Requires exactly one argument (the target directory)
- Uses `chdir()` system call
- Errors on invalid paths or wrong number of arguments

#### `path`
Manages the executable search paths.
```bash
wish> path /bin /usr/bin          # Set search paths
wish> path /bin /usr/bin /usr/local/bin  # Multiple paths
wish> path                         # Clear all paths
```
- Takes 0 or more arguments
- Completely overwrites the previous path
- Default path: `/bin`

### 🔀 Output Redirection

Redirect both stdout and stderr to a file using `>`:
```bash
wish> ls -l > output.txt
wish> ls /nonexistent > errors.txt   # Errors also redirected
wish> echo Hello World > greeting.txt
```

**Features:**
- Redirects both stdout AND stderr to the same file
- Creates/truncates output files automatically
- Supports redirection without spaces: `cmd>file` works!
- Error handling for:
  - Missing filename after `>`
  - Multiple `>` operators
  - Extra arguments after output file
  - Empty command before `>`

### ⚡ Parallel Command Execution

Execute multiple commands simultaneously using `&`:
```bash
wish> cmd1 & cmd2 & cmd3
wish> echo First & echo Second & echo Third
wish> ls > out1.txt & date > out2.txt & pwd > out3.txt
```

**Features:**
- All commands start simultaneously before any waiting
- Shell waits for all processes to complete
- Works seamlessly with output redirection
- Supports `&` without surrounding spaces: `cmd1&cmd2` works!

**Restrictions:**
- Built-in commands cannot run in parallel with other commands
- Example: `cd /tmp & ls` will error

### 🛡️ Error Handling

- **Standardized Error Message**: All errors output `An error has occurred\n` to stderr
- **Graceful Continuation**: Shell continues after most errors
- **Critical Errors**: Exits with code 1 for:
  - Bad batch file
  - Multiple arguments (more than 1 file)
- **Comprehensive Validation**:
  - Command not found in search paths
  - Invalid built-in command usage
  - Redirection syntax errors
  - File I/O errors

## 🔨 Building

### Prerequisites
- GCC compiler
- Standard C library
- POSIX-compliant system (Linux, macOS, etc.)

### Compilation
```bash
gcc -Wall -Wextra -o wish main.c
```

The executable `wish` will be created in the current directory.

## 🚀 Usage

### Interactive Mode
Start the shell without arguments:
```bash
./wish
wish> ls
wish> cd /tmp
wish> pwd
wish> exit
```

### Batch Mode
Execute commands from a file:
```bash
./wish script.txt
```

**Example batch file** (`script.txt`):
```bash
path /bin /usr/bin
echo Starting batch execution
ls -l > files.txt
date > timestamp.txt
cat files.txt
```

## 🎬 Demo

### Basic Commands
```bash
$ ./wish
wish> ls
main.c  README.md  tests  wish
wish> pwd
/home/user/Unix-Shell
wish> echo Hello, WISH!
Hello, WISH!
wish> exit
```

### Path Management
```bash
wish> path /bin /usr/bin
wish> ls                    # Works - ls is in /bin
main.c  wish
wish> path                   # Clear path
wish> ls                    # Error - no search path
An error has occurred
wish> path /bin             # Restore path
wish> ls                    # Works again
main.c  wish
```

### Output Redirection
```bash
wish> ls -l > filelist.txt
wish> cat filelist.txt
total 32
-rw-r--r-- 1 user user 11234 Oct 16 09:31 main.c
-rwxr-xr-x 1 user user 17120 Oct 16 09:31 wish
wish> date > today.txt
wish> cat today.txt
Thu Oct 17 10:30:45 AM EDT 2025
```

### Redirection Without Spaces
```bash
wish> echo Testing>output.txt
wish> cat output.txt
Testing
wish> ls -la>/tmp/list.txt
wish> cat /tmp/list.txt
total 48
drwxr-xr-x 2 user user 4096 Oct 17 10:31 .
...
```

### Parallel Execution
```bash
wish> echo First & echo Second & echo Third
First
Second
Third
wish> sleep 1 & sleep 1 & sleep 1   # All execute in parallel (~1 sec total)
wish> date > time.txt & ls > files.txt & pwd > dir.txt
wish> cat time.txt
Thu Oct 17 10:32:15 AM EDT 2025
wish> cat dir.txt
/home/user/Unix-Shell
```

### Combined Features
```bash
wish> path /bin /usr/bin
wish> echo Line1 > out1.txt & echo Line2 > out2.txt & echo Line3 > out3.txt
wish> cat out1.txt & cat out2.txt & cat out3.txt
Line1
Line2
Line3
```

### Error Handling
```bash
wish> cd                    # Wrong number of arguments
An error has occurred
wish> cd /tmp /home         # Too many arguments
An error has occurred
wish> exit now              # Exit takes no arguments
An error has occurred
wish> > output.txt          # No command before >
An error has occurred
wish> ls > out1.txt out2.txt  # Multiple output files
An error has occurred
wish> ls >                  # No file after >
An error has occurred
```

## 🧪 Testing

### Test Suite
The project includes a comprehensive test suite with 22 tests covering all features.

**Run all tests:**
```bash
./run_tests.sh
```

### Test Results: ✅ 22/22 Passing (100%)

#### Test Coverage
1. ✅ Bad cd (no arguments)
2. ✅ Bad cd (2 arguments)
3. ✅ ls with bad directory name
4. ✅ Run miscellaneous commands
5. ✅ Exit with argument (should error)
6. ✅ Run shell script without setting path
7. ✅ Set path, run script, overwrite path
8. ✅ Redirection with no output file
9. ✅ Redirection with multiple output files
10. ✅ Multiple '>' operators
11. ✅ Normal redirection (without spaces)
12. ✅ No command before '>'
13. ✅ Shell invoked with 2 batch files (should error)
14. ✅ Bad batch file
15. ✅ Variable whitespace handling
16. ✅ Command contains only &
17. ✅ Parallel command with & at end
18. ✅ Basic parallel commands
19. ✅ Parallel commands without space before &
20. ✅ Redirection and parallel commands combined
21. ✅ Empty commands
22. ✅ Parallel execution verification (not serial)

### Test Categories
- **Built-in Commands**: Tests for `exit`, `cd`, and `path`
- **Redirection**: All redirection scenarios including edge cases
- **Parallel Execution**: Multiple commands with `&` operator
- **Error Handling**: Invalid syntax and usage errors
- **Batch Mode**: File-based command execution
- **Edge Cases**: Empty lines, whitespace variations, etc.

## 🔍 Implementation Details

### System Calls Used
- `fork()` - Create child processes
- `execv()` - Execute programs (not `system()` per spec)
- `wait()` / `waitpid()` - Wait for child process completion
- `chdir()` - Change current directory
- `exit()` - Exit the shell
- `open()`, `dup2()`, `close()` - I/O redirection
- `stat()` - Check file existence and executability
- `getline()` - Read input lines

### Key Design Decisions

#### 1. Redirection Normalization
The `normalize_redirection()` function preprocesses input to add spaces around `>`, allowing flexible syntax:
- `cmd>file` → `cmd > file`
- `cmd >file` → `cmd > file`
- `cmd> file` → `cmd > file`

#### 2. Parallel Execution Flow
```
Input → Split by & → Parse each command → Fork all processes → Wait for all
```
- All child processes are started before any `wait()` calls
- Ensures true parallelism, not sequential execution

#### 3. Error Detection
Parse errors are detected early and prevent command execution, avoiding duplicate error messages.

#### 4. Both stdout and stderr Redirection
Per specification, both stdout (fd 1) and stderr (fd 2) are redirected to the same file:
```c
dup2(fd, STDOUT_FILENO);
dup2(fd, STDERR_FILENO);
```

### Code Structure
```
main.c (423 lines)
├── Error handling (print_error)
├── Path management (init_paths, find_executable)
├── Input parsing (normalize_redirection, parse_single_command, split_parallel_commands)
├── Built-in commands (builtin_exit, builtin_cd, builtin_path, handle_builtin)
├── Command execution (execute_command_async)
├── Main loop (process_commands)
└── Entry point (main)
```

## 📁 Project Structure

```
Unix-Shell/
├── main.c              # Complete shell implementation (423 lines)
├── wish                # Compiled executable
├── README.md           # This file
├── run_tests.sh        # Automated test runner script
├── test-wish.sh        # Original test script
└── tests/              # Test suite directory
    ├── 1.in - 22.in    # Test input files
    ├── 1.out - 22.out  # Expected stdout
    ├── 1.err - 22.err  # Expected stderr
    ├── 1.rc - 22.rc    # Expected return codes
    ├── 1.desc - 22.desc # Test descriptions
    ├── p1.sh - p5.sh   # Test helper scripts
    └── p2a-test/       # Test data directory
```

## 🎓 Learning Outcomes

This project demonstrates understanding of:
- **Process Management**: Fork-exec model, process creation and synchronization
- **System Calls**: Low-level OS interfaces for process control and I/O
- **File Descriptors**: Manipulation for I/O redirection
- **Shell Internals**: How shells parse and execute commands
- **Error Handling**: Robust error detection and recovery
- **Concurrent Execution**: Parallel process management

## 📝 Notes

- **Spec Compliance**: 100% compliant with OSTEP project specification
- **Test Pass Rate**: 22/22 (100%)
- **No Memory Leaks**: Proper cleanup of allocated resources
- **POSIX Compatible**: Works on Linux, macOS, and other POSIX systems
- **Code Quality**: Compiled with `-Wall -Wextra` with no errors

## 📄 License

This project is part of the OSTEP educational projects. See the [OSTEP Projects repository](https://github.com/remzi-arpacidusseau/ostep-projects) for more information.

## 🙏 Acknowledgments

- **OSTEP (Operating Systems: Three Easy Pieces)** by Remzi H. Arpaci-Dusseau and Andrea C. Arpaci-Dusseau
- Project specification from the [OSTEP Projects repository](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/processes-shell)

---

**Built with** ❤️ **as part of learning operating systems fundamentals**
