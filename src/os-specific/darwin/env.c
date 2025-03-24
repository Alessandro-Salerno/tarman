/*************************************************************************
| tarman                                                                 |
| Copyright (C) 2024 - 2025 Alessandro Salerno                                  |
|                                                                        |
| This program is free software: you can redistribute it and/or modify   |
| it under the terms of the GNU General Public License as published by   |
| the Free Software Foundation, either version 2 of the License, or      |
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

#include <pwd.h>
#include <stdbool.h>
#include <unistd.h>

#include "os/posix/env.h"

bool os_env_path_add(const char *executable) {
    return posix_env_path_add(executable);
}

bool os_env_path_rm(const char *executable) {
    return posix_env_path_rm(executable);
}

bool os_env_desktop_add(const char *app_name,
                        const char *executable_path,
                        const char *icon_path,
                        const char *wrk_dir) {
    // Not implemented
    (void)app_name;
    (void)executable_path;
    (void)icon_path;
    (void)wrk_dir;
    return false;
}

bool os_env_desktop_rm(const char *app_name) {
    // Not implemented
    (void)app_name;
    return false;
}
