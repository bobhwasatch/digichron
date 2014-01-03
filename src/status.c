/****************************************************************************/
/**
* Status bar functions.
*
* @file   status.c
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
#include "status.h"
#include "utils.h"


static void battery_handler(BatteryChargeState battery_state)
{
    display_set_battery(battery_state.charge_percent,
                        battery_state.is_charging);
}


static void bt_handler(bool connected)
{
    display_set_bluetooth(connected);
    vibes_double_pulse();
}


/**
* Initialize resources. Must be called before any of the global resources
* are used.
*****************************************************************************/
void status_create(void)
{
    BatteryChargeState battery_state = battery_state_service_peek();
    bool bt_connected = bluetooth_connection_service_peek();

    display_set_battery(battery_state.charge_percent,
                        battery_state.is_charging);
    battery_state_service_subscribe(battery_handler);

    display_set_bluetooth(bt_connected);
    if (!bt_connected)
    {
        vibes_double_pulse();
    }
    bluetooth_connection_service_subscribe(bt_handler);
}


/**
* Clean up global resources.
*****************************************************************************/
void status_destroy(void)
{
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
}

