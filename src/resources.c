/****************************************************************************/
/**
* Global resources for the watch app.
*
* @file   resources.h
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
#include "resources.h"
#include "utils.h"


GFont *res_font_large;
GFont *res_font_medium;
GFont *res_font_small;


/**
* Initialize resources. Must be called before any of the global resources
* are used.
*****************************************************************************/
int res_create(void)
{
    int err = true;
    ResHandle res_s = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_14);
    ResHandle res_m = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_22);
    ResHandle res_l = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_42);

    res_font_small = fonts_load_custom_font(res_s);
    if (res_font_small == NULL)
    {
        LOG_MSG_ERROR("Can't load small font");
        goto error_0;
    }

    res_font_medium = fonts_load_custom_font(res_m);
    if (res_font_medium == NULL)
    {
        LOG_MSG_ERROR("Can't load medium font");
        goto error_1;
    }

    res_font_large = fonts_load_custom_font(res_l);
    if (res_font_large == NULL)
    {
        LOG_MSG_ERROR("Can't load large font");
        goto error_2;
    }

    err = false;
    goto error_0;

error_2:
    fonts_unload_custom_font(res_font_medium);

error_1:
    fonts_unload_custom_font(res_font_small);

error_0:
    return err;
}


/**
* Clean up global resources.
*****************************************************************************/
void res_destroy(void)
{
    fonts_unload_custom_font(res_font_large);
    fonts_unload_custom_font(res_font_medium);
    fonts_unload_custom_font(res_font_small);
}

