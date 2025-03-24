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

#include "cli/input.h"
#include "cli/output.h"
#include "config.h"
#include "download.h"
#include "os/env.h"
#include "os/fs.h"
#include "package.h"
#include "tm-mem.h"
#include "util/misc.h"
#include "util/pkg.h"

static const char *override_if_dst_unset(const char *dst, const char *src) {
    if (NULL == dst || 0 == dst[0]) {
        return src;
    }

    mem_safe_free(src);
    return dst;
}

bool util_pkg_fetch_archive(char      **dst_file,
                            const char *pkg_name,
                            const char *pkg_fmt,
                            const char *url,
                            bool        log) {
    util_misc_dytmpfile(dst_file, pkg_name, pkg_fmt);

    if (log) {
        cli_out_progress(
            "Downloading package from '%s' to '%s'", url, *dst_file);
    }

    if (!download(*dst_file, url)) {
        if (log) {
            cli_out_error("Unable to download package");
        }
        return false;
    }

    return true;
}

bool util_pkg_create_directory_from_path(const char *path, bool log, bool in) {
    if (log) {
        cli_out_progress("Creating package in '%s'", path);
    }

    // Repeat a maximum of two times
    for (size_t i = 0; i < 2; i++) {
        fs_dirop_status_t pkgdir_status = os_fs_mkdir(path);

        switch (pkgdir_status) {
        case TM_FS_DIROP_STATUS_EXIST:
            if (in) {
                return cli_in_bool("This pacakge is already installed, proceed "
                                   "with clean install?");
            }
            if (TM_FS_DIROP_STATUS_OK != os_fs_dir_rm(path)) {
                if (log) {
                    cli_out_error(
                        "Unable to remove pre-existing package directory '%s'",
                        path);
                }
                return false;
            }
            break;

        case TM_FS_DIROP_STATUS_OK:
            return true;

        default:
            goto fail;
        }
    }

fail:
    if (log) {
        cli_out_error("Unable to create directory in '%s'", path);
    }

    return false;
}

bool util_pkg_create_directory(char      **path,
                               const char *pkg_name,
                               bool        log,
                               bool        in) {
    os_fs_tm_dypkg(path, pkg_name);
    return util_pkg_create_directory_from_path(*path, log, in);
}

bool util_pkg_add_to_path(const char *exec_full_path, bool log) {
    if (log) {
        cli_out_progress("Adding executable '%s' to PATH", exec_full_path);
    }

    if (!os_env_path_add(exec_full_path)) {
        if (log) {
            cli_out_warning("Could not add executable to PATH");
        }
        return false;
    }

    return true;
}

bool util_pkg_add_to_desktop(const char *pkg_path,
                             const char *app_name,
                             const char *exec_full_path,
                             const char *wrk_dir,
                             const char *icon,
                             bool        log) {
    bool ret = true;
    cli_out_progress("Adding app '%s' to installed apps", app_name);

    const char *icon_full_path = NULL;
    const char *wrk_full_path  = NULL;

    if (NULL != icon) {
        os_fs_path_dyconcat((char **)&icon_full_path, 2, pkg_path, icon);
    } else if (log) {
        cli_out_warning("Application has no icon");
    }

    if (NULL != wrk_dir) {
        os_fs_path_dyconcat((char **)&wrk_full_path, 2, pkg_path, wrk_dir);
    } else if (log) {
        cli_out_warning("Application has no explicit working directory");
    }

    if (!os_env_desktop_add(
            app_name, exec_full_path, icon_full_path, wrk_full_path)) {
        if (log) {
            cli_out_warning("Unable to add app to system applications");
        }
        ret = false;
    }

    mem_safe_free(icon_full_path);
    mem_safe_free(wrk_full_path);
    return ret;
}

bool util_pkg_parse_config(pkg_info_t *pkg,
                           char      **cfg_path,
                           const char *pkg_path,
                           bool        log) {
    os_fs_path_dyconcat(cfg_path, 2, pkg_path, "package.tarman");
    cfg_parse_status_t status = pkg_parse_tmpkg(pkg, *cfg_path);

    switch (status) {
    case TM_CFG_PARSE_STATUS_NOFILE:
        if (log) {
            cli_out_warning("Package configuration file '%s' does not exist",
                            *cfg_path);
        }
        return false;

    case TM_CFG_PARSE_STATUS_INVVAL:
    case TM_CFG_PARSE_STATUS_MALFORMED:
        if (log) {
            cli_out_warning(
                "Ignoring malformed package configuration file at '%s'",
                *cfg_path);
        }
        return false;

    case TM_CFG_PARSE_STATUS_ERR:
    case TM_CFG_PARSE_STATUS_PERM:
        if (log) {
            cli_out_error(
                "Unable to read contents of package configuration file at '%s'",
                *cfg_path);
        }
        return false;

    default:
        break;
    }

    return true;
}

bool util_pkg_load_config(pkg_info_t *pkg, const char *pkg_path, bool log) {
    pkg_info_t tmpkg_file_data = {0};
    char      *tmpkg_file_path = NULL;
    bool       ret             = false;

    if (!util_pkg_parse_config(
            &tmpkg_file_data, &tmpkg_file_path, pkg_path, log)) {
        goto cleanup;
    }

    if (log) {
        cli_out_progress("Using package configuration file at '%s'",
                         tmpkg_file_path);
    }

    pkg->url = override_if_dst_unset(pkg->url, tmpkg_file_data.url);
    pkg->application_name = override_if_dst_unset(
        pkg->application_name, tmpkg_file_data.application_name);
    pkg->executable_path = override_if_dst_unset(
        pkg->executable_path, tmpkg_file_data.executable_path);
    pkg->working_directory = override_if_dst_unset(
        pkg->working_directory, tmpkg_file_data.working_directory);
    pkg->icon_path =
        override_if_dst_unset(pkg->icon_path, tmpkg_file_data.icon_path);

    ret = true;

cleanup:
    mem_safe_free(tmpkg_file_path);
    return ret;
}

bool util_pkg_parse_recipe(recipe_t   *recipe,
                           char      **rcp_path,
                           const char *repo,
                           const char *rcp_name,
                           bool        log) {
    os_fs_tm_dyrecipe(rcp_path, repo, rcp_name);

    if (log) {
        cli_out_progress("Using recipe file '%s'", *rcp_path);
    }

    cfg_parse_status_t status = pkg_parse_tmrcp(recipe, *rcp_path);

    switch (status) {
    case TM_CFG_PARSE_STATUS_INVVAL:
    case TM_CFG_PARSE_STATUS_MALFORMED:
        if (log) {
            cli_out_error(
                "Recipe file for package '%s' in repository '%s' is malformed",
                rcp_name,
                repo);
        }
        return false;

    case TM_CFG_PARSE_STATUS_ERR:
    case TM_CFG_PARSE_STATUS_PERM:
        if (log) {
            cli_out_error("Unable to read contents of recipe file '%s'",
                          *rcp_path);
        }
        return false;

    case TM_CFG_PARSE_STATUS_NOFILE:
        if (log) {
            cli_out_error("Cannot open recipe file '%s', no such file",
                          *rcp_path);
        }
        return false;

    default:
        break;
    }

    return true;
}

bool util_pkg_load_recipe(recipe_t   *recipe,
                          const char *repo,
                          const char *rcp_name,
                          bool        log) {
    bool     ret           = false;
    recipe_t rcp_file_data = {0};
    char    *rcp_file_path = NULL;

    if (!util_pkg_parse_recipe(
            &rcp_file_data, &rcp_file_path, repo, rcp_name, log)) {
        goto cleanup;
    }

    pkg_info_t *pkg = &recipe->pkg_info;

    pkg->url = override_if_dst_unset(pkg->url, rcp_file_data.pkg_info.url);
    pkg->application_name = override_if_dst_unset(
        pkg->application_name, rcp_file_data.pkg_info.application_name);
    pkg->executable_path = override_if_dst_unset(
        pkg->executable_path, rcp_file_data.pkg_info.executable_path);
    pkg->working_directory = override_if_dst_unset(
        pkg->working_directory, rcp_file_data.pkg_info.working_directory);
    pkg->icon_path =
        override_if_dst_unset(pkg->icon_path, rcp_file_data.pkg_info.icon_path);
    recipe->package_format = override_if_dst_unset(
        recipe->package_format, rcp_file_data.package_format);

    recipe->add_to_path    = rcp_file_data.add_to_path;
    recipe->add_to_desktop = rcp_file_data.add_to_desktop;
    recipe->add_to_tarman  = rcp_file_data.add_to_tarman;

    ret = true;

cleanup:
    mem_safe_free(rcp_file_path);
    return ret;
}
