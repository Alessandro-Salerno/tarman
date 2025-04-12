# Plugins
Plugins allow users and developers to customze and extend tarman's features in a few simple steps. Plugins are simple executables that are launched on-demande by tarman based on their name.

## Usecases
Plugins can be used, for example, to add support for other archive formats other than the `tar` family. They can also be used to provide a different implementation for these same archive types (e.g., using `libarchive` directly instead of calling GNU tar). Plugins can also be used to facilitate customization thanks to their associated configuration file. 

Generally, plugins are used to perform some kind of transformation such as downloading a file, extracting an archive, etc.

## Plugin API
Plugins are launched by tarman with three command-line arguments in this order:
- Source: the origin of the transformation (e.g., path to an archive file)
- Destination: the destination of the transformation (e.g., path to the destination directory for an archive extraction)
- Configuration: path to the `txt` config file for this plugin

Plugins are expected to return an integer exit code equal to the system's `EXIT_SUCCESS` upon successful completion and equal to the system's `EXIT_FAILURE` otherwise. Plugins are expected **NOT** to perform any I/O operation outside interacting with the configuration file and the subjects of the transformation. In particular, plugins are expected not to read from `stdin` or write to `stdout` and `stderr`. 

### Writing a plugin
A basic SDK is provided in the tarman source tree in [src/plugin-sdk](../src/plugin-sdk). The loader expects to be linked with one or more other files (wether source files, object files, static library archives, etc.) that provide a definition for the `plugin_main` symbol that takes a pointer to `sdk_handover_t` as parameter and returns an integer value.

Following is an example of a plugin that writes "Hello, world!" to the destination file:
```c
#include <stdlib.h>
#include <stdio.h>

#include "plugin/sdk.h"

int plugin_main(sdk_handover_t *handover) {
    FILE *fp = fopen(handover->dst, "w");

    if (NULL == fp) {
        return EXIT_FAILURE;
    }

    fprintf(fp, "Hello, world!\n");
    fclose(fp);

    return EXIT_SUCCESS;
}
```

The SDK is in very early development and most features are not available yet. Documentation on the SDK is provided directly in ther SDK header file at [include/plugin/sdk.h](../include/plugin/sdk.h).

### Executable names
The name of the executable for a tarman plugin is **EXTREMELY** important. The following names are reserved for special features (OS-specific file extensions are excluded):

| Name              | Description                                  | Source | Destination                          |
| ----------------- | -------------------------------------------- | ------ | ------------------------------------ |
| `download-plugin` | Used by tarman to download a file from a URL | URL    | Destination file on host file system | 

Plugins with reserved names **CAN** be overriden. In fact, the reserved name is used to easily allow users to change the implementation they use. For example, to change the download handler, one only has to override the `download-plugin` file.

Plugins that do not match any reserved name are invoked if they match the file extension of a package to be installed. For example, a plugin with `zip` as the name of its executable will be used to extract packages that use the `.zip` format.

### Structure of the `plugins/` directory
The [plugins/](../plugins/) directory contains the source code for all built-in tarman plugins. Built-in plugins are provided to users directly upon installation depending on their platofrm.

This directory contains one subdirectory for each built-in plugin. Each subdirectory is independent and can be strucutred as desired by the developers as long as it caontains a `Makefile` with a valid first target that can be invoked by the main `Makefile` when compiling the entire project. Plugin Makefiles can be as simple as:
```Makefile
$(DIST)/myplugin: plugin.c
	@$(CC) plugin.c $(SDK) $(SDK_FLAGS) -o $(DIST)/myplugin
```

Some Make variables are provided to plugin makefiles by the main one

| Variable    | Description                                                              |
| ----------- | ------------------------------------------------------------------------ |
| `DIST`      | Directory meant to contain all plugin binaries (i.e., `bin/plugins`)     |
| `CC`        | C Compiler used to compile the whole project                             |
| `SDK`       | SDK object file (i.e., `bin/plugin-sdk.o`)                               |
| `SDK_FLAGS` | Compiler flags used to compile the SDK, likely needed for the plugin too |

## Plugins as tarman packages
> [!WARNING]
> This feature is not fully implemented!

Plugins can be distributed and installed as tarman packages. To add the executable inside a tarman package as a plugin, one only has to add this line to the package's recipe:
```
ADD_TO_TARMAN=true
```
If the recipe does not specify this line or if no recipe is provided, the `-t` and `--add-tarman` command-line options can be used during installation.
