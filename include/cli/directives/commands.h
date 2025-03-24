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

#pragma once

#include <stdbool.h>

#include "cli/directives/types.h"

#define TARMAN_CMD_HELP        "help"
#define TARMAN_CMD_INSTALL     "install"
#define TARMAN_CMD_LIST        "list"
#define TARMAN_CMD_REMOVE      "remove"
#define TARMAN_CMD_UPDATE      "update"
#define TARMAN_CMD_UPDATE_ALL  "update-all"
#define TARMAN_CMD_ADD_REPO    "add-repo"
#define TARMAN_CMD_REMOVE_REPO "remove-repo"
#define TARMAN_CMD_LIST_REPOS  "list-repos"
#define TARMAN_CMD_TEST        "test"
#define TARMAN_CMD_VERSION     "version"

int cli_cmd_help(cli_info_t info);
int cli_cmd_install(cli_info_t info);
int cli_cmd_list(cli_info_t info);
int cli_cmd_remove(cli_info_t info);
int cli_cmd_update(cli_info_t info);
int cli_cmd_update_all(cli_info_t info);
int cli_cmd_add_repo(cli_info_t info);
int cli_cmd_remove_repo(cli_info_t info);
int cli_cmd_list_repos(cli_info_t info);
int cli_cmd_test(cli_info_t info);
int cli_cmd_version(cli_info_t info);
