/* With non GPL files, use following license */
/*
 * Copyright 2008 Sony Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* Otherwise with GPL files, use following license */
/*
 * Copyright 2008 Sony Corporation.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License.
 *
 * THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 * WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 * USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "tinyfat.h"

#define ERR(format, ...) \
    fprintf(stderr, "%s(): ERROR: " format "\n", __func__, ##__VA_ARGS__)


static const char HELP[] =
    "Usage: tinyfat [OPTION] FAT 8_3_FILENAME\n"
    "  Reads a file named '8_3_FILENAME' from FAT12/16 partiton 'FAT'.\n"
    "  Only traditional 8.3 file on the topmost directory is supported.\n"
    "  If a option is '-r'(read), the content of the file is written to\n"
    "  standard output.\n"
    "  If no option given, -s is the default\n"
    "\n"
    "OPTIONS\n"
    "  -h  Show this help.\n"
    "  -r  Read the file.\n"
    "  -s  Get the file size in bytes.\n"
;


static int lba_emulation_fd = -1;

int lba_read_emulation_start(char* filename)
{
    int fd = open(filename, O_RDONLY);
    if ( fd >= 0 ) {
        lba_emulation_fd = fd;
    }
    else {
        ERR("File open failed for %s (%s)", filename, strerror(errno));
    }
    return fd;
}

int lba_read_emulation(uint32_t lba_start, int lba_blocks, uint8_t* buf)
{
    off_t   lseek_ret;
    ssize_t read_ret;
    if ( lba_emulation_fd < 0 ) {
        ERR("not initialized yet");
        return -1;
    }

    lseek_ret = lseek(lba_emulation_fd, lba_start*512, SEEK_SET);
    if ( lseek_ret == (off_t)-1) {
        ERR("lseek failed for offset %d", lba_start*512);
        return -1;
    }

    read_ret = read(lba_emulation_fd, buf, lba_blocks*512);
    if ( read_ret != lba_blocks*512 ) {
        ERR("read() failed for %d, returned %d", lba_blocks*512, read_ret);
        return -1;
    }

    return lba_blocks;
}

void lba_read_emulation_end(void)
{
    close(lba_emulation_fd);
    lba_emulation_fd = -1;
    return;
}

enum {
    DO_READFILE,
    DO_GETSIZE,
};

int main(int argc, char** argv)
{
    char* fat_partition;
    char* filename_8_3;
    int whattodo;
    int argstart = 1;
    if ( argc < 1 ) {
        puts(HELP);
        exit(EXIT_FAILURE);
    }
    if ( argc >= 2 ) {
        if ( argv[1][0] == '-' ) {
            switch (argv[1][1]) {
                case 'r': whattodo = DO_READFILE; break;
                case 's': whattodo = DO_GETSIZE; break;
                case 'h': puts(HELP);
                    exit(EXIT_FAILURE);
                    break;
                default:
                    ERR("Unknown option: %s", argv[1]);
                    exit(EXIT_FAILURE);
            }
            argstart++;
        } else {
            whattodo = DO_GETSIZE;
        }
    }

    if (argc >= argstart + 2) {
        fat_partition = argv[argstart++];
        filename_8_3  = argv[argstart++];
    }
    else {
        ERR("Not enough argument. Use '-h' option for help.");
        exit(EXIT_FAILURE);
    }

    int rc;
    rc = lba_read_emulation_start(fat_partition);
    if ( rc < 0 ) {
        exit(EXIT_FAILURE);
    }

    rc = fat_init(&lba_read_emulation, 0);
    if ( rc < 0 ) {
        ERR("fat_init() returned error");
    }

    switch (whattodo) {
        case DO_GETSIZE:
            {
                int size = fat_getsize(filename_8_3);
                if ( size < 0 ) {
                    ERR("");
                } else {
                    printf("%s: %dbytes\n", filename_8_3, size);
                }
            }
            break;
        case DO_READFILE:
            {
                int size = fat_getsize(filename_8_3);
                if ( size < 0 ) {
                    ERR("Failed to get filesize for %s", filename_8_3);
                    exit(EXIT_FAILURE);
                }
                char* buf = malloc(size);
                int loadsize = fat_loadfile(filename_8_3, buf, size);
                write(1, buf, loadsize); /* just write to stdout */
                free(buf);
            }
            break;
        default:
            break;
    }

    fat_term();

    return 0;
}
