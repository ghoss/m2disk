# Lilith Modula-2 Machine Disk Utility
This is a disk utility to create image files in Lilith Machine Format (compatible with e.g. Jos Dreesen's **Emulith** Lilith Emulator). Also, this tool can import and export files from/to a Lilith image file.

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
-p	List page tables of files matching file_arg
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
* ```m2disk -c test.img```

  Create the empty Lilith disk image ```test.img```. NOTE: This creates directory entries for the mandatory reserved system files such as *FS.DumpFile*, *FS.BootFile* etc., but does **NOT** import them automatically. You must do this yourself via the ```-i``` function in order to make a bootable disk.

* ```m2disk -cf test.img```

  Same as above, but overwrite the image file if it already exists.

* ```m2disk -l test.img```

  List the contents of the image file ```test.img```.

* ```m2disk -x test.img '*.DEF'```

  Export all Lilith files ending in "*.DEF" from the image ```test.img``` to the current host directory. No text conversion is performed. NOTE: Wildcard file arguments as in the example must be enclosed in single quotes to avoid shell expansion.

* ```m2disk -xf test.img '*.DEF'```

  Same as above, but overwrite existing files with the same name(s) in the host directory.

* ```m2disk -xt -d testdir test.img InOut.MOD```

  Export the file named ```InOut.MOD``` from the image ```test.img``` to the target directory ```testdir``` and performs text conversion.

* ```m2disk -i test.img InOut.MOD```

  Import the file named ```InOut.MOD``` from the current directory into the image file ```test.img```. 

* ```m2disk -ift test.img InOut.MOD```

  Same as above, but perform text conversion and overwrite existing files with the same name(s) in the image. 

* ```m2disk -p test.img PC.BootFile```

  Displays the list of disk pages occupied by the file ```PC.BootFile``` in the image ```test.img```.