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

// MUST BE HERE
#include <tm-os-defs.h>

// General includes
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "os/fs.h"
#include "os/posix/fs.h"
#include "tm-mem.h"

typedef struct {
    char  *buf;
    size_t len;
} tmstr_t;

static tmstr_t Home       = {0};
static tmstr_t Repos      = {0};
static tmstr_t Pkgs       = {0};
static tmstr_t Extract    = {0};
static tmstr_t Plugins    = {0};
static tmstr_t PluginConf = {0};
static tmstr_t Path       = {0};

static const char *get_home_directory(void) {
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
}

static fs_dirop_status_t simplify(fs_dirop_status_t s) {
    if (TM_FS_DIROP_STATUS_EXIST == s) {
        return TM_FS_DIROP_STATUS_OK;
    }

    return s;
}

static fs_dirop_status_t translate_direrr(void) {
    switch (errno) {
    case EACCES:
        return TM_FS_DIROP_STATUS_PERM;

    case ENOENT:
    case ENOTDIR:
        return TM_FS_DIROP_STATUS_NOEXIST;

    case EEXIST:
        return TM_FS_DIROP_STATUS_EXIST;

    default:
        return TM_FS_DIROP_STATUS_ERR;
    }
}

static fs_fileop_status_t translate_fileerr(void) {
    switch (errno) {
    case EACCES:
        return TM_FS_FILEOP_STATUS_PERM;

    case ENOENT:
        return TM_FS_FILEOP_STATUS_NOEXIST;

    default:
        return TM_FS_FILEOP_STATUS_ERR;
    }
}

fs_dirop_status_t posix_fs_mkdir(const char *path) {
    struct stat st = {0};

    if (-1 != stat(path, &st)) {
        return TM_FS_DIROP_STATUS_EXIST;
    }

    if (0 == mkdir(path, 0700)) {
        return TM_FS_DIROP_STATUS_OK;
    }

    return translate_direrr();
}

fs_dirop_status_t posix_fs_dir_rm(const char *path) {
    os_fs_dirstream_t dir;
    fs_dirop_status_t status = os_fs_dir_open(&dir, path);

    if (TM_FS_DIROP_STATUS_OK != status) {
        return status;
    }

    fs_dirop_status_t enum_status;
    fs_dirent_t       ent;
    while (TM_FS_DIROP_STATUS_END !=
           (enum_status = os_fs_dir_next(dir, &ent))) {
        if (TM_FS_DIROP_STATUS_OK != enum_status) {
            return enum_status;
        }

        char *full_path;
        os_fs_path_dyconcat(&full_path, 2, path, ent.name);

        if (NULL == full_path) {
            return TM_FS_DIROP_STATUS_ERR;
        }

        switch (ent.file_type) {
        case TM_FS_FILETYPE_DIR: {
            fs_dirop_status_t s = os_fs_dir_rm(full_path);
            if (TM_FS_DIROP_STATUS_OK != s) {
                mem_safe_free(full_path);
                return s;
            }
            break;
        }

        case TM_FS_FILETYPE_REGULAR:
        case TM_FS_FILETYPE_EXEC:
        case TM_FS_FILETYPE_UNKNOWN:
            if (0 != unlink(full_path)) {
                mem_safe_free(full_path);
                return TM_FS_DIROP_STATUS_ERR;
            }
            break;
        }

        mem_safe_free(full_path);
    }

    status = os_fs_dir_close(dir);
    if (TM_FS_DIROP_STATUS_OK != status) {
        return status;
    }

    if (0 != rmdir(path)) {
        return TM_FS_DIROP_STATUS_ERR;
    }

    return TM_FS_DIROP_STATUS_OK;
}

fs_dirop_status_t posix_fs_dir_count(size_t *count, const char *path) {
    size_t m_count = 0;

    os_fs_dirstream_t dir;
    fs_dirop_status_t status = os_fs_dir_open(&dir, path);

    if (TM_FS_DIROP_STATUS_OK != status) {
        return status;
    }

    fs_dirop_status_t enum_status;
    fs_dirent_t       ent;
    for (; TM_FS_DIROP_STATUS_END != (enum_status = os_fs_dir_next(dir, &ent));
         m_count++) {
        if (TM_FS_DIROP_STATUS_OK != enum_status) {
            return enum_status;
        }
    }

    *count = m_count;
    return TM_FS_DIROP_STATUS_OK;
}

fs_dirop_status_t posix_fs_dir_open(os_fs_dirstream_t *stream,
                                    const char        *path) {
    DIR *dir = opendir(path);

    if (NULL == dir) {
        *stream = NULL;
        return translate_direrr();
    }

    *stream = dir;
    return TM_FS_DIROP_STATUS_OK;
}

fs_dirop_status_t posix_fs_dir_close(os_fs_dirstream_t stream) {
    DIR *dir = (DIR *)stream;

    if (0 == closedir(dir)) {
        return TM_FS_DIROP_STATUS_OK;
    }

    return TM_FS_DIROP_STATUS_ERR;
}

fs_dirop_status_t posix_fs_dir_next(os_fs_dirstream_t stream,
                                    fs_dirent_t      *ent) {
    DIR           *dir  = (DIR *)stream;
    struct dirent *next = readdir(dir);

    while (NULL != next && (0 == strcmp(next->d_name, ".") ||
                            0 == strcmp(next->d_name, ".."))) {
        next = readdir(dir);
    }

    if (NULL == next) {
        return TM_FS_DIROP_STATUS_END;
    }

    int         dird      = dirfd(dir);
    struct stat st        = {0};
    bool        stat_done = false;

    if (DT_UNKNOWN == next->d_type) {
        if (0 > fstatat(dird, next->d_name, &st, 0)) {
            return TM_FS_DIROP_STATUS_ERR;
        }

        stat_done = true;

        if (S_ISREG(st.st_mode)) {
            next->d_type = DT_REG;
        } else if (S_ISDIR(st.st_mode)) {
            next->d_type = DT_DIR;
        }
    }

    fs_dirent_t m_ent = (fs_dirent_t){.name = next->d_name};

    switch (next->d_type) {
    case DT_REG:
        if (!stat_done && 0 > fstatat(dird, next->d_name, &st, 0)) {
            return TM_FS_DIROP_STATUS_ERR;
        }
        if (S_IXUSR & st.st_mode) {
            m_ent.file_type = TM_FS_FILETYPE_EXEC;
        } else {
            m_ent.file_type = TM_FS_FILETYPE_REGULAR;
        }
        break;

    case DT_DIR:
        m_ent.file_type = TM_FS_FILETYPE_DIR;
        break;

    default:
        m_ent.file_type = TM_FS_FILETYPE_UNKNOWN;
        break;
    }

    *ent = m_ent;
    return TM_FS_DIROP_STATUS_OK;
}

fs_fileop_status_t posix_fs_file_rm(const char *path) {
    if (0 == unlink(path)) {
        return TM_FS_FILEOP_STATUS_OK;
    }

    return translate_fileerr();
}

fs_fileop_status_t posix_fs_file_gettype(fs_filetype_t *dst, const char *path) {
    struct stat st;

    if (0 > stat(path, &st)) {
        return TM_FS_FILEOP_STATUS_ERR;
    }

    if (S_ISREG(st.st_mode) && S_IXUSR & st.st_mode) {
        *dst = TM_FS_FILETYPE_EXEC;
        return TM_FS_FILEOP_STATUS_OK;
    }

    if (S_ISREG(st.st_mode)) {
        *dst = TM_FS_FILETYPE_DIR;
        return TM_FS_FILEOP_STATUS_OK;
    }

    if (S_ISDIR(st.st_mode)) {
        *dst = TM_FS_FILETYPE_DIR;
        return TM_FS_FILEOP_STATUS_OK;
    }

    return TM_FS_FILEOP_STATUS_ERR;
}

size_t posix_fs_path_vlen(size_t num_args, va_list args) {
    size_t len = 0;

    for (size_t i = 0; i < num_args; i++) {
        const char *path = va_arg(args, char *);
        size_t      plen = strlen(path);
        len += plen;
        if ('/' != path[plen - 1]) {
            len++; // Count the / separator
        }
    }

    return len - 1; // Remove last / separator
}

size_t posix_fs_path_vconcat(char *dst, size_t num_args, va_list args) {
    size_t j = 0; // Index into dst

    for (size_t i = 0; i < num_args; i++) {
        const char *path = va_arg(args, char *);
        // Copy path to dst
        for (; *path; path++, j++) {
            dst[j] = *path;
        }
        if ('/' != *(path - 1)) {
            dst[j] = '/'; // Add separator
        }
        j++;
    }

    dst[--j] = 0; // Add string terminator
    return j;
}

size_t posix_fs_path_dyparent(char **dst, const char *path) {
    size_t last_sep_idx = 0;

    for (size_t i = 0; path[i]; i++) {
        if ('/' == path[i]) {
            last_sep_idx = i;
        }
    }

    // If the last separator is also the last char
    if (!path[last_sep_idx + 1]) {
        // Avoid situations like: path/to/something//
        //                                          ^
        for (size_t back = last_sep_idx - 1; back > 0 && '/' == path[back];
             back--) {
            last_sep_idx = back;
        }

        // Move back to previous /
        for (size_t back = last_sep_idx - 1; back > 0 && '/' != path[back];
             back--) {
            last_sep_idx = back;
        }

        // Point to last / instead of char
        last_sep_idx--;
    }

    // Avoid issues like path//to//////something/////////
    for (size_t back = last_sep_idx - 1; back > 0 && '/' == path[back];
         back--) {
        last_sep_idx = back;
    }

    const char *target_path = path;

    if (0 == last_sep_idx) {
        last_sep_idx++;
        target_path = ".";
    }

    size_t buf_len = last_sep_idx + 1;
    char  *buf     = (char *)malloc(buf_len * sizeof(char));

    mem_chkoom(buf);
    // if (NULL == buf) {
    //   *dst = NULL;
    //   return 0;
    // }

    strncpy(buf, target_path, last_sep_idx);
    buf[last_sep_idx] = 0; // Add string terminator

    *dst = buf;
    return buf_len;
}

size_t posix_fs_tm_dyhome(char **dst) {
    char *tm_home = (char *)malloc((Home.len + 1) * sizeof(char));
    mem_chkoom(tm_home);
    strcpy(tm_home, Home.buf);
    *dst = tm_home;
    return Home.len;
}

size_t posix_fs_tm_dyrepos(char **dst) {
    char *tm_repos = (char *)malloc((Repos.len + 1) * sizeof(char));
    mem_chkoom(tm_repos);
    strcpy(tm_repos, Repos.buf);
    *dst = tm_repos;
    return Repos.len;
}

size_t posix_fs_tm_dypkgs(char **dst) {
    char *tm_pkgs = (char *)malloc((Pkgs.len + 1) * sizeof(char));
    mem_chkoom(tm_pkgs);
    strcpy(tm_pkgs, Pkgs.buf);
    *dst = tm_pkgs;
    return Pkgs.len;
}

size_t posix_fs_tm_dyextract(char **dst) {
    char *tm_extract = (char *)malloc((Extract.len + 1) * sizeof(char));
    mem_chkoom(tm_extract);
    strcpy(tm_extract, Extract.buf);
    *dst = tm_extract;
    return Extract.len;
}

size_t posix_fs_tm_dyrepo(char **dst, const char *repo_name) {
    char  *tm_repo;
    size_t ret = os_fs_path_dyconcat(&tm_repo, 2, Repos.buf, repo_name);
    mem_chkoom(tm_repo);
    *dst = tm_repo;
    return ret;
}

size_t posix_fs_tm_dypkg(char **dst, const char *pkg_name) {
    char  *tm_pkg;
    size_t ret = os_fs_path_dyconcat(&tm_pkg, 2, Pkgs.buf, pkg_name);
    mem_chkoom(tm_pkg);
    *dst = tm_pkg;
    return ret;
}

size_t posix_fs_tm_dycached(char **dst, const char *item_name) {
    char  *tm_cached;
    size_t ret = os_fs_path_dyconcat(&tm_cached, 2, Extract.buf, item_name);
    mem_chkoom(tm_cached);
    *dst = tm_cached;
    return ret;
}

size_t
posix_fs_tm_dyrecipe(char **dst, const char *repo_name, const char *pkg_name) {
    size_t ret = 0;

    char *rec_name = (char *)malloc((strlen(pkg_name) + strlen(".tarman") + 1) *
                                    sizeof(char));
    mem_chkoom(rec_name);
    sprintf(rec_name, "%s.tarman", pkg_name);

    char *tm_recipe;
    ret = os_fs_path_dyconcat(&tm_recipe, 3, Repos.buf, repo_name, rec_name);

    mem_safe_free(rec_name);
    *dst = tm_recipe;
    return ret;
}

size_t posix_fs_tm_dyplugins(const char **dst) {
    char *tm_plugins = (char *)malloc((Plugins.len + 1) * sizeof(char));
    mem_chkoom(tm_plugins);
    strcpy(tm_plugins, Plugins.buf);
    *dst = tm_plugins;
    return Plugins.len;
}

size_t posix_fs_tm_dyplugin(const char **dst, const char *plugin) {
    char  *tm_plugin;
    size_t ret = os_fs_path_dyconcat(&tm_plugin, 2, Plugins.buf, plugin);
    mem_chkoom(tm_plugin);
    *dst = tm_plugin;
    return ret;
}

size_t posix_fs_tm_dyplugconf(const char **dst, const char *plugin) {
    size_t ret = 0;

    char *conf_name =
        (char *)malloc((strlen(plugin) + strlen(".txt") + 1) * sizeof(char));
    mem_chkoom(conf_name);
    sprintf(conf_name, "%s.txt", plugin);

    char *tm_plugconf;
    ret = os_fs_path_dyconcat(&tm_plugconf, 2, PluginConf.buf, conf_name);

    mem_safe_free(conf_name);
    *dst = tm_plugconf;
    return ret;
}

size_t posix_fs_tm_dyexecpath(const char **dst, const char *exec) {
    char  *tm_pathexec;
    size_t ret = os_fs_path_dyconcat(&tm_pathexec, 2, Path.buf, exec);
    mem_chkoom(tm_pathexec);
    *dst = tm_pathexec;
    return ret;
}

bool posix_fs_tm_init(void) {
    const char *usr_home = get_home_directory();

    Home.len = os_fs_path_dyconcat(&Home.buf, 2, usr_home, ".tarman");
    Repos.len =
        os_fs_path_dyconcat(&Repos.buf, 3, usr_home, ".tarman", "repos");
    Pkgs.len = os_fs_path_dyconcat(&Pkgs.buf, 3, usr_home, ".tarman", "pkgs");
    Extract.len =
        os_fs_path_dyconcat(&Extract.buf, 3, usr_home, ".tarman", "tmp");
    Plugins.len =
        os_fs_path_dyconcat(&Plugins.buf, 3, usr_home, ".tarman", "plugins");
    PluginConf.len =
        os_fs_path_dyconcat(&PluginConf.buf, 3, usr_home, ".tarman", "conf");
    Path.len = os_fs_path_dyconcat(&Path.buf, 3, usr_home, ".tarman", "path");

    if (NULL == Home.buf || NULL == Repos.buf || NULL == Pkgs.buf ||
        NULL == Extract.buf || NULL == Plugins.buf || NULL == PluginConf.buf ||
        NULL == Path.buf) {
        return false;
    }

    if (TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Home.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Repos.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Pkgs.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Extract.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Plugins.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(PluginConf.buf)) ||
        TM_FS_DIROP_STATUS_OK != simplify(os_fs_mkdir(Path.buf))) {
        return false;
    }

    return true;
}
