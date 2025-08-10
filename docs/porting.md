# Porting
Tarman is designed to be easily portable to any Operating System that supports basic userspace functionality.

## Requirements
| Type      | Requirement                   | Details                                                                                |
| --------- | ----------------------------- | -------------------------------------------------------------------------------------- |
| Mandatory | `libc` port                   | Must support all `libc` features used in the source code                               |
| Mandatory | Disk file system support      | Must implement all functions in [include/os/fs.h](../include/os/fs.h)                  |
| Mandatory | Spawning processes            | Must implement all functions in [include/os/exec.h](../include/os/exec.h)              |
| Mandatory | Environment management        | Must implement all functions in [include/os/env.h](../include/os/env.h)                |
| Optional  | Querying the console/terminal | Shall implement `os_console_get_sz` in [include/os/console.h](../include/os/console.h) |
| Optional  | Changing console text color   | Shall implement all functions in [include/os/console.h](../include/os/console.h)       |
| Optional  | Network support               | Shall provide a plugin `download-plugin` that can download files from a URL            |

> [!IMPORTANT]
> Optional requirements for symbol definitions may still have to be implemented. This is to avoid linking issues with undefined symbols. Empty definitions for optional functions are provided in [src/os-common/no-optional](../src/os-common/no-optional).

## Providing concrete implementations
To port tarman to another OS, as detailed in the previous section, implementations must be provided for a range of tarman interface functions. Generally, implementations should be provided for all functions under the [include/os](../include/os/) directory (**excluding sudirectories**).

All symbols that rely on OS-specific implementations start with `os_` (e.g., `os_fs_path_len`). All functions that take a `va_list` as an argument have the last word of their name starting with a `v` (e.g., `os_fs_path_vlen`). All functions that perform dynamic memory allocations and return the allocated buffer to the caller use the first parameter (of type pointer-pointer, e.g., `void **`) to return the buffer pointer and have the last word of their name starting with `dy` (e.g., `os_fs_path_dyconcat`).

### Common implementations
Some platforms share specifications that describe how to interact with them. For example, both macOS (Darwin) and Linux implement the POSIX specification. In these cases, new symbols can be declared and defined in a new directory `include/os/$TARMAN_OSCOMMON/` (where `$TARMAN_OSCOMMON` is the name of the specification, e.g. `posix`). Implementations for the symbols declared in `include/os$TARMAN_OSCOMMON/*.h` shall be provided in `src/os-common/$TARMAN_OSCOMMON/*.c`.

All files under `src/os-common/$TARMAN_OSCOMMON` shall be structured as follows:
```c
// License here
// ...

#include <tm-os-defs.h>

// Other includes here
// ...

// Code here
```

**Example:** tarman implements POSIX support with symbols declared in [include/os/posix/](../include/os/posix/) that can then be called by OS-specific implementations. Implmeentations for all symbols declared by tarman's POSIX headers are found in [src/os-common/posix/](../include/os/posix/).

### Naming
OS ports must choose a name that reflects the name of the platform they're targetting. This name shall be all lowercase and will be referred to as `$TARMAN_OS` from here on in this document.

### Header files
OS ports must provide a header file named `tm-os-defs.h` under `include/os/$TARMAN_OS` if they wish to use common implementations such as POSIX. This header file is used to define macros/symbols that may be used by common implementations. For example, the `tm-os-defs.h` file for macOS is:
```c
#pragma once

#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif
```

### Source files
Implementations for OS-specific symbols shall be provided under `src/os-specific/$TARMAN_OS` as C source files. If the implementation makes use of common code, these files shall include the necessary headers (e.g., implementations that rely on POSIX for FS operations shall include `os/posix/fs.h` alongside `os/fs.h`).

### Build files
A Makefile must be provided in `src/os-specific/$TARMAN_OS/Makefile`. This file is used to include additional sources such as OS-common implementations. Following is an example taken from [src/os-specific/linux/Makefile](../src/os-specific/linux/Makefile):
```Makefile
SRC+=$(call rwildcard, src/os-common/posix, *.c) # Adds POSIX sources
```

The root Makefile provides a number of variables that OS-specific Makefiles can read and write to.

| Variable    | Access     | Description                                |
| ----------- | ---------- | ------------------------------------------ |
| `SRC`       | Read/Write | All C source files                         |
| `CFLAGS`    | Read/Write | Flags passed to the C Compiler             |
| `LDFLAGS`   | Read/Write | Flags passed to the C Compiler for linking |
| `rwildcard` | Execute    | Recursive wildcard function                |

> [!NOTE]
> OS-specific Makefiles can make use of files in [src/os-common/no-optional](../src/os-common/no-optional) by adding them to the `SRC` variable much like any other OS Common implemenation.
