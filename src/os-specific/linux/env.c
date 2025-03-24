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
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "os/fs.h"
#include "os/posix/env.h"
#include "tm-mem.h"

static const char *get_home_directory(void) {
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
}

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
    if (NULL == app_name) {
        return false;
    }

    const char *usr_home      = get_home_directory();
    const char *app_file_path = NULL;

    size_t app_bufflen   = strlen(app_name) + 1 + strlen("desktop") + 1;
    char  *app_file_name = (char *)malloc(app_bufflen * sizeof(char));
    mem_chkoom(app_file_name);
    snprintf(app_file_name, app_bufflen, "%s.%s", app_name, "desktop");

    if (0 == os_fs_path_dyconcat((char **)&app_file_path,
                                 5,
                                 usr_home,
                                 ".local",
                                 "share",
                                 "applications",
                                 app_file_name)) {
        mem_safe_free(app_file_name);
        return false;
    }

    bool  ret = false;
    FILE *fp  = fopen(app_file_path, "w");

    if (NULL == fp) {
        goto cleanup;
    }

    fprintf(fp, "[Desktop Entry]\n");
    fprintf(fp, "Comment=Installed with tarman\n");
    fprintf(fp, "Exec=%s\n", executable_path);

    if (NULL != icon_path) {
        fprintf(fp, "Icon=%s\n", icon_path);
    }

    fprintf(fp, "Name=%s\n", app_name);
    fprintf(fp, "NoDisplay=false\n");

    if (NULL != wrk_dir) {
        fprintf(fp, "Path=%s\n", wrk_dir);
    }

    fprintf(fp, "StartupNotify=true\n");
    fprintf(fp, "Terminal=false\n");
    fprintf(fp, "TerminalOptions=\n");
    fprintf(fp, "Type=Application\n");

    fclose(fp);
    ret = true;

cleanup:
    mem_safe_free(app_file_name);
    mem_safe_free(app_file_path);
    return ret;
}

bool os_env_desktop_rm(const char *app_name) {
    if (NULL == app_name) {
        return false;
    }

    const char *usr_home      = get_home_directory();
    const char *app_file_path = NULL;

    size_t app_bufflen   = strlen(app_name) + 1 + strlen("desktop") + 1;
    char  *app_file_name = (char *)malloc(app_bufflen * sizeof(char));
    mem_chkoom(app_file_name);
    snprintf(app_file_name, app_bufflen, "%s.%s", app_name, "desktop");

    if (0 == os_fs_path_dyconcat((char **)&app_file_path,
                                 5,
                                 usr_home,
                                 ".local",
                                 "share",
                                 "applications",
                                 app_file_name)) {
        mem_safe_free(app_file_name);
        return false;
    }

    fs_fileop_status_t rm_status = os_fs_file_rm(app_file_path);
    mem_safe_free(app_file_name);
    mem_safe_free(app_file_path);
    return TM_FS_FILEOP_STATUS_OK == rm_status;
}
