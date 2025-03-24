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

#include "stream.h"
#include "tm-mem.h"

size_t stream_dyreadline(FILE *stream, char **dst) {
    size_t len        = 128;
    char  *buf        = NULL;
    size_t i          = 0;
    char   ch         = 0;
    bool   found_chrs = false;

    while (EOF != (ch = fgetc(stream)) && '\n' != ch) {
        if ('\r' == ch) {
            continue;
        }

        if (!found_chrs && ' ' == ch) {
            continue;
        }

        if (NULL == buf) {
            buf = (char *)malloc(len * sizeof(char));
            mem_chkoom(buf);
        }

        if (len - 1 == i) {
            len *= 2;
            buf = realloc(buf, len * sizeof(char));
            mem_chkoom(buf);
        }

        found_chrs = true;
        buf[i]     = ch;
        i++;
    }

    if (NULL != buf) {
        buf[i] = 0;
        *dst   = buf;
    }

    return i;
}
