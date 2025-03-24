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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"
#include "os/exec.h"
#include "os/fs.h"
#include "plugin/plugin.h"
#include "tm-mem.h"

typedef bool (*extract_handler_t)(const char *dst, const char *src);

typedef struct {
    const char       *file_type;
    extract_handler_t handler;
} embedded_extract_t;

static embedded_extract_t extractLookup[] = {{"tar", archive_tar_extract},
                                             {"tar.gz", archive_tar_extract},
                                             {"tar.xz", archive_tar_extract}};

static int
extcmp(const char *src, const char *ft, size_t src_tail, size_t ft_tail) {
    for (size_t i = src_tail, j = ft_tail; j > 0; j--, i--) {
        if (src[i] != ft[j]) {
            return -1;
        }
    }

    return 0;
}

bool archive_tar_extract(const char *dst, const char *src) {
    return EXIT_SUCCESS == os_exec("tar", "-xf", src, "-C", dst, NULL);
}

bool archive_extract(const char *dst, const char *src, const char *file_type) {
    if (NULL != file_type) {
        if (plugin_exists(file_type)) {
            return EXIT_SUCCESS == plugin_run(file_type, dst, src);
        }

        goto search_embedded;
    }

    // If no file type has been set
    // Try to find plugin based on file extension
    // This loop is set up to avoid issues with multiple dots
    for (const char *cp = src; *cp; cp++) {
        const char ch   = *cp;
        const char next = *(cp + 1);

        // Check if a dot is found
        // but avoid cases in which the dot is the last character
        // alongside cases in which it is used for directories
        // and finally, cases in which the plugin does not exist
        if ('.' == ch && 0 != next && isalnum(next) && plugin_exists(cp + 1)) {
            return EXIT_SUCCESS == plugin_run(cp + 1, dst, src);
        }
    }

search_embedded:
    // If no plugin was found, this searches for
    // embedded implementations
    for (size_t i = 0; i < sizeof extractLookup / sizeof(embedded_extract_t);
         i++) {
        embedded_extract_t extractor = extractLookup[i];

        if (NULL != file_type) {
            if (0 == strcmp(extractor.file_type, file_type)) {
                return extractor.handler(dst, src);
            }
            continue;
        }

        size_t src_len  = strlen(src);
        size_t src_tail = src_len - 1;
        size_t ft_len   = strlen(extractor.file_type);
        size_t ft_tail  = ft_len - 1;

        if (src_len < ft_len) {
            continue;
        }

        if (0 == extcmp(src, extractor.file_type, src_tail, ft_tail)) {
            return extractor.handler(dst, src);
        }
    }

    return false;
}
