/****************************************************************************/
/**
* Definitions for a stopwatch face that times intervals. Captures both
* split times (cumulative time for all laps since the start) and lap
* times (time since the start or last split, whichever is newer).
*
* Titles:
*       STW     Stopwatch mode. Ready to start timing or capture a split.
*       SPLT    Split mode. A split time is being displayed.
*       LAP     Lap mode. A lap time is being displayed.
*
* Buttons:
*       UP      In SPLT or LAP mode, switch to the other mode.
*       SEL     In STW mode:
*                       Starts timing or captures a split.
*               In SPLT or LAP mode:
*                       Returns to STW mode, resumes updating the display,
*                       ready to capture another split. Timing continues in
*                       the background while in SPLT or LAP mode.
*       L-SEL   In any mode, resets the stopwatch and stops timing.
*       DN      Not used, performs the default action.
*
* @file   stopwatch.h
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
#ifndef STOPWATCH_H
#define STOPWATCH_H


#include "face.h"


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*
* @param  name  Name of the face.
* @param  key   Storage key to use.
*
*
* @return  Pointer to the face, or NULL on error.
*****************************************************************************/
Face *stopwatch_create(const char *name, uint32_t key);


/**
* Destroy the watch face. De-allocate memory and release resources.
*****************************************************************************/
void stopwatch_destroy(Face *face);


#endif  /* include guard */
