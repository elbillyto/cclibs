/*!
 * @file  regMeas.c
 * @brief Converter Control Regulation library measurement-related functions
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2015. This project is released under the GNU Lesser General
 * Public License version 3.
 * 
 * <h2>License</h2>
 *
 * This file is part of libreg.
 *
 * libreg is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include "libreg.h"

/*!
 * Classical two-stage box-car FIR filter used by regMeasFilterRT() and regMeasFilterInit().
 *
 * @param[in,out] filter    Measurement filter parameters and values
 * @returns       Value of reg_meas_filter::fir_accumulator adjusted by reg_meas_filter::integer_to_float factor
 */
static REG_float regMeasFirFilterRT(struct REG_meas_filter *filter);



// Background functions - do not call these from the real-time thread or interrupt

void regMeasFilterInitBuffer(struct REG_meas_filter *filter, int32_t *buf, uint32_t buf_len)
{
    filter->fir_buf[0] = buf;
    filter->buf_len    = buf_len;
}



void regMeasFilterInit(struct REG_meas_filter *filter, uint32_t fir_length[2],
                       uint32_t extrapolation_len_iters, REG_float pos, REG_float neg, REG_float meas_delay_iters)
{
    uint32_t    total_fir_len;
    uint32_t     buf_len;
    REG_float       filter_delay;
    REG_float      *extrapolation_buf;

    // Stop the filter
    
    filter->is_running = false;

    // Disable filter stages that are not used, or both stages if the buffer has not been specified

    if(fir_length[0] == 1 || filter->fir_buf[0] == NULL)
    {
        filter->fir_length[0] = 0;
    }
    else
    {
        filter->fir_length[0] = fir_length[0];
    }

    if(fir_length[1] == 1 || filter->fir_buf[0] == NULL)
    {
        filter->fir_length[1] = 0;
    }
    else
    {
        filter->fir_length[1] = fir_length[1];
    }

    // Sort filter stages to have the longest first

    if(filter->fir_length[0] < filter->fir_length[1])
    {
        uint32_t temp_fir_length = filter->fir_length[0];

        filter->fir_length[0] = filter->fir_length[1];
        filter->fir_length[1] = temp_fir_length;
    }
    
    // Clip filter lengths if they exceed the buffer space

    buf_len = filter->buf_len;

    if(extrapolation_len_iters > buf_len)
    {
        extrapolation_len_iters = buf_len;
    }

    buf_len -= extrapolation_len_iters;

    if(filter->fir_length[0] > buf_len)
    {
        filter->fir_length[0] = buf_len;
    }

    buf_len -= filter->fir_length[0];

    if(filter->fir_length[1] > buf_len)
    {
        filter->fir_length[1] = buf_len;
    }

    // Set the pointers to the second stage FIR buffer and extrapolation buffer

    filter->fir_buf[1]              = filter->fir_buf[0] + filter->fir_length[0];
    filter->extrapolation_buf       = extrapolation_buf = (REG_float*)(filter->fir_buf[1] + filter->fir_length[1]);
    filter->extrapolation_len_iters = extrapolation_len_iters;

    // If at least one stage is in use, calculate important filter variables

    if(filter->fir_length[0] != 0)
    {
        total_fir_len = filter->fir_length[0] + filter->fir_length[1];
        filter->max_meas_value  = 1.1 * (pos > -neg ? pos : -neg);

        // Set filter delay

        filter_delay = 0.5 * (REG_float)(total_fir_len - (filter->fir_length[1] > 0 ? 2 : 1));

        // Calculate REG_float/integer scalings for FIR filter stages
    
        filter->float_to_integer = INT32_MAX / (filter->fir_length[0] * filter->max_meas_value);
        filter->integer_to_float = 1.0 / filter->float_to_integer;

        if(filter->fir_length[1] == 0)
        {
            filter->integer_to_float /= (REG_float)filter->fir_length[0];
        }
        else
        {
            filter->integer_to_float /= (REG_float)filter->fir_length[1];
        }

        // Initialise the FIR filter stages

        filter->fir_accumulator[0] = filter->fir_accumulator[1] = 0;
    
        memset(filter->fir_buf[0], 0, total_fir_len * sizeof(int32_t));
    
        // Initialise FIR filter stages to the value in filter->signal[REG_MEAS_UNFILTERED]
    
        while(total_fir_len--)
        {
            filter->signal[REG_MEAS_FILTERED] = regMeasFirFilterRT(filter);
        }
    }
    else
    {
        filter->signal[REG_MEAS_FILTERED] = filter->signal[REG_MEAS_UNFILTERED];
        filter_delay = 0.0;
    }

    // Set measurement delays


    filter->delay_iters[REG_MEAS_UNFILTERED]   = meas_delay_iters;
    filter->delay_iters[REG_MEAS_FILTERED]     = meas_delay_iters + filter_delay;
    filter->delay_iters[REG_MEAS_EXTRAPOLATED] = 0.0;

    // Calculate extrapolation factor so that it cancels the filtered measurement delay

    if(extrapolation_len_iters > 0)
    {
        filter->extrapolation_factor = filter->delay_iters[REG_MEAS_FILTERED] / (REG_float)extrapolation_len_iters;

        // Initialise extrapolation buffer

        while(extrapolation_len_iters--)
        {
            *(extrapolation_buf++) = filter->signal[REG_MEAS_FILTERED];
        }
    }

    filter->signal[REG_MEAS_EXTRAPOLATED] = filter->signal[REG_MEAS_FILTERED];

    // Restart the filter

    filter->is_running = true;
}



void regMeasSetNoiseAndTone(struct REG_noise_and_tone *noise_and_tone, REG_float noise_pp,
                            REG_float tone_pp, uint32_t tone_period_iters)
{
    // Store peak-peak noise level

    noise_and_tone->noise_pp = noise_pp;

    // For the tone, if the period is not an even number of iterations, then the tone will be an square wave with a
    // asymmetric mark-space ratio.  The positive and negative offsets of the square wave are adjusted so that the
    // average offset will be zero.

    if(tone_period_iters > 1 && tone_pp > 0.0)
    {
        noise_and_tone->iter_counter_start =  tone_period_iters / 2;
        noise_and_tone->iter_counter_end   = -(tone_period_iters + 1) / 2;
        noise_and_tone->tone_positive      = -tone_pp * (REG_float)noise_and_tone->iter_counter_end   / (REG_float)(tone_period_iters);
        noise_and_tone->tone_negative      = -tone_pp * (REG_float)noise_and_tone->iter_counter_start / (REG_float)(tone_period_iters);
    }
    else
    {
        // Set iter_counter_start to deactivate the tone

        noise_and_tone->iter_counter_start = 0;
    }
}



// Real-Time Functions

static REG_float regMeasFirFilterRT(struct REG_meas_filter *filter)
{
    int32_t input_integer;
    REG_float   input_meas = filter->signal[REG_MEAS_UNFILTERED];

    // Clip unfiltered input measurement value to avoid crazy roll-overs in the integer stage

    if(input_meas > filter->max_meas_value)
    {
        input_meas = filter->max_meas_value;
    }
    else if(input_meas < -filter->max_meas_value)
    {
        input_meas = -filter->max_meas_value;
    }

    // Filter stage 1

    input_integer = (int32_t)(filter->float_to_integer * input_meas);

    filter->fir_accumulator[0] += (input_integer - filter->fir_buf[0][filter->fir_index[0]]);

    filter->fir_buf[0][filter->fir_index[0]] = input_integer;

    // Do not use modulus (%) operator to wrap fir_index as it is very slow in TMS320C32 DSP

    if(++filter->fir_index[0] >= filter->fir_length[0])
    {
        filter->fir_index[0] = 0;
    }

    // Return immediately if second filter stage is not in use

    if(filter->fir_length[1] == 0)
    {
        return(filter->integer_to_float * (REG_float)filter->fir_accumulator[0]);
    }

    // Filter stage 2

    input_integer = filter->fir_accumulator[0] / (int32_t)filter->fir_length[0];

    filter->fir_accumulator[1] += (input_integer - filter->fir_buf[1][filter->fir_index[1]]);

    filter->fir_buf[1][filter->fir_index[1]] = input_integer;

    // Do not use modulus (%) operator to wrap fir_index as it is very slow in TMS320C32 DSP

    if(++filter->fir_index[1] >= filter->fir_length[1])
    {
        filter->fir_index[1] = 0;
    }

    // Convert filter output back to floating point

    return(filter->integer_to_float * (REG_float)filter->fir_accumulator[1]);
}



void regMeasFilterRT(struct REG_meas_filter *filter)
{
    REG_float   old_filtered_value;
    REG_float   extrapolated_signal;

    // If filter is stopped

    if(filter->is_running == false)
    {
        // Bypass the filter - simply set the output values to the input value

        filter->signal[REG_MEAS_EXTRAPOLATED] = filter->signal[REG_MEAS_FILTERED] = filter->signal[REG_MEAS_UNFILTERED];
    }
    else // Filter is running
    {
        if(filter->fir_length[0] > 0)
        {
            filter->signal[REG_MEAS_FILTERED] = regMeasFirFilterRT(filter);
        }
        else
        {
            filter->signal[REG_MEAS_FILTERED] = filter->signal[REG_MEAS_UNFILTERED];
        }

        // If required, then extrapolate from filtered measurement to compensate for measurement and filter delay

        extrapolated_signal = filter->signal[REG_MEAS_FILTERED];

        if(filter->extrapolation_len_iters > 0)
        {
            // Prepare to extrapolate to estimate the measurement without a delay

            old_filtered_value = filter->extrapolation_buf[filter->extrapolation_index];

            filter->extrapolation_buf[filter->extrapolation_index] = filter->signal[REG_MEAS_FILTERED];

            // Do not use modulus (%) operator to wrap fir_index as it is very slow in TMS320C32 DSP

            if(++filter->extrapolation_index >= filter->extrapolation_len_iters)
            {
                filter->extrapolation_index = 0;
            }

            // Extrapolate from filtered measurement

            extrapolated_signal += filter->extrapolation_factor * (filter->signal[REG_MEAS_FILTERED] - old_filtered_value);
        }

        filter->signal[REG_MEAS_EXTRAPOLATED] = extrapolated_signal;
    }
}



REG_float regMeasWhiteNoiseRT(REG_float noise_pp)
{
    static uint32_t  noise_random_generator = 0x8E35B19C;   // Use fixed initial seed

    // Only calculate random noise if peak-peak noise level if positive

    if(noise_pp > 0.0)
    {
        // Use efficient 32-bit pseudo-random number generator to calculate the roughly white noise

        noise_random_generator = (noise_random_generator << 16) +
                               (((noise_random_generator >> 12) ^ (noise_random_generator >> 15)) & 0x0000FFFF);

        // Return noise in the range -noise_pp/2 to +noise_pp/2

        return(noise_pp * (REG_float)((int32_t)noise_random_generator) / 4294967296.0);
    }

    // Return zero if noise_pp is zero or negative

    return(0.0);
}



REG_float regMeasNoiseAndToneRT(struct REG_noise_and_tone *noise_and_tone)
{
    REG_float   tone;           // Square wave tone with zero average value

    // Use efficient square tone generator to create tone

    if(noise_and_tone->iter_counter_start > 0)
    {
        tone = noise_and_tone->iter_counter > 0 ? noise_and_tone->tone_positive : noise_and_tone->tone_negative;

        if(--noise_and_tone->iter_counter <= noise_and_tone->iter_counter_end)
        {
            noise_and_tone->iter_counter_start = 0;
        }
    }
    else
    {
        tone = 0.0;
    }

    // Return sum of noise and tone

    return(regMeasWhiteNoiseRT(noise_and_tone->noise_pp) + tone);
}



void regMeasRateRT(struct REG_meas_rate *meas_rate, REG_float filtered_meas, REG_float inv_period, int32_t period_iters)
{
    REG_float    *history_buf = meas_rate->history_buf;     // Local pointer to history buffer for efficiency
    uint32_t  idx;                                      // Local copy of index of most recent sample

    // Store measurement at the specified period

    if(++meas_rate->iter_counter >= period_iters)
    {
        meas_rate->iter_counter = 0;
        idx = meas_rate->history_index = (meas_rate->history_index + 1) & REG_MEAS_RATE_BUF_MASK;

        history_buf[idx] = filtered_meas;

        // Estimate rate using linear regression through last four samples

        meas_rate->estimate = (2.0 / 20.0) * inv_period * (3.0 * (history_buf[ idx ] -
                                                                  history_buf[(idx - 3) & REG_MEAS_RATE_BUF_MASK]) +
                                                                 (history_buf[(idx - 1) & REG_MEAS_RATE_BUF_MASK]  -
                                                                  history_buf[(idx - 2) & REG_MEAS_RATE_BUF_MASK]));
    }
}

// EOF
