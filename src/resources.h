/****************************************************************************/
/**
* Global resources for the watch app.
*
* @file   resources.h
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
#ifndef RESOURCES_H
#define RESOURCES_H


#include <pebble.h>


typedef enum
{
    PERSIST_KEY_MAIN_STATE,
    PERSIST_KEY_WATCH_STATE,
    PERSIST_KEY_STW_STATE,
    PERSIST_KEY_TMR1_STATE,
    PERSIST_KEY_TMR2_STATE,
}
PersistKey;


extern GFont *res_font_large;
extern GFont *res_font_medium;
extern GFont *res_font_small;


/**
* Initialize resources. Must be called before any of the global resources
* are used.
*
* @return  Non-zero on error
*****************************************************************************/
int res_create(void);


/**
* Clean up global resources.
*****************************************************************************/
void res_destroy(void);


#endif  /* include guard */
