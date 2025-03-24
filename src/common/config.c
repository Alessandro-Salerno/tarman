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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "stream.h"
#include "tm-mem.h"

static bool tokenize(char *line, const char **key, const char **value) {
    for (size_t i = 0; line[i]; i++) {
        char ch = line[i];

        if (' ' == ch) {
            return false;
        }

        if ('=' == ch) {
            line[i] = 0;
            *key    = line;
            *value  = &line[i + 1];
            return true;
        }
    }

    return false;
}

cfg_prop_match_t cfg_eval_prop(const char  *prop,
                               const char  *key,
                               const char  *value,
                               const char **target,
                               size_t       num_args,
                               ...) {
    // Declared here for backwards compatibility
    char *value_cpy = NULL;

    if (0 != strcmp(prop, key)) {
        return TM_CFG_PROP_MATCH_FALSE;
    }

    if (0 == num_args) {
        goto success;
    }

    va_list args;
    va_start(args, num_args);

    // Check the match range
    // This is if the propertly can only have
    // a select number of values (such as true or false)
    for (size_t i = 0; i < num_args; i++) {
        const char *range_elem = va_arg(args, char *);

        if (0 == strcmp(value, range_elem)) {
            va_end(args);
            goto success;
        }
    }

    // If the code gets here, the functions has failed
    // This is because of the goto success;
    va_end(args);
    return TM_CFG_PROP_MATCH_ERR;

success:
    value_cpy = (char *)malloc((strlen(value) + 1) * sizeof(char));
    mem_chkoom(value_cpy);
    strcpy(value_cpy, value);
    *target = value_cpy;

    return TM_CFG_PROP_MATCH_OK;
}

cfg_prop_match_t cfg_eval_prop_matches(size_t num_args, ...) {
    va_list args;
    va_start(args, num_args);

    cfg_prop_match_t ret = TM_CFG_PROP_MATCH_FALSE;

    for (size_t i = 0; i < num_args; i++) {
        cfg_prop_match_t match = va_arg(args, cfg_prop_match_t);

        if (TM_CFG_PROP_MATCH_ERR == match) {
            va_end(args);
            return TM_CFG_PROP_MATCH_ERR;
        }

        if (TM_CFG_PROP_MATCH_OK == match) {
            ret = TM_CFG_PROP_MATCH_OK;
        }
    }

    va_end(args);
    return ret;
}

cfg_parse_status_t
cfg_parse(FILE *stream, cfg_translator_t translator, cfg_generic_info_t *info) {
    char *line_buffer;

    while (0 != stream_dyreadline(stream, &line_buffer)) {
        const char        *key   = NULL;
        const char        *value = NULL;
        cfg_parse_status_t s     = TM_CFG_PARSE_STATUS_MALFORMED;

        if (!tokenize(line_buffer, &key, &value) ||
            TM_CFG_PARSE_STATUS_OK != (s = translator(key, value, info))) {
            mem_safe_free(line_buffer);
            return s;
        }

        mem_safe_free(line_buffer);
    }

    return TM_CFG_PARSE_STATUS_OK;
}
