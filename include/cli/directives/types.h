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

typedef struct {
    const char *input;
    bool        from_url;
    bool        from_repo;
    const char *pkg_fmt;
    const char *pkg_name;
    const char *app_name;
    const char *exec_path;
    const char *working_dir;
    const char *icon_path;
    bool        add_path;
    bool        add_desktop;
    bool        add_tarman;
} cli_info_t;

typedef bool (*cli_fcn_t)(cli_info_t *info, const char *next);
typedef int (*cli_exec_t)(cli_info_t info);

typedef struct {
    const char *short_option;
    const char *full_option;
    cli_fcn_t   handler;
    bool        has_argument;
    cli_exec_t  exec_handler;
    const char *description;
} cli_drt_desc_t;
