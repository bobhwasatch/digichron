/****************************************************************************/
/**
* The watch display. There is only one of these and it controls the look
* of the display regardless of which face is active.
*
* @file   display.h
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
#ifndef DISPLAY_H
#define DISPLAY_H


#include <pebble.h>


typedef enum
{
    HL_NONE,
    HL_SECONDS,
    HL_MINUTES,
    HL_HOURS,
    HL_AMPM,
    HL_DATE,
}
HighlightFields;


/**
* Create the display. Allocate memory and set up the data structures. Do
* not draw anything until the load_handler() is called.
*
* @param window Pointer to the window to draw the display on.
*
* @return  Pointer to the face, or NULL on error.
*****************************************************************************/
bool display_create(Window *window);


/**
* Destroy the display. De-allocate memory and release resources.
*****************************************************************************/
void display_destroy(void);


/**
* Clear display by resetting all fields to default values.
*****************************************************************************/
void display_clear(void);


/**
* Draw a title on the display. On some faces this may do nothing or may
* share space with another field.
*
* @param title  Pointer to the string to use for the title
*****************************************************************************/
void display_set_title(const char *title);


/**
* Show the time on the display.
*
* @param time_now         The time to display.
* @param units_to_update  Bitmask of which fields to update.
* @param force_style      -1 = ampm, 0 = default, 1 = 24h
*****************************************************************************/
void display_set_time(struct tm *time_now,
                      TimeUnits units_to_update,
                      int force_style);


/**
* Show a time interval. This is used mainly by faces that act like a
* stopwatch. The intent is to show the time since sime starting point.
* This may do nothing or share space with another field.
*
* @param sec    Seconds since the epoch
* @param ms     Milliseconds since the last second.
*****************************************************************************/
void display_set_interval(time_t sec, uint16_t ms);


/**
* Highlight particular fields of the time display. Can be used to set
* countdown intervals interactively.
*
* @param what_to_highlight      The field designator to highlight.
*****************************************************************************/
void display_set_highlight(HighlightFields what_to_highlight);


/**
* Set the count of missed SMS messages.
*
* @param count  Number of missed messages.
*****************************************************************************/
void display_set_sms(unsigned int count);


/**
* Set the count of missed emails.
*
* @param count  Number of missed emails.
*****************************************************************************/
void display_set_email(unsigned int count);


/**
* Set the count of missed calls.
*
* @param count  Number of missed calls.
*****************************************************************************/
void display_set_calls(unsigned int count);


/**
* Set the battery percentage and charge state.
*
* @param percent        Set the battery percentage, must be 0 <= x <= 100.
* @param charge         TRUE if charging.
*****************************************************************************/
void display_set_battery(unsigned int percent, bool charge);


/**
* Enable or disable the bluetooth indicator.
*
* @param connected      Set to TRUE to enable the indicator.
*****************************************************************************/
void display_set_bluetooth(bool connected);


/**
* Invert the display colors.
*
* @param invert TRUE for black on white, FALSE for white on black.
*****************************************************************************/
void display_set_invert(bool invert);


/**
* Is the display currently inverted?
*
* @return invert TRUE for black on white, FALSE for white on black.
*****************************************************************************/
bool display_get_invert(void);


#endif  /* include guard */
