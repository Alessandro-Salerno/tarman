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

#include <stdlib.h>

#include "cli/directives/commands.h"
#include "cli/directives/types.h"
#include "cli/input.h"
#include "cli/output.h"
#include "os/env.h"
#include "os/fs.h"
#include "package.h"
#include "tm-mem.h"

int cli_cmd_remove_repo(cli_info_t info) {
    int         ret       = EXIT_FAILURE;
    const char *repo_name = info.input;
    char       *repo_path = NULL;

    if (NULL == repo_name) {
        cli_out_error(
            "You must specify a repository name for it to be removed. Use "
            "'tarman remove-repo <repo name>'");
        return ret;
    }

    cli_out_progress("Initializing host file system");

    if (!os_fs_tm_init()) {
        cli_out_progress("Failed to inizialize host file system");
        goto cleanup;
    }

    os_fs_tm_dyrepo(&repo_path, repo_name);

    os_fs_dirstream_t dir_stream;

    switch (os_fs_dir_open(&dir_stream, repo_path)) {
    case TM_FS_DIROP_STATUS_NOEXIST:
        cli_out_error("The repository '%s' is not present on this system",
                      repo_name);
        goto cleanup;

    case TM_FS_DIROP_STATUS_OK:
        break;

    default:
        cli_out_error("Unable to open repository directory '%s'", repo_path);
        goto cleanup;
    }

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_close(dir_stream)) {
        cli_out_error(
            "Error while closing preliminary directory stream to '%s'",
            repo_path);
        goto cleanup;
    }

    if (!cli_in_bool("Proceed with removal?")) {
        goto cleanup;
    }

    cli_out_progress("Removing repository directory '%s'", repo_path);

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_rm(repo_path)) {
        cli_out_error("Unable to remove repository directory '%s'", repo_path);
        goto cleanup;
    }

    cli_out_success("Repository '%s' removed successfully", repo_name);
    ret = EXIT_SUCCESS;

cleanup:
    mem_safe_free(repo_path);
    return ret;
}
