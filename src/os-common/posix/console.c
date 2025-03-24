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
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "os/console.h"

csz_t posix_console_get_sz(void) {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return (csz_t){.rows = size.ws_row, .columns = size.ws_col};
}

void posix_console_set_color(color_t color, bool bold) {
    if (!isatty(fileno(stdout))) {
        return;
    }

    int ansi_color = -1;

    switch (color) {
    case TM_COLOR_RED:
        ansi_color = 31;
        break;
    case TM_COLOR_GREEN:
        ansi_color = 32;
        break;
    case TM_COLOR_YELLOW:
        ansi_color = 33;
        break;
    case TM_COLOR_MAGENTA:
        ansi_color = 35;
        break;
    case TM_COLOR_CYAN:
        ansi_color = 36;
        break;
    case TM_COLOR_TEXT:
        ansi_color = 39;
        break;
    case TM_COLOR_RESET:
        printf("\033[m");
        return;

    default:
        return;
    }

    if (!bold) {
        printf("\033[0;%dm", ansi_color);
        return;
    }

    printf("\033[1;%dm", ansi_color);
}
