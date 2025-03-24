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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"
#include "cli/directives/commands.h"
#include "cli/directives/types.h"
#include "cli/input.h"
#include "cli/output.h"
#include "config.h"
#include "download.h"
#include "os/fs.h"
#include "package.h"
#include "tm-mem.h"
#include "util/misc.h"
#include "util/pkg.h"

static const char *
override_if_src_set(const char *dst, const char *src, bool copy) {
    if (NULL != src && 0 != src[0]) {
        if (copy) {
            char *buf = (char *)malloc(strlen(src) + 1);
            mem_chkoom(buf);
            strcpy(buf, src);
            return buf;
        }

        return src;
    }

    return dst;
}

static void override_recipe(rt_recipe_t *recipe, cli_info_t cli_info) {
    recipe_t   *rcp = &recipe->recipe;
    pkg_info_t *pkg = &recipe->recipe.pkg_info;

    recipe->pkg_name =
        override_if_src_set(recipe->pkg_name, cli_info.pkg_name, true);

    rcp->package_format =
        override_if_src_set(rcp->package_format, cli_info.pkg_fmt, true);
    pkg->url =
        override_if_src_set(pkg->application_name, cli_info.app_name, true);
    pkg->executable_path =
        override_if_src_set(pkg->executable_path, cli_info.exec_path, true);
    pkg->application_name =
        override_if_src_set(pkg->application_name, cli_info.app_name, true);
    pkg->working_directory =
        override_if_src_set(pkg->working_directory, cli_info.working_dir, true);
    pkg->icon_path =
        override_if_src_set(pkg->icon_path, cli_info.icon_path, true);

    rcp->add_to_path    = cli_info.add_path;
    rcp->add_to_desktop = cli_info.add_desktop;
    rcp->add_to_tarman  = cli_info.add_tarman;
}

static void remove_pkg_cache(const char *archive_path) {
    cli_out_progress("Removing cache '%s'", archive_path);

    if (TM_FS_FILEOP_STATUS_OK != os_fs_file_rm(archive_path)) {
        cli_out_warning("Unable to delete cache");
    }
}

static bool gen_repos_list(char     ***repos_list,
                           size_t     *repos_count,
                           const char *pkg_name,
                           const char *repos_path) {
    os_fs_dirstream_t repos_stream;
    fs_dirop_status_t open_status = os_fs_dir_open(&repos_stream, repos_path);

    if (TM_FS_DIROP_STATUS_OK != open_status) {
        cli_out_error("Unable to open repositories directory");
        return false;
    }

    fs_dirent_t ent;
    size_t      repos_buf_sz = 16;
    char      **repos        = (char **)malloc(repos_buf_sz * sizeof(char *));
    size_t      i            = 0;
    mem_chkoom(repos);

    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(repos_stream, &ent)) {
        if (TM_FS_FILETYPE_DIR != ent.file_type) {
            continue;
        }

        char *pkg_recipe = NULL;
        os_fs_tm_dyrecipe(&pkg_recipe, ent.name, pkg_name);

        fs_filetype_t rcp_file_type;

        if (TM_FS_FILEOP_STATUS_OK ==
            os_fs_file_gettype(&rcp_file_type, pkg_recipe) /* &&
        TM_FS_FILETYPE_REGULAR == rcp_file_type */) {
            if (repos_buf_sz - 1 == i) {
                repos_buf_sz *= 2;
                repos = (char **)realloc(repos, repos_buf_sz * sizeof(char *));
                mem_chkoom(repos);
            }

            repos[i] = (char *)override_if_src_set(repos[i], ent.name, true);
            i++;
        }

        mem_safe_free(pkg_recipe);
    }

    *repos_count = i;
    *repos_list  = repos;
    os_fs_dir_close(repos_stream);
    return true;
}

static size_t user_choose(char      **options,
                          size_t      options_count,
                          bool        allow_custom,
                          const char *msg_fmt,
                          ...) {
    if (1 == options_count && !allow_custom) {
        return 1;
    }

    cli_out_newline();
    va_list args;
    va_start(args, msg_fmt);
    vprintf(msg_fmt, args);
    va_end(args);
    putchar(':');
    cli_out_reset();
    cli_out_newline();

    size_t range_min = 1;

    if (allow_custom) {
        cli_out_space(8);
        printf("0. [Custom]");
        cli_out_newline();
        range_min = 0;
    }

    for (size_t i = 0; i < options_count; i++) {
        cli_out_space(8);
        printf("%ld. %s", i + 1, options[i]);
        cli_out_newline();
        cli_out_reset();
    }

    return cli_in_int(
        "Enter the desired option number", range_min, options_count);
}

static bool find_repository(rt_recipe_t *recipe) {
    const char *repos_path = NULL;
    os_fs_tm_dyrepos((char **)&repos_path);

    bool   ret         = false;
    char **repos       = NULL;
    size_t repos_count = 0;

    if (!gen_repos_list(&repos, &repos_count, recipe->pkg_name, repos_path)) {
        goto cleanup;
    }

    if (0 == repos_count) {
        cli_out_error("Package '%s' not found in local repositories",
                      recipe->pkg_name);
        goto cleanup;
    }

    unsigned long user_choice = user_choose(
        repos,
        repos_count,
        false,
        "Multiple repositories found for package '%s', choose between",
        recipe->pkg_name);

    recipe->recipe.pkg_info.from_repoistory = repos[user_choice - 1];

    if (util_pkg_load_recipe(&recipe->recipe,
                             recipe->recipe.pkg_info.from_repoistory,
                             recipe->pkg_name,
                             LOG_ON)) {
        ret = true;
    }

cleanup:
    for (size_t i = 0; i < repos_count; i++) {
        if (i != user_choice - 1) {
            mem_safe_free(repos[i]);
        }
    }

    mem_safe_free(repos);
    mem_safe_free(repos_path);

    return ret;
}

static bool infer_app_name(rt_recipe_t *recipe, const char *pkg_path) {
    cli_out_progress("Inferring application name");

    os_fs_dirstream_t stream;
    fs_dirop_status_t open_status = os_fs_dir_open(&stream, pkg_path);

    if (TM_FS_DIROP_STATUS_OK != open_status) {
        cli_out_error("Unable to visit package directory '%s'", pkg_path);
        return false;
    }

    fs_dirent_t ent;
    size_t      names_buf_sz = 16;
    char      **names        = (char **)malloc(names_buf_sz * sizeof(char *));
    size_t      count = 1; // Count is one because there's a default value
    mem_chkoom(names);

    names[0]    = (char *)override_if_src_set(names[0], recipe->pkg_name, true);
    names[0][0] = toupper(names[0][0]);

    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(stream, &ent)) {
        if (TM_FS_FILETYPE_DIR == ent.file_type) {
            if (0 == strcmp(ent.name, names[0])) {
                continue;
            }

            if (names_buf_sz - 1 == count) {
                names_buf_sz *= 2;
                names = (char **)realloc(names, names_buf_sz * sizeof(char *));
                mem_chkoom(names);
            }

            names[count] =
                (char *)override_if_src_set(names[count], ent.name, true);
            count++;
        }
    }

    os_fs_dir_close(stream);

    unsigned long user_choice =
        user_choose(names, count, true, "Choose an application name");

    if (0 == user_choice) {
        cli_in_dystr("Enter working directory",
                     (char **)&recipe->recipe.pkg_info.application_name);
    } else {
        recipe->recipe.pkg_info.application_name = names[user_choice - 1];
    }

    for (size_t i = 0; i < count; i++) {
        if (i != user_choice - 1) {
            mem_safe_free(names[i]);
        }
    }

    mem_safe_free(names);
    return true;
}

static bool find_executables(char     ***execs,
                             size_t     *count,
                             size_t     *bufsz,
                             const char *base_path,
                             const char *subdir) {
    char *dir_path = (char *)base_path;

    if (NULL != subdir) {
        os_fs_path_dyconcat(&dir_path, 2, base_path, subdir);
    }

    os_fs_dirstream_t stream;
    fs_dirop_status_t open_status = os_fs_dir_open(&stream, dir_path);
    bool              ret         = false;

    if (TM_FS_DIROP_STATUS_OK != open_status) {
        cli_out_error("Unable to visit subdirectory '%s'", dir_path);
        goto cleanup;
    }

    fs_dirent_t ent;

    while (TM_FS_DIROP_STATUS_OK == os_fs_dir_next(stream, &ent)) {
        if (TM_FS_FILETYPE_DIR == ent.file_type) {
            char *sub_subdir = NULL;

            if (NULL == subdir) {
                if (!find_executables(
                        execs, count, bufsz, base_path, ent.name)) {
                    goto close;
                }
                continue;
            }

            os_fs_path_dyconcat(&sub_subdir, 2, subdir, ent.name);

            if (!find_executables(execs, count, bufsz, base_path, sub_subdir)) {
                mem_safe_free(sub_subdir);
                goto close;
            }

            mem_safe_free(sub_subdir);
        }

        else if (TM_FS_FILETYPE_EXEC == ent.file_type) {
            if (*bufsz - 1 == *count) {
                *bufsz *= 2;
                *execs = (char **)realloc(*execs, *bufsz * sizeof(char *));
                mem_chkoom(*execs);
            }

            char *exec_path = NULL;
            os_fs_path_dyconcat(&exec_path, 2, subdir, ent.name);

            (*execs)[*count] = exec_path;
            (*count)++;
        }
    }

    ret = true;

close:
    os_fs_dir_close(stream);
cleanup:
    if (NULL != subdir) {
        mem_safe_free(dir_path);
    }

    return ret;
}

static bool infer_exec(rt_recipe_t *recipe, const char *pkg_path) {
    bool          ret          = false;
    size_t        execs_buf_sz = 16;
    char        **execs        = (char **)malloc(execs_buf_sz * sizeof(char *));
    size_t        count        = 0;
    unsigned long user_choice  = 0;
    mem_chkoom(execs);

    if (find_executables(&execs, &count, &execs_buf_sz, pkg_path, NULL)) {
        user_choice = user_choose(execs, count, true, "Choose an executable");
        recipe->recipe.pkg_info.executable_path = execs[user_choice - 1];
        ret                                     = true;
    }

    for (size_t i = 0; i < count; i++) {
        if (i != user_choice - 1) {
            mem_safe_free(execs[i]);
        }
    }

    mem_safe_free(execs);
    return ret;
}

static void infer_working_dir(rt_recipe_t *recipe) {
    cli_out_progress("Inferring working directory");

    char *parent = NULL;
    os_fs_path_dyparent(&parent, recipe->recipe.pkg_info.executable_path);
    recipe->recipe.pkg_info.working_directory = parent;
}

static bool infer_additional_info(rt_recipe_t *recipe,
                                  cli_info_t   cli_info,
                                  const char  *pkg_path) {
    if (recipe->is_remote) {
        return true;
    }

    if (!recipe->recipe.add_to_path && !cli_info.add_path) {
        recipe->recipe.add_to_path =
            cli_in_bool("Do you want to add this package to PATH?");
    }

    if (NULL == recipe->recipe.pkg_info.executable_path &&
        !infer_exec(recipe, pkg_path)) {
        return false;
    }

    if (!recipe->recipe.add_to_desktop && !cli_info.add_desktop) {
        recipe->recipe.add_to_desktop =
            cli_in_bool("Do you want to add this package as an app?");
    }

    if (recipe->recipe.add_to_desktop &&
        NULL == recipe->recipe.pkg_info.application_name &&
        !infer_app_name(recipe, pkg_path)) {
        return false;
    }

    if (recipe->recipe.add_to_desktop &&
        NULL != recipe->recipe.pkg_info.executable_path &&
        NULL == recipe->recipe.pkg_info.working_directory) {
        infer_working_dir(recipe);
    }

    return true;
}

int cli_cmd_install(cli_info_t info) {
    if (NULL == info.input) {
        cli_out_error("Must specify a package to install'");
        return EXIT_FAILURE;
    }

    rt_recipe_t recipe = {0};
    int         ret    = EXIT_FAILURE;

    // Variables to be cleaned up
    // Declartion is here to avoid issues with goto
    char       *pkg_path     = NULL;
    char       *archive_path = NULL;
    char       *pkg_rcp_path = NULL;
    const char *exec_path    = NULL;

    cli_out_progress("Initializing host file system");

    if (!os_fs_tm_init()) {
        cli_out_progress("Failed to inizialize host file system");
        goto cleanup;
    }

    cli_out_progress("Initiating installation process");

    override_recipe(&recipe, info);

    if (info.from_url && NULL == recipe.recipe.package_format) {
        cli_out_warning(
            "Package format not specified for remote download, using 'tar.gz'");
        recipe.recipe.package_format =
            override_if_src_set(recipe.recipe.package_format, "tar.gz", true);
    }

    // Check if -r is set and use repositories
    if (info.from_repo) {
        recipe.is_remote = true;
        recipe.pkg_name =
            override_if_src_set(recipe.pkg_name, info.input, true);

        if (!find_repository(&recipe)) {
            goto cleanup;
        }

        if (NULL == recipe.recipe.pkg_info.url) {
            cli_out_error("Package URL not found in recipe");
            goto cleanup;
        }

        if (!util_pkg_fetch_archive(&archive_path,
                                    recipe.pkg_name,
                                    recipe.recipe.package_format,
                                    recipe.recipe.pkg_info.url,
                                    LOG_ON)) {
            goto cleanup;
        }
    }

    // Ask user to enter the package name
    while (NULL == recipe.pkg_name &&
           0 == cli_in_dystr("Enter package name", (char **)&recipe.pkg_name))
        ;

    // If -u is used to download from a URL
    if (info.from_url) {
        recipe.recipe.pkg_info.url =
            override_if_src_set(recipe.recipe.pkg_info.url, info.input, true);

        if (!util_pkg_fetch_archive(&archive_path,
                                    recipe.pkg_name,
                                    recipe.recipe.package_format,
                                    recipe.recipe.pkg_info.url,
                                    LOG_ON)) {
            goto cleanup;
        }
    }

    if (NULL == archive_path) {
        archive_path =
            (char *)override_if_src_set(archive_path, info.input, true);
    }

    if (!util_pkg_create_directory(
            &pkg_path, recipe.pkg_name, LOG_ON, INPUT_ON)) {
        goto cleanup;
    }

    cli_out_progress("Extracting archive '%s' to '%s'", archive_path, pkg_path);

    if (!archive_extract(pkg_path, archive_path, NULL)) {
        cli_out_error(
            "Unable to extract archive. You may be missing the plugin "
            "for this archive type");
        goto cleanup;
    }

    if (!recipe.is_remote) {
        util_pkg_load_config(&recipe.recipe.pkg_info, pkg_path, LOG_ON);
    }

    if (!infer_additional_info(&recipe, info, pkg_path)) {
        goto cleanup;
    }

    os_fs_path_dyconcat(&pkg_rcp_path, 2, pkg_path, "recipe.tarman");
    cli_out_progress("Creating recipe artifact in '%s'", pkg_rcp_path);
    pkg_dump_rcp(pkg_rcp_path, recipe.recipe);

    if (NULL != recipe.recipe.pkg_info.executable_path) {
        os_fs_path_dyconcat((char **)&exec_path,
                            2,
                            pkg_path,
                            recipe.recipe.pkg_info.executable_path);

        if (recipe.recipe.add_to_path) {
            util_pkg_add_to_path(exec_path, LOG_ON);
        }

        if (recipe.recipe.add_to_desktop) {
            util_pkg_add_to_desktop(pkg_path,
                                    recipe.recipe.pkg_info.application_name,
                                    exec_path,
                                    recipe.recipe.pkg_info.working_directory,
                                    recipe.recipe.pkg_info.icon_path,
                                    LOG_ON);
        }
    }

    if ((info.from_url || info.from_repo) && NULL != archive_path) {
        remove_pkg_cache(archive_path);
    }

    cli_out_success("Package '%s' installed successfully", recipe.pkg_name);
    ret = EXIT_SUCCESS;

cleanup:
    mem_safe_free(archive_path);
    mem_safe_free(pkg_path);
    mem_safe_free(exec_path);
    mem_safe_free(pkg_rcp_path);
    mem_safe_free(recipe.pkg_name);
    pkg_free_rcp(recipe.recipe);
    return ret;
}
