/****************************************************************************/
/**
* Definitions for a countdown timer. To the user it has two main modes,
* timer and setting. In timer mode, timers can be started, stopped, or
* cancelled. Setting mode is for configuring the countdown time. The
* maximum time is 23:59:59.
*
* Titles:
*       TMR     Static label.
*
* Buttons:
*       UP      In setting mode, increase the highlighted value. When
*               the timer is running, cancel it.
*       SEL     In timer mode, start and stop the countdown. In setting mode,
*               move the highlight to the next field.
*       L-SEL   Switch between timer and setting mode. Disabled when the
*               timer is running.
*       DN      In setting mode, decrease the highlighted value.
*
* @file   timer.h
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
#ifndef TIMER_H
#define TIMER_H


#include "face.h"


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*
* @param  name  Name of the face.
* @param  key   Storage key to use.
*
* @return  Pointer to the face, or NULL on error.
*****************************************************************************/
Face *timer_create(const char *name, uint32_t key);


/**
* Destroy the watch face. De-allocate memory and release resources.
*****************************************************************************/
void timer_destroy(Face *face);


#endif  /* include guard */
