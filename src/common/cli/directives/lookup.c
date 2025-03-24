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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cli/directives/commands.h"
#include "cli/directives/lookup.h"
#include "cli/directives/options.h"
#include "cli/directives/types.h"

static cli_drt_desc_t commands[] = {
    {NULL, TARMAN_CMD_HELP, NULL, false, cli_cmd_help, "Show this menu"},

    {NULL,
     TARMAN_CMD_INSTALL,
     NULL,
     false,
     cli_cmd_install,
     "Install a package"},

    {NULL,
     TARMAN_CMD_LIST,
     NULL,
     false,
     cli_cmd_list,
     "List all installed packages"},

    {NULL,
     TARMAN_CMD_REMOVE,
     NULL,
     false,
     cli_cmd_remove,
     "Remove an installed package"},

    {NULL,
     TARMAN_CMD_UPDATE,
     NULL,
     false,
     cli_cmd_update,
     "Update an installed package"},

    // {NULL,
    //  TARMAN_CMD_UPDATE_ALL,
    //  NULL,
    //  false,
    //  cli_cmd_update_all,
    //  "Update all installed repos and pacakges"},

    {NULL,
     TARMAN_CMD_ADD_REPO,
     NULL,
     false,
     cli_cmd_add_repo,
     "Add a remote repository to the local database"},

    {NULL,
     TARMAN_CMD_REMOVE_REPO,
     NULL,
     false,
     cli_cmd_remove_repo,
     "Remove a local repository"},

    // {NULL,
    //  TARMAN_CMD_LIST_REPOS,
    //  NULL,
    //  false,
    //  cli_cmd_list_repos,
    //  "List all local repositories"},

    {NULL,
     TARMAN_CMD_VERSION,
     NULL,
     false,
     cli_cmd_version,
     "Show version information"},
};
// {NULL, TARMAN_CMD_TEST, NULL, false, cli_cmd_test, "Test tarman"}};

static cli_drt_desc_t options[] = {
    {TARMAN_SOPT_FROM_URL,
     TARMAN_FOPT_FROM_URL,
     cli_opt_from_url,
     false,
     NULL,
     "[Install] Use URL as package input and perform download"},

    {TARMAN_SOPT_FROM_REPO,
     TARMAN_FOPT_FROM_REPO,
     cli_opt_from_repo,
     false,
     NULL,
     "[Install] use package name as input and perform local repository lookup"},

    {TARMAN_SOPT_PKG_NAME,
     TARMAN_FOPT_PKG_NAME,
     cli_opt_pkg_name,
     true,
     NULL,
     "[Install] Specify package name"},

    {TARMAN_SOPT_APP_NAME,
     TARMAN_FOPT_APP_NAME,
     cli_opt_app_name,
     true,
     NULL,
     "[Install] Specify application name"},

    {TARMAN_SOPT_EXEC,
     TARMAN_FOPT_EXEC,
     cli_opt_exec,
     true,
     NULL,
     "[Install] Specify relative path to executable"},

    {TARMAN_SOPT_WRK_DIR,
     TARMAN_FOPT_WRK_DIR,
     cli_opt_wrk_dir,
     true,
     NULL,
     "[Install] Specify a relative directory to use as WD for the desktop "
     "application"},

    {TARMAN_SOPT_ICON,
     TARMAN_FOPT_ICON,
     cli_opt_icon,
     true,
     NULL,
     "[Install] Specify realtive path to icon file for desktop application"},

    {TARMAN_SOPT_ADD_PATH,
     TARMAN_FOPT_ADD_PATH,
     cli_opt_add_path,
     false,
     NULL,
     "[Install] Add package executable to PATH"},

    {TARMAN_SOPT_ADD_DESKTOP,
     TARMAN_FOPT_ADD_DESKTOP,
     cli_opt_add_desktop,
     false,
     NULL,
     "[Install] Add package as desktop application"},

    {TARMAN_SOPT_ADD_TARMAN,
     TARMAN_FOPT_ADD_TARMAN,
     cli_opt_add_tarman,
     false,
     NULL,
     "[Install] Add package to tarman plugins"},

    {TARMAN_SOPT_PKG_FMT,
     TARMAN_FOPT_PKG_FMT,
     cli_opt_pkg_fmt,
     true,
     NULL,
     "Specify archive format (e.g., tar.gz, tar.xz, zip)"},
};

static bool find_desc(cli_drt_desc_t  descriptors[],
                      size_t          num_desc,
                      const char     *arg,
                      cli_drt_desc_t *dst) {
    for (size_t i = 0; i < num_desc; i++) {
        cli_drt_desc_t opt_desc = descriptors[i];

        if ((NULL == opt_desc.short_option ||
             0 != strcmp(opt_desc.short_option, arg)) &&
            (NULL == opt_desc.full_option ||
             0 != strcmp(opt_desc.full_option, arg))) {
            continue;
        }

        *dst = opt_desc;
        return true;
    }

    return false;
}

bool cli_lkup_command(const char *command, cli_drt_desc_t *dst) {
    return find_desc(
        commands, sizeof commands / sizeof(cli_drt_desc_t), command, dst);
}

bool cli_lkup_option(const char *option, cli_drt_desc_t *dst) {
    return find_desc(
        options, sizeof options / sizeof(cli_drt_desc_t), option, dst);
}

cli_lkup_table_t cli_lkup_cmdtable(void) {
    return (cli_lkup_table_t){.table = commands,
                              .num_entries =
                                  sizeof commands / sizeof(cli_drt_desc_t)};
}

cli_lkup_table_t cli_lkup_opttable(void) {
    return (cli_lkup_table_t){.table = options,
                              .num_entries =
                                  sizeof options / sizeof(cli_drt_desc_t)};
}
