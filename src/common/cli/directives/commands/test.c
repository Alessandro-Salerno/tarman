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
#include <stdio.h>
#include <stdlib.h>

#include "archive.h"
#include "cli/directives/commands.h"
#include "cli/output.h"
#include "os/fs.h"

#define TARMAN_TEST_INIT() bool tarman_tests_work = true
#define TARMAN_TEST(val)              \
    bool tarman_test_##val = val();   \
    if (!tarman_test_##val) {         \
        cli_out_error("Test failed"); \
    }                                 \
    tarman_tests_work = tarman_tests_work && tarman_test_##val;

#define TARMAN_TEST_END()                          \
    if (!tarman_tests_work) {                      \
        cli_out_error("One or more tests failed"); \
        return EXIT_FAILURE;                       \
    }                                              \
    cli_out_success("All tests passed");           \
    return EXIT_SUCCESS;

// static bool test_tar(void) {
//   cli_out_progress("Testing tar extraction...");
//   return archive_tar_extract("extract", "archive.tar");
// }

static bool test_dypath(void) {
    cli_out_progress("Testing dypath...");
    char *path;
    os_fs_tm_dyhome(&path);
    printf("%s\n", path);
    return true;
}

static bool test_dirs(void) {
    cli_out_progress("Testing dir creation...");
    if (TM_FS_DIROP_STATUS_OK != os_fs_mkdir("test")) {
        return false;
    }

    cli_out_progress("Testing dir enumeration...");
    size_t count;
    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_count(&count, "test")) {
        return false;
    }
    printf("Count: %ld, expected: %d\n", count, 0);
    if (0 != count) {
        return false;
    }

    cli_out_progress("Testing dir deletion...");
    if (TM_FS_DIROP_STATUS_OK != os_fs_dir_rm("test")) {
        return false;
    }

    return true;
}

static bool test_init(void) {
    cli_out_progress("Testing fs init...");
    return os_fs_tm_init();
}

int cli_cmd_test(cli_info_t info) {
    (void)info;
    TARMAN_TEST_INIT();
    // TARMAN_TEST(test_tar);
    TARMAN_TEST(test_init);
    TARMAN_TEST(test_dypath);
    TARMAN_TEST(test_dirs);
    TARMAN_TEST_END();
}
