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
#include <string.h>

#include "cli/directives/commands.h"
#include "cli/directives/lookup.h"
#include "cli/output.h"
#include "os/console.h"

#define BASE_LINE_LEN        4
#define OPT_SEPARATOR_LEN    2
#define COLUMN_SEPARATOR_LEN 4

static void print_indent(void) {
    cli_out_space(BASE_LINE_LEN);
}

static size_t find_line_len(cli_drt_desc_t desc) {
    size_t cmd_len = BASE_LINE_LEN + COLUMN_SEPARATOR_LEN;

    if (NULL != desc.short_option) {
        cmd_len += strlen(desc.short_option);
    }

    if (NULL != desc.full_option) {
        cmd_len += strlen(desc.full_option);
    }

    return cmd_len;
}

static size_t find_max_line_len(cli_lkup_table_t table) {
    size_t max_cmd_len = BASE_LINE_LEN + COLUMN_SEPARATOR_LEN;
    for (size_t i = 0; i < table.num_entries; i++) {
        cli_drt_desc_t desc    = table.table[i];
        size_t         cmd_len = find_line_len(desc);

        if (cmd_len > max_cmd_len) {
            max_cmd_len = cmd_len;
        }
    }

    return max_cmd_len;
}

static void
print_help_line(cli_drt_desc_t desc, size_t max_line_len, csz_t csz) {
    size_t line_len = find_line_len(desc);
    size_t rem      = max_line_len - line_len;

    print_indent();

    if (NULL != desc.short_option && NULL != desc.full_option) {
        printf("%s, %s", desc.short_option, desc.full_option);
    } else if (NULL != desc.short_option) {
        printf("%s", desc.short_option);
    } else if (NULL != desc.full_option) {
        printf("%s", desc.full_option);
    }

    cli_out_space(COLUMN_SEPARATOR_LEN);
    cli_out_space(rem);

    size_t tot_off = max_line_len + COLUMN_SEPARATOR_LEN;
    cli_out_tab_words(tot_off, desc.description, csz);
}

static void
print_help_list(const char *title, cli_lkup_table_t table, csz_t csz) {
    printf("%s:\n", title);

    size_t max_cmd_len = find_max_line_len(table);

    for (size_t i = 0; i < table.num_entries; i++) {
        cli_drt_desc_t desc = table.table[i];
        print_help_line(desc, max_cmd_len, csz);
    }

    puts("");
}

int cli_cmd_help(cli_info_t info) {
    (void)info;
    csz_t console_sz = os_console_get_sz();

    cli_lkup_table_t cmd_table = cli_lkup_cmdtable();
    cli_lkup_table_t opt_table = cli_lkup_opttable();

    puts("tarman by Alessandro Salerno");
    puts("The portable, cross-platform, extensible, and simple package "
         "manager\n");

    printf("Usage: tarman <command> [<options>] [<package|url|repo>]\n\n");

    print_help_list("COMMANDS", cmd_table, console_sz);
    print_help_list("OPTIONS", opt_table, console_sz);

    return EXIT_SUCCESS;
}
