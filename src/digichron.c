/****************************************************************************/
/**
* DigiChron application. Acts like a normal Casio-style digital watch.
* Each mode (timer, stopwatch, etc) is implemented as a Face. The Faces use the
* Display to show their output and provide callback functions to capture input.
*
* A Face may completely override the default action by returning TRUE from the
* callback or it may return FALSE to allow the default action.
*
* Default button actions:
*       DBL-BACK        Invert the display.
*       SEL             Force a time update.
*       DN              Switch to the next face.
*
* All buttons, regardess of whether or not the default action has been
* overridden, will call the shut_up() function of all faces that have one.
* This is so pressing any key will stop alerts even if the responsible Face
* is not the active one.
*
* The display inversion mode and the currently-displayed face are saved when
* exiting the app.
*
* @file   digichron.c
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
#include <pebble.h>
#include "display.h"
#include "resources.h"
#include "utils.h"
#include "status.h"
#include "stopwatch.h"
#include "timer.h"
#include "watch.h"


typedef struct _FaceRecord
{
    Face *(*create)(const char *name, uint32_t key);
    void (*destroy)(Face *);
    uint32_t key;
    const char *name;
    Face *face;
}
FaceRecord;

typedef struct _Active
{
    int face;
    int invert_mode;
}
Active;


static Window *window;
static Active active;
static FaceRecord faces[] =
{
    {watch_create, watch_destroy, PERSIST_KEY_WATCH_STATE, "MAIN", NULL},
    {timer_create, timer_destroy, PERSIST_KEY_TMR1_STATE, "TMR1", NULL},
    {timer_create, timer_destroy, PERSIST_KEY_TMR2_STATE, "TMR2", NULL},
    {stopwatch_create, stopwatch_destroy, PERSIST_KEY_STW_STATE, "STW", NULL},
    {NULL, NULL, 0, NULL, NULL},
};


static void shut_up(void)
{
    int i = 0;

    while (faces[i].face)
    {
        if (faces[i].face->shut_up)
        {
            faces[i].face->shut_up(faces[i].face);
        }

        i++;
    }
}


static void update_time(void)
{
    time_t t = time(NULL);
    struct tm *tick_time = localtime(&t);
    TimeUnits units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

    faces[active.face].face->update_handler(faces[active.face].face,
                                            tick_time,
                                            units_changed);
}


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
    faces[active.face].face->update_handler(faces[active.face].face,
                                            tick_time,
                                            units_changed);
}


static void click_back_handler(ClickRecognizerRef recognizer, void *ctx)
{
    shut_up();
    update_time();
    light_enable_interaction();
}


static void click_sel_handler(ClickRecognizerRef recognizer, void *ctx)
{
    if (!faces[active.face].face->click_sel
        || !faces[active.face].face->click_sel(faces[active.face].face))
    {
        update_time();
    }

    shut_up();
}


static void click_up_handler(ClickRecognizerRef recognizer, void *ctx)
{
    uint8_t count = click_number_of_clicks_counted(recognizer);

    if (faces[active.face].face->click_up)
    {
        faces[active.face].face->click_up(faces[active.face].face, count);
    }

    shut_up();
}


static void click_down_handler(ClickRecognizerRef recognizer, void *ctx)
{
    uint8_t count = click_number_of_clicks_counted(recognizer);

    if (!faces[active.face].face->click_dn
        || !faces[active.face].face->click_dn(faces[active.face].face, count))
    {
        faces[active.face].face->unload_handler(faces[active.face].face);
        active.face++;
        if (faces[active.face].face == NULL)
        {
            active.face = 0;
        }
        faces[active.face].face->load_handler(faces[active.face].face);
    }

    shut_up();
}


static void click_long_sel_handler(ClickRecognizerRef recognizer, void *ctx)
{
    if (faces[active.face].face->click_long_sel)
    {
        faces[active.face].face->click_long_sel(faces[active.face].face);
    }
}


static void click_multi_back_handler(ClickRecognizerRef recognizer, void *ctx)
{
    active.invert_mode = !display_get_invert();
    display_set_invert(active.invert_mode);
}


static void click_config_provider(void *ctx)
{
    window_single_click_subscribe(BUTTON_ID_BACK, click_back_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, click_sel_handler);

    window_single_repeating_click_subscribe(BUTTON_ID_UP,
                                            500,
                                            click_up_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN,
                                            500,
                                            click_down_handler);

    window_long_click_subscribe(BUTTON_ID_SELECT,
                                0,
                                click_long_sel_handler,
                                NULL);

    window_multi_click_subscribe(BUTTON_ID_BACK,
                                 2,
                                 0,
                                 0,
                                 false,
                                 click_multi_back_handler);
}


static void window_load(Window *window)
{
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);

    if (persist_get_size(PERSIST_KEY_MAIN_STATE) == sizeof(Active))
    {
        persist_read_data(PERSIST_KEY_MAIN_STATE, &active, sizeof(Active));
    }

    display_set_invert(active.invert_mode);
    faces[active.face].face->load_handler(faces[active.face].face);

    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}


static void window_unload(Window *window)
{
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);

    persist_write_data(PERSIST_KEY_MAIN_STATE, &active, sizeof(Active));
    faces[active.face].face->unload_handler(faces[active.face].face);

    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
}


static bool faces_create(Window *window)
{
    int i = 0;

    while(faces[i].create)
    {
        faces[i].face = faces[i].create(faces[i].name, faces[i].key);
        if (faces[i].face == NULL)
        {
            break;
        }
        i++;
    }

    active.face = 0;

    return faces[active.face].face == NULL; /* we created at least one */
}


static void faces_destroy(void)
{
    int i = 0;

    while(faces[i].destroy)
    {
        faces[i].destroy(faces[i].face);
        i++;
    }
}


static bool init(void)
{
    bool err = true;
    WindowHandlers wh = { .load = window_load, .unload = window_unload };

    if (res_create())
    {
        LOG_MSG_ERROR("Can't initialize global resources");
        goto error_0;
    }

    window = window_create();
    if (window == NULL)
    {
        LOG_MSG_ERROR("Can't initialize main window");
        goto error_1;
    }

    if (display_create(window))
    {
        LOG_MSG_ERROR("Can't create display");
        goto error_2;
    }

    if (faces_create(window))
    {
        LOG_MSG_ERROR("One or more sub-faces could not be created, aborting");
        goto error_3;
    }

    status_create();

    display_set_invert(active.invert_mode);
    window_set_fullscreen(window, true);
    window_set_window_handlers(window, wh);
    window_set_click_config_provider(window, click_config_provider);
    window_stack_push(window, true);

    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
    err = false;
    goto error_0;

error_3:
    display_destroy();

error_2:
    window_destroy(window);

error_1:
    res_destroy();

error_0:
    return err;
}


static void deinit(void)
{
    tick_timer_service_unsubscribe();
    status_destroy();
    faces_destroy();
    display_destroy();
    window_destroy(window);
    res_destroy();
}


int main(void)
{
    if (!init())
    {
        LOG_MSG_DEBUG("Done initializing");
        app_event_loop();
        deinit();
    }

    return 0;
}

