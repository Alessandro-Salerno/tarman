/*************************************************************************
| tarman                                                                 |
| Copyright (C) 2024 - 2025 Alessandro Salerno                                  |
|                                                                        |
| This program is free software: you can redistribute it and/or modify   |
| it under the terms of the GNU General Public License as published by   |
| the Free Software Foundation, either version 3 of the License, or      |
| (at your option) any later version.                                    |
|                                                                        |
| This program is distributed in the hope that it will be useful,        |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with this program.  If not, see <https://www.gnu.org/licenses/>. |
*************************************************************************/

// MUST BE HERE
#include <stdio.h>
#include <tm-os-defs.h>

// Other includes
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "os/posix/exec.h"
#include "tm-mem.h"

static size_t count_args(va_list args) {
    va_list copy;
    va_copy(copy, args);
    size_t count = 0;
    for (; NULL != va_arg(copy, char *); count++)
        ;
    va_end(copy);
    return count + 1 + 1; // Add 1 for NULL and for the program
}

int posix_vexec(const char *executable, va_list args) {
    size_t       arg_count = count_args(args);
    const char **argv      = (const char **)malloc(arg_count * sizeof(char *));
    mem_chkoom(argv);

    argv[0] = executable;
    for (size_t i = 1; i < arg_count; i++) {
        char *arg = va_arg(args, char *);
        argv[i]   = arg;
    }

    pid_t pid = fork();

    if (0 > pid) {
        return EXIT_FAILURE;
    }

    // When fork gets here it means
    // that this is the child process
    if (0 == pid) {
        fclose(stdout);
        fclose(stderr);
        fclose(stdin);
        return execvp(executable, (char **)argv);
    }

    int status;
    int ret = EXIT_FAILURE;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        ret = WEXITSTATUS(status);
    }

    mem_safe_free(argv);
    return ret;
}
