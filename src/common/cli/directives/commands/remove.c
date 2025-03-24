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

int cli_cmd_remove(cli_info_t info) {
    int         ret             = EXIT_FAILURE;
    const char *pkg_name        = info.input;
    char       *pkg_path        = NULL;
    char       *artifact_path   = NULL;
    recipe_t    recipe_artifact = {0};

    if (NULL == pkg_name) {
        cli_out_error(
            "You must specify a package name for it to be removed. Use "
            "'tarman remove <pkg name>'");
        return ret;
    }

    cli_out_progress("Initializing host file system");

    if (!os_fs_tm_init()) {
        cli_out_progress("Failed to inizialize host file system");
        goto cleanup;
    }

    os_fs_tm_dypkg(&pkg_path, pkg_name);

    os_fs_dirstream_t dir_stream;

    switch (os_fs_dir_open(&dir_stream, pkg_path)) {
    case TM_FS_DIROP_STATUS_NOEXIST:
        cli_out_error(
            "The package '%s' is not installed on this system, at least "
            "not as a tarman package. Try with other package managers "
            "you may have on your system");
        goto cleanup;

    case TM_FS_DIROP_STATUS_OK:
        break;

    default:
        cli_out_error("Unable to open package directory '%s'", pkg_path);
        goto cleanup;
    }

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_close(dir_stream)) {
        cli_out_error(
            "Error while closing preliminary directory stream to '%s'",
            pkg_path);
        goto cleanup;
    }

    if (!cli_in_bool("Proceed with removal?")) {
        goto cleanup;
    }

    os_fs_path_dyconcat(&artifact_path, 2, pkg_path, "recipe.tarman");

    if (pkg_parse_tmrcp(&recipe_artifact, artifact_path)) {
        if (recipe_artifact.add_to_path) {
            cli_out_progress("Removing executable from PATH");

            if (!os_env_path_rm(recipe_artifact.pkg_info.executable_path)) {
                cli_out_warning("Could not remove executable from PATH");
            }
        }

        if (recipe_artifact.add_to_desktop) {
            cli_out_progress("Removing app from system applications");

            if (!os_env_desktop_rm(recipe_artifact.pkg_info.application_name)) {
                cli_out_warning(
                    "Could not remove app from system applications");
            }
        }

        // TODO: Remove tarman plugin
    } else {
        cli_out_warning(
            "Removing package without metadata (recipe artifact), some "
            "files may persist");
    }

    cli_out_progress("Removing package directory '%s'", pkg_path);

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_rm(pkg_path)) {
        cli_out_error(
            "Unable to remove package directory '%s'. The package may "
            "now be fully or partially as a result. You can attempt "
            "manual removal of the package by deleting the package "
            "directory and all PATH or Desktop references that may exist",
            pkg_path);
        goto cleanup;
    }

    cli_out_success("Package '%s' removed successfully", pkg_name);
    ret = EXIT_SUCCESS;

cleanup:
    mem_safe_free(pkg_path);
    mem_safe_free(artifact_path);
    pkg_free_rcp(recipe_artifact);
    return ret;
}
