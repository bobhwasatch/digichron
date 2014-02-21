/****************************************************************************/
/**
* Definitions for a stopwatch face that times intervals. Captures both
* split times (cumulative time since the start) and lap times (time since
* the start or the last split, whichever is newer).
*
* Titles:
*       STW     Stopwatch mode. Ready to start timing or capture a split.
*       SPLT    Split mode. A split time is being displayed.
*       LAP     Lap mode. A lap time is being displayed.
*
* Buttons:
*       UP      In STW mode:
*                       Capture the split & lap times and go to SPLIT mode. The
*                       timing continues in the background.
*               In SPLT or LAP mode:
*                       Switch to the other mode.
*       SEL     In STW mode:
*                       Starts timing if not running, otherwise pause timing,
                        highlight the status, and got to SPLIT mode.
*               In SPLT or LAP mode:
*                       Returns to STW mode, resumes updating the display,
*                       ready to capture another split.
*       L-SEL   In any mode, resets the stopwatch and stops timing.
*       DN      Not used, performs the default action.
*
*       If SPLIT or LAP mode was entered by clicking UP in STW mode, then the
*       timing continues in the background (normal split/lap mode). If SPLIT
*       or lap was entered by pausing timing with the SEL button, then timing
*       remains paused while in SPLIT or LAP (stopped split/lap mode).
*
*       This supports two use cases, one for timing things like races where
*       you want timing to continue after capturing the last lap, the other
*       for timing tasks where you want to stop timing in between sessions.
*
* @file   stopwatch.c
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
#include "stopwatch.h"


#define TIMER_FAST_MS   (200)
#define TIMER_SLOW_MS   (1000)
#define MAX_FAST_SEC    (300) /* max time to display hundreths */


typedef enum
{
    STATE_START,
    STATE_RUN,
    STATE_SPLIT,
    STATE_LAP,
    STATE_STOP_LAP,
    STATE_STOP_SPLIT,
}
State;

typedef struct _Private
{
    union
    {
        struct
        {
            State state;        /* state machine variable */
            AppTimer *timer;    /* timer for background tasks */

            TimeMS start_time;  /* starting time */
            TimeMS last_time;   /* saved stop time for calculating laps */
            TimeMS split_time;  /* total delta from start to split */
            TimeMS lap_time;    /* delta since last split */

            bool visible;

            TimeMS stop_time;   /* saved stop time */

            /* Always add more at the end */
        };

        /* Force this thing to always be the same size so upgrades don't
        * screw up the persistence.
        */
        uint8_t reserved[128];
    };
} Private;


static void calculate_splits(Private *pvt)
{
    TimeMS now;

    time_ms(&now.sec, &now.ms);
    time_diff(&pvt->lap_time, &now, &pvt->last_time);
    time_diff(&pvt->split_time, &now, &pvt->start_time);
    pvt->last_time = now;
}


static void timer_handler(void *data)
{
    TimeMS now;
    Private *pvt = data;

    if (pvt->state == STATE_RUN)
    {
        uint32_t interval;

        time_ms(&now.sec, &now.ms);
        time_diff(&now, &now, &pvt->start_time);

        interval = now.sec < MAX_FAST_SEC ? TIMER_FAST_MS : TIMER_SLOW_MS;
        pvt->timer = app_timer_register(interval,  timer_handler, pvt);

        if (pvt->visible)
        {
            /* Round up to the next multiple of 100 ms so that the last digit
            * in the display doesn't look so random.
            */
            display_set_interval(now.sec, now.ms + 100 - (now.ms % 100));
        }
    }
}


static bool click_sel(Face *face)
{
    Private *pvt = (Private *)face->data;

    switch (pvt->state)
    {
    case STATE_START:
        pvt->state = STATE_RUN;
        time_ms(&pvt->start_time.sec, &pvt->start_time.ms);
        pvt->last_time = pvt->start_time;
        timer_handler(pvt);
        display_set_title(face->name);
        break;

    case STATE_RUN:
        pvt->state = STATE_STOP_SPLIT;
        time_ms(&pvt->stop_time.sec, &pvt->stop_time.ms);
        calculate_splits(pvt);
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLIT");
        display_set_highlight(HL_DATE);
        break;

    case STATE_STOP_LAP:
    case STATE_STOP_SPLIT:
        {
            TimeMS now;

            /* Move the start time up by how long we were stopped.
            */
            time_ms(&now.sec, &now.ms);
            time_diff(&now, &now, &pvt->stop_time);
            time_sum(&pvt->last_time, &now, &pvt->last_time);
            time_sum(&pvt->start_time, &now, &pvt->start_time);
        }
        /* fall thru */
    case STATE_SPLIT:
        /* fall thru */
    case STATE_LAP:
        pvt->state = STATE_RUN;
        display_set_title(face->name);
        display_set_highlight(HL_NONE);
        timer_handler(pvt);
        break;

    default:
        return false;
    }

    return true;
}


static bool click_long_sel(Face *face)
{
    Private *pvt = (Private *)face->data;

    pvt->start_time.sec = 0;
    pvt->start_time.ms = 0;

    pvt->last_time.sec = 0;
    pvt->last_time.ms = 0;

    pvt->split_time.sec = 0;
    pvt->split_time.ms = 0;

    pvt->lap_time.sec = 0;
    pvt->lap_time.ms = 0;

    display_set_interval(0, 0);
    display_set_title(face->name);
    display_set_highlight(HL_NONE);
    pvt->state = STATE_START;

    return true;
}


static bool click_up(Face *face, uint8_t count)
{
    Private *pvt = (Private *)face->data;

    switch (pvt->state)
    {
    case STATE_RUN:
        pvt->state = STATE_SPLIT;
        calculate_splits(pvt);
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLT");
        break;

    case STATE_STOP_LAP:
        pvt->state = STATE_STOP_SPLIT;
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLT");
        break;

    case STATE_STOP_SPLIT:
        pvt->state = STATE_STOP_LAP;
        display_set_interval(pvt->lap_time.sec, pvt->lap_time.ms);
        display_set_title("LAP");
        break;

    case STATE_SPLIT:
        pvt->state = STATE_LAP;
        display_set_interval(pvt->lap_time.sec, pvt->lap_time.ms);
        display_set_title("LAP");
        break;

    case STATE_LAP:
        pvt->state = STATE_SPLIT;
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLT");
        break;

    default:
        return false;
    }

    return true;
}


static void load_handler(Face *face)
{
    Private *pvt = (Private *)face->data;

    pvt->visible = true;

    switch (pvt->state)
    {
    case STATE_RUN:
        display_set_title("STW");
        timer_handler(pvt);
        break;

    case STATE_SPLIT:
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLT");
        break;

    case STATE_LAP:
        display_set_interval(pvt->lap_time.sec, pvt->lap_time.ms);
        display_set_title("LAP");
        break;

    case STATE_STOP_SPLIT:
        display_set_interval(pvt->split_time.sec, pvt->split_time.ms);
        display_set_title("SPLT");
        display_set_highlight(HL_DATE);
        break;

    case STATE_STOP_LAP:
        display_set_interval(pvt->lap_time.sec, pvt->lap_time.ms);
        display_set_title("LAP");
        display_set_highlight(HL_DATE);
        break;

    case STATE_START:
        /* fall thru */
    default:
        display_set_interval(0, 0);
        display_set_title("STW");
        break;
    }
}


static void unload_handler(Face *face)
{
    Private *pvt = (Private *)face->data;

    if (pvt->timer)
    {
        app_timer_cancel(pvt->timer);
        pvt->timer = NULL;
    }

    display_clear();
    pvt->visible = false;
}


static void update_handler(Face *face, struct tm *tt, TimeUnits uc)
{
    /* nothing to do */
}


/**
* Create the watch face. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*****************************************************************************/
Face *stopwatch_create(const char *name, uint32_t key)
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
void stopwatch_destroy(Face *face)
{
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    persist_write_data(face->key, face->data, sizeof(Private));
    free(face);
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}

