Copyright 2008 Sony Corporation

    tinyfat - An extremely simple FAT12/16 implementation


ABSTRACT
    Tinyfat is an extremely simple and feature-limited FAT12/16 implementation.
    You can use this to read a file from any FAT12/16 formatted Logical Block
    Addressing(LBA) devices (Or any thing that can emulate LBA access) with 512
    byte blocks.

    Though tinyfat has very limited set of functions, it should be enough for
    simple embedded device bootloader:

        * 512byte block size only.
        * Only FAT12/16 are supported (No FAT32 support).
        * Read-only support only for files on topmost directory.
        * Traditional 8.3 filename support only.

    Tinyfat does not require so-called 'block device' layer, and thus,
    using tinyfat is extremely easy.


LICENSE
    Tinyfat is dual licensed under the GPLv2 and BSD-like license
    (without advertisement clause).  For the full statements of
    the GPLv2 license, see LICENSE.GPL files.


FILES
    README.TXT - This file
    tinyfat.c  - Main implementation of tinyfat.
    tinyfat.h  - A header file for tinyfat.
    debug.h    - A printf() wrapper.
    main.c     - Sample code for tinyfat usage.


API
    typedef int (*read_callback)(uint32_t start, int blocks, uint8_t* buf);
    int fat_init(read_callback func, uint32_t lba_top);

        This function initializes the tinyfat with 'func', a read callback
        function, and 'lba_top', the block count of where the FAT partiton
        starts in the device.  For example, for the first DOS/MBR partion,
        'lba_top' would be 1.

        You have to prepare a 'read_callback' function for your device.
        The callback function must take the following three arguments,
        read a specified region of data from device, copy it into the buffer
        'buf', and read the number of blocks actually read.

            start  - Block offset from the where.
            blocks - Number of blocks to read.
            buf    - Where to copy the read data.

        Currently, returning number less than requested (like POSIX read(2)
        system call) in the callback function does not work.  Thus, the
        callback function must return error (negative value) when exactly
        requested number of blocks are unavailable.

        You have to call this function only once, before you call
        fat_getsize() or fat_loadfile().
        Returns a negative value on error.


    ssize_t fat_getsize(const char* filename);

        This function returns the size of file named 'filename' in bytes.
        Returns a negative value on error.


    ssize_t fat_loadfile(const char* filename, void* dest, size_t limit);

        This function copys the content of a file 'filename' to a caller-
        prepared buffer 'dest' with a size limit of 'limit' in bytes.
        Returns a size of loaded data in bytes.
        Returns a negative value on error.

    void fat_term(void);

        This function terminates tinyfat and releases internally allocated data.


SAMPLE CODE
    Just type 'make' to compile.  You need a gcc compiler.

    If you have FAT12/16 partiton as '/dev/sda1', the follwing command-line
    sequence will give you an idea how it works.

        $ ./tinyfat /dev/sdc1 README.TXT
        README.TXT: 3554bytes
        $ ./tinyfat -r /dev/sdc1 README.TXT > README.TXT
        $ ls -l README.TXT
        -rw-r--r-- 1 rook rook 3554 Mar  8 18:54 README.TXT

    You can also run the command for FAT image file:

        $ dd if=/dev/zero of=fat.img count=$((1024*2*8)) #8MiB
        $ mkdosfs ./fat.img
        $ mcopy -i ./fat.img README.TXT ::/
        $ ./tinyfat ./fat.img README.TXT
        README.TXT: 3554bytes
        $ ./tinyfat -r ./fat.img README.TXT > README.TXT
        $ ls -l README.TXT
        -rw-r--r-- 1 rook rook 3554 Mar  8 18:54 README.TXT

