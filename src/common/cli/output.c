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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "cli/output.h"
#include "os/console.h"

static bool last_is_newline = false;

static size_t print_word(char *word, size_t rem, size_t offset, csz_t csz) {
    size_t len = strlen(word);

    while (true) {
        if (rem > len) {
            printf("%s", word);

            if (rem >= len + 1) {
                printf(" ");
                rem--;
            }

            rem -= len;
            return rem;
        }

        puts("");
        rem = csz.columns - offset;
        cli_out_space(offset);
    }

    return rem;
}

static size_t aligned_putch(char c, size_t cwidth, size_t pad, size_t used) {
    if (used == cwidth) {
        cli_out_newline();
        cli_out_space(pad);
        used = pad;
    }

    putchar(c);
    return used + 1;
}

static size_t
aligned_puts(const char *s, size_t cwidth, size_t pad, size_t used) {
    for (size_t j = 0; s[j]; j++) {
        used = aligned_putch(s[j], cwidth, pad, used);
    }

    return used;
}

static void aligned_vprintf(const char *fmt, va_list args, size_t pad) {
    size_t used   = pad;
    size_t cwidth = os_console_get_sz().columns;

    for (size_t i = 0; fmt[i]; i++) {
        if ('%' == fmt[i]) {
            if ('s' == fmt[i + 1]) {
                char *str = va_arg(args, char *);
                aligned_puts(str, cwidth, pad, used);
            } else if ('d' == fmt[i + 1]) {
                char buf[24] = {0};
                snprintf(buf, 24, "%d", va_arg(args, int));
                aligned_puts(buf, cwidth, pad, used);
            } else {
                used = aligned_putch('%', cwidth, pad, used);
                i--;
            }

            i++;
            continue;
        }

        used = aligned_putch(fmt[i], cwidth, pad, used);
    }
}

void cli_out_newline(void) {
    if (!last_is_newline) {
        puts("");
        last_is_newline = true;
    }
}

void cli_out_reset(void) {
    last_is_newline = false;
}

void prefix(color_t color) {
    os_console_set_color(color, false);
    printf("=> ");
}

void cli_out_progress(const char *fmt, ...) {
    prefix(TM_COLOR_MAGENTA);

    va_list args;
    va_start(args, fmt);
    os_console_set_color(TM_COLOR_TEXT, true);
    aligned_vprintf(fmt, args, 3);
    va_end(args);

    os_console_set_color(TM_COLOR_RESET, false);
    puts("");
    last_is_newline = false;
}

void cli_out_success(const char *fmt, ...) {
    prefix(TM_COLOR_GREEN);

    va_list args;
    va_start(args, fmt);
    os_console_set_color(TM_COLOR_GREEN, true);
    aligned_vprintf(fmt, args, 3);
    va_end(args);

    os_console_set_color(TM_COLOR_RESET, false);
    puts("");
    last_is_newline = false;
}

void cli_out_error(const char *fmt, ...) {
    prefix(TM_COLOR_RED);

    va_list args;
    va_start(args, fmt);
    os_console_set_color(TM_COLOR_RED, true);
    printf("ERROR: ");
    aligned_vprintf(fmt, args, 10);
    va_end(args);

    os_console_set_color(TM_COLOR_RESET, false);
    puts("");
    last_is_newline = false;
}

void cli_out_warning(const char *fmt, ...) {
    prefix(TM_COLOR_YELLOW);

    va_list args;
    va_start(args, fmt);
    os_console_set_color(TM_COLOR_YELLOW, true);
    printf("WARNING: ");
    aligned_vprintf(fmt, args, 12);
    va_end(args);

    os_console_set_color(TM_COLOR_RESET, false);
    puts("");
    last_is_newline = false;
}

void cli_out_prompt(const char *fmt, ...) {
    os_console_set_color(TM_COLOR_CYAN, true);
    printf(":: ");

    va_list args;
    va_start(args, fmt);
    aligned_vprintf(fmt, args, 3);
    va_end(args);

    os_console_set_color(TM_COLOR_RESET, false);
    putchar(' ');
    last_is_newline = false;
}

void cli_out_space(size_t num) {
    for (size_t i = 0; i < num; i++) {
        printf(" ");
    }
    last_is_newline = false;
}

void cli_out_tab_words(size_t offset, const char *text, csz_t csz) {
    char *buf = malloc(strlen(text) + 1);
    strcpy(buf, text);

    char  *word = strtok(buf, " ");
    size_t rem  = csz.columns - offset;

    if (NULL == word) {
        printf("%s\n", text);
        free(buf);
        return;
    }

    while (NULL != word) {
        rem  = print_word(word, rem, offset, csz);
        word = strtok(NULL, " ");
    }

    free(buf);
    puts("");
    last_is_newline = false;
}
