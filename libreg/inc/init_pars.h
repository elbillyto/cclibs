/*!
 * @file  init_pars.h
 * @brief Libreg generated parameter initialisation header file
 *
 * IMPORTANT - DO NOT EDIT - This file is generated from libreg/parameters/pars.csv
 *
 * All libreg parameters are defined in pars.csv and this is transformed into
 * this header file (and pars.h) by libreg/parameters/pars.awk.
 *
 * <h2>Contact</h2>
 *
 * cclibs-devs@cern.ch
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2014. This project is released under the GNU Lesser General
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

#ifndef LIBREG_INIT_PARS_H
#define LIBREG_INIT_PARS_H

static void regConvParsInit(struct reg_conv *conv)
{
    uint32_t i;

    conv->pars.reg_err_rate.copy_of_value = &conv->par_values.reg_err_rate;
    conv->pars.reg_err_rate.size_in_bytes = sizeof(conv->par_values.reg_err_rate);
    conv->pars.reg_err_rate.sizeof_type   = sizeof(enum reg_err_rate);
    conv->pars.reg_err_rate.flags         = 0|REG_TEST_PAR|REG_MODE_NONE_ONLY|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.reg_err_rate[0]      = REG_ERR_RATE_REGULATION;

    conv->pars.breg_period_iters.copy_of_value = &conv->par_values.breg_period_iters;
    conv->pars.breg_period_iters.size_in_bytes = sizeof(conv->par_values.breg_period_iters);
    conv->pars.breg_period_iters.sizeof_type   = sizeof(uint32_t);
    conv->pars.breg_period_iters.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_MODE_NONE_ONLY|REG_PAR_B_MEAS_FILTER|REG_PAR_BREG|REG_PAR_BREG_TEST;
    conv->par_values.breg_period_iters[0]      = 10;

    conv->pars.breg_pure_delay_periods.copy_of_value = &conv->par_values.breg_pure_delay_periods;
    conv->pars.breg_pure_delay_periods.size_in_bytes = sizeof(conv->par_values.breg_pure_delay_periods);
    conv->pars.breg_pure_delay_periods.sizeof_type   = sizeof(float);
    conv->pars.breg_pure_delay_periods.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_pure_delay_periods[i]=0;

    conv->pars.breg_track_delay_periods.copy_of_value = &conv->par_values.breg_track_delay_periods;
    conv->pars.breg_track_delay_periods.size_in_bytes = sizeof(conv->par_values.breg_track_delay_periods);
    conv->pars.breg_track_delay_periods.sizeof_type   = sizeof(float);
    conv->pars.breg_track_delay_periods.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_track_delay_periods[i]=0;

    conv->pars.breg_auxpole1_hz.copy_of_value = &conv->par_values.breg_auxpole1_hz;
    conv->pars.breg_auxpole1_hz.size_in_bytes = sizeof(conv->par_values.breg_auxpole1_hz);
    conv->pars.breg_auxpole1_hz.sizeof_type   = sizeof(float);
    conv->pars.breg_auxpole1_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_auxpole1_hz[i]=10;

    conv->pars.breg_auxpoles2_hz.copy_of_value = &conv->par_values.breg_auxpoles2_hz;
    conv->pars.breg_auxpoles2_hz.size_in_bytes = sizeof(conv->par_values.breg_auxpoles2_hz);
    conv->pars.breg_auxpoles2_hz.sizeof_type   = sizeof(float);
    conv->pars.breg_auxpoles2_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_auxpoles2_hz[i]=10;

    conv->pars.breg_auxpoles2_z.copy_of_value = &conv->par_values.breg_auxpoles2_z;
    conv->pars.breg_auxpoles2_z.size_in_bytes = sizeof(conv->par_values.breg_auxpoles2_z);
    conv->pars.breg_auxpoles2_z.sizeof_type   = sizeof(float);
    conv->pars.breg_auxpoles2_z.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_auxpoles2_z[i]=0.50;

    conv->pars.breg_auxpole4_hz.copy_of_value = &conv->par_values.breg_auxpole4_hz;
    conv->pars.breg_auxpole4_hz.size_in_bytes = sizeof(conv->par_values.breg_auxpole4_hz);
    conv->pars.breg_auxpole4_hz.sizeof_type   = sizeof(float);
    conv->pars.breg_auxpole4_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_auxpole4_hz[i]=10.00;

    conv->pars.breg_auxpole5_hz.copy_of_value = &conv->par_values.breg_auxpole5_hz;
    conv->pars.breg_auxpole5_hz.size_in_bytes = sizeof(conv->par_values.breg_auxpole5_hz);
    conv->pars.breg_auxpole5_hz.sizeof_type   = sizeof(float);
    conv->pars.breg_auxpole5_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.breg_auxpole5_hz[i]=10.00;

    conv->pars.breg_r.copy_of_value = &conv->par_values.breg_r;
    conv->pars.breg_r.size_in_bytes = sizeof(conv->par_values.breg_r);
    conv->pars.breg_r.sizeof_type   = sizeof(float);
    conv->pars.breg_r.flags         = 0|REG_FIELD_REG|REG_PAR_BREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_r[i]=0.00;

    conv->pars.breg_s.copy_of_value = &conv->par_values.breg_s;
    conv->pars.breg_s.size_in_bytes = sizeof(conv->par_values.breg_s);
    conv->pars.breg_s.sizeof_type   = sizeof(float);
    conv->pars.breg_s.flags         = 0|REG_FIELD_REG|REG_PAR_BREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_s[i]=0.00;

    conv->pars.breg_t.copy_of_value = &conv->par_values.breg_t;
    conv->pars.breg_t.size_in_bytes = sizeof(conv->par_values.breg_t);
    conv->pars.breg_t.sizeof_type   = sizeof(float);
    conv->pars.breg_t.flags         = 0|REG_FIELD_REG|REG_PAR_BREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_t[i]=0.00;

    conv->pars.breg_test_r.copy_of_value = &conv->par_values.breg_test_r;
    conv->pars.breg_test_r.size_in_bytes = sizeof(conv->par_values.breg_test_r);
    conv->pars.breg_test_r.sizeof_type   = sizeof(float);
    conv->pars.breg_test_r.flags         = 0|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_test_r[i]=0.00;

    conv->pars.breg_test_s.copy_of_value = &conv->par_values.breg_test_s;
    conv->pars.breg_test_s.size_in_bytes = sizeof(conv->par_values.breg_test_s);
    conv->pars.breg_test_s.sizeof_type   = sizeof(float);
    conv->pars.breg_test_s.flags         = 0|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_test_s[i]=0.00;

    conv->pars.breg_test_t.copy_of_value = &conv->par_values.breg_test_t;
    conv->pars.breg_test_t.size_in_bytes = sizeof(conv->par_values.breg_test_t);
    conv->pars.breg_test_t.sizeof_type   = sizeof(float);
    conv->pars.breg_test_t.flags         = 0|REG_TEST_PAR|REG_FIELD_REG|REG_PAR_BREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.breg_test_t[i]=0.00;

    conv->pars.ireg_period_iters.copy_of_value = &conv->par_values.ireg_period_iters;
    conv->pars.ireg_period_iters.size_in_bytes = sizeof(conv->par_values.ireg_period_iters);
    conv->pars.ireg_period_iters.sizeof_type   = sizeof(uint32_t);
    conv->pars.ireg_period_iters.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_MODE_NONE_ONLY|REG_PAR_I_MEAS_FILTER|REG_PAR_IREG|REG_PAR_IREG_TEST;
    conv->par_values.ireg_period_iters[0]      = 10.00;

    conv->pars.ireg_pure_delay_periods.copy_of_value = &conv->par_values.ireg_pure_delay_periods;
    conv->pars.ireg_pure_delay_periods.size_in_bytes = sizeof(conv->par_values.ireg_pure_delay_periods);
    conv->pars.ireg_pure_delay_periods.sizeof_type   = sizeof(float);
    conv->pars.ireg_pure_delay_periods.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_pure_delay_periods[i]=0.00;

    conv->pars.ireg_track_delay_periods.copy_of_value = &conv->par_values.ireg_track_delay_periods;
    conv->pars.ireg_track_delay_periods.size_in_bytes = sizeof(conv->par_values.ireg_track_delay_periods);
    conv->pars.ireg_track_delay_periods.sizeof_type   = sizeof(float);
    conv->pars.ireg_track_delay_periods.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_track_delay_periods[i]=0.00;

    conv->pars.ireg_auxpole1_hz.copy_of_value = &conv->par_values.ireg_auxpole1_hz;
    conv->pars.ireg_auxpole1_hz.size_in_bytes = sizeof(conv->par_values.ireg_auxpole1_hz);
    conv->pars.ireg_auxpole1_hz.sizeof_type   = sizeof(float);
    conv->pars.ireg_auxpole1_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_auxpole1_hz[i]=10.00;

    conv->pars.ireg_auxpoles2_hz.copy_of_value = &conv->par_values.ireg_auxpoles2_hz;
    conv->pars.ireg_auxpoles2_hz.size_in_bytes = sizeof(conv->par_values.ireg_auxpoles2_hz);
    conv->pars.ireg_auxpoles2_hz.sizeof_type   = sizeof(float);
    conv->pars.ireg_auxpoles2_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_auxpoles2_hz[i]=10.00;

    conv->pars.ireg_auxpoles2_z.copy_of_value = &conv->par_values.ireg_auxpoles2_z;
    conv->pars.ireg_auxpoles2_z.size_in_bytes = sizeof(conv->par_values.ireg_auxpoles2_z);
    conv->pars.ireg_auxpoles2_z.sizeof_type   = sizeof(float);
    conv->pars.ireg_auxpoles2_z.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_auxpoles2_z[i]=0.50;

    conv->pars.ireg_auxpole4_hz.copy_of_value = &conv->par_values.ireg_auxpole4_hz;
    conv->pars.ireg_auxpole4_hz.size_in_bytes = sizeof(conv->par_values.ireg_auxpole4_hz);
    conv->pars.ireg_auxpole4_hz.sizeof_type   = sizeof(float);
    conv->pars.ireg_auxpole4_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_auxpole4_hz[i]=10.00;

    conv->pars.ireg_auxpole5_hz.copy_of_value = &conv->par_values.ireg_auxpole5_hz;
    conv->pars.ireg_auxpole5_hz.size_in_bytes = sizeof(conv->par_values.ireg_auxpole5_hz);
    conv->pars.ireg_auxpole5_hz.sizeof_type   = sizeof(float);
    conv->pars.ireg_auxpole5_hz.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.ireg_auxpole5_hz[i]=10.00;

    conv->pars.ireg_r.copy_of_value = &conv->par_values.ireg_r;
    conv->pars.ireg_r.size_in_bytes = sizeof(conv->par_values.ireg_r);
    conv->pars.ireg_r.sizeof_type   = sizeof(float);
    conv->pars.ireg_r.flags         = 0|REG_CURRENT_REG|REG_PAR_IREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_r[i]=0.00;

    conv->pars.ireg_s.copy_of_value = &conv->par_values.ireg_s;
    conv->pars.ireg_s.size_in_bytes = sizeof(conv->par_values.ireg_s);
    conv->pars.ireg_s.sizeof_type   = sizeof(float);
    conv->pars.ireg_s.flags         = 0|REG_CURRENT_REG|REG_PAR_IREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_s[i]=0.00;

    conv->pars.ireg_t.copy_of_value = &conv->par_values.ireg_t;
    conv->pars.ireg_t.size_in_bytes = sizeof(conv->par_values.ireg_t);
    conv->pars.ireg_t.sizeof_type   = sizeof(float);
    conv->pars.ireg_t.flags         = 0|REG_CURRENT_REG|REG_PAR_IREG;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_t[i]=0.00;

    conv->pars.ireg_test_r.copy_of_value = &conv->par_values.ireg_test_r;
    conv->pars.ireg_test_r.size_in_bytes = sizeof(conv->par_values.ireg_test_r);
    conv->pars.ireg_test_r.sizeof_type   = sizeof(float);
    conv->pars.ireg_test_r.flags         = 0|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_test_r[i]=0.00;

    conv->pars.ireg_test_s.copy_of_value = &conv->par_values.ireg_test_s;
    conv->pars.ireg_test_s.size_in_bytes = sizeof(conv->par_values.ireg_test_s);
    conv->pars.ireg_test_s.sizeof_type   = sizeof(float);
    conv->pars.ireg_test_s.flags         = 0|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_test_s[i]=0.00;

    conv->pars.ireg_test_t.copy_of_value = &conv->par_values.ireg_test_t;
    conv->pars.ireg_test_t.size_in_bytes = sizeof(conv->par_values.ireg_test_t);
    conv->pars.ireg_test_t.sizeof_type   = sizeof(float);
    conv->pars.ireg_test_t.flags         = 0|REG_TEST_PAR|REG_CURRENT_REG|REG_PAR_IREG_TEST;
    for(i=0;i<REG_NUM_RST_COEFFS;i++) conv->par_values.ireg_test_t[i]=0.00;

    conv->pars.limits_b_pos.copy_of_value = &conv->par_values.limits_b_pos;
    conv->pars.limits_b_pos.size_in_bytes = sizeof(conv->par_values.limits_b_pos);
    conv->pars.limits_b_pos.sizeof_type   = sizeof(float);
    conv->pars.limits_b_pos.flags         = REG_LOAD_SELECT|REG_MODE_NONE_ONLY|REG_FIELD_REG|REG_PAR_B_LIMITS_MEAS|REG_PAR_B_LIMITS_REF|REG_PAR_B_MEAS_FILTER;
    conv->par_values.limits_b_pos[0]      = 1.00;

    conv->pars.limits_b_min.copy_of_value = &conv->par_values.limits_b_min;
    conv->pars.limits_b_min.size_in_bytes = sizeof(conv->par_values.limits_b_min);
    conv->pars.limits_b_min.sizeof_type   = sizeof(float);
    conv->pars.limits_b_min.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_MEAS;
    conv->par_values.limits_b_min[0]      = 0.00;

    conv->pars.limits_b_neg.copy_of_value = &conv->par_values.limits_b_neg;
    conv->pars.limits_b_neg.size_in_bytes = sizeof(conv->par_values.limits_b_neg);
    conv->pars.limits_b_neg.sizeof_type   = sizeof(float);
    conv->pars.limits_b_neg.flags         = REG_LOAD_SELECT|REG_MODE_NONE_ONLY|REG_FIELD_REG|REG_PAR_B_LIMITS_MEAS|REG_PAR_B_LIMITS_REF|REG_PAR_B_MEAS_FILTER;
    conv->par_values.limits_b_neg[0]      = -1.00;

    conv->pars.limits_b_rate.copy_of_value = &conv->par_values.limits_b_rate;
    conv->pars.limits_b_rate.size_in_bytes = sizeof(conv->par_values.limits_b_rate);
    conv->pars.limits_b_rate.sizeof_type   = sizeof(float);
    conv->pars.limits_b_rate.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_REF;
    conv->par_values.limits_b_rate[0]      = 5.00;

    conv->pars.limits_b_acceleration.copy_of_value = &conv->par_values.limits_b_acceleration;
    conv->pars.limits_b_acceleration.size_in_bytes = sizeof(conv->par_values.limits_b_acceleration);
    conv->pars.limits_b_acceleration.sizeof_type   = sizeof(float);
    conv->pars.limits_b_acceleration.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_REF;
    conv->par_values.limits_b_acceleration[0]      = 1000000.00;

    conv->pars.limits_b_closeloop.copy_of_value = &conv->par_values.limits_b_closeloop;
    conv->pars.limits_b_closeloop.size_in_bytes = sizeof(conv->par_values.limits_b_closeloop);
    conv->pars.limits_b_closeloop.sizeof_type   = sizeof(float);
    conv->pars.limits_b_closeloop.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_REF;
    conv->par_values.limits_b_closeloop[0]      = 0.00;

    conv->pars.limits_b_low.copy_of_value = &conv->par_values.limits_b_low;
    conv->pars.limits_b_low.size_in_bytes = sizeof(conv->par_values.limits_b_low);
    conv->pars.limits_b_low.sizeof_type   = sizeof(float);
    conv->pars.limits_b_low.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_MEAS;
    conv->par_values.limits_b_low[0]      = 0.00;

    conv->pars.limits_b_zero.copy_of_value = &conv->par_values.limits_b_zero;
    conv->pars.limits_b_zero.size_in_bytes = sizeof(conv->par_values.limits_b_zero);
    conv->pars.limits_b_zero.sizeof_type   = sizeof(float);
    conv->pars.limits_b_zero.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_MEAS;
    conv->par_values.limits_b_zero[0]      = 0.00;

    conv->pars.limits_b_err_warning.copy_of_value = &conv->par_values.limits_b_err_warning;
    conv->pars.limits_b_err_warning.size_in_bytes = sizeof(conv->par_values.limits_b_err_warning);
    conv->pars.limits_b_err_warning.sizeof_type   = sizeof(float);
    conv->pars.limits_b_err_warning.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_ERR;
    conv->par_values.limits_b_err_warning[0]      = 0.00;

    conv->pars.limits_b_err_fault.copy_of_value = &conv->par_values.limits_b_err_fault;
    conv->pars.limits_b_err_fault.size_in_bytes = sizeof(conv->par_values.limits_b_err_fault);
    conv->pars.limits_b_err_fault.sizeof_type   = sizeof(float);
    conv->pars.limits_b_err_fault.flags         = REG_LOAD_SELECT|REG_FIELD_REG|REG_PAR_B_LIMITS_ERR;
    conv->par_values.limits_b_err_fault[0]      = 0.00;

    conv->pars.limits_i_pos.copy_of_value = &conv->par_values.limits_i_pos;
    conv->pars.limits_i_pos.size_in_bytes = sizeof(conv->par_values.limits_i_pos);
    conv->pars.limits_i_pos.sizeof_type   = sizeof(float);
    conv->pars.limits_i_pos.flags         = REG_LOAD_SELECT|REG_MODE_NONE_ONLY|REG_PAR_I_LIMITS_MEAS|REG_PAR_I_LIMITS_REF|REG_PAR_I_MEAS_FILTER;
    conv->par_values.limits_i_pos[0]      = 1.00;

    conv->pars.limits_i_min.copy_of_value = &conv->par_values.limits_i_min;
    conv->pars.limits_i_min.size_in_bytes = sizeof(conv->par_values.limits_i_min);
    conv->pars.limits_i_min.sizeof_type   = sizeof(float);
    conv->pars.limits_i_min.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_REF;
    conv->par_values.limits_i_min[0]      = 0.00;

    conv->pars.limits_i_neg.copy_of_value = &conv->par_values.limits_i_neg;
    conv->pars.limits_i_neg.size_in_bytes = sizeof(conv->par_values.limits_i_neg);
    conv->pars.limits_i_neg.sizeof_type   = sizeof(float);
    conv->pars.limits_i_neg.flags         = REG_LOAD_SELECT|REG_MODE_NONE_ONLY|REG_PAR_I_LIMITS_MEAS|REG_PAR_I_LIMITS_REF|REG_PAR_I_MEAS_FILTER;
    conv->par_values.limits_i_neg[0]      = -1.00;

    conv->pars.limits_i_rate.copy_of_value = &conv->par_values.limits_i_rate;
    conv->pars.limits_i_rate.size_in_bytes = sizeof(conv->par_values.limits_i_rate);
    conv->pars.limits_i_rate.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rate.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_REF;
    conv->par_values.limits_i_rate[0]      = 5.00;

    conv->pars.limits_i_acceleration.copy_of_value = &conv->par_values.limits_i_acceleration;
    conv->pars.limits_i_acceleration.size_in_bytes = sizeof(conv->par_values.limits_i_acceleration);
    conv->pars.limits_i_acceleration.sizeof_type   = sizeof(float);
    conv->pars.limits_i_acceleration.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_REF;
    conv->par_values.limits_i_acceleration[0]      = 1000000.00;

    conv->pars.limits_i_closeloop.copy_of_value = &conv->par_values.limits_i_closeloop;
    conv->pars.limits_i_closeloop.size_in_bytes = sizeof(conv->par_values.limits_i_closeloop);
    conv->pars.limits_i_closeloop.sizeof_type   = sizeof(float);
    conv->pars.limits_i_closeloop.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_REF;
    conv->par_values.limits_i_closeloop[0]      = 0.00;

    conv->pars.limits_i_low.copy_of_value = &conv->par_values.limits_i_low;
    conv->pars.limits_i_low.size_in_bytes = sizeof(conv->par_values.limits_i_low);
    conv->pars.limits_i_low.sizeof_type   = sizeof(float);
    conv->pars.limits_i_low.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_MEAS;
    conv->par_values.limits_i_low[0]      = 0.00;

    conv->pars.limits_i_zero.copy_of_value = &conv->par_values.limits_i_zero;
    conv->pars.limits_i_zero.size_in_bytes = sizeof(conv->par_values.limits_i_zero);
    conv->pars.limits_i_zero.sizeof_type   = sizeof(float);
    conv->pars.limits_i_zero.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_MEAS;
    conv->par_values.limits_i_zero[0]      = 0.00;

    conv->pars.limits_i_err_warning.copy_of_value = &conv->par_values.limits_i_err_warning;
    conv->pars.limits_i_err_warning.size_in_bytes = sizeof(conv->par_values.limits_i_err_warning);
    conv->pars.limits_i_err_warning.sizeof_type   = sizeof(float);
    conv->pars.limits_i_err_warning.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_ERR;
    conv->par_values.limits_i_err_warning[0]      = 0.00;

    conv->pars.limits_i_err_fault.copy_of_value = &conv->par_values.limits_i_err_fault;
    conv->pars.limits_i_err_fault.size_in_bytes = sizeof(conv->par_values.limits_i_err_fault);
    conv->pars.limits_i_err_fault.sizeof_type   = sizeof(float);
    conv->pars.limits_i_err_fault.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_ERR;
    conv->par_values.limits_i_err_fault[0]      = 0.00;

    conv->pars.limits_i_rms_tc.copy_of_value = &conv->par_values.limits_i_rms_tc;
    conv->pars.limits_i_rms_tc.size_in_bytes = sizeof(conv->par_values.limits_i_rms_tc);
    conv->pars.limits_i_rms_tc.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_tc.flags         = 0|REG_PAR_I_LIMITS_RMS;
    conv->par_values.limits_i_rms_tc[0]      = 0.00;

    conv->pars.limits_i_rms_warning.copy_of_value = &conv->par_values.limits_i_rms_warning;
    conv->pars.limits_i_rms_warning.size_in_bytes = sizeof(conv->par_values.limits_i_rms_warning);
    conv->pars.limits_i_rms_warning.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_warning.flags         = 0|REG_PAR_I_LIMITS_RMS;
    conv->par_values.limits_i_rms_warning[0]      = 0.00;

    conv->pars.limits_i_rms_fault.copy_of_value = &conv->par_values.limits_i_rms_fault;
    conv->pars.limits_i_rms_fault.size_in_bytes = sizeof(conv->par_values.limits_i_rms_fault);
    conv->pars.limits_i_rms_fault.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_fault.flags         = 0|REG_PAR_I_LIMITS_RMS;
    conv->par_values.limits_i_rms_fault[0]      = 0.00;

    conv->pars.limits_i_rms_load_tc.copy_of_value = &conv->par_values.limits_i_rms_load_tc;
    conv->pars.limits_i_rms_load_tc.size_in_bytes = sizeof(conv->par_values.limits_i_rms_load_tc);
    conv->pars.limits_i_rms_load_tc.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_load_tc.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_RMS_LOAD;
    conv->par_values.limits_i_rms_load_tc[0]      = 0.00;

    conv->pars.limits_i_rms_load_warning.copy_of_value = &conv->par_values.limits_i_rms_load_warning;
    conv->pars.limits_i_rms_load_warning.size_in_bytes = sizeof(conv->par_values.limits_i_rms_load_warning);
    conv->pars.limits_i_rms_load_warning.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_load_warning.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_RMS_LOAD;
    conv->par_values.limits_i_rms_load_warning[0]      = 0.00;

    conv->pars.limits_i_rms_load_fault.copy_of_value = &conv->par_values.limits_i_rms_load_fault;
    conv->pars.limits_i_rms_load_fault.size_in_bytes = sizeof(conv->par_values.limits_i_rms_load_fault);
    conv->pars.limits_i_rms_load_fault.sizeof_type   = sizeof(float);
    conv->pars.limits_i_rms_load_fault.flags         = REG_LOAD_SELECT|REG_PAR_I_LIMITS_RMS_LOAD;
    conv->par_values.limits_i_rms_load_fault[0]      = 0.00;

    conv->pars.limits_i_quadrants41.copy_of_value = &conv->par_values.limits_i_quadrants41;
    conv->pars.limits_i_quadrants41.size_in_bytes = sizeof(conv->par_values.limits_i_quadrants41);
    conv->pars.limits_i_quadrants41.sizeof_type   = sizeof(float);
    conv->pars.limits_i_quadrants41.flags         = 0|REG_PAR_V_LIMITS_REF;
    for(i=0;i<2;i++) conv->par_values.limits_i_quadrants41[i]=0.00;

    conv->pars.limits_v_pos.copy_of_value = &conv->par_values.limits_v_pos;
    conv->pars.limits_v_pos.size_in_bytes = sizeof(conv->par_values.limits_v_pos);
    conv->pars.limits_v_pos.sizeof_type   = sizeof(float);
    conv->pars.limits_v_pos.flags         = REG_LOAD_SELECT|REG_PAR_V_LIMITS_REF;
    conv->par_values.limits_v_pos[0]      = 100.00;

    conv->pars.limits_v_neg.copy_of_value = &conv->par_values.limits_v_neg;
    conv->pars.limits_v_neg.size_in_bytes = sizeof(conv->par_values.limits_v_neg);
    conv->pars.limits_v_neg.sizeof_type   = sizeof(float);
    conv->pars.limits_v_neg.flags         = REG_LOAD_SELECT|REG_PAR_V_LIMITS_REF;
    conv->par_values.limits_v_neg[0]      = -100.00;

    conv->pars.limits_v_rate.copy_of_value = &conv->par_values.limits_v_rate;
    conv->pars.limits_v_rate.size_in_bytes = sizeof(conv->par_values.limits_v_rate);
    conv->pars.limits_v_rate.sizeof_type   = sizeof(float);
    conv->pars.limits_v_rate.flags         = 0|REG_PAR_V_LIMITS_REF;
    conv->par_values.limits_v_rate[0]      = 1000.00;

    conv->pars.limits_v_acceleration.copy_of_value = &conv->par_values.limits_v_acceleration;
    conv->pars.limits_v_acceleration.size_in_bytes = sizeof(conv->par_values.limits_v_acceleration);
    conv->pars.limits_v_acceleration.sizeof_type   = sizeof(float);
    conv->pars.limits_v_acceleration.flags         = 0|REG_PAR_V_LIMITS_REF;
    conv->par_values.limits_v_acceleration[0]      = 1000000.00;

    conv->pars.limits_v_err_warning.copy_of_value = &conv->par_values.limits_v_err_warning;
    conv->pars.limits_v_err_warning.size_in_bytes = sizeof(conv->par_values.limits_v_err_warning);
    conv->pars.limits_v_err_warning.sizeof_type   = sizeof(float);
    conv->pars.limits_v_err_warning.flags         = 0|REG_PAR_V_LIMITS_ERR;
    conv->par_values.limits_v_err_warning[0]      = 0.00;

    conv->pars.limits_v_err_fault.copy_of_value = &conv->par_values.limits_v_err_fault;
    conv->pars.limits_v_err_fault.size_in_bytes = sizeof(conv->par_values.limits_v_err_fault);
    conv->pars.limits_v_err_fault.sizeof_type   = sizeof(float);
    conv->pars.limits_v_err_fault.flags         = 0|REG_PAR_V_LIMITS_ERR;
    conv->par_values.limits_v_err_fault[0]      = 0.00;

    conv->pars.limits_v_quadrants41.copy_of_value = &conv->par_values.limits_v_quadrants41;
    conv->pars.limits_v_quadrants41.size_in_bytes = sizeof(conv->par_values.limits_v_quadrants41);
    conv->pars.limits_v_quadrants41.sizeof_type   = sizeof(float);
    conv->pars.limits_v_quadrants41.flags         = 0|REG_PAR_V_LIMITS_REF;
    for(i=0;i<2;i++) conv->par_values.limits_v_quadrants41[i]=0.00;

    conv->pars.limits_invert.copy_of_value = &conv->par_values.limits_invert;
    conv->pars.limits_invert.size_in_bytes = sizeof(conv->par_values.limits_invert);
    conv->pars.limits_invert.sizeof_type   = sizeof(enum reg_enabled_disabled);
    conv->pars.limits_invert.flags         = 0|REG_PAR_INVERT_LIMITS;
    conv->par_values.limits_invert[0]      = REG_DISABLED;

    conv->pars.load_ohms_ser.copy_of_value = &conv->par_values.load_ohms_ser;
    conv->pars.load_ohms_ser.size_in_bytes = sizeof(conv->par_values.load_ohms_ser);
    conv->pars.load_ohms_ser.sizeof_type   = sizeof(float);
    conv->pars.load_ohms_ser.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD|REG_PAR_LOAD_SIM|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_LOAD_TEST|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.load_ohms_ser[i]=0.50;

    conv->pars.load_ohms_par.copy_of_value = &conv->par_values.load_ohms_par;
    conv->pars.load_ohms_par.size_in_bytes = sizeof(conv->par_values.load_ohms_par);
    conv->pars.load_ohms_par.sizeof_type   = sizeof(float);
    conv->pars.load_ohms_par.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD|REG_PAR_LOAD_SIM|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_LOAD_TEST|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.load_ohms_par[i]=100000000.00;

    conv->pars.load_ohms_mag.copy_of_value = &conv->par_values.load_ohms_mag;
    conv->pars.load_ohms_mag.size_in_bytes = sizeof(conv->par_values.load_ohms_mag);
    conv->pars.load_ohms_mag.sizeof_type   = sizeof(float);
    conv->pars.load_ohms_mag.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD|REG_PAR_LOAD_SIM|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_LOAD_TEST|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.load_ohms_mag[i]=1.00;

    conv->pars.load_henrys.copy_of_value = &conv->par_values.load_henrys;
    conv->pars.load_henrys.size_in_bytes = sizeof(conv->par_values.load_henrys);
    conv->pars.load_henrys.sizeof_type   = sizeof(float);
    conv->pars.load_henrys.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD|REG_PAR_LOAD_SIM|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_LOAD_TEST|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.load_henrys[i]=1.00;

    conv->pars.load_henrys_sat.copy_of_value = &conv->par_values.load_henrys_sat;
    conv->pars.load_henrys_sat.size_in_bytes = sizeof(conv->par_values.load_henrys_sat);
    conv->pars.load_henrys_sat.sizeof_type   = sizeof(float);
    conv->pars.load_henrys_sat.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD_SAT|REG_PAR_LOAD_SIM|REG_PAR_LOAD_SAT_TEST;
    for(i=0;i<2;i++) conv->par_values.load_henrys_sat[i]=1.00;

    conv->pars.load_i_sat_start.copy_of_value = &conv->par_values.load_i_sat_start;
    conv->pars.load_i_sat_start.size_in_bytes = sizeof(conv->par_values.load_i_sat_start);
    conv->pars.load_i_sat_start.sizeof_type   = sizeof(float);
    conv->pars.load_i_sat_start.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD_SAT|REG_PAR_LOAD_SIM|REG_PAR_LOAD_SAT_TEST;
    for(i=0;i<2;i++) conv->par_values.load_i_sat_start[i]=0.00;

    conv->pars.load_i_sat_end.copy_of_value = &conv->par_values.load_i_sat_end;
    conv->pars.load_i_sat_end.size_in_bytes = sizeof(conv->par_values.load_i_sat_end);
    conv->pars.load_i_sat_end.sizeof_type   = sizeof(float);
    conv->pars.load_i_sat_end.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD_SAT|REG_PAR_LOAD_SIM|REG_PAR_LOAD_SAT_TEST;
    for(i=0;i<2;i++) conv->par_values.load_i_sat_end[i]=0.00;

    conv->pars.load_gauss_per_amp.copy_of_value = &conv->par_values.load_gauss_per_amp;
    conv->pars.load_gauss_per_amp.size_in_bytes = sizeof(conv->par_values.load_gauss_per_amp);
    conv->pars.load_gauss_per_amp.sizeof_type   = sizeof(float);
    conv->pars.load_gauss_per_amp.flags         = REG_LOAD_SELECT|REG_TEST_PAR|REG_PAR_LOAD|REG_PAR_LOAD_SIM|REG_PAR_BREG|REG_PAR_LOAD_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.load_gauss_per_amp[i]=1.00;

    conv->pars.load_sim_tc_error.copy_of_value = &conv->par_values.load_sim_tc_error;
    conv->pars.load_sim_tc_error.size_in_bytes = sizeof(conv->par_values.load_sim_tc_error);
    conv->pars.load_sim_tc_error.sizeof_type   = sizeof(float);
    conv->pars.load_sim_tc_error.flags         = 0|REG_PAR_LOAD_SIM;
    conv->par_values.load_sim_tc_error[0]      = 0.00;

    conv->pars.load_select.copy_of_value = &conv->par_values.load_select;
    conv->pars.load_select.size_in_bytes = sizeof(conv->par_values.load_select);
    conv->pars.load_select.sizeof_type   = sizeof(uint32_t);
    conv->pars.load_select.flags         = 0;
    conv->par_values.load_select[0]      = 0.00;

    conv->pars.load_test_select.copy_of_value = &conv->par_values.load_test_select;
    conv->pars.load_test_select.size_in_bytes = sizeof(conv->par_values.load_test_select);
    conv->pars.load_test_select.sizeof_type   = sizeof(uint32_t);
    conv->pars.load_test_select.flags         = 0;
    conv->par_values.load_test_select[0]      = 0.00;

    conv->pars.meas_b_reg_select.copy_of_value = &conv->par_values.meas_b_reg_select;
    conv->pars.meas_b_reg_select.size_in_bytes = sizeof(conv->par_values.meas_b_reg_select);
    conv->pars.meas_b_reg_select.sizeof_type   = sizeof(enum reg_meas_select);
    conv->pars.meas_b_reg_select.flags         = 0|REG_MODE_NONE_ONLY|REG_FIELD_REG|REG_PAR_B_MEAS_REG_SELECT|REG_PAR_BREG|REG_PAR_BREG_TEST;
    conv->par_values.meas_b_reg_select[0]      = REG_MEAS_UNFILTERED;

    conv->pars.meas_i_reg_select.copy_of_value = &conv->par_values.meas_i_reg_select;
    conv->pars.meas_i_reg_select.size_in_bytes = sizeof(conv->par_values.meas_i_reg_select);
    conv->pars.meas_i_reg_select.sizeof_type   = sizeof(enum reg_meas_select);
    conv->pars.meas_i_reg_select.flags         = 0|REG_MODE_NONE_ONLY|REG_CURRENT_REG|REG_PAR_I_MEAS_REG_SELECT|REG_PAR_IREG|REG_PAR_IREG_TEST;
    conv->par_values.meas_i_reg_select[0]      = REG_MEAS_UNFILTERED;

    conv->pars.meas_b_delay_iters.copy_of_value = &conv->par_values.meas_b_delay_iters;
    conv->pars.meas_b_delay_iters.size_in_bytes = sizeof(conv->par_values.meas_b_delay_iters);
    conv->pars.meas_b_delay_iters.sizeof_type   = sizeof(float);
    conv->pars.meas_b_delay_iters.flags         = 0|REG_MODE_NONE_ONLY|REG_FIELD_REG|REG_PAR_B_MEAS_FILTER|REG_PAR_MEAS_SIM_DELAYS|REG_PAR_BREG|REG_PAR_BREG_TEST;
    conv->par_values.meas_b_delay_iters[0]      = 1.30;

    conv->pars.meas_i_delay_iters.copy_of_value = &conv->par_values.meas_i_delay_iters;
    conv->pars.meas_i_delay_iters.size_in_bytes = sizeof(conv->par_values.meas_i_delay_iters);
    conv->pars.meas_i_delay_iters.sizeof_type   = sizeof(float);
    conv->pars.meas_i_delay_iters.flags         = 0|REG_MODE_NONE_ONLY|REG_PAR_I_MEAS_FILTER|REG_PAR_MEAS_SIM_DELAYS|REG_PAR_IREG|REG_PAR_IREG_TEST;
    conv->par_values.meas_i_delay_iters[0]      = 1.30;

    conv->pars.meas_v_delay_iters.copy_of_value = &conv->par_values.meas_v_delay_iters;
    conv->pars.meas_v_delay_iters.size_in_bytes = sizeof(conv->par_values.meas_v_delay_iters);
    conv->pars.meas_v_delay_iters.sizeof_type   = sizeof(float);
    conv->pars.meas_v_delay_iters.flags         = 0|REG_MODE_NONE_ONLY|REG_PAR_MEAS_SIM_DELAYS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.meas_v_delay_iters[0]      = 1.30;

    conv->pars.meas_b_fir_lengths.copy_of_value = &conv->par_values.meas_b_fir_lengths;
    conv->pars.meas_b_fir_lengths.size_in_bytes = sizeof(conv->par_values.meas_b_fir_lengths);
    conv->pars.meas_b_fir_lengths.sizeof_type   = sizeof(uint32_t);
    conv->pars.meas_b_fir_lengths.flags         = 0|REG_MODE_NONE_ONLY|REG_FIELD_REG|REG_PAR_B_MEAS_FILTER|REG_PAR_BREG|REG_PAR_BREG_TEST;
    for(i=0;i<2;i++) conv->par_values.meas_b_fir_lengths[i]=1.00;

    conv->pars.meas_i_fir_lengths.copy_of_value = &conv->par_values.meas_i_fir_lengths;
    conv->pars.meas_i_fir_lengths.size_in_bytes = sizeof(conv->par_values.meas_i_fir_lengths);
    conv->pars.meas_i_fir_lengths.sizeof_type   = sizeof(uint32_t);
    conv->pars.meas_i_fir_lengths.flags         = 0|REG_MODE_NONE_ONLY|REG_PAR_I_MEAS_FILTER|REG_PAR_IREG|REG_PAR_IREG_TEST;
    for(i=0;i<2;i++) conv->par_values.meas_i_fir_lengths[i]=1.00;

    conv->pars.meas_b_sim_noise_pp.copy_of_value = &conv->par_values.meas_b_sim_noise_pp;
    conv->pars.meas_b_sim_noise_pp.size_in_bytes = sizeof(conv->par_values.meas_b_sim_noise_pp);
    conv->pars.meas_b_sim_noise_pp.sizeof_type   = sizeof(float);
    conv->pars.meas_b_sim_noise_pp.flags         = 0|REG_FIELD_REG|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_b_sim_noise_pp[0]      = 0.00;

    conv->pars.meas_i_sim_noise_pp.copy_of_value = &conv->par_values.meas_i_sim_noise_pp;
    conv->pars.meas_i_sim_noise_pp.size_in_bytes = sizeof(conv->par_values.meas_i_sim_noise_pp);
    conv->pars.meas_i_sim_noise_pp.sizeof_type   = sizeof(float);
    conv->pars.meas_i_sim_noise_pp.flags         = 0|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_i_sim_noise_pp[0]      = 0.00;

    conv->pars.meas_v_sim_noise_pp.copy_of_value = &conv->par_values.meas_v_sim_noise_pp;
    conv->pars.meas_v_sim_noise_pp.size_in_bytes = sizeof(conv->par_values.meas_v_sim_noise_pp);
    conv->pars.meas_v_sim_noise_pp.sizeof_type   = sizeof(float);
    conv->pars.meas_v_sim_noise_pp.flags         = 0|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_v_sim_noise_pp[0]      = 0.00;

    conv->pars.meas_tone_half_period_iters.copy_of_value = &conv->par_values.meas_tone_half_period_iters;
    conv->pars.meas_tone_half_period_iters.size_in_bytes = sizeof(conv->par_values.meas_tone_half_period_iters);
    conv->pars.meas_tone_half_period_iters.sizeof_type   = sizeof(uint32_t);
    conv->pars.meas_tone_half_period_iters.flags         = 0|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_tone_half_period_iters[0]      = 10.00;

    conv->pars.meas_b_sim_tone_amp.copy_of_value = &conv->par_values.meas_b_sim_tone_amp;
    conv->pars.meas_b_sim_tone_amp.size_in_bytes = sizeof(conv->par_values.meas_b_sim_tone_amp);
    conv->pars.meas_b_sim_tone_amp.sizeof_type   = sizeof(float);
    conv->pars.meas_b_sim_tone_amp.flags         = 0|REG_FIELD_REG|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_b_sim_tone_amp[0]      = 0.00;

    conv->pars.meas_i_sim_tone_amp.copy_of_value = &conv->par_values.meas_i_sim_tone_amp;
    conv->pars.meas_i_sim_tone_amp.size_in_bytes = sizeof(conv->par_values.meas_i_sim_tone_amp);
    conv->pars.meas_i_sim_tone_amp.sizeof_type   = sizeof(float);
    conv->pars.meas_i_sim_tone_amp.flags         = 0|REG_PAR_MEAS_SIM_NOISE_AND_TONE;
    conv->par_values.meas_i_sim_tone_amp[0]      = 0.00;

    conv->pars.pc_actuation.copy_of_value = &conv->par_values.pc_actuation;
    conv->pars.pc_actuation.size_in_bytes = sizeof(conv->par_values.pc_actuation);
    conv->pars.pc_actuation.sizeof_type   = sizeof(enum reg_actuation);
    conv->pars.pc_actuation.flags         = 0|REG_MODE_NONE_ONLY;
    conv->par_values.pc_actuation[0]      = REG_VOLTAGE_REF;

    conv->pars.pc_act_delay_iters.copy_of_value = &conv->par_values.pc_act_delay_iters;
    conv->pars.pc_act_delay_iters.size_in_bytes = sizeof(conv->par_values.pc_act_delay_iters);
    conv->pars.pc_act_delay_iters.sizeof_type   = sizeof(float);
    conv->pars.pc_act_delay_iters.flags         = 0|REG_MODE_NONE_ONLY|REG_PAR_SIM_VS|REG_PAR_MEAS_SIM_DELAYS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.pc_act_delay_iters[0]      = 1.00;

    conv->pars.pc_bandwidth.copy_of_value = &conv->par_values.pc_bandwidth;
    conv->pars.pc_bandwidth.size_in_bytes = sizeof(conv->par_values.pc_bandwidth);
    conv->pars.pc_bandwidth.sizeof_type   = sizeof(float);
    conv->pars.pc_bandwidth.flags         = 0|REG_PAR_SIM_VS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.pc_bandwidth[0]      = 1000.00;

    conv->pars.pc_z.copy_of_value = &conv->par_values.pc_z;
    conv->pars.pc_z.size_in_bytes = sizeof(conv->par_values.pc_z);
    conv->pars.pc_z.sizeof_type   = sizeof(float);
    conv->pars.pc_z.flags         = 0|REG_PAR_SIM_VS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.pc_z[0]      = 0.90;

    conv->pars.pc_tau_zero.copy_of_value = &conv->par_values.pc_tau_zero;
    conv->pars.pc_tau_zero.size_in_bytes = sizeof(conv->par_values.pc_tau_zero);
    conv->pars.pc_tau_zero.sizeof_type   = sizeof(float);
    conv->pars.pc_tau_zero.flags         = 0|REG_PAR_SIM_VS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    conv->par_values.pc_tau_zero[0]      = 0.00;

    conv->pars.pc_sim_num.copy_of_value = &conv->par_values.pc_sim_num;
    conv->pars.pc_sim_num.size_in_bytes = sizeof(conv->par_values.pc_sim_num);
    conv->pars.pc_sim_num.sizeof_type   = sizeof(float);
    conv->pars.pc_sim_num.flags         = 0|REG_PAR_SIM_VS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<REG_NUM_PC_SIM_COEFFS;i++) conv->par_values.pc_sim_num[i]=0.00;

    conv->pars.pc_sim_den.copy_of_value = &conv->par_values.pc_sim_den;
    conv->pars.pc_sim_den.size_in_bytes = sizeof(conv->par_values.pc_sim_den);
    conv->pars.pc_sim_den.sizeof_type   = sizeof(float);
    conv->pars.pc_sim_den.flags         = 0|REG_PAR_SIM_VS|REG_PAR_IREG|REG_PAR_BREG|REG_PAR_IREG_TEST|REG_PAR_BREG_TEST;
    for(i=0;i<REG_NUM_PC_SIM_COEFFS;i++) conv->par_values.pc_sim_den[i]=0.00;
}

#endif // LIBREG_INIT_PARS_H

// EOF
