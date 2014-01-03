/****************************************************************************/
/**
* Definitions for the main watch face that shows time & date.
*
* Titles:
*       MAIN    Initially shows when started or this face is selected,
*               replaced by day or date in 1-2 seconds.
* Buttons:
*       UP      Default action
*       SEL     Switches between date and day display.
*       DN      Default action
*
* @file   watch.c
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
    union
    {
        struct
        {
            bool day_flag;              /* TRUE displays day instead of date */
            int force_day_update;       /* Forces day/date update n seconds */

            /* Always add more at the end */
        };

        /* Force this thing to always be the same size so upgrades don't
        * screw up the persistence.
        */
        uint8_t reserved[128];
    };
} Private;


static bool click_sel(Face *face)
{
    Private *pvt = (Private *)face->data;

    pvt->day_flag ^= 1;
    pvt->force_day_update = true;
    LOG_MSG_DEBUG("pvt->day_flag=%d", pvt->day_flag);

    return 0;
}


static void load_handler(Face *face)
{
    Private *pvt = (Private *)face->data;
    time_t t = time(NULL);
    struct tm *tick_time = localtime(&t);
    TimeUnits units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

    /* Display the title for a moment before the next tick update
    * overwrites it.
    */
    display_set_time(tick_time, units_changed, 0);
    display_set_title(face->name);
    pvt->force_day_update = 1;
}


static void unload_handler(Face *face)
{
    display_clear();
}


static void update_handler(Face *face, struct tm *tt, TimeUnits uc)
{
    Private *pvt = (Private *)face->data;

    if ((uc & DAY_UNIT) || pvt->force_day_update == 0)
    {
        char date_string[10];

        if (pvt->day_flag)
        {
            strftime(date_string, sizeof(date_string), "%a", tt);
        }
        else
        {
            strftime(date_string, sizeof(date_string), "%b %d", tt);
        }

        display_set_title(upcase(date_string));
        pvt->force_day_update = -1;
    }

    display_set_time(tt, uc & ~DAY_UNIT, 0);

    if (pvt->force_day_update > 0)
    {
        pvt->force_day_update--;
    }
}


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*****************************************************************************/
Face *watch_create(const char *name, uint32_t key)
{
    Face *face = malloc(sizeof(Face) + sizeof(Private));

    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    if (face)
    {
        Private *pvt = (Private *)face->data;

        memset(face, 0, sizeof(Face) + sizeof(Private));

        face->load_handler = load_handler;
        face->unload_handler = unload_handler;
        face->update_handler = update_handler;
        face->click_sel = click_sel;

        strncpy(face->name, name, sizeof(face->name));
        face->name[sizeof(face->name) - 1] = 0;
        face->key = key;

        pvt->day_flag = persist_read_bool(face->key);
    }

    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
    return face;
}


/**
* Destroy the watch face. De-allocate memory and release resources.
*****************************************************************************/
void watch_destroy(Face *face)
{
    Private *pvt = (Private *)face->data;

    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    persist_write_bool(face->key, pvt->day_flag);
    free(face);
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}

