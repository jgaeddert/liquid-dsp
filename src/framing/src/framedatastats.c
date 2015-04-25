/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// framedatastats.c
//
// Default and generic frame statistics
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

// reset framedatastats object
void framedatastats_reset(framedatastats_s * _stats)
{
    if (_stats == NULL)
        return;

    _stats->num_frames_detected = 0;
    _stats->num_headers_valid   = 0;
    _stats->num_payloads_valid  = 0;
    _stats->num_bytes_received  = 0;
}

// print framedatastats object
void framedatastats_print(framedatastats_s * _stats)
{
    if (_stats == NULL)
        return;

    float percent_headers  = 0.0f;
    float percent_payloads = 0.0f;
    if (_stats->num_frames_detected > 0) {
        percent_headers  = 100.0f*(float)_stats->num_headers_valid  / (float)_stats->num_frames_detected;
        percent_payloads = 100.0f*(float)_stats->num_payloads_valid / (float)_stats->num_frames_detected;
    }

    printf("  frames detected   : %u\n", _stats->num_frames_detected);
    printf("  headers valid     : %-6u (%8.4f %%)\n", _stats->num_headers_valid,  percent_headers);
    printf("  payloads valid    : %-6u (%8.4f %%)\n", _stats->num_payloads_valid, percent_payloads);
    printf("  bytes received    : %lu\n", _stats->num_bytes_received);
}

