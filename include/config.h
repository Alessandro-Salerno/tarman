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

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TM_CFG_PARSE_STATUS_NOFILE,
    TM_CFG_PARSE_STATUS_PERM,
    TM_CFG_PARSE_STATUS_MALFORMED,
    TM_CFG_PARSE_STATUS_INVKEY,
    TM_CFG_PARSE_STATUS_INVVAL,
    TM_CFG_PARSE_STATUS_ERR,
    TM_CFG_PARSE_STATUS_OK
} cfg_parse_status_t;

typedef enum {
    TM_CFG_PROP_MATCH_FALSE,
    TM_CFG_PROP_MATCH_OK,
    TM_CFG_PROP_MATCH_ERR
} cfg_prop_match_t;

typedef void cfg_generic_info_t;
typedef cfg_parse_status_t (*cfg_translator_t)(const char         *key,
                                               const char         *value,
                                               cfg_generic_info_t *info);

cfg_prop_match_t cfg_eval_prop(const char  *prop,
                               const char  *key,
                               const char  *value,
                               const char **target,
                               size_t       num_args,
                               ...);
cfg_prop_match_t cfg_eval_prop_matches(size_t num_args, ...);
cfg_parse_status_t
cfg_parse(FILE *stream, cfg_translator_t translator, cfg_generic_info_t *info);
