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

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    size_t rows;
    size_t columns;
} csz_t;

typedef enum {
    TM_COLOR_RED,
    TM_COLOR_GREEN,
    TM_COLOR_YELLOW,
    TM_COLOR_MAGENTA,
    TM_COLOR_CYAN,
    TM_COLOR_TEXT,
    TM_COLOR_RESET
} color_t;

csz_t os_console_get_sz(void);
void  os_console_set_color(color_t color, bool bold);
