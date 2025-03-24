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

#include "os/posix/fs.h"

#include "os/fs.h"

fs_dirop_status_t os_fs_mkdir(const char *path) {
    return posix_fs_mkdir(path);
}

fs_dirop_status_t os_fs_dir_rm(const char *path) {
    return posix_fs_dir_rm(path);
}

fs_dirop_status_t os_fs_dir_count(size_t *count, const char *path) {
    return posix_fs_dir_count(count, path);
}

fs_dirop_status_t os_fs_dir_open(os_fs_dirstream_t *stream, const char *path) {
    return posix_fs_dir_open(stream, path);
}

fs_dirop_status_t os_fs_dir_close(os_fs_dirstream_t stream) {
    return posix_fs_dir_close(stream);
}

fs_dirop_status_t os_fs_dir_next(os_fs_dirstream_t stream, fs_dirent_t *ent) {
    return posix_fs_dir_next(stream, ent);
}

fs_fileop_status_t os_fs_file_rm(const char *path) {
    return posix_fs_file_rm(path);
}

fs_fileop_status_t os_fs_file_gettype(fs_filetype_t *dst, const char *path) {
    return posix_fs_file_gettype(dst, path);
}

size_t os_fs_path_vlen(size_t num_args, va_list args) {
    return posix_fs_path_vlen(num_args, args);
}

size_t os_fs_path_len(size_t num_args, ...) {
    va_list args;
    va_start(args, num_args);
    size_t ret = os_fs_path_vlen(num_args, args);
    va_end(args);
    return ret;
}

size_t os_fs_path_vconcat(char *dst, size_t num_args, va_list args) {
    return posix_fs_path_vconcat(dst, num_args, args);
}

size_t os_fs_path_concat(char *dst, size_t num_args, ...) {
    va_list args;
    va_start(args, num_args);
    size_t ret = os_fs_path_vconcat(dst, num_args, args);
    va_end(args);
    return ret;
}

size_t os_fs_path_dyconcat(char **dst, size_t num_args, ...) {
    va_list args;
    va_start(args, num_args);
    size_t len = os_fs_path_vlen(num_args, args);
    va_end(args);

    size_t buf_sz = len + 1;
    char  *buf    = (char *)malloc(buf_sz * sizeof(char));

    if (NULL == buf) {
        *dst = NULL;
        return 0;
    }

    va_start(args, num_args);
    os_fs_path_vconcat(buf, num_args, args);
    va_end(args);

    *dst = buf;
    return len;
}

size_t os_fs_path_dyparent(char **dst, const char *path) {
    return posix_fs_path_dyparent(dst, path);
}

size_t os_fs_tm_dyhome(char **dst) {
    return posix_fs_tm_dyhome(dst);
}

size_t os_fs_tm_dyrepos(char **dst) {
    return posix_fs_tm_dyrepos(dst);
}

size_t os_fs_tm_dypkgs(char **dst) {
    return posix_fs_tm_dypkgs(dst);
}

size_t os_fs_tm_dyextract(char **dst) {
    return posix_fs_tm_dyextract(dst);
}

size_t os_fs_tm_dyrepo(char **dst, const char *repo_name) {
    return posix_fs_tm_dyrepo(dst, repo_name);
}

size_t os_fs_tm_dypkg(char **dst, const char *pkg_name) {
    return posix_fs_tm_dypkg(dst, pkg_name);
}

size_t os_fs_tm_dycached(char **dst, const char *item_name) {
    return posix_fs_tm_dycached(dst, item_name);
}

size_t
os_fs_tm_dyrecipe(char **dst, const char *repo_name, const char *pkg_name) {
    return posix_fs_tm_dyrecipe(dst, repo_name, pkg_name);
}

size_t os_fs_tm_dyplugins(const char **dst) {
    return posix_fs_tm_dyplugins(dst);
}

size_t os_fs_tm_dyplugin(const char **dst, const char *plugin) {
    return posix_fs_tm_dyplugin(dst, plugin);
}

size_t os_fs_tm_dyplugconf(const char **dst, const char *plugin) {
    return posix_fs_tm_dyplugconf(dst, plugin);
}

bool os_fs_tm_init(void) {
    return posix_fs_tm_init();
}
