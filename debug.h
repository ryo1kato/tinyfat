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
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#define    LOG_EMERG     0  //system is unusable
#define    LOG_ALERT     1  //action must be taken immediately
#define    LOG_CRIT      2  //critical conditions
#define    LOG_ERR       3  //error conditions
#define    LOG_WARNING   4  //warning conditions
#define    LOG_NOTICE    5  //normal but significant condition
#define    LOG_INFO      6  //informational
#define    LOG_DEBUG     7  //debug-level messages

#ifndef COMPILE_IN_DEBUG_LEVEL
#define COMPILE_IN_DEBUG_LEVEL LOG_DEBUG
#endif

/*
 *
 */
#define __DEBUG_MSG(level, format, ...) \
    printf(format, ##__VA_ARGS__);


//   #!/bin/sh
//
//   levels='EMERG ALERT CRIT ERR WARNING NOTICE INFO DEBUG'
//
//   for level in $levels
//   do
//       echo "// $level"
//       echo "#if COMPILE_IN_DEBUG_LEVEL >= LOG_$level"
//
//       echo -n "#    define DEBUG_MSG_$level(format, ...)"
//       echo    "  DEBUG_MSG(LOG_$level, format, ##__VA_ARGS__);"
//
//       echo "#else"
//       echo "#    define DEBUG_MSG_$level(format, ...)  /* debug off */"
//       echo "#endif"
//    done

// EMERG
#if COMPILE_IN_DEBUG_LEVEL >= LOG_EMERG
#    define MSG_EMERG(format, ...)  __DEBUG_MSG(LOG_EMERG, "EMERG: " format, ##__VA_ARGS__);
#else
#    define MSG_EMERG(format, ...)  /* debug off */
#endif
// ALERT
#if COMPILE_IN_DEBUG_LEVEL >= LOG_ALERT
#    define MSG_ALERT(format, ...)  __DEBUG_MSG(LOG_ALERT, "ALERT: " format, ##__VA_ARGS__);
#else
#    define MSG_ALERT(format, ...)  /* debug off */
#endif
// CRIT
#if COMPILE_IN_DEBUG_LEVEL >= LOG_CRIT
#    define MSG_CRIT(format, ...)  __DEBUG_MSG(LOG_CRIT, "CRIT: " format, ##__VA_ARGS__);
#else
#    define MSG_CRIT(format, ...)  /* debug off */
#endif
// ERR
#if COMPILE_IN_DEBUG_LEVEL >= LOG_ERR
#    define MSG_ERR(format, ...)  __DEBUG_MSG(LOG_ERR, "ERROR: " format, ##__VA_ARGS__);
#else
#    define MSG_ERR(format, ...)  /* debug off */
#endif
// WARNING
#if COMPILE_IN_DEBUG_LEVEL >= LOG_WARNING
#    define MSG_WARNING(format, ...)  __DEBUG_MSG(LOG_WARNING, "WARNING: " format, ##__VA_ARGS__);
#else
#    define MSG_WARNING(format, ...)  /* debug off */
#endif
// NOTICE
#if COMPILE_IN_DEBUG_LEVEL >= LOG_NOTICE
#    define MSG_NOTICE(format, ...)  __DEBUG_MSG(LOG_NOTICE, format, ##__VA_ARGS__);
#else
#    define MSG_NOTICE(format, ...)  /* debug off */
#endif
// INFO
#if COMPILE_IN_DEBUG_LEVEL >= LOG_INFO
#    define MSG_INFO(format, ...)  __DEBUG_MSG(LOG_INFO, format, ##__VA_ARGS__);
#else
#    define MSG_INFO(format, ...)  /* debug off */
#endif
// DEBUG
#if COMPILE_IN_DEBUG_LEVEL >= LOG_DEBUG
#    define MSG_DEBUG(format, ...)  __DEBUG_MSG(LOG_DEBUG, format, ##__VA_ARGS__);
#else
#    define MSG_DEBUG(format, ...)  /* debug off */
#endif

#endif //DEBUG_H_INCLUDED

