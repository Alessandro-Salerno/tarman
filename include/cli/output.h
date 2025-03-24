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

#pragma once

#include <stdlib.h>

#include "os/console.h"

void cli_out_newline(void);
void cli_out_reset(void);
void cli_out_progress(const char *fmt, ...);
void cli_out_success(const char *fmt, ...);
void cli_out_error(const char *fmt, ...);
void cli_out_warning(const char *fmt, ...);
void cli_out_prompt(const char *fmt, ...);

void cli_out_space(size_t num);

void cli_out_tab_words(size_t offset, const char *text, csz_t csz);
