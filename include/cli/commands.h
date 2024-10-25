/*************************************************************************
| tarman                                                                 |
| Copyright (C) 2024 Alessandro Salerno                                  |
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

typedef struct {
  const char *input;
  bool        from_repo;
  const char *app_name;
  const char *exec_path;
  const char *working_dir;
  bool        add_path;
  bool        add_desktop;
} cli_info_t;

typedef bool (*cli_fcn_t)(cli_info_t *info, const char *next);
typedef int (*cli_exec_t)(cli_info_t info);

typedef struct {
  const char *short_option;
  const char *full_option;
  cli_fcn_t   handler;
  bool        has_argument;
  cli_exec_t  exec_handler;
} cli_cmd_desc_t;

bool cli_opt_from_url(cli_info_t *info, const char *next);
bool cli_opt_from_repo(cli_info_t *info, const char *next);
bool cli_opt_app_name(cli_info_t *info, const char *next);
bool cli_opt_exec(cli_info_t *info, const char *next);
bool cli_opt_wrk_dir(cli_info_t *info, const char *next);
bool cli_opt_add_path(cli_info_t *info, const char *next);
bool cli_opt_add_desktop(cli_info_t *info, const char *next);

// Commands
int cli_cmd_help(cli_info_t info);
int cli_cmd_install(cli_info_t info);
int cli_cmd_list(cli_info_t info);
int cli_cmd_remove(cli_info_t info);
int cli_cmd_update(cli_info_t info);
int cli_cmd_update_all(cli_info_t info);
int cli_cmd_add_repo(cli_info_t info);
int cli_cmd_remove_repo(cli_info_t info);
int cli_cmd_list_repos(cli_info_t info);
