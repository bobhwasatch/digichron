/****************************************************************************/
/**
* Generic structure for a watch face. The init() function for each specific
* type of face should return one of these with the data filled in.
*
* @file   face.h
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
#ifndef FACE_H
#define FACE_H


#include <pebble.h>


typedef struct Face Face;       /* forward declaration... */


struct Face
{
    /* These are required. Every face must have one of each.
    */
    void (*load_handler)(Face *);
    void (*unload_handler)(Face *);
    void (*update_handler)(Face *, struct tm *, TimeUnits);

    /* These are optional. Set to NULL if not used. Return false to perform
    * the default action (i.e. the face didn't handle it).
    */
    bool (*click_up)(Face *, uint8_t);
    bool (*click_dn)(Face *, uint8_t);
    bool (*click_sel)(Face *);

    bool (*click_long_up)(Face *);
    bool (*click_long_dn)(Face *);
    bool (*click_long_sel)(Face *);

    /* Shut up that damn alarm that's sounding (optional).
    */
    void (*shut_up)(Face *);

    /* Required face data.
    */
    GRect bounds;
    char name[8];
    uint32_t key;

    /* This can be used to store private data.
    */
    unsigned long data[0];
};


#endif  /* include guard */