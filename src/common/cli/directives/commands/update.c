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

#include "archive.h"
#include "cli/directives/commands.h"
#include "cli/input.h"
#include "cli/output.h"
#include "config.h"
#include "download.h"
#include "os/fs.h"
#include "package.h"
#include "tm-mem.h"
#include "util/misc.h"
#include "util/pkg.h"

int cli_cmd_update(cli_info_t info) {
    int         ret              = EXIT_FAILURE;
    const char *pkg_name         = info.input;
    char       *pkg_path         = NULL;
    char       *tmp_archive_path = NULL;
    char       *artifact_path    = NULL;
    char       *pkg_rcp_path     = NULL;
    recipe_t    recipe_artifact  = {0};

    if (NULL == pkg_name) {
        cli_out_error(
            "You must specify a package name for it to be removed. Use "
            "'tarman update <pkg name>'");
        return ret;
    }

    cli_out_progress("Initializing host file system");

    if (!os_fs_tm_init()) {
        cli_out_progress("Failed to inizialize host file system");
        return ret;
    }

    os_fs_tm_dypkg(&pkg_path, pkg_name);

    os_fs_path_dyconcat(&artifact_path, 2, pkg_path, "recipe.tarman");
    cli_out_progress("Using metadata (recipe artifact) file '%s'",
                     artifact_path);

    if (TM_CFG_PARSE_STATUS_OK !=
        pkg_parse_tmrcp(&recipe_artifact, artifact_path)) {
        cli_out_error("Cannot update package '%s'. Missing or corrupt metadata "
                      "(recipe artifact) file",
                      pkg_name);
        goto cleanup;
    }

    if (NULL != recipe_artifact.pkg_info.from_repoistory &&
        !util_pkg_load_recipe(&recipe_artifact,
                              recipe_artifact.pkg_info.from_repoistory,
                              pkg_name,
                              LOG_ON)) {
        cli_out_warning(
            "Updating using package metadata (recipe artifact) instead "
            "of repository recipe");
    }

    if (NULL == recipe_artifact.pkg_info.url ||
        NULL == recipe_artifact.package_format) {
        cli_out_error(
            "Cannot update package '%s'. Some metadata properties are missing",
            pkg_name);
        goto cleanup;
    }

    if (!util_pkg_fetch_archive(&tmp_archive_path,
                                pkg_name,
                                recipe_artifact.package_format,
                                recipe_artifact.pkg_info.url,
                                LOG_ON)) {
        goto cleanup;
    }

    cli_out_progress("Removing old package directory '%s'", pkg_path);

    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_rm(pkg_path)) {
        cli_out_error("Unable to remove package directory '%s'", pkg_path);
        goto cleanup;
    }

    if (!util_pkg_create_directory_from_path(pkg_path, LOG_ON, INPUT_OFF)) {
        goto cleanup;
    }

    cli_out_progress(
        "Extracting archive '%s' to '%s'", tmp_archive_path, pkg_path);

    if (!archive_extract(
            pkg_path, tmp_archive_path, recipe_artifact.package_format)) {
        cli_out_error(
            "Unable to extract archive, package has been removed but not "
            "reinstalled, sorry");
        goto cleanup;
    }

    os_fs_path_dyconcat(&pkg_rcp_path, 2, pkg_path, "recipe.tarman");
    cli_out_progress("Creating recipe artifact in '%s'", pkg_rcp_path);
    pkg_dump_rcp(pkg_rcp_path, recipe_artifact);

    if (NULL != recipe_artifact.pkg_info.executable_path) {
        char *exec_full_path = NULL;
        os_fs_path_dyconcat(&exec_full_path,
                            2,
                            pkg_path,
                            recipe_artifact.pkg_info.executable_path);

        if (recipe_artifact.add_to_path) {
            util_pkg_add_to_path(exec_full_path, LOG_ON);
        }

        if (recipe_artifact.add_to_desktop) {
            util_pkg_add_to_desktop(pkg_path,
                                    recipe_artifact.pkg_info.application_name,
                                    exec_full_path,
                                    recipe_artifact.pkg_info.working_directory,
                                    recipe_artifact.pkg_info.icon_path,
                                    LOG_ON);
        }

        mem_safe_free(exec_full_path);
    }

    cli_out_progress("Removing cache '%s'", tmp_archive_path);

    if (TM_FS_FILEOP_STATUS_OK != os_fs_file_rm(tmp_archive_path)) {
        cli_out_warning("Unable to remove cache");
    }

    cli_out_success("Package '%s' updated successfully", tmp_archive_path);
    ret = EXIT_SUCCESS;

cleanup:
    mem_safe_free(pkg_path);
    mem_safe_free(tmp_archive_path);
    mem_safe_free(artifact_path);
    mem_safe_free(pkg_rcp_path);
    pkg_free_rcp(recipe_artifact);
    return ret;
}
