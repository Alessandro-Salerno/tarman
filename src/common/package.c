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
#include "package.h"
#include "tm-mem.h"

static cfg_parse_status_t
pkg_translator(const char *key, const char *value, pkg_info_t *pkg_info) {
    cfg_prop_match_t match = cfg_eval_prop_matches(
        6,
        cfg_eval_prop("URL", key, value, &pkg_info->url, 0),
        cfg_eval_prop(
            "FROM_REPOSITORY", key, value, &pkg_info->from_repoistory, 0),
        cfg_eval_prop(
            "APPLICATION_NAME", key, value, &pkg_info->application_name, 0),
        cfg_eval_prop(
            "EXECUTABLE_PATH", key, value, &pkg_info->executable_path, 0),
        cfg_eval_prop(
            "WORKING_DIRECTORY", key, value, &pkg_info->working_directory, 0),
        cfg_eval_prop("ICON_PATH", key, value, &pkg_info->icon_path, 0));

    if (TM_CFG_PROP_MATCH_ERR == match) {
        return TM_CFG_PARSE_STATUS_INVVAL;
    }

    return TM_CFG_PARSE_STATUS_OK;
}

static cfg_parse_status_t
rcp_translator(const char *key, const char *value, recipe_t *rcp) {
    cfg_parse_status_t pkg_status = pkg_translator(key, value, &rcp->pkg_info);

    if (TM_CFG_PARSE_STATUS_OK != pkg_status) {
        return pkg_status;
    }

    const char *add_to_path    = NULL;
    const char *add_to_desktop = NULL;
    const char *add_to_tarman  = NULL;

    cfg_prop_match_t match = cfg_eval_prop_matches(
        4,
        cfg_eval_prop("PACKAGE_FORMAT", key, value, &rcp->package_format, 0),
        cfg_eval_prop(
            "ADD_TO_PATH", key, value, &add_to_path, 2, "true", "false"),
        cfg_eval_prop(
            "ADD_TO_DESKTOP", key, value, &add_to_desktop, 2, "true", "false"),
        cfg_eval_prop(
            "ADD_TO_TARMAN", key, value, &add_to_tarman, 2, "true", "false"));

    if (TM_CFG_PROP_MATCH_ERR == match) {
        return TM_CFG_PARSE_STATUS_INVVAL;
    }

    if (NULL != add_to_path && 0 == strcmp(add_to_path, "true")) {
        rcp->add_to_path = true;
    }

    if (NULL != add_to_desktop && 0 == strcmp(add_to_desktop, "true")) {
        rcp->add_to_desktop = true;
    }

    if (NULL != add_to_tarman && 0 == strcmp(add_to_tarman, "true")) {
        rcp->add_to_tarman = true;
    }

    mem_safe_free(add_to_path);
    mem_safe_free(add_to_desktop);
    mem_safe_free(add_to_tarman);
    return TM_CFG_PARSE_STATUS_OK;
}

static void dump_if_set(FILE *fp, const char *key, const char *value) {
    if (NULL != value) {
        fprintf(fp, "%s=%s\n", key, value);
    }
}

static void dump_bool(FILE *fp, const char *key, bool value) {
    if (value) {
        fprintf(fp, "%s=true\n", key);
    } else {
        fprintf(fp, "%s=false\n", key);
    }
}

cfg_parse_status_t pkg_parse_ftmpkg(pkg_info_t *pkg_info, FILE *pkg_file) {
    if (NULL == pkg_file) {
        return TM_CFG_PARSE_STATUS_NOFILE;
    }

    cfg_parse_status_t ret =
        cfg_parse(pkg_file, (cfg_translator_t)pkg_translator, pkg_info);

    if (TM_CFG_PARSE_STATUS_OK != ret) {
        pkg_free_pkg(*pkg_info);
    }

    return ret;
}

cfg_parse_status_t pkg_parse_tmpkg(pkg_info_t *pkg_info,
                                   const char *pkg_file_path) {
    FILE              *fp  = fopen(pkg_file_path, "r");
    cfg_parse_status_t ret = pkg_parse_ftmpkg(pkg_info, fp);

    if (NULL != fp) {
        fclose(fp);
    }

    return ret;
}

cfg_parse_status_t pkg_parse_ftmrcp(recipe_t *rcp, FILE *rcp_file) {
    if (NULL == rcp_file) {
        return TM_CFG_PARSE_STATUS_NOFILE;
    }

    cfg_parse_status_t ret =
        cfg_parse(rcp_file, (cfg_translator_t)rcp_translator, rcp);

    if (TM_CFG_PARSE_STATUS_OK != ret) {
        pkg_free_rcp(*rcp);
    }

    return ret;
}

cfg_parse_status_t pkg_parse_tmrcp(recipe_t *rcp, const char *rcp_file_path) {
    FILE              *fp  = fopen(rcp_file_path, "r");
    cfg_parse_status_t ret = pkg_parse_ftmrcp(rcp, fp);

    if (NULL != fp) {
        fclose(fp);
    }

    return ret;
}

bool pkg_dump_frcp(FILE *fp, recipe_t recipe) {
    dump_if_set(fp, "URL", recipe.pkg_info.url);
    dump_if_set(fp, "FROM_REPOSITORY", recipe.pkg_info.from_repoistory);
    dump_if_set(fp, "APPLICATION_NAME", recipe.pkg_info.application_name);
    dump_if_set(fp, "EXECUTABLE_PATH", recipe.pkg_info.executable_path);
    dump_if_set(fp, "WORKING_DIRECTORY", recipe.pkg_info.working_directory);
    dump_if_set(fp, "ICON_PATH", recipe.pkg_info.icon_path);
    dump_if_set(fp, "PACKAGE_FORMAT", recipe.package_format);
    dump_bool(fp, "ADD_TO_PATH", recipe.add_to_path);
    dump_bool(fp, "ADD_TO_DESKTOP", recipe.add_to_desktop);
    dump_bool(fp, "ADD_TO_TARMAN", recipe.add_to_tarman);

    return true;
}

bool pkg_dump_rcp(const char *file_path, recipe_t recipe) {
    FILE *fp = fopen(file_path, "w");

    if (NULL == fp) {
        return false;
    }

    bool ret = pkg_dump_frcp(fp, recipe);
    fclose(fp);
    return ret;
}

void pkg_free_pkg(pkg_info_t pkg_info) {
    mem_safe_free(pkg_info.url);
    mem_safe_free(pkg_info.from_repoistory);
    mem_safe_free(pkg_info.executable_path);
    mem_safe_free(pkg_info.application_name);
    mem_safe_free(pkg_info.working_directory);
    mem_safe_free(pkg_info.icon_path);
}

void pkg_free_rcp(recipe_t recipe) {
    pkg_free_pkg(recipe.pkg_info);
    mem_safe_free(recipe.package_format);
}
