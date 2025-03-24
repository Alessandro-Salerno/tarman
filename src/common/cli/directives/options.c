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

#include "cli/directives/options.h"

#include "cli/output.h"

static bool
set_opt_using_next(const char *opt, const char **target, const char *next) {
    if (NULL != *target) {
        cli_out_warning("Ignoring repeated option '%s' with value '%s', using "
                        "previous '%s'",
                        opt,
                        next,
                        *target);
        return true; // Return true anyway since this is just a warning
    }

    if (NULL == next) {
        cli_out_error("Unexpected end-of-command after last option");
        return false;
    }

    *target = next;
    return true;
}

bool cli_opt_from_url(cli_info_t *info, const char *next) {
    (void)next;
    if (info->from_repo) {
        cli_out_error("Options '%s' and '%s' are not compatible",
                      TARMAN_FOPT_FROM_REPO,
                      TARMAN_FOPT_FROM_URL);
        return false;
    }

    info->from_url = true;
    return true;
}

bool cli_opt_from_repo(cli_info_t *info, const char *next) {
    (void)next;
    if (info->from_url) {
        cli_out_error("Options '%s' and '%s' are not compatible",
                      TARMAN_FOPT_FROM_URL,
                      TARMAN_FOPT_FROM_REPO);
        return false;
    }

    info->from_repo = true;
    return true;
}

bool cli_opt_pkg_fmt(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_PKG_FMT, &info->pkg_fmt, next);
}

bool cli_opt_pkg_name(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_PKG_NAME, &info->pkg_name, next);
}

bool cli_opt_app_name(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_APP_NAME, &info->app_name, next);
}

bool cli_opt_exec(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_EXEC, &info->exec_path, next);
}

bool cli_opt_wrk_dir(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_WRK_DIR, &info->working_dir, next);
}

bool cli_opt_icon(cli_info_t *info, const char *next) {
    return set_opt_using_next(TARMAN_FOPT_ICON, &info->icon_path, next);
}

bool cli_opt_add_path(cli_info_t *info, const char *next) {
    (void)next;
    info->add_path = true;
    return true;
}

bool cli_opt_add_desktop(cli_info_t *info, const char *next) {
    (void)next;
    info->add_desktop = true;
    return true;
}

bool cli_opt_add_tarman(cli_info_t *info, const char *next) {
    (void)next;
    info->add_tarman = true;
    return true;
}
