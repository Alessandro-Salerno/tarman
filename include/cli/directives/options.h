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

#include "cli/directives/types.h"

#define TARMAN_SOPT_FROM_URL    "-u"
#define TARMAN_SOPT_FROM_REPO   "-r"
#define TARMAN_SOPT_PKG_FMT     "-f"
#define TARMAN_SOPT_PKG_NAME    "-n"
#define TARMAN_SOPT_APP_NAME    "-a"
#define TARMAN_SOPT_EXEC        "-x"
#define TARMAN_SOPT_WRK_DIR     "-w"
#define TARMAN_SOPT_ICON        "-i"
#define TARMAN_SOPT_ADD_PATH    "-p"
#define TARMAN_SOPT_ADD_DESKTOP "-d"
#define TARMAN_SOPT_ADD_TARMAN  "-t"

#define TARMAN_FOPT_FROM_URL    "--from-url"
#define TARMAN_FOPT_FROM_REPO   "--from-repo"
#define TARMAN_FOPT_PKG_FMT     "--format"
#define TARMAN_FOPT_PKG_NAME    "--pkg-name"
#define TARMAN_FOPT_APP_NAME    "--app-name"
#define TARMAN_FOPT_EXEC        "--exec"
#define TARMAN_FOPT_WRK_DIR     "--working-dir"
#define TARMAN_FOPT_ICON        "--icon"
#define TARMAN_FOPT_ADD_PATH    "--add-path"
#define TARMAN_FOPT_ADD_DESKTOP "--add-desktop"
#define TARMAN_FOPT_ADD_TARMAN  "--add-tarman"

bool cli_opt_from_url(cli_info_t *info, const char *next);
bool cli_opt_from_repo(cli_info_t *info, const char *next);
bool cli_opt_pkg_fmt(cli_info_t *info, const char *next);
bool cli_opt_pkg_name(cli_info_t *info, const char *next);
bool cli_opt_app_name(cli_info_t *info, const char *next);
bool cli_opt_exec(cli_info_t *info, const char *next);
bool cli_opt_wrk_dir(cli_info_t *info, const char *next);
bool cli_opt_icon(cli_info_t *info, const char *next);
bool cli_opt_add_path(cli_info_t *info, const char *next);
bool cli_opt_add_desktop(cli_info_t *info, const char *next);
bool cli_opt_add_tarman(cli_info_t *info, const char *next);
