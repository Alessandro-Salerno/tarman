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

#include "cli/directives/types.h"

typedef struct {
    cli_drt_desc_t *table;
    size_t          num_entries;
} cli_lkup_table_t;

bool             cli_lkup_command(const char *command, cli_drt_desc_t *dst);
bool             cli_lkup_option(const char *command, cli_drt_desc_t *dst);
cli_lkup_table_t cli_lkup_cmdtable(void);
cli_lkup_table_t cli_lkup_opttable(void);
