/*!
 * @file  pars.h
 * @brief Converter Control Regulation library generated parameters header file
 *
 * IMPORTANT - DO NOT EDIT - This file is generated from libreg/parameters/pars.csv
 *
 * All libreg parameters are defined in pars.csv and this is transformed into
 * this header file by libreg/parameters/pars.awk.
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
 *
 * <h2>Parameter initialization template</h2>
 *
 * To use libreg, the calling program should initialize the parameter value
 * pointers to access the parameter variables in the calling program. Here
 * is an example template for all the libreg parameters, which assumes that
 * the reg_conv struct is called conv. Copy this into your initialization
 * function and for each parameter that you want to control, replace
 * REG_PAR_NOT_USED with the address of your variable.

    regConvParInitPointer(&conv,reg_err_rate                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_period_iters             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_pure_delay_periods       ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_track_delay_periods      ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_auxpole1_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_auxpoles2_hz             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_auxpoles2_z              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_auxpole4_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_auxpole5_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_r                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_s                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_t                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_test_r                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_test_s                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,breg_test_t                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_period_iters             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_pure_delay_periods       ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_track_delay_periods      ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_auxpole1_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_auxpoles2_hz             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_auxpoles2_z              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_auxpole4_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_auxpole5_hz              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_r                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_s                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_t                        ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_test_r                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_test_s                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,ireg_test_t                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_pos                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_min                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_neg                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_rate                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_acceleration         ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_closeloop            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_low                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_zero                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_err_warning          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_b_err_fault            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_pos                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_min                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_neg                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rate                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_acceleration         ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_closeloop            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_low                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_zero                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_err_warning          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_err_fault            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_tc               ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_warning          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_fault            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_load_tc          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_load_warning     ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_rms_load_fault       ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_i_quadrants41          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_pos                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_neg                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_rate                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_acceleration         ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_err_warning          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_err_fault            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_v_quadrants41          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,limits_invert                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_ohms_ser                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_ohms_par                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_ohms_mag                 ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_henrys                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_henrys_sat               ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_i_sat_start              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_i_sat_end                ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_gauss_per_amp            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_sim_tc_error             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_select                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,load_test_select              ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_b_reg_select             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_i_reg_select             ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_b_delay_iters            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_i_delay_iters            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_v_delay_iters            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_b_fir_lengths            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_i_fir_lengths            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_b_sim_noise_pp           ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_i_sim_noise_pp           ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_v_sim_noise_pp           ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_tone_half_period_iters   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_b_sim_tone_amp           ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,meas_i_sim_tone_amp           ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_actuation                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_act_delay_iters            ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_bandwidth                  ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_z                          ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_tau_zero                   ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_sim_num                    ,REG_PAR_NOT_USED);
    regConvParInitPointer(&conv,pc_sim_den                    ,REG_PAR_NOT_USED);

 * If a parameter is not used by your program, you can delete the macro
 * and the default value from pars.csv will be used instead.
 * If you want a fixed value for a parameter that is different to the
 * default value in pars.csv, you can set it using another macro:
 *
 *  regConvParInitValue(&conv,{parameter_name},{array_index},{initial_value});
 *
 * For example, if a given application will always work with the actuation
 * set to REG_CURRENT_REF, then this can be done using:

    regConvParInitValue(&conv,global_actuation,0,REG_CURRENT_REF);
 */

#ifndef LIBREG_PARS_H
#define LIBREG_PARS_H

#define REG_NUM_PARS                   95
#define REG_PAR_NOT_USED              (void*)0

#define regConvParInitPointer(conv,par_name,value_p)        (conv)->pars.par_name.value=value_p
#define regConvParInitValue(conv,par_name,index,init_value) (conv)->par_values.par_name[index]=init_value

#define REG_LOAD_SELECT                         (1u<<0)
#define REG_TEST_PAR                            (1u<<1)
#define REG_MODE_NONE_ONLY                      (1u<<2)
#define REG_FIELD_REG                           (1u<<3)
#define REG_CURRENT_REG                         (1u<<4)
#define REG_PAR_SIM_VS                          (1u<<5)
#define REG_PAR_INVERT_LIMITS                   (1u<<6)
#define REG_PAR_V_LIMITS_REF                    (1u<<7)
#define REG_PAR_V_LIMITS_ERR                    (1u<<8)
#define REG_PAR_I_LIMITS_MEAS                   (1u<<9)
#define REG_PAR_I_LIMITS_REF                    (1u<<10)
#define REG_PAR_I_LIMITS_ERR                    (1u<<11)
#define REG_PAR_B_LIMITS_MEAS                   (1u<<12)
#define REG_PAR_B_LIMITS_REF                    (1u<<13)
#define REG_PAR_B_LIMITS_ERR                    (1u<<14)
#define REG_PAR_I_LIMITS_RMS                    (1u<<15)
#define REG_PAR_I_LIMITS_RMS_LOAD               (1u<<16)
#define REG_PAR_I_MEAS_REG_SELECT               (1u<<17)
#define REG_PAR_I_MEAS_FILTER                   (1u<<18)
#define REG_PAR_B_MEAS_REG_SELECT               (1u<<19)
#define REG_PAR_B_MEAS_FILTER                   (1u<<20)
#define REG_PAR_MEAS_SIM_DELAYS                 (1u<<21)
#define REG_PAR_MEAS_SIM_NOISE_AND_TONE         (1u<<22)
#define REG_PAR_LOAD                            (1u<<23)
#define REG_PAR_LOAD_SAT                        (1u<<24)
#define REG_PAR_LOAD_SIM                        (1u<<25)
#define REG_PAR_IREG                            (1u<<26)
#define REG_PAR_BREG                            (1u<<27)
#define REG_PAR_LOAD_TEST                       (1u<<28)
#define REG_PAR_LOAD_SAT_TEST                   (1u<<29)
#define REG_PAR_IREG_TEST                       (1u<<30)
#define REG_PAR_BREG_TEST                       (1u<<31)

struct reg_par
{
    void                     *value;
    void                     *copy_of_value;
    uint32_t                  size_in_bytes;
    uint32_t                  sizeof_type;
    uint32_t                  flags;
};

struct reg_pars
{
    struct reg_par            reg_err_rate;
    struct reg_par            breg_period_iters;
    struct reg_par            breg_pure_delay_periods;
    struct reg_par            breg_track_delay_periods;
    struct reg_par            breg_auxpole1_hz;
    struct reg_par            breg_auxpoles2_hz;
    struct reg_par            breg_auxpoles2_z;
    struct reg_par            breg_auxpole4_hz;
    struct reg_par            breg_auxpole5_hz;
    struct reg_par            breg_r;
    struct reg_par            breg_s;
    struct reg_par            breg_t;
    struct reg_par            breg_test_r;
    struct reg_par            breg_test_s;
    struct reg_par            breg_test_t;
    struct reg_par            ireg_period_iters;
    struct reg_par            ireg_pure_delay_periods;
    struct reg_par            ireg_track_delay_periods;
    struct reg_par            ireg_auxpole1_hz;
    struct reg_par            ireg_auxpoles2_hz;
    struct reg_par            ireg_auxpoles2_z;
    struct reg_par            ireg_auxpole4_hz;
    struct reg_par            ireg_auxpole5_hz;
    struct reg_par            ireg_r;
    struct reg_par            ireg_s;
    struct reg_par            ireg_t;
    struct reg_par            ireg_test_r;
    struct reg_par            ireg_test_s;
    struct reg_par            ireg_test_t;
    struct reg_par            limits_b_pos;
    struct reg_par            limits_b_min;
    struct reg_par            limits_b_neg;
    struct reg_par            limits_b_rate;
    struct reg_par            limits_b_acceleration;
    struct reg_par            limits_b_closeloop;
    struct reg_par            limits_b_low;
    struct reg_par            limits_b_zero;
    struct reg_par            limits_b_err_warning;
    struct reg_par            limits_b_err_fault;
    struct reg_par            limits_i_pos;
    struct reg_par            limits_i_min;
    struct reg_par            limits_i_neg;
    struct reg_par            limits_i_rate;
    struct reg_par            limits_i_acceleration;
    struct reg_par            limits_i_closeloop;
    struct reg_par            limits_i_low;
    struct reg_par            limits_i_zero;
    struct reg_par            limits_i_err_warning;
    struct reg_par            limits_i_err_fault;
    struct reg_par            limits_i_rms_tc;
    struct reg_par            limits_i_rms_warning;
    struct reg_par            limits_i_rms_fault;
    struct reg_par            limits_i_rms_load_tc;
    struct reg_par            limits_i_rms_load_warning;
    struct reg_par            limits_i_rms_load_fault;
    struct reg_par            limits_i_quadrants41;
    struct reg_par            limits_v_pos;
    struct reg_par            limits_v_neg;
    struct reg_par            limits_v_rate;
    struct reg_par            limits_v_acceleration;
    struct reg_par            limits_v_err_warning;
    struct reg_par            limits_v_err_fault;
    struct reg_par            limits_v_quadrants41;
    struct reg_par            limits_invert;
    struct reg_par            load_ohms_ser;
    struct reg_par            load_ohms_par;
    struct reg_par            load_ohms_mag;
    struct reg_par            load_henrys;
    struct reg_par            load_henrys_sat;
    struct reg_par            load_i_sat_start;
    struct reg_par            load_i_sat_end;
    struct reg_par            load_gauss_per_amp;
    struct reg_par            load_sim_tc_error;
    struct reg_par            load_select;
    struct reg_par            load_test_select;
    struct reg_par            meas_b_reg_select;
    struct reg_par            meas_i_reg_select;
    struct reg_par            meas_b_delay_iters;
    struct reg_par            meas_i_delay_iters;
    struct reg_par            meas_v_delay_iters;
    struct reg_par            meas_b_fir_lengths;
    struct reg_par            meas_i_fir_lengths;
    struct reg_par            meas_b_sim_noise_pp;
    struct reg_par            meas_i_sim_noise_pp;
    struct reg_par            meas_v_sim_noise_pp;
    struct reg_par            meas_tone_half_period_iters;
    struct reg_par            meas_b_sim_tone_amp;
    struct reg_par            meas_i_sim_tone_amp;
    struct reg_par            pc_actuation;
    struct reg_par            pc_act_delay_iters;
    struct reg_par            pc_bandwidth;
    struct reg_par            pc_z;
    struct reg_par            pc_tau_zero;
    struct reg_par            pc_sim_num;
    struct reg_par            pc_sim_den;
};

struct reg_par_values
{
    enum reg_err_rate         reg_err_rate                  [1];
    uint32_t                  breg_period_iters             [1];
    float                     breg_pure_delay_periods       [2];
    float                     breg_track_delay_periods      [2];
    float                     breg_auxpole1_hz              [2];
    float                     breg_auxpoles2_hz             [2];
    float                     breg_auxpoles2_z              [2];
    float                     breg_auxpole4_hz              [2];
    float                     breg_auxpole5_hz              [2];
    float                     breg_r                        [REG_NUM_RST_COEFFS];
    float                     breg_s                        [REG_NUM_RST_COEFFS];
    float                     breg_t                        [REG_NUM_RST_COEFFS];
    float                     breg_test_r                   [REG_NUM_RST_COEFFS];
    float                     breg_test_s                   [REG_NUM_RST_COEFFS];
    float                     breg_test_t                   [REG_NUM_RST_COEFFS];
    uint32_t                  ireg_period_iters             [1];
    float                     ireg_pure_delay_periods       [2];
    float                     ireg_track_delay_periods      [2];
    float                     ireg_auxpole1_hz              [2];
    float                     ireg_auxpoles2_hz             [2];
    float                     ireg_auxpoles2_z              [2];
    float                     ireg_auxpole4_hz              [2];
    float                     ireg_auxpole5_hz              [2];
    float                     ireg_r                        [REG_NUM_RST_COEFFS];
    float                     ireg_s                        [REG_NUM_RST_COEFFS];
    float                     ireg_t                        [REG_NUM_RST_COEFFS];
    float                     ireg_test_r                   [REG_NUM_RST_COEFFS];
    float                     ireg_test_s                   [REG_NUM_RST_COEFFS];
    float                     ireg_test_t                   [REG_NUM_RST_COEFFS];
    float                     limits_b_pos                  [1];
    float                     limits_b_min                  [1];
    float                     limits_b_neg                  [1];
    float                     limits_b_rate                 [1];
    float                     limits_b_acceleration         [1];
    float                     limits_b_closeloop            [1];
    float                     limits_b_low                  [1];
    float                     limits_b_zero                 [1];
    float                     limits_b_err_warning          [1];
    float                     limits_b_err_fault            [1];
    float                     limits_i_pos                  [1];
    float                     limits_i_min                  [1];
    float                     limits_i_neg                  [1];
    float                     limits_i_rate                 [1];
    float                     limits_i_acceleration         [1];
    float                     limits_i_closeloop            [1];
    float                     limits_i_low                  [1];
    float                     limits_i_zero                 [1];
    float                     limits_i_err_warning          [1];
    float                     limits_i_err_fault            [1];
    float                     limits_i_rms_tc               [1];
    float                     limits_i_rms_warning          [1];
    float                     limits_i_rms_fault            [1];
    float                     limits_i_rms_load_tc          [1];
    float                     limits_i_rms_load_warning     [1];
    float                     limits_i_rms_load_fault       [1];
    float                     limits_i_quadrants41          [2];
    float                     limits_v_pos                  [1];
    float                     limits_v_neg                  [1];
    float                     limits_v_rate                 [1];
    float                     limits_v_acceleration         [1];
    float                     limits_v_err_warning          [1];
    float                     limits_v_err_fault            [1];
    float                     limits_v_quadrants41          [2];
    enum reg_enabled_disabled limits_invert                 [1];
    float                     load_ohms_ser                 [2];
    float                     load_ohms_par                 [2];
    float                     load_ohms_mag                 [2];
    float                     load_henrys                   [2];
    float                     load_henrys_sat               [2];
    float                     load_i_sat_start              [2];
    float                     load_i_sat_end                [2];
    float                     load_gauss_per_amp            [2];
    float                     load_sim_tc_error             [1];
    uint32_t                  load_select                   [1];
    uint32_t                  load_test_select              [1];
    enum reg_meas_select      meas_b_reg_select             [1];
    enum reg_meas_select      meas_i_reg_select             [1];
    float                     meas_b_delay_iters            [1];
    float                     meas_i_delay_iters            [1];
    float                     meas_v_delay_iters            [1];
    uint32_t                  meas_b_fir_lengths            [2];
    uint32_t                  meas_i_fir_lengths            [2];
    float                     meas_b_sim_noise_pp           [1];
    float                     meas_i_sim_noise_pp           [1];
    float                     meas_v_sim_noise_pp           [1];
    uint32_t                  meas_tone_half_period_iters   [1];
    float                     meas_b_sim_tone_amp           [1];
    float                     meas_i_sim_tone_amp           [1];
    enum reg_actuation        pc_actuation                  [1];
    float                     pc_act_delay_iters            [1];
    float                     pc_bandwidth                  [1];
    float                     pc_z                          [1];
    float                     pc_tau_zero                   [1];
    float                     pc_sim_num                    [REG_NUM_PC_SIM_COEFFS];
    float                     pc_sim_den                    [REG_NUM_PC_SIM_COEFFS];
};

#endif // LIBREG_PARS_H

// EOF
