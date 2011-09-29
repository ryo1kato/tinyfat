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
#include <string.h>

#include "debug.h"
#include "tinyfat.h"


#define SIZEOF_LBA_BLOCK 512

/* uint8_t[] { 0xaa, 0xbb, 0xcc, 0xdd, } -> uint32_t 0xddccbbaa */

/*--- boot partition block ---*/
/* #define BPB_JUMP_CODE(bpb) unused */
#define BPB_TITLE(bpb) ((char*)( &((uint8_t*)(bpb))[0x03] ))
#define BPB_BYTE_PER_SECTOR(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x0C] )) <<8) + \
                                  ( ((uint32_t)( ((uint8_t*)(bpb)) [0x0B] )) <<0))
#define BPB_SECTOR_PER_CLUSTER(bpb) ( ((uint32_t)( ((uint8_t*)(bpb)) [0x0D] )))
#define BPB_NOOF_BOOT_SECTOR(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x0F] )) <<8) + \
                                   ( ((uint32_t)( ((uint8_t*)(bpb)) [0x0E] )) <<0))
#define BPB_NOOF_FAT(bpb) ( ((uint32_t)( ((uint8_t*)(bpb)) [0x10] )))
#define BPB_NOOF_DIR_ENTRY(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x12] )) <<8) + \
                                 ( ((uint32_t)( ((uint8_t*)(bpb)) [0x11] )) <<0))
#define BPB_NOOF_SECTOR(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x14] )) <<8) + \
                              ( ((uint32_t)( ((uint8_t*)(bpb)) [0x13] )) <<0))
#define BPB_MEDIA_DESCRIPTOR(bpb) ( ((uint32_t)( ((uint8_t*)(bpb)) [0x15] )))
#define BPB_SECTOR_PER_FAT(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x17] )) <<8) + \
                                 ( ((uint32_t)( ((uint8_t*)(bpb)) [0x16] )) <<0))
/* #define BPB_SECTOR_PER_TRACK(bpb) unused */
/* #define BPB_NOOF_HEAD(bpb) unused */
/* #define BPB_NOOF_BYGONE_SECTOR(bpb) unused */
#define BPB_NOOF_SECTOR_ON_BIG(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x23] )) <<24) + \
                                     ( ((uint32_t)( ((uint8_t*)(bpb)) [0x22] )) <<16) + \
                                     ( ((uint32_t)( ((uint8_t*)(bpb)) [0x21] )) <<8) + \
                                     ( ((uint32_t)( ((uint8_t*)(bpb)) [0x20] )) <<0))
/* #define BPB_PHYSICAL_DRIVE_ID(bpb) unused */
/* #define BPB_RESERVED(bpb) unused */
#define BPB_ENHANCED_RECORD_SIGN(bpb) ( ((uint32_t)( ((uint8_t*)(bpb)) [0x26] )))
/* #define BPB_VOLUME_SERIAL_NO(bpb) unused */
/* #define BPB_VOLUME_LABEL(bpb) unused */
/* #define BPB_FAT_TYPE(bpb) unused */
/* #define BPB_BOOT_CODE_AREA(bpb) unused */
#define BPB_SIGN(bpb) (( ((uint32_t)( ((uint8_t*)(bpb)) [0x1FF] )) <<8) + \
                       ( ((uint32_t)( ((uint8_t*)(bpb)) [0x1FE] )) <<0))

/*--- directory entry ---*/
#define DIR_NAME(dir) ((char*)( &((uint8_t*)(dir))[0x0] ))
#define DIR_EXTENSION(dir) ((char*)( &((uint8_t*)(dir))[0x8] ))
#define DIR_ATTRIBUTE(dir) ( ((uint32_t)( ((uint8_t*)(dir)) [0xB] )))
/* #define DIR_RESERVED(dir) unused */
/* #define DIR_CREATE_TIME_mS(dir) unused */
/* #define DIR_CREATE_TIME(dir) unused */
/* #define DIR_CREATE_DATE(dir) unused */
/* #define DIR_ACCESS_DATE(dir) unused */
/* #define DIR_CLUSTER_HIGH(dir) unused */
/* #define DIR_UPDATE_TIME(dir) unused */
/* #define DIR_UPDATE_DATE(dir) unused */
#define DIR_START_CLUSTER(dir) (( ((uint32_t)( ((uint8_t*)(dir)) [0x1B] )) <<8) + \
                                ( ((uint32_t)( ((uint8_t*)(dir)) [0x1A] )) <<0))
#define DIR_SIZEOF_FILE(dir) (( ((uint32_t)( ((uint8_t*)(dir)) [0x1F] )) <<24) + \
                              ( ((uint32_t)( ((uint8_t*)(dir)) [0x1E] )) <<16) + \
                              ( ((uint32_t)( ((uint8_t*)(dir)) [0x1D] )) <<8) + \
                              ( ((uint32_t)( ((uint8_t*)(dir)) [0x1C] )) <<0))

#define SIZEOF_DIR_ENTRY 32

static int (*read_func)(uint32_t lba, int noof, uint8_t* data) = NULL;
static uint32_t lba_fs_top = 0;

static ssize_t getsize_or_loadfile(const char* srcfilename, void* dest, size_t limit, int load_to_dest);
static int get_filename(uint8_t* dir_entry, char buff[8+1+3+1]); /* buffsize == name + . + extension + '\0' */

int fat_init(read_callback func, uint32_t lba_top)
{
    int ret = 0;
    if(read_func == NULL){
        read_func = func;
        lba_fs_top = lba_top;
        ret = 1;
    }
    return ret;
}

void fat_term(void)
{
    read_func = NULL;
    lba_fs_top = 0;
}

ssize_t fat_getsize(const char* srcfilename)
{
    return getsize_or_loadfile(srcfilename, NULL, 0, 0);
}

ssize_t fat_loadfile(const char* srcfilename, void* dest, size_t limit)
{
    return getsize_or_loadfile(srcfilename, dest, limit, 1);
}

ssize_t getsize_or_loadfile(const char* srcfilename, void* dest, size_t limit, int load_to_dest)
{
    int i;
    uint8_t* destaddr;
    static uint8_t readbuff[SIZEOF_LBA_BLOCK];
    uint32_t lba_cur = 0;
    uint32_t lba_fat_top;
    uint32_t lba_direntry;
    uint32_t lba_data;
    int sector_per_cluster;
    int noof_dir_entry;
    uint32_t noof_cluster;
    uint8_t* dir_entry_cur;
    char filename_cur[8+1+3+1];
    uint32_t cluster_cur;
    uint32_t sizeof_file;
    uint32_t sizeof_read;

    /* for fast loading */
    static uint8_t fat_cache[SIZEOF_LBA_BLOCK*2];
    uint32_t lba_fat_last;

    if(read_func == NULL) {
        MSG_ERR("FAT: not registered read function\n");
        return -1;
    }

    if((*read_func)(lba_fs_top, 1, readbuff) < 0) {
        MSG_ERR("FAT: can not read BPB. LBA = 0x%08x\n", lba_fs_top);
        return -1;
    }

    if(BPB_SIGN(readbuff) != 0xAA55){
        MSG_ERR("FAT: invalid BPB\n");
        return -1;
    }

#if 0
    char str[9];
    MSG_DEBUG("FAT boot partition block\n");
    strncpy(str, BPB_TITLE(readbuff), 8); str[8] = '\0';
    MSG_DEBUG("  title                  : %s\n",     str);
    MSG_DEBUG("  byte / sector          : 0x%08x\n", BPB_BYTE_PER_SECTOR(readbuff));
    MSG_DEBUG("  sector / cluster       : 0x%08x\n", BPB_SECTOR_PER_CLUSTER(readbuff));
    MSG_DEBUG("  no of boot sector      : 0x%08x\n", BPB_NOOF_BOOT_SECTOR(readbuff));
    MSG_DEBUG("  no of FAT              : 0x%08x\n", BPB_NOOF_FAT(readbuff));
    MSG_DEBUG("  no of directory entry  : 0x%08x\n", BPB_NOOF_DIR_ENTRY(readbuff));
    MSG_DEBUG("  no of sector           : 0x%08x\n", BPB_NOOF_SECTOR(readbuff));
    MSG_DEBUG("  media descriptor       : 0x%08x\n", BPB_MEDIA_DESCRIPTOR(readbuff));
    MSG_DEBUG("  sector / FAT           : 0x%08x\n", BPB_SECTOR_PER_FAT(readbuff));
    MSG_DEBUG("  no of sector over 32MB : 0x%08x\n", BPB_NOOF_SECTOR_ON_BIG(readbuff));
    MSG_DEBUG("  enhanced record sign   : 0x%08x\n", BPB_ENHANCED_RECORD_SIGN(readbuff));
    MSG_DEBUG("  FAT type               : %s\n",     str);
#endif

    lba_fat_top = lba_fs_top + BPB_NOOF_BOOT_SECTOR(readbuff);
    lba_direntry = lba_fat_top + (BPB_SECTOR_PER_FAT(readbuff) * BPB_NOOF_FAT(readbuff));
    noof_dir_entry = BPB_NOOF_DIR_ENTRY(readbuff);
    sector_per_cluster = BPB_SECTOR_PER_CLUSTER(readbuff);
    lba_data = lba_direntry + (noof_dir_entry / (SIZEOF_LBA_BLOCK / SIZEOF_DIR_ENTRY));

    if(sector_per_cluster <= 0) {
        MSG_ERR("FAT : invalid FAT format\n");
        MSG_ERR("  Top LBA                : 0x%08x\n", lba_fs_top);
        MSG_ERR("  byte / sector          : 0x%08x\n", BPB_BYTE_PER_SECTOR(readbuff));
        MSG_ERR("  sector / cluster       : 0x%08x\n", BPB_SECTOR_PER_CLUSTER(readbuff));
        MSG_ERR("  no of boot sector      : 0x%08x\n", BPB_NOOF_BOOT_SECTOR(readbuff));
        MSG_ERR("  no of FAT              : 0x%08x\n", BPB_NOOF_FAT(readbuff));
        MSG_ERR("  no of directory entry  : 0x%08x\n", BPB_NOOF_DIR_ENTRY(readbuff));
        MSG_ERR("  no of sector           : 0x%08x\n", BPB_NOOF_SECTOR(readbuff));
        MSG_ERR("  media descriptor       : 0x%08x\n", BPB_MEDIA_DESCRIPTOR(readbuff));
        MSG_ERR("  sector / FAT           : 0x%08x\n", BPB_SECTOR_PER_FAT(readbuff));
        MSG_ERR("  no of sector over 32MB : 0x%08x\n", BPB_NOOF_SECTOR_ON_BIG(readbuff));
        MSG_ERR("  enhanced record sign   : 0x%08x\n", BPB_ENHANCED_RECORD_SIGN(readbuff));
        return -1;
    }
    noof_cluster = ((((BPB_NOOF_SECTOR(readbuff) != 0) ? BPB_NOOF_SECTOR(readbuff) : BPB_NOOF_SECTOR_ON_BIG(readbuff)) - lba_data) / sector_per_cluster) + 2;

    lba_cur = 0;
    cluster_cur = 0;

    for(i = 0; i < noof_dir_entry; i++) {
        if(lba_cur < lba_direntry + ((SIZEOF_DIR_ENTRY*i)/SIZEOF_LBA_BLOCK)) {
            lba_cur = lba_direntry + ((SIZEOF_DIR_ENTRY*i)/SIZEOF_LBA_BLOCK);
            if((*read_func)(lba_cur, 1, readbuff) < 0) {
                MSG_ERR("FAT: can not read DIR ENTRY. LBA = 0x%08x\n", lba_cur);
                return -1;
            }
        }

        dir_entry_cur = &(readbuff[SIZEOF_DIR_ENTRY * (i%(SIZEOF_LBA_BLOCK/SIZEOF_DIR_ENTRY))]);

        if(0 != get_filename(dir_entry_cur, filename_cur)) {
#if 0
            MSG_DEBUG("directory %d\n", i);
            MSG_DEBUG("  filename         : %s\n", filename_cur);
            MSG_DEBUG("  start cluster no : 0x%08x\n", DIR_START_CLUSTER(dir_entry_cur));
            MSG_DEBUG("  size of file     : 0x%08x\n", DIR_SIZEOF_FILE(dir_entry_cur));
            MSG_DEBUG("\n");
#endif
            if(0 == strcasecmp(srcfilename, filename_cur)) {
                cluster_cur = DIR_START_CLUSTER(dir_entry_cur);
                sizeof_file = DIR_SIZEOF_FILE(dir_entry_cur);
                break;
            }
        }
    }

    if(cluster_cur == 0) {
        MSG_ERR("FAT: file is not found \"%s\"\n", srcfilename);
        return -1;
    }

    /* get size only or load file to dest */
    if(load_to_dest == 0) {
        return sizeof_file;
    }

    if(dest == NULL) {
        MSG_ERR("FAT: invalid destination address NULL\n");
        return -1;
    }

    if(limit < sizeof_file) {
        sizeof_file = limit; /* limitter */
    }

#define loaded_size ((uint32_t)destaddr - (uint32_t)dest)
#define sizeof_remain (sizeof_file - loaded_size)
#define sizeof_cluster (sector_per_cluster * SIZEOF_LBA_BLOCK)

    lba_fat_last = 0;

    destaddr = (uint8_t*)dest;
    while(loaded_size < sizeof_file) {
        /* load cluster to data */
        lba_cur = lba_data + ((cluster_cur-2) * sector_per_cluster);

        if(sizeof_cluster <= sizeof_remain){
            if((*read_func)(lba_cur, sector_per_cluster, destaddr) < 0) {
                MSG_ERR("FAT: can not read DATA. LBA = 0x%08x\n", lba_cur);
                return -1;
            }
            destaddr += sizeof_cluster;
        }
        /* find end of file, read size of single LBA block */
        else {
            for(i = 0; i < sector_per_cluster; i++) {

                if((*read_func)(lba_cur, 1, readbuff) < 0) {
                    MSG_ERR("FAT: can not read DATA. LBA = 0x%08x\n", lba_cur);
                    return -1;
                }
                if( SIZEOF_LBA_BLOCK <= sizeof_remain ) {
                    sizeof_read = SIZEOF_LBA_BLOCK;
                }
                else {
                    sizeof_read = sizeof_remain;
                }
                memcpy(destaddr, readbuff, sizeof_read);
                destaddr += sizeof_read;
                lba_cur++;

            }
        }
        /* get next cluster */
        if(noof_cluster <= 0xFF6) {
            /* FAT12 */
            lba_cur = lba_fat_top + (((cluster_cur * 3) / 2) / SIZEOF_LBA_BLOCK);
            if(lba_fat_last != lba_cur) {
                /* update file allocation table */
                if((*read_func)(lba_cur, 2, fat_cache) < 0) {
                    MSG_ERR("FAT: can not read FAT. LBA = 0x%08x\n", lba_cur);
                    return -1;
                }
                lba_fat_last = lba_cur;
            }

            if((cluster_cur & 0x001) == 0) {
                cluster_cur = ( (uint32_t)(fat_cache[(((cluster_cur * 3) / 2) % SIZEOF_LBA_BLOCK)+1] << 8) +
                                (uint32_t)(fat_cache[(((cluster_cur * 3) / 2) % SIZEOF_LBA_BLOCK)]) ) & 0xFFF;
            }
            else {
                cluster_cur = (( (uint32_t)(fat_cache[(((cluster_cur * 3) / 2) % SIZEOF_LBA_BLOCK)+1] << 8) +
                                 (uint32_t)(fat_cache[(((cluster_cur * 3) / 2) % SIZEOF_LBA_BLOCK)]) ) & 0xFFF0) >> 4;
            }

        }
        else {
            /* FAT16 */
            lba_cur = lba_fat_top + ((cluster_cur * 2) / SIZEOF_LBA_BLOCK);
            if(lba_fat_last != lba_cur) {
                /* update file allocation table */
                if((*read_func)(lba_cur, 1, fat_cache) < 0) {
                    MSG_ERR("FAT: can not read FAT. LBA = 0x%08x\n", lba_cur);
                    return -1;
                }
                lba_fat_last = lba_cur;
            }

            cluster_cur = ( (uint32_t)(fat_cache[((cluster_cur * 2) % SIZEOF_LBA_BLOCK)+1] << 8) +
                            (uint32_t)(fat_cache[((cluster_cur * 2) % SIZEOF_LBA_BLOCK)]) );

        }

        if(noof_cluster < cluster_cur){
            /* next cluster is not exist */
            break;
        }
    }

    return (ssize_t)loaded_size;
}

int get_filename(uint8_t* dir_entry, char buff[8+1+3+1])
{
    char* curpos;
    char* chkterm;

    if(DIR_ATTRIBUTE(dir_entry) == 0x0f) {
        return 0; /* long file name is not support */
    }
    if((DIR_ATTRIBUTE(dir_entry) & 0x1E) != 0) {
        return 0; /* attribute is not file */
    }

    curpos = buff;

    strncpy( curpos, DIR_NAME(dir_entry), 8);
    if( (('A' <= curpos[0]) && (curpos[0] <= 'Z')) ||
        (('a' <= curpos[0]) && (curpos[0] <= 'z')) ){

        curpos[8] = '\0';
        chkterm = strchr(curpos, ' ');

        if(chkterm != NULL){
            curpos = chkterm;
        }
        else {
            curpos = &(curpos[8]);
        }

        /* extension exist */
        if((DIR_EXTENSION(dir_entry))[0] != ' ') {
            *curpos = '.';
            curpos++;

            strncpy(curpos, DIR_EXTENSION(dir_entry), 3);

            curpos[3] = '\0';
            chkterm = strchr(curpos, ' ');
            if(chkterm != NULL){
                curpos = chkterm;
            }
            else {
                curpos = &(curpos[3]);
            }
        }

        *curpos = '\0';
    }
    else {
        return 0; /* file name is invalid */
    }

    return 1;
}
