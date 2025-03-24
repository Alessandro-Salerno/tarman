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
#include <stdio.h>

#include "config.h"

// Structure for tarman package files (packaage.tarman)
typedef struct {
    const char *url;
    const char *from_repoistory;
    const char *application_name;
    const char *executable_path;
    const char *working_directory;
    const char *icon_path;
} pkg_info_t;

// Structure of tarman recipe fles (recipe.tarman or <packagename>.tarman)
typedef struct {
    pkg_info_t  pkg_info;
    const char *package_format;
    bool        add_to_path;
    bool        add_to_desktop;
    bool        add_to_tarman;
} recipe_t;

// "Runtime" recipe
typedef struct {
    recipe_t    recipe;
    const char *pkg_name;
    bool        is_remote;
} rt_recipe_t;

cfg_parse_status_t pkg_parse_ftmpkg(pkg_info_t *pkg_info, FILE *pkg_file);
cfg_parse_status_t pkg_parse_tmpkg(pkg_info_t *pkg_info,
                                   const char *pkg_file_path);

cfg_parse_status_t pkg_parse_ftmrcp(recipe_t *rcp, FILE *rcp_file);
cfg_parse_status_t pkg_parse_tmrcp(recipe_t *rcp, const char *rcp_file_path);

bool pkg_dump_frcp(FILE *fp, recipe_t recipe);
bool pkg_dump_rcp(const char *file_path, recipe_t recipe);

void pkg_free_pkg(pkg_info_t pkg_info);
void pkg_free_rcp(recipe_t recipe);
