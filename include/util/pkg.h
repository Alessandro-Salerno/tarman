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

#include "package.h"

#define LOG_ON    true
#define LOG_QUIET false

#define INPUT_ON  true
#define INPUT_OFF false

bool util_pkg_fetch_archive(char      **dst_file,
                            const char *pkg_name,
                            const char *pkg_fmt,
                            const char *url,
                            bool        log);
bool util_pkg_create_directory_from_path(const char *path, bool log, bool in);
bool util_pkg_create_directory(char      **path,
                               const char *pkg_name,
                               bool        log,
                               bool        in);
bool util_pkg_add_to_path(const char *exec_full_path, bool log);
bool util_pkg_add_to_desktop(const char *pkg_path,
                             const char *app_name,
                             const char *exec_full_path,
                             const char *wrk_dir,
                             const char *icon,
                             bool        log);
bool util_pkg_parse_config(pkg_info_t *pkg,
                           char      **cfg_path,
                           const char *pkg_path,
                           bool        log);
bool util_pkg_load_config(pkg_info_t *pkg, const char *pkg_path, bool log);
bool util_pkg_parse_recipe(recipe_t   *recipe,
                           char      **rcp_path,
                           const char *repo,
                           const char *rcp_name,
                           bool        log);
bool util_pkg_load_recipe(recipe_t   *recipe,
                          const char *repo,
                          const char *rcp_name,
                          bool        log);
