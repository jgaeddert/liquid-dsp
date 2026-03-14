/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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
 */

#include "liquid.autotest.h"
#include "liquid.internal.h"

// user-defined callback
int autotest_logging_callback(liquid_log_event event, void * context, int config)
{
    *((bool*)context) = true;
    return LIQUID_OK;
}

// user-defined lock function
int autotest_logging_lock(int _lock, void * context)
{
    *((bool*)context) = true;
    return LIQUID_OK;
}

LIQUID_AUTOTEST(logging,"test basic logging functionality","",0.1)
{
    bool callback_invoked = false;
    bool lock_invoked     = false;

    // create custom logging object
    liquid_logger custom_log = liquid_logger_create();
    liquid_logger_set_level(custom_log, LIQUID_INFO);
    liquid_logger_set_config(custom_log, LIQUID_LOG_COMPACT | LIQUID_LOG_COLOR);

    // both flags should be false
    LIQUID_CHECK( !callback_invoked );
    LIQUID_CHECK( !lock_invoked );

    //
    liquid_logger_add_callback(custom_log, autotest_logging_callback, (void*)&callback_invoked, LIQUID_INFO);
    liquid_logger_set_lock(custom_log, autotest_logging_lock, (void*)&lock_invoked);
    //liquid_logger_add_file    (custom_log, logfile, -1);

    // check values
    LIQUID_CHECK(liquid_logger_get_num_callbacks(custom_log) == 1U);

    // log at a low level
    liquid_log(custom_log,LIQUID_DEBUG,__FILE__,__LINE__,"autotest / custom log");

    // lock flag should be checked, but not the callback as it is at a higher level (info)
    LIQUID_CHECK( !callback_invoked );
    LIQUID_CHECK( lock_invoked );

    // log at a higher level
    liquid_log(custom_log,LIQUID_INFO,__FILE__,__LINE__,"autotest / custom log");

    // now both should be true
    LIQUID_CHECK( callback_invoked );
    LIQUID_CHECK( lock_invoked );

    // TODO: check counts

    // clean it up
    liquid_logger_destroy(custom_log);
}

