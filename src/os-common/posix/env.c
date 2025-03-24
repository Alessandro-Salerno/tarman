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

// MUST BE HERE
#include <tm-os-defs.h>

// Other includes
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "os/fs.h"
#include "os/posix/env.h"
#include "os/posix/fs.h"
#include "tm-mem.h"

static const char *get_name(const char *path) {
    size_t len      = strlen(path);
    size_t last_idx = len - 2;

    for (size_t i = last_idx; i > 0; i--) {
        if ('/' == path[i]) {
            return &path[i + 1];
        }
    }

    return path;
}

bool posix_env_path_add(const char *executable) {
    if (NULL == executable) {
        return false;
    }

    const char *path_item = NULL;
    const char *exec_name = get_name(executable);

    if (0 == posix_fs_tm_dyexecpath(&path_item, exec_name)) {
        return false;
    }

    if (0 != symlink(executable, path_item)) {
        mem_safe_free(path_item);
        return false;
    }

    mem_safe_free(path_item);
    return true;
}

bool posix_env_path_rm(const char *executable) {
    if (NULL == executable) {
        return false;
    }

    const char *path_item = NULL;
    const char *exec_name = get_name(executable);

    if (0 == posix_fs_tm_dyexecpath(&path_item, exec_name)) {
        return false;
    }

    fs_fileop_status_t rm_status = os_fs_file_rm(path_item);
    mem_safe_free(path_item);
    return TM_FS_FILEOP_STATUS_OK == rm_status;
}
