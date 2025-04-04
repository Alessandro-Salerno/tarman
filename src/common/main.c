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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli/directives/commands.h"
#include "cli/directives/types.h"
#include "cli/parser.h"

int main(int argc, char *argv[]) {
    cli_info_t cli_info        = {0};
    cli_exec_t command_handler = NULL;

    if (!cli_parse(argc, argv, &cli_info, &command_handler)) {
        return EXIT_FAILURE;
    }

    if (NULL == command_handler) {
        return cli_cmd_help(cli_info);
    }

    return command_handler(cli_info);
}
