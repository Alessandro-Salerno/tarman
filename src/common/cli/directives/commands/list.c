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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli/directives/commands.h"
#include "cli/output.h"
#include "os/console.h"
#include "os/fs.h"
#include "tm-mem.h"

static bool find_max_pkg_name_len(size_t *len) {
    char  *pkg_dir = NULL;
    bool   ret     = false;
    size_t max_len = 0;
    os_fs_tm_dypkgs(&pkg_dir);

    os_fs_dirstream_t dir_stream;

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_open(&dir_stream, pkg_dir)) {
        goto cleanup;
    }

    fs_dirent_t ent;
    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(dir_stream, &ent)) {
        if (TM_FS_FILETYPE_DIR == ent.file_type) {
            size_t len = strlen(ent.name);

            if (len > max_len) {
                max_len = len;
            }
        }
    }

    os_fs_dir_close(dir_stream);
    *len = max_len;
    ret  = true;

cleanup:
    mem_safe_free(pkg_dir);
    return ret;
}

static void simple_print(void) {
    char *pkg_dir = NULL;
    os_fs_tm_dypkgs(&pkg_dir);

    os_fs_dirstream_t dir_stream;

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_open(&dir_stream, pkg_dir)) {
        return;
    }

    fs_dirent_t ent;
    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(dir_stream, &ent)) {
        if (TM_FS_FILETYPE_DIR == ent.file_type) {
            printf("%s", ent.name);
            cli_out_newline();
        }
    }

    os_fs_dir_close(dir_stream);
    mem_safe_free(pkg_dir);
}

static void table_print(size_t max_len, size_t width) {
    char *pkg_dir = NULL;
    os_fs_tm_dypkgs(&pkg_dir);

    os_fs_dirstream_t dir_stream;

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_open(&dir_stream, pkg_dir)) {
        return;
    }

    size_t path_space = width - max_len - 8 - 5 - 1;

    fs_dirent_t ent;
    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(dir_stream, &ent)) {
        if (TM_FS_FILETYPE_DIR == ent.file_type) {
            size_t ent_len = strlen(ent.name);
            printf(" --- %s", ent.name);

            size_t padding = max_len - ent_len;
            cli_out_space(padding);
            cli_out_space(8);

            char *pkg_path = NULL;
            os_fs_tm_dypkg(&pkg_path, ent.name);
            size_t pkg_path_len = strlen(pkg_path);

            if (pkg_path_len < path_space) {
                printf("%s", pkg_path);
            } else {
                char *base = &pkg_path[pkg_path_len - path_space + 5];
                printf("[...]%s", base);
            }

            cli_out_newline();
            mem_safe_free(pkg_path);
        }
    }

    os_fs_dir_close(dir_stream);
    mem_safe_free(pkg_dir);
}

int cli_cmd_list(cli_info_t info) {
    (void)info;

    if (!os_fs_tm_init()) {
        cli_out_error("Failed to inizialize host file system");
        return EXIT_FAILURE;
    }

    csz_t  csz         = os_console_get_sz();
    size_t max_pkg_len = 0;

    if (!find_max_pkg_name_len(&max_pkg_len)) {
        cli_out_error("Unable to access package directory");
        return EXIT_FAILURE;
    }

    if (csz.columns > max_pkg_len + 8 + 5 + 5) {
        table_print(max_pkg_len, csz.columns);
        return EXIT_SUCCESS;
    }

    simple_print();
    return EXIT_SUCCESS;
}
