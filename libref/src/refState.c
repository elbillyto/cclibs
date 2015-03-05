/*!
 * @file  refState.c
 * @brief Converter Control Function Manager library state machine functions
 *
 * <h2>Copyright</h2>
 *
 * Copyright CERN 2015. This project is released under the GNU Lesser General
 * Public License version 3.
 * 
 * <h2>License</h2>
 *
 * This file is part of libref.
 *
 * libref is free software: you can redistribute it and/or modify it under the
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

#include "libref.h"
#include "libref/transitions.h"

/*!
 * Macros
 */
#define ArrayLen(arr)           ( sizeof arr / sizeof arr[0] )

/*!
 * Reference state function declarations
 */
static void refStateOF  (struct ref_mgr *ref_mgr, bool first_call);     //  0  OFF
static void refStatePS  (struct ref_mgr *ref_mgr, bool first_call);     //  1  POL_SWITCHING
static void refStateTO  (struct ref_mgr *ref_mgr, bool first_call);     //  2  TO_OFF
static void refStateTS  (struct ref_mgr *ref_mgr, bool first_call);     //  3  TO_STANDBY
static void refStateTC  (struct ref_mgr *ref_mgr, bool first_call);     //  4  TO_CYCLING
static void refStateTI  (struct ref_mgr *ref_mgr, bool first_call);     //  5  TO_IDLE
static void refStateDT  (struct ref_mgr *ref_mgr, bool first_call);     //  6  DIRECT
static void refStateSB  (struct ref_mgr *ref_mgr, bool first_call);     //  7  ON_STANDBY
static void refStateCY  (struct ref_mgr *ref_mgr, bool first_call);     //  8  CYCLING
static void refStateEC  (struct ref_mgr *ref_mgr, bool first_call);     //  9  ECONOMY
static void refStateIL  (struct ref_mgr *ref_mgr, bool first_call);     // 10  IDLE
static void refStateAR  (struct ref_mgr *ref_mgr, bool first_call);     // 11  ARMED
static void refStateRN  (struct ref_mgr *ref_mgr, bool first_call);     // 12  RUNNING
static void refStatePD  (struct ref_mgr *ref_mgr, bool first_call);     // 13  PAUSED

/*!
 * Transition function indexes
 */
enum ref_trans_enum
{
    REF_XXtoOF,                     //  0  ANY             to   OFF
    REF_XXtoTO,                     //  1  ANY             to   TO_OFF
    REF_XXtoTS,                     //  2  ANY             to   TO_STANDBY
    REF_XXtoTC,                     //  3  ANY             to   TO_CYCLING
    REF_XXtoTI,                     //  4  ANY             to   TO_IDLE
    REF_XXtoDT,                     //  5  ANY             to   DIRECT
    REF_OFtoPS,                     //  6  OFF             to   POL_SWITCHING
    REF_OFtoDT,                     //  7  OFF             to   DIRECT
    REF_OFtoTS,                     //  8  OFF             to   TO_STANDBY
    REF_OFtoTC,                     //  9  OFF             to   TO_CYCLING
    REF_OFtoTI,                     // 10  OFF             to   TO_IDLE
    REF_PStoOF,                     // 11  POL_SWITCHING   to   OFF
    REF_PStoDT,                     // 12  POL_SWITCHING   to   DIRECT
    REF_PStoCY,                     // 13  POL_SWITCHING   to   CYCLING
    REF_TStoSB,                     // 14  TO_STANDBY      to   ON_STANDBY
    REF_TCtoCY,                     // 15  TO_CYCLING      to   CYCLING
    REF_TItoIL,                     // 16  TO_IDLE         to   IDLE
    REF_DTtoPS,                     // 17  DIRECT          to   POL_SWITCHING
    REF_SBtoIL,                     // 18  ON_STANDBY      to   IDLE
    REF_CYtoPS,                     // 19  CYCLING         to   POL_SWITCHING
    REF_CYtoTC,                     // 20  CYCLING         to   TO_CYCLING
    REF_CYtoEC,                     // 21  CYCLING         to   ECONOMY
    REF_ECtoCY,                     // 22  ECONOMY         to   CYCLING
    REF_ILtoAR,                     // 23  IDLE            to   ARMED
    REF_ARtoIL,                     // 24  ARMED           to   IDLE
    REF_ARtoRN,                     // 25  ARMED           to   RUNNING
    REF_RNtoTI,                     // 26  RUNNING         to   TO_IDLE
    REF_RNtoIL,                     // 27  RUNNING         to   IDLE
    REF_RNtoPD,                     // 28  RUNNING         to   PAUSED
    REF_PDtoRN,                     // 29  PAUSED          to   RUNNING
};

/*!
 * Reference state machine structure
 */
struct ref_fsm_state
{
    void                                (* const state_func)(struct ref_mgr *, bool);       //!< Pointer to ref state function
    uint32_t const                      n_trans;                                            //!< Number of possible transitions from this state
    enum ref_trans_enum const * const   trans;                                              //!< List of possible transitions in priority order
};

struct ref_fsm_trans
{
    bool                                (* const condition)(struct ref_mgr *ref_mgr);       //!< Pointer to transition condition function
    enum ref_state const                next_state;                                         //!< Next state if condition is true
};

/*!
 * Define transitions - this must be in the order defined by enum ref_trans_enum
 */
static struct ref_fsm_trans ref_trans[] =
{
    { refTransXXtoOF, REF_OFF           },
    { refTransXXtoTO, REF_TO_OFF        },
    { refTransXXtoTS, REF_TO_STANDBY    },
    { refTransXXtoTC, REF_TO_CYCLING    },
    { refTransXXtoTI, REF_TO_IDLE       },
    { refTransXXtoDT, REF_DIRECT        },
    { refTransOFtoPS, REF_POL_SWITCHING },
    { refTransOFtoDT, REF_DIRECT        },
    { refTransOFtoTS, REF_TO_STANDBY    },
    { refTransOFtoTC, REF_TO_CYCLING    },
    { refTransOFtoTI, REF_TO_IDLE       },
    { refTransPStoOF, REF_OFF           },
    { refTransPStoDT, REF_DIRECT        },
    { refTransPStoCY, REF_CYCLING       },
    { refTransTStoSB, REF_ON_STANDBY    },
    { refTransTCtoCY, REF_CYCLING       },
    { refTransTItoIL, REF_IDLE          },
    { refTransDTtoPS, REF_POL_SWITCHING },
    { refTransSBtoIL, REF_IDLE          },
    { refTransCYtoPS, REF_POL_SWITCHING },
    { refTransCYtoTC, REF_TO_CYCLING    },
    { refTransCYtoEC, REF_ECONOMY       },
    { refTransECtoCY, REF_CYCLING       },
    { refTransILtoAR, REF_ARMED         },
    { refTransARtoIL, REF_IDLE          },
    { refTransARtoRN, REF_RUNNING       },
    { refTransRNtoTI, REF_TO_IDLE       },
    { refTransRNtoIL, REF_IDLE          },
    { refTransRNtoPD, REF_PAUSED        },
    { refTransPDtoRN, REF_RUNNING       },
};

/*!
 * Transition conditions are checked in order from left to right
 */
static enum ref_trans_enum ref_trans_OF[] = {                                                                                     REF_OFtoPS, REF_OFtoDT, REF_OFtoTS, REF_OFtoTC, REF_OFtoTI };
static enum ref_trans_enum ref_trans_PS[] = {                                                                                     REF_PStoOF, REF_PStoDT, REF_PStoCY                         };
static enum ref_trans_enum ref_trans_TO[] = { REF_XXtoOF,             REF_XXtoTS, REF_XXtoTC, REF_XXtoTI, REF_XXtoDT                                                                         };
static enum ref_trans_enum ref_trans_TS[] = { REF_XXtoOF, REF_XXtoTO,             REF_XXtoTC, REF_XXtoTI, REF_XXtoDT, REF_TStoSB                                                             };
static enum ref_trans_enum ref_trans_TC[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS,             REF_XXtoTI, REF_XXtoDT, REF_TCtoCY                                                             };
static enum ref_trans_enum ref_trans_TI[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC,             REF_XXtoDT, REF_TItoIL                                                             };
static enum ref_trans_enum ref_trans_DT[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC, REF_XXtoTI,             REF_DTtoPS                                                             };
static enum ref_trans_enum ref_trans_SB[] = { REF_XXtoOF, REF_XXtoTO,             REF_XXtoTC,             REF_XXtoDT, REF_SBtoIL                                                             };
static enum ref_trans_enum ref_trans_CY[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS,             REF_XXtoTI, REF_XXtoDT, REF_CYtoPS, REF_CYtoTC, REF_CYtoEC                                     };
static enum ref_trans_enum ref_trans_EC[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS,             REF_XXtoTI, REF_XXtoDT, REF_ECtoCY                                                             };
static enum ref_trans_enum ref_trans_IL[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC,             REF_XXtoDT, REF_ILtoAR                                                             };
static enum ref_trans_enum ref_trans_AR[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC,             REF_XXtoDT, REF_ARtoIL, REF_ARtoRN                                                 };
static enum ref_trans_enum ref_trans_RN[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC,             REF_XXtoDT, REF_RNtoTI, REF_RNtoIL, REF_RNtoPD                                     };
static enum ref_trans_enum ref_trans_PD[] = { REF_XXtoOF, REF_XXtoTO, REF_XXtoTS, REF_XXtoTC, REF_XXtoTI, REF_XXtoDT, REF_PDtoRN                                                             };

/*!
 * Reference state structures - this must match the order of enum ref_state_enum
 */
static struct ref_fsm_state ref_states[] =
{
    { refStateOF, ArrayLen(ref_trans_OF), ref_trans_OF },    //!< REF_OFF
    { refStatePS, ArrayLen(ref_trans_PS), ref_trans_PS },    //!< REF_POL_SWITCHING
    { refStateTO, ArrayLen(ref_trans_TO), ref_trans_TO },    //!< REF_TO_OFF
    { refStateTS, ArrayLen(ref_trans_TS), ref_trans_TS },    //!< REF_TO_STANDBY
    { refStateTC, ArrayLen(ref_trans_TC), ref_trans_TC },    //!< REF_TO_CYCLING
    { refStateTI, ArrayLen(ref_trans_TI), ref_trans_TI },    //!< REF_TO_IDLE
    { refStateDT, ArrayLen(ref_trans_DT), ref_trans_DT },    //!< REF_DIRECT
    { refStateSB, ArrayLen(ref_trans_SB), ref_trans_SB },    //!< REF_ON_STANDBY
    { refStateCY, ArrayLen(ref_trans_CY), ref_trans_CY },    //!< REF_CYCLING
    { refStateEC, ArrayLen(ref_trans_EC), ref_trans_EC },    //!< REF_ECONOMY
    { refStateIL, ArrayLen(ref_trans_IL), ref_trans_IL },    //!< REF_IDLE
    { refStateAR, ArrayLen(ref_trans_AR), ref_trans_AR },    //!< REF_ARMED
    { refStateRN, ArrayLen(ref_trans_RN), ref_trans_RN },    //!< REF_RUNNING
    { refStatePD, ArrayLen(ref_trans_PD), ref_trans_PD },    //!< REF_PAUSED
};



bool refState(struct ref_mgr *ref_mgr)
{
    enum ref_trans_enum const  *trans_idx;
    uint32_t                    num_trans;
    enum ref_state              current_state   = ref_mgr->ref_state;
    enum ref_state              next_state;

    // Scan transitions for the current state, testing each condition in priority order

    trans_idx = ref_states[current_state].trans;
    num_trans = ref_states[current_state].n_trans;

    while(num_trans && ref_trans[*trans_idx].condition(ref_mgr) == false)
    {
        trans_idx++;
        num_trans--;
    }

    // If a condition is true then switch to the new state

    if(num_trans != 0)
    {
        // Run new state function with first_call set to true

        next_state = ref_trans[*trans_idx].next_state;

        ref_states[next_state].state_func(ref_mgr, true);

        // Only update ref_state after the call so the function can know the previous state

        ref_mgr->ref_state = next_state;

        return(true);
    }

    // Stay in current state and run the state function with the first_call parameter set to false

    ref_states[current_state].state_func(ref_mgr, false);

    return(false);
}


// State functions

static void refStateOF(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStatePS(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateTO(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateTS(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateTC(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateTI(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateDT(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateSB(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateCY(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateEC(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateIL(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateAR(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStateRN(struct ref_mgr *ref_mgr, bool first_call)
{

}



static void refStatePD(struct ref_mgr *ref_mgr, bool first_call)
{

}

// EOF
