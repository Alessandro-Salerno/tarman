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

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    TM_FS_DIROP_STATUS_NOEXIST = 0,
    TM_FS_DIROP_STATUS_EXIST   = 1,
    TM_FS_DIROP_STATUS_PERM    = 2,
    TM_FS_DIROP_STATUS_END     = 3,
    TM_FS_DIROP_STATUS_ERR     = 4,
    TM_FS_DIROP_STATUS_OK      = 5
} fs_dirop_status_t;

typedef enum {
    TM_FS_FILEOP_STATUS_NOEXIST = 0,
    TM_FS_FILEOP_STATUS_PERM    = 1,
    TM_FS_FILEOP_STATUS_ERR     = 2,
    TM_FS_FILEOP_STATUS_OK      = 3
} fs_fileop_status_t;

typedef enum {
    TM_FS_FILETYPE_DIR,
    TM_FS_FILETYPE_REGULAR,
    TM_FS_FILETYPE_EXEC,
    TM_FS_FILETYPE_UNKNOWN
} fs_filetype_t;

typedef void *os_fs_dirstream_t;

typedef struct {
    fs_filetype_t file_type;
    const char   *name;
} fs_dirent_t;

fs_dirop_status_t os_fs_mkdir(const char *path);
fs_dirop_status_t os_fs_dir_rm(const char *path);
fs_dirop_status_t os_fs_dir_count(size_t *count, const char *path);
fs_dirop_status_t os_fs_dir_open(os_fs_dirstream_t *stream, const char *path);
fs_dirop_status_t os_fs_dir_close(os_fs_dirstream_t stream);
fs_dirop_status_t os_fs_dir_next(os_fs_dirstream_t stream, fs_dirent_t *ent);

fs_fileop_status_t os_fs_file_rm(const char *path);
fs_fileop_status_t os_fs_file_gettype(fs_filetype_t *dst, const char *path);

size_t os_fs_path_vlen(size_t num_args, va_list args);
size_t os_fs_path_len(size_t num_args, ...);
size_t os_fs_path_vconcat(char *dst, size_t num_args, va_list args);
size_t os_fs_path_concat(char *dst, size_t num_args, ...);
size_t os_fs_path_dyconcat(char **dst, size_t num_args, ...);
size_t os_fs_path_dyparent(char **dst, const char *path);

size_t os_fs_tm_dyhome(char **dst);
size_t os_fs_tm_dyrepos(char **dst);
size_t os_fs_tm_dypkgs(char **dst);
size_t os_fs_tm_dyextract(char **dst);
size_t os_fs_tm_dyrepo(char **dst, const char *repo_name);
size_t os_fs_tm_dypkg(char **dst, const char *pkg_name);
size_t os_fs_tm_dycached(char **dst, const char *item_name);
size_t
os_fs_tm_dyrecipe(char **dst, const char *repo_name, const char *pkg_name);
size_t os_fs_tm_dyplugins(const char **dst);
size_t os_fs_tm_dyplugin(const char **dst, const char *plugin);
size_t os_fs_tm_dyplugconf(const char **dst, const char *plugin);
bool   os_fs_tm_init(void);
