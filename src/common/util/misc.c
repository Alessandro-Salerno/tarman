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
#include <string.h>

#include "os/fs.h"
#include "tm-mem.h"
#include "util/misc.h"

size_t util_misc_dyfile(char      **dst,
                        const char *base_path,
                        const char *filename,
                        const char *filetype) {
    size_t bufsz = strlen(filename) + 1 + strlen(filetype) + 1;
    char  *fname = (char *)malloc(bufsz * sizeof(char));
    mem_chkoom(fname);
    snprintf(fname, bufsz, "%s.%s", filename, filetype);

    size_t ret = os_fs_path_dyconcat(dst, 2, base_path, fname);

    mem_safe_free(fname);
    return ret;
}

size_t
util_misc_dytmpfile(char **dst, const char *filename, const char *filetype) {
    size_t bufsz = strlen(filename) + 1 + strlen(filetype) + 1;
    char  *fname = (char *)malloc(bufsz * sizeof(char));
    mem_chkoom(fname);
    snprintf(fname, bufsz, "%s.%s", filename, filetype);

    os_fs_tm_dycached(dst, fname);
    mem_safe_free(fname);

    return bufsz - 1;
}
