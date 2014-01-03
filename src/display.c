/****************************************************************************/
/**
* The watch display. There is only one of these and it controls the look
* of the display regardless of which face is active.
*
* @file   display.c
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
#include "resources.h"
#include "utils.h"


typedef struct _Display
{
    /* Global inverter layer.
    */
    InverterLayer *invert_layer;

    /* Main display.
    */
    TextLayer *hour_layer;
    TextLayer *hm_layer;        /* holds the ':' separator */
    TextLayer *mins_layer;
    TextLayer *secs_layer;
    TextLayer *ampm_layer;
    TextLayer *date_layer;
    Layer *watch_layer;
    Layer *box_layer;
    InverterLayer *hl_layer;    /* highlight layer */

    /* Status display.
    */
    TextLayer *batt_layer;
    Layer *status_layer;
    Layer *hide_layer;          /* hides un-implemented icons */
    Layer *bt_layer;
    BitmapLayer *image_layer;

    GBitmap *status_bitmap;

    /* Other state.
    */
    struct
    {
        time_t sec;
        uint16_t ms;
    } last_interval;

    /* Field buffers...make them pack neatly into 4-byte words.
    */
    char hour_string[4];        /* hh */
    char hm_string[4];          /* : */
    char mins_string[4];        /* mm */
    char secs_string[4];        /* ss */
    char ampm_string[4];        /* am */
    char date_string[8];        /* mmm dd or ddd */
    char batt_string[4];        /* +99 */
} Display;

static Display display;


static void status_update_callback(Layer *l, GContext *ctx)
{
    GRect bounds = layer_get_bounds(l);

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}


static void watch_update_callback(Layer *l, GContext *ctx)
{
    GRect bounds = layer_get_bounds(l);

    graphics_context_set_stroke_color(ctx, GColorWhite);

    bounds.origin.x += 1;
    bounds.origin.y += 3;
    bounds.size.w -= 2;
    bounds.size.h -= 6;

    graphics_draw_round_rect(ctx, bounds, 3);

    bounds.origin.x += 1;
    bounds.origin.y += 1;
    bounds.size.w -= 2;
    bounds.size.h -= 2;

    graphics_draw_round_rect(ctx, bounds, 3);
}


static void box_update_callback(Layer *l, GContext *ctx)
{
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_round_rect(ctx, layer_get_bounds(l), 3);
}


static bool create_main_layers(Window *window, GRect bounds)
{
    bool err = true;
    GRect hour_frame = GRect(7, (bounds.size.h / 2) - 6, 48, 60);
    GRect hm_frame = GRect(hour_frame.origin.x + hour_frame.size.w,
                           hour_frame.origin.y,
                           12,
                           60);
    GRect mins_frame = GRect(hm_frame.origin.x + hm_frame.size.w,
                             hour_frame.origin.y,
                             48,
                             60);
    GRect secs_frame = GRect(mins_frame.origin.x + mins_frame.size.w + 1,
                             hour_frame.origin.y + 20,
                             22,
                             30);
    GRect ampm_frame = GRect(8, (bounds.size.h / 2) - 40, 56, 28);
    GRect date_frame = GRect(ampm_frame.origin.x + ampm_frame.size.w + 1,
                             ampm_frame.origin.y,
                             74,
                             ampm_frame.size.h);

    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);
    LOG_MSG_DEBUG("hour_frame={%d,%d,%d,%d}",
                  hour_frame.origin.x,
                  hour_frame.origin.y,
                  hour_frame.size.w,
                  hour_frame.size.h);
    LOG_MSG_DEBUG("hm_frame={%d,%d,%d,%d}",
                  hm_frame.origin.x,
                  hm_frame.origin.y,
                  hm_frame.size.w,
                  hm_frame.size.h);
    LOG_MSG_DEBUG("mins_frame={%d,%d,%d,%d}",
                  mins_frame.origin.x,
                  mins_frame.origin.y,
                  mins_frame.size.w,
                  mins_frame.size.h);
    LOG_MSG_DEBUG("secs_frame={%d,%d,%d,%d}",
                  secs_frame.origin.x,
                  secs_frame.origin.y,
                  secs_frame.size.w,
                  secs_frame.size.h);
    LOG_MSG_DEBUG("ampm_frame={%d,%d,%d,%d}",
                  ampm_frame.origin.x,
                  ampm_frame.origin.y,
                  ampm_frame.size.w,
                  ampm_frame.size.h);
    LOG_MSG_DEBUG("date_frame={%d,%d,%d,%d}",
                  date_frame.origin.x,
                  date_frame.origin.y,
                  date_frame.size.w,
                  date_frame.size.h);

    window_set_background_color(window, GColorBlack);

    /* This creates a manager window for our children so we don't
    * have to worry about the origin of the main window when placing
    * them.
    */
    display.watch_layer = layer_create(bounds);
    if (display.watch_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.watch_layer");
        goto error_0;
    }
    layer_set_update_proc(display.watch_layer, watch_update_callback);

    display.hour_layer = text_layer_create(hour_frame);
    if (display.hour_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.hour_layer");
        goto error_1;
    }

    display.hm_layer = text_layer_create(hm_frame);
    if (display.hm_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.hm_layer");
        goto error_2;
    }

    display.mins_layer = text_layer_create(mins_frame);
    if (display.mins_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.minslayer");
        goto error_3;
    }

    display.secs_layer = text_layer_create(secs_frame);
    if (display.secs_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.secs_layer");
        goto error_4;
    }

    display.ampm_layer = text_layer_create(ampm_frame);
    if (display.ampm_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.ampm_layer");
        goto error_5;
    }

    display.date_layer = text_layer_create(date_frame);
    if (display.date_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.date_layer");
        goto error_6;
    }

    display.box_layer = layer_create(date_frame);
    if (display.box_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.box_layer");
        goto error_7;
    }
    layer_set_update_proc(display.box_layer, box_update_callback);

    display.hl_layer = inverter_layer_create(date_frame);
    if (display.hl_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create display.hl_layer");
        goto error_8;
    }

    text_layer_set_font(display.date_layer, res_font_medium);
    text_layer_set_font(display.ampm_layer, res_font_medium);
    text_layer_set_font(display.secs_layer, res_font_medium);
    text_layer_set_font(display.mins_layer, res_font_large);
    text_layer_set_font(display.hm_layer, res_font_large);
    text_layer_set_font(display.hour_layer, res_font_large);

    text_layer_set_text_color(display.date_layer, GColorWhite);
    text_layer_set_text_color(display.ampm_layer, GColorWhite);
    text_layer_set_text_color(display.secs_layer, GColorWhite);
    text_layer_set_text_color(display.mins_layer, GColorWhite);
    text_layer_set_text_color(display.hm_layer, GColorWhite);
    text_layer_set_text_color(display.hour_layer, GColorWhite);

    text_layer_set_background_color(display.date_layer, GColorClear);
    text_layer_set_background_color(display.ampm_layer, GColorClear);
    text_layer_set_background_color(display.secs_layer, GColorClear);
    text_layer_set_background_color(display.mins_layer, GColorClear);
    text_layer_set_background_color(display.hm_layer, GColorClear);
    text_layer_set_background_color(display.hour_layer, GColorClear);

    text_layer_set_text_alignment(display.date_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(display.ampm_layer, GTextAlignmentLeft);
    text_layer_set_text_alignment(display.secs_layer, GTextAlignmentLeft);
    text_layer_set_text_alignment(display.mins_layer, GTextAlignmentLeft);
    text_layer_set_text_alignment(display.hm_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(display.hour_layer, GTextAlignmentRight);

    layer_add_child(window_get_root_layer(window),
                    display.watch_layer);
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.date_layer));
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.ampm_layer));
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.secs_layer));
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.mins_layer));
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.hm_layer));
    layer_add_child(display.watch_layer,
                    text_layer_get_layer(display.hour_layer));

    layer_add_child(display.watch_layer, display.box_layer);
    text_layer_set_text(display.hm_layer, ":");

    err = false;
    goto error_0;

error_8:
    layer_destroy(display.box_layer);

error_7:
    text_layer_destroy(display.date_layer);

error_6:
    text_layer_destroy(display.ampm_layer);

error_5:
    text_layer_destroy(display.secs_layer);

error_4:
    text_layer_destroy(display.hour_layer);

error_3:
    text_layer_destroy(display.hm_layer);

error_2:
    text_layer_destroy(display.mins_layer);

error_1:
    layer_destroy(display.watch_layer);

error_0:
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
    return err;
}


static void destroy_main_layers(void)
{
    inverter_layer_destroy(display.hl_layer);
    text_layer_destroy(display.date_layer);
    text_layer_destroy(display.ampm_layer);
    text_layer_destroy(display.secs_layer);
    text_layer_destroy(display.hour_layer);
    text_layer_destroy(display.hm_layer);
    text_layer_destroy(display.mins_layer);
    layer_destroy(display.box_layer);
    layer_destroy(display.watch_layer);
}


static int create_status_layers(Window *window, GRect bounds)
{
    bool err = true;
    int bw = bounds.size.w;
    int bh = bounds.size.h;
    LOG_MSG_DEBUG("Entering %s", __FUNCTION__);

    display.status_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STATUS_BAR);
    if (display.status_bitmap == NULL)
    {
        LOG_MSG_ERROR("Can't create status bar bitmap");
        goto error_0;
    }

    display.image_layer = bitmap_layer_create(bounds);
    if (display.image_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create image layer");
        goto error_1;
    }

    display.batt_layer = text_layer_create(GRect(122, 0, bw - 122, bh));
    if (display.batt_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create battery status layer");
        goto error_2;
    }

    display.bt_layer = layer_create(GRect(90, 0, 15, bh));
    if (display.bt_layer == NULL)
    {
        goto error_3;
        LOG_MSG_ERROR("Can't create bluetooth status layer");
    }

    display.hide_layer = layer_create(GRect(0, 0, 90, bh));
    if (display.hide_layer == NULL)
    {
        LOG_MSG_ERROR("Can't hide unused status icons");
        goto error_4;
    }

    bitmap_layer_set_bitmap(display.image_layer, display.status_bitmap);
    bitmap_layer_set_alignment(display.image_layer, GAlignTopLeft);

    layer_set_update_proc(display.hide_layer, status_update_callback);
    layer_set_update_proc(display.bt_layer, status_update_callback);

    text_layer_set_font(display.batt_layer, res_font_small);
    text_layer_set_text_color(display.batt_layer, GColorWhite);
    text_layer_set_background_color(display.batt_layer, GColorClear);
    text_layer_set_text_alignment(display.batt_layer, GTextAlignmentLeft);

    layer_add_child(window_get_root_layer(window),
                    bitmap_layer_get_layer(display.image_layer));
    layer_add_child(bitmap_layer_get_layer(display.image_layer),
                    display.hide_layer);
    layer_add_child(bitmap_layer_get_layer(display.image_layer),
                    display.bt_layer);
    layer_add_child(bitmap_layer_get_layer(display.image_layer),
                    text_layer_get_layer(display.batt_layer));

    err = false;
    goto error_0;

error_4:
    layer_destroy(display.bt_layer);

error_3:
    text_layer_destroy(display.batt_layer);

error_2:
    bitmap_layer_destroy(display.image_layer);

error_1:
    gbitmap_destroy(display.status_bitmap);

error_0:
    LOG_MSG_DEBUG("Exiting %s", __FUNCTION__);
    return err;
}


static void destroy_status_layers(void)
{
    gbitmap_destroy(display.status_bitmap);
    bitmap_layer_destroy(display.image_layer);
    layer_destroy(display.hide_layer);
    layer_destroy(display.bt_layer);
    text_layer_destroy(display.batt_layer);
    layer_destroy(display.status_layer);
}



/**
* Create the display. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*****************************************************************************/
bool display_create(Window *window)
{
    bool err = true;

    if (create_main_layers(window, GRect(0, 0, 144, 168-18)))
    {
        LOG_MSG_ERROR("Can't create main display layers");
        goto error_0;
    }

    if (create_status_layers(window, GRect(0, 168-18, 144, 18)))
    {
        LOG_MSG_ERROR("Can't create status display layers");
        goto error_1;
    }

    display.invert_layer = inverter_layer_create(GRect(0, 0, 144, 168));
    if (display.invert_layer == NULL)
    {
        LOG_MSG_ERROR("Can't create inversion layer");
        goto error_2;
    }
    layer_set_hidden(inverter_layer_get_layer(display.invert_layer), true);
    layer_add_child(window_get_root_layer(window),
                    inverter_layer_get_layer(display.invert_layer));

    display_clear();

    err = 0;
    goto error_0;

error_2:
    destroy_status_layers();

error_1:
    destroy_main_layers();

error_0:
    return err;
}


/**
* Destroy the display. De-allocate memory and release resources.
*****************************************************************************/
void display_destroy(void)
{
    destroy_main_layers();
    destroy_status_layers();
    inverter_layer_destroy(display.invert_layer);
}


/**
* Draw a title on the display. On some faces this may do nothing or may
* share space with another field.
*****************************************************************************/
void display_set_title(const char *title)
{
    snprintf(display.date_string,
             sizeof(display.date_string),
             "%s",
             title);
    text_layer_set_text(display.date_layer, display.date_string);
}


/**
* Clear display by resetting all fields to default values.
*****************************************************************************/
void display_clear(void)
{
    text_layer_set_text(display.date_layer, " ");
    text_layer_set_text(display.ampm_layer, " ");
    text_layer_set_text(display.secs_layer, " ");
    text_layer_set_text(display.hm_layer, ":");
    layer_remove_from_parent(inverter_layer_get_layer(display.hl_layer));

    display.last_interval.sec = 99 * 3600; /* max hours */
    display.last_interval.ms = 999;
}


/**
* Show the time on the display.
*****************************************************************************/
void display_set_time(struct tm *time_now,
                      TimeUnits units_to_update,
                      int force_style)
{
    if (units_to_update & DAY_UNIT)
    {
        strftime(display.date_string,
                 sizeof(display.date_string),
                 "%b %d",
                 time_now);

        text_layer_set_text(display.date_layer, upcase(display.date_string));
    }

    if (units_to_update & (MINUTE_UNIT | HOUR_UNIT))
    {
        if (force_style >= 1 || (force_style == 0 && clock_is_24h_style()))
        {
            strcpy(display.ampm_string, "  ");
            strftime(display.hour_string,
                     sizeof(display.hour_string),
                     "%H",
                     time_now);
            strftime(display.mins_string,
                     sizeof(display.mins_string),
                     "%M",
                     time_now);
        }
        else
        {
            strcpy(display.ampm_string, time_now->tm_hour < 12 ? "AM" : "PM");
            strftime(display.hour_string,
                     sizeof(display.hour_string),
                     "%l",
                     time_now);
            strftime(display.mins_string,
                     sizeof(display.mins_string),
                     "%M",
                     time_now);
        }

        text_layer_set_text(display.ampm_layer, display.ampm_string);
        text_layer_set_text(display.hour_layer, display.hour_string);
        text_layer_set_text(display.mins_layer, display.mins_string);
    }

    if (units_to_update & SECOND_UNIT)
    {
        strftime(display.secs_string,
                 sizeof(display.secs_string),
                 "%S",
                 time_now);
        text_layer_set_text(display.secs_layer, display.secs_string);
    }
}


/**
* Show a time interval. This is used mainly by faces that act like a
* stopwatch. The intent is to show the time since sime starting point.
*****************************************************************************/
void display_set_interval(time_t sec, uint16_t ms)
{
    if (display.last_interval.ms != ms)
    {
        int hundredths = (ms + 5) / 10;

        while (hundredths >= 100)
        {
            sec++;
            hundredths -= 100;
        }

        snprintf(display.secs_string, sizeof(display.secs_string),
                 "%02d",
                 hundredths);
        text_layer_set_text(display.secs_layer, display.secs_string);
    }

    if (display.last_interval.sec != sec)
    {
        int hours;
        int minutes;
        int seconds = sec;

        hours = seconds / 3600;
        if (hours > 99)
        {
            hours = 99;
            minutes = 59;
            seconds = 59;
        }
        else
        {
            seconds -= hours * 3600;
            minutes  = seconds / 60;
            seconds -= minutes * 60;
        }

        if (hours != display.last_interval.sec / 3600)
        {
            snprintf(display.ampm_string,
                     sizeof(display.ampm_string),
                     "%dH",
                     hours);
            text_layer_set_text(display.ampm_layer, display.ampm_string);
        }

        snprintf(display.hour_string,
                 sizeof(display.hour_string),
                 "%02d",
                 minutes);

        snprintf(display.mins_string,
                 sizeof(display.mins_string),
                 "%02d",
                 seconds);

        text_layer_set_text(display.hour_layer, display.hour_string);
        text_layer_set_text(display.mins_layer, display.mins_string);
    }

    display.last_interval.sec = sec;
    display.last_interval.ms = ms;
}


/**
* Highlight particular fields of the time display. Can be used to set
* countdown intervals interactively.
*****************************************************************************/
void display_set_highlight(HighlightFields what_to_highlight)
{
    GRect f;
    Layer *l = NULL;
    Layer *hl_layer = inverter_layer_get_layer(display.hl_layer);

    layer_remove_from_parent(hl_layer);

    switch (what_to_highlight)
    {
    case HL_NONE:
        break;

    case HL_SECONDS:
        l = text_layer_get_layer(display.secs_layer);
        break;

    case HL_MINUTES:
        l = text_layer_get_layer(display.mins_layer);
        break;

    case HL_HOURS:
        l = text_layer_get_layer(display.hour_layer);
        break;

    case HL_AMPM:
        l = text_layer_get_layer(display.ampm_layer);
        break;

    case HL_DATE:
        l = text_layer_get_layer(display.date_layer);
        break;

    default:
        LOG_MSG_WARNING("Unknown highlight value %d", what_to_highlight);
        break;
    }

    if (l)
    {
        f = layer_get_frame(l);
        f.origin.x = 0;
        f.origin.y = 0;
        layer_set_frame(hl_layer, f);
        layer_add_child(l, hl_layer);
    }
}


/**
* Set the count of missed SMS messages.
*****************************************************************************/
void display_set_sms(unsigned int count)
{
    /* not implemented yet */
}


/**
* Set the count of missed emails.
*****************************************************************************/
void display_set_email(unsigned int count)
{
    /* not implemented yet */
}


/**
* Set the count of missed calls.
*****************************************************************************/
void display_set_calls(unsigned int count)
{
    /* not implemented yet */
}


/**
* Set the battery percentage.
*****************************************************************************/
void display_set_battery(unsigned int percent, bool charge)
{
    if (percent >= 100)
    {
        strcpy(display.batt_string, "100");
    }
    else
    {
        snprintf(display.batt_string,
                 sizeof(display.batt_string),
                 "%0d%c",
                 percent,
                 charge ? '+' : ' ');
    }

    text_layer_set_text(display.batt_layer, display.batt_string);
}


/**
* Enable or disable the bluetooth indicator.
*****************************************************************************/
void display_set_bluetooth(bool connected)
{
    layer_set_hidden(display.bt_layer, connected);
}


/**
* Invert the display colors.
*****************************************************************************/
void display_set_invert(bool invert)
{
    layer_set_hidden(inverter_layer_get_layer(display.invert_layer), !invert);
}


/**
* Is the display currently inverted?
*****************************************************************************/
bool display_get_invert(void)
{
    return !layer_get_hidden(inverter_layer_get_layer(display.invert_layer));
}

