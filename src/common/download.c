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

#include "download.h"
#include <stdlib.h>

#include "os/exec.h"
#include "plugin/plugin.h"

bool download(const char *dst, const char *url) {
    if (plugin_exists("download-plugin")) {
        return EXIT_SUCCESS == plugin_run("download-plugin", dst, url);
    }

    return EXIT_SUCCESS == os_exec("curl", "-L", url, "-o", dst, NULL);
}
