/**
* Utility functions for watch faces.
*
* @file   utils.c
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
#include <ctype.h>
#include "utils.h"


/**
* Convert a string to upper case, in place.
*****************************************************************************/
char *upcase(char *str)
{
    char *s = str;

    while (*s)
    {
        *s++ = toupper((int)*s);
    }

    return str;
}


/**
* Convert a string to lower case, in place.
*****************************************************************************/
char *dncase(char *str)
{
    char *s = str;

    while (*s)
    {
        *s++ = tolower((int)*s);
    }

    return str;
}


/**
* Adds a - b, puts result in c.
*****************************************************************************/
void time_sum(TimeMS *c, TimeMS *a, TimeMS *b)
{
    long sec = (long)a->sec + (long)b->sec;
    long ms = (long)a->ms + (long)b->ms;

    while (ms > 1000)
    {
        sec++;
        ms -= 1000;
    }

    c->sec = sec;
    c->ms = ms;
}


/**
* Subtracts a - b, puts result in c.
*****************************************************************************/
void time_diff(TimeMS *c, TimeMS *a, TimeMS *b)
{
    long sec = (long)a->sec - (long)b->sec;
    long ms = (long)a->ms - (long)b->ms;

    while (ms < 0)
    {
        sec--;
        ms += 1000;
    }

    c->sec = sec;
    c->ms = ms;
}

