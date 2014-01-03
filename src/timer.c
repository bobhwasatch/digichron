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
* @file   timer.c
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


#define MAX_TIME        ((23 * 3600) + (59 * 60) + 59)


typedef enum
{
    STATE_START,
    STATE_SET_HRS,
    STATE_SET_MIN,
    STATE_SET_SEC,
    STATE_RUN,
    STATE_STOP,
    STATE_ALERT,
    STATE_CLEAR,
}
State;

typedef struct _Private
{
    union
    {
        struct
        {
            State state;                /* state machine state */
            AppTimer *timer_handle;     /* timer for background tasks */

            time_t time_start;          /* when the timer was started */
            time_t time_interval;       /* how long it is to run */
            time_t time_left;           /* how much us left (for pausing) */
            time_t time_end;            /* when the timer expires */

            /* Always add more at the end */
        };

        /* Force this thing to always be the same size so upgrades don't
        * screw up the persistence.
        */
        uint8_t reserved[128];
    };
} Private;


static void update_interval_display(time_t time_interval)
{
    struct tm *time_now = gmtime(&time_interval);

    display_set_time(time_now, HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT, true);
}


static bool click_sel(Face *face)
{
    Private *pvt = (Private *)face->data;

    switch(pvt->state)
    {
    case STATE_START:
        pvt->state = STATE_RUN;
        pvt->time_start = time(NULL);
        pvt->time_end = pvt->time_start + pvt->time_interval;
        pvt->time_left = pvt->time_interval;
        break;

    case STATE_RUN:
        pvt->state = STATE_STOP;
        pvt->time_left = pvt->time_left - (time(NULL) - pvt->time_start);
        break;

    case STATE_STOP:
        pvt->state = STATE_RUN;
        pvt->time_start = time(NULL);
        pvt->time_end = pvt->time_start + pvt->time_left;
        break;

    case STATE_SET_HRS:
        pvt->state = STATE_SET_SEC;
        display_set_highlight(HL_SECONDS);
        break;

    case STATE_SET_MIN:
        pvt->state = STATE_SET_HRS;
        display_set_highlight(HL_HOURS);
        break;

    case STATE_SET_SEC:
        pvt->state = STATE_SET_MIN;
        display_set_highlight(HL_MINUTES);
        break;

    default:
        /* ignore */
        break;
    }

    return true;
}


static bool click_long_sel(Face *face)
{
    Private *pvt = (Private *)face->data;

    switch(pvt->state)
    {
    case STATE_START:
        pvt->state = STATE_SET_MIN;
        update_interval_display(pvt->time_interval);
        display_set_highlight(HL_MINUTES);
        break;

    case STATE_SET_HRS:
        /* fall thru */
    case STATE_SET_MIN:
        /* fall thru */
    case STATE_SET_SEC:
        pvt->state = STATE_START;
        display_set_highlight(HL_NONE);
        break;

    default:
        /* ignore */
        break;
    }

    return true;
}


static bool click_up(Face *face, uint8_t count)
{
    Private *pvt = (Private *)face->data;
    int n = 1;

    if (count > 10)
    {
        n = 10;
    }
    else if (count > 5)
    {
        n = 5;
    }

    switch(pvt->state)
    {
    case STATE_SET_HRS:
        while (n > 0 && pvt->time_interval > MAX_TIME - (3600 * n))
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval += 3600 * n;
        }
        update_interval_display(pvt->time_interval);
        break;

    case STATE_SET_MIN:
        while (n > 0 && pvt->time_interval > MAX_TIME - (60 * n))
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval += 60 * n;
        }
        update_interval_display(pvt->time_interval);
        break;

    case STATE_SET_SEC:
        while (n > 0 && pvt->time_interval > MAX_TIME - n)
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval += n;
        }
        update_interval_display(pvt->time_interval);
        break;

    case STATE_RUN:
    case STATE_STOP:
        pvt->state = STATE_START;
        update_interval_display(pvt->time_interval);
        break;

    default:
        return false;
    }

    return true;
}


static bool click_dn(Face *face, uint8_t count)
{
    Private *pvt = (Private *)face->data;
    int n = 1;

    if (count > 10)
    {
        n = 10;
    }
    else if (count > 5)
    {
        n = 5;
    }

    switch(pvt->state)
    {
    case STATE_SET_HRS:
        while (n > 0 && pvt->time_interval < (3600 * n))
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval -= 3600 * n;
            update_interval_display(pvt->time_interval);
        }
        break;

    case STATE_SET_MIN:
        while (n > 0 && pvt->time_interval < (60 * n))
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval -= 60 * n;
            update_interval_display(pvt->time_interval);
        }
        break;

    case STATE_SET_SEC:
        while (n > 0 && pvt->time_interval < n)
        {
            n--;
        }
        if (n)
        {
            pvt->time_interval -= n;
            update_interval_display(pvt->time_interval);
        }
        break;

    default:
        return false;
    }

    return true;
}


static void update_handler(Face *face, struct tm *tt, TimeUnits uc)
{
    Private *pvt = (Private *)face->data;
    time_t time_remaining = pvt->time_end - time(NULL);
    struct tm *time_count = NULL;

    switch(pvt->state)
    {
    case STATE_RUN:
        if (time_remaining <= 0)
        {
            pvt->state = STATE_ALERT;
            time_remaining = 0;
            display_set_highlight(HL_DATE);
            vibes_short_pulse();
        }
        time_count = gmtime(&time_remaining);
        display_set_time(time_count,
                         HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT,
                         true);
        break;

    case STATE_ALERT:
        vibes_short_pulse();
        break;


    case STATE_CLEAR:
        pvt->state = STATE_START;
        time_count = gmtime(&pvt->time_interval);
        display_set_time(time_count,
                         HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT,
                         true);
        break;

    default:
        /* ignore */
        break;
    }
}


static void timer_handler(void *data)
{
    Private *pvt = data;
    time_t time_remaining = pvt->time_end - time(NULL);

    switch (pvt->state)
    {
    case STATE_RUN:
        if (time_remaining <= 0)
        {
            pvt->state = STATE_ALERT;
            time_remaining = 0;
            display_set_highlight(HL_DATE);
            vibes_short_pulse();
        }
        pvt->timer_handle = app_timer_register(1000, timer_handler, pvt);
        break;

    case STATE_ALERT:
        vibes_short_pulse();
        pvt->timer_handle = app_timer_register(1000, timer_handler, pvt);
        break;

    default:
        /* ignore */
        break;
    }
}


static void shut_up(Face *face)
{
    Private *pvt = (Private *)face->data;

    if (pvt->state == STATE_ALERT)
    {
        pvt->state = STATE_CLEAR;
        display_set_highlight(HL_NONE);
    }
}


static void load_handler(Face *face)
{
    Private *pvt = (Private *)face->data;
    time_t time_remaining = 0;
    struct tm *time_count;

    if (pvt->timer_handle)
    {
        app_timer_cancel(pvt->timer_handle);
        pvt->timer_handle = NULL;
    }

    switch (pvt->state)
    {
    case STATE_RUN:
        time_remaining = pvt->time_end - time(NULL);
        time_count = gmtime(&time_remaining);
        break;

    case STATE_STOP:
        time_count = gmtime(&pvt->time_left);
        break;

    case STATE_ALERT:
        time_count = gmtime(&time_remaining);
        break;

    default:
        time_count = gmtime(&pvt->time_interval);
        break;
    }

    display_set_time(time_count, 0xff, 1);
    display_set_title(face->name);
}


static void unload_handler(Face *face)
{
    Private *pvt = (Private *)face->data;

    if (pvt->state == STATE_RUN)
    {
        pvt->timer_handle = app_timer_register(1000, timer_handler, pvt);
    }
    display_clear();
}


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*
* @return  Pointer to the face, or NULL on error.
*****************************************************************************/
Face *timer_create(const char *name, uint32_t key)
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
        face->click_long_sel = click_long_sel;
        face->click_up = click_up;
        face->click_dn = click_dn;
        face->shut_up = shut_up;

        strncpy(face->name, name, sizeof(face->name));
        face->name[sizeof(face->name) - 1] = 0;
        face->key = key;

        if (persist_get_size(face->key) == sizeof(Private))
        {
            persist_read_data(face->key, face->data, sizeof(Private));
        }
    }

    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
    return face;
}


/**
* Destroy the watch face. De-allocate memory and release resources.
*****************************************************************************/
void timer_destroy(Face *face)
{
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    persist_write_data(face->key, face->data, sizeof(Private));
    free(face);
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}

