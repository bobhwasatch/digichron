/****************************************************************************/
/**
* Definitions for a watch face that sets an alarm.
*
* @file   alarm.c
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
#include "display.h"
#include "utils.h"
#include "watch.h"


typedef struct _Private
{
    int day_flag;               /* TRUE displays day instead of date */
    int force_day_update;       /* Forces day/date update after seconds */
} Private;


static bool click_sel(Face *face)
{
    return 0;
}


static void load_handler(Face *face)
{
    time_t t = 0;
    struct tm *zero = gmtime(&t);

    display_set_time(zero, 0xff, 0);
    display_set_title("ALM");
}


static void unload_handler(Face *face)
{
    display_clear();
}


static void update_handler(Face *face, struct tm *tt, TimeUnits uc)
{
}


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*
* @return  Pointer to the face, or NULL on error.
*****************************************************************************/
Face *alarm_create(void)
{
    Face *face = malloc(sizeof(Face) + sizeof(Private));

    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    if (face)
    {
        memset(face, 0, sizeof(Face) + sizeof(Private));

        face->load_handler = load_handler;
        face->unload_handler = unload_handler;
        face->update_handler = update_handler;
        face->click_sel = click_sel;
    }

    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
    return face;
}


/**
* Destroy the watch face. De-allocate memory and release resources.
*****************************************************************************/
void alarm_destroy(Face *face)
{
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    free(face);
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}

