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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli/directives/types.h"
#include "cli/output.h"

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

#ifndef EXT_TARMAN_BUILD
#define EXT_TARMAN_BUILD "unknown"
#endif

#ifndef EXT_TARMAN_OS
#define EXT_TARMAN_OS "unknown"
#endif

#ifndef EXT_TARMAN_COMPILER
#if defined(__GNUC__) && defined(__GNUC_MINOR__) &&        \
    defined(__GNUC_PATCHLEVEL__) && !defined(__clang__) && \
    !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define EXT_TARMAN_COMPILER \
    "gcc " STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#elif defined(__clang_minor__) && defined(__clang_major__)
#define EXT_TARMAN_COMPILER \
    "clang " STR(__clang_major__) "." STR(__clang_minor__)
#elif defined(__clang__)
#define EXT_TARMAN_COMPILER "clang (unknown version)"
#elif defined(__llvm__)
#define EXT_TARMAN_COMPILER "generic llvm"
#elif defined(_MSC_VER)
#define EXT_TARMAN_COMPILER "msc " _MSC_VER
#else
#define EXT_TARMAN_COMPILER "unknown"
#endif
#endif // EXT_TARMAN_COMPILER

int cli_cmd_version(cli_info_t info) {
    (void)info;

    puts("tarman version " EXT_TARMAN_BUILD);
    puts("target: " EXT_TARMAN_OS);
    puts("compiled with: " EXT_TARMAN_COMPILER);

    return EXIT_SUCCESS;
}
