/**
* Utility functions for watch faces.
*
* @file   utils.h
*
* @author Bob Hauck <bobh@haucks.org>
*
* Copyright (c) 2014, Bob Hauck
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*****************************************************************************/
#ifndef UTILS_H
#define UTILS_H


#include <pebble.h>


#define DEBUG   (true)  /* false saves ~2.5K of code/data size */

#if !DEBUG

#define LOG_MSG_DEBUG(m, ...)
#define LOG_MSG_INFO(m, ...)
#define LOG_MSG_WARNING(m, ...)
#define LOG_MSG_ERROR(m, ...)

#else

/** Macro to log a message in standard format.
*/
#define LOG_STD_MSG(priority, msg, ...) \
    app_log((priority), __FILE__, __LINE__, msg, ## __VA_ARGS__)

/** Some useful macros for capturing where an error happened.  Ordered
* from least to most critical.
*/
#define LOG_MSG_DEBUG(m, ...)   \
        LOG_STD_MSG(APP_LOG_LEVEL_DEBUG,   m, ## __VA_ARGS__)
#define LOG_MSG_INFO(m, ...)    \
        LOG_STD_MSG(APP_LOG_LEVEL_INFO,    m, ## __VA_ARGS__)
#define LOG_MSG_WARNING(m, ...) \
        LOG_STD_MSG(APP_LOG_LEVEL_WARNING, m, ## __VA_ARGS__)
#define LOG_MSG_ERROR(m, ...)   \
        LOG_STD_MSG(APP_LOG_LEVEL_ERROR,   m, ## __VA_ARGS__)

#endif


typedef struct _TimeMS
{
    time_t sec;
    uint16_t ms;
}
TimeMS;


/**
* Convert a string to upper case, in place.
*
* @param str    Pointer to the string to be converted.
*
* @return  Pointer to the converted string.
*****************************************************************************/
char *upcase(char *str);


/**
* Convert a string to lower case, in place.
*
* @param str    Pointer to the string to be converted.
*
* @return  Pointer to the converted string.
*****************************************************************************/
char *dncase(char *str);


/**
* Adds a - b, puts result in c.
*
* @param a      Pointer to the first addend.
* @param b      Pointer to the second addend.
* @param c      Pointer to result, which is allowed to be a or b.
*
*****************************************************************************/
void time_sum(TimeMS *c, TimeMS *a, TimeMS *b);


/**
* Subtracts a - b, puts result in c.
*
* @param a      Pointer to the minuend
* @param b      Pointer to the subtrahend
* @param c      Pointer to result, which is allowed to be a or b.
*
*****************************************************************************/
void time_diff(TimeMS *c, TimeMS *a, TimeMS *b);


#endif  /* include guard */
