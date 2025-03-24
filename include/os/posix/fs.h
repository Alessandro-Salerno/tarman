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

#include "os/fs.h"

fs_dirop_status_t posix_fs_mkdir(const char *path);
fs_dirop_status_t posix_fs_dir_rm(const char *path);
fs_dirop_status_t posix_fs_dir_count(size_t *count, const char *path);
fs_dirop_status_t posix_fs_dir_open(os_fs_dirstream_t *stream,
                                    const char        *path);
fs_dirop_status_t posix_fs_dir_close(os_fs_dirstream_t stream);
fs_dirop_status_t posix_fs_dir_next(os_fs_dirstream_t stream, fs_dirent_t *ent);

fs_fileop_status_t posix_fs_file_rm(const char *path);
fs_fileop_status_t posix_fs_file_gettype(fs_filetype_t *dst, const char *path);

size_t posix_fs_path_vlen(size_t num_args, va_list args);
size_t posix_fs_path_vconcat(char *dst, size_t num_args, va_list args);
size_t posix_fs_path_dyparent(char **dst, const char *path);

size_t posix_fs_tm_dyhome(char **dst);
size_t posix_fs_tm_dyrepos(char **dst);
size_t posix_fs_tm_dypkgs(char **dst);
size_t posix_fs_tm_dyextract(char **dst);
size_t posix_fs_tm_dyrepo(char **dst, const char *repo_name);
size_t posix_fs_tm_dypkg(char **dst, const char *pkg_name);
size_t posix_fs_tm_dycached(char **dst, const char *item_name);
size_t
posix_fs_tm_dyrecipe(char **dst, const char *repo_name, const char *pkg_name);
size_t posix_fs_tm_dyplugins(const char **dst);
size_t posix_fs_tm_dyplugin(const char **dst, const char *plugin);
size_t posix_fs_tm_dyplugconf(const char **dst, const char *plugin);
size_t posix_fs_tm_dyexecpath(const char **dst, const char *exec);
bool   posix_fs_tm_init(void);
