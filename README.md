# Lilith Modula-2 Machine Disk Utility
This is a disk utility to create image files in Lilith Machine Format (compatible with e.g. Jos Dreesen's **Emulith** Lilith Emulator). Also, this tool can import and export files from/to a Lilith image file.

Work in progress. Not in a usable state yet!

## Compiling And Installation
1. Download the .tar.gz packages from the "[Releases](https://github.com/ghoss/m2disk/releases)" page.
2. Extract and build:
    ```
    $ tar xzf m2disk-x.y.tar.gz
    $ cd m2disk-x.y
    $ ./configure
    $ make && make install
    ```

## Usage
```
USAGE: m2disk [-Vvlxhfic] [-d dest_dir] img_file [file_arg|files]

-l	List directory of img_file
	If file_arg is omitted: list all entries
	otherwise, list files matching regex in file_arg

-c	Create and format new (empty) image file as img_file
-i	Import specified files into img_file
-x	Extract files matching file_arg from img_file
-d	Extract into destination 'dest_dir' (must already exist)

-f	Force mode (overwrites existing files and images)
-t	Convert text file EOL characters (Lilith<->Unix)
-v	Verbose output
-h	Show this help information
-V	Show version information

img_file is the filename of a disk image of an
original Lilith Modula-2 machine.
```

## Examples
* ```m2disk```

  (To be updated)