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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"
#include "cli/directives/commands.h"
#include "cli/output.h"
#include "download.h"
#include "os/fs.h"
#include "tm-mem.h"
#include "util/misc.h"

int cli_cmd_add_repo(cli_info_t info) {
    if (NULL == info.input) {
        cli_out_error("Must specify a package to install'");
        return EXIT_FAILURE;
    }

    char       *archive_path = NULL;
    const char *repo_url     = info.input;
    const char *repo_fmt     = info.pkg_fmt;
    const char *repos_path   = NULL;
    int         ret          = EXIT_FAILURE;

    cli_out_progress("Initializing host file system");

    if (!os_fs_tm_init()) {
        cli_out_progress("Failed to inizialize host file system");
        goto cleanup;
    }

    os_fs_tm_dyrepos((char **)&repos_path);

    if (NULL == repo_fmt) {
        cli_out_warning("Repository format not specified, using 'tar.gz'");
        repo_fmt = "tar.gz";
    }

    util_misc_dytmpfile(&archive_path, "__downloaded_repo", repo_fmt);
    cli_out_progress("Fetching repository from '%s'", repo_url);

    if (!download(archive_path, repo_url)) {
        cli_out_error("Unable to download package");
        goto cleanup;
    }

    cli_out_progress("Extracting repository files");

    if (!archive_extract(repos_path, archive_path, NULL)) {
        cli_out_error(
            "Unable to extract archive. You may be missing the plugin "
            "for this archive type");
        goto cleanup;
    }

    cli_out_progress("Removing cache '%s'", archive_path);

    if (TM_FS_FILEOP_STATUS_OK != os_fs_file_rm(archive_path)) {
        cli_out_warning("Unable to delete cache");
    }

    cli_out_success("Repository added successfully");

    ret = EXIT_SUCCESS;

cleanup:
    mem_safe_free(archive_path);
    mem_safe_free(repos_path);
    return ret;
}
