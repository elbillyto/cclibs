/*!
 * @file  libref/transitions.h
 * @brief Converter Control Reference Manager library state machine transitions
 *
 * The reference state machine is the at the centre of the operation of the
 * reference manager library. The state machine is implemented as a limited
 * Petri net (http://en.wikipedia.org/wiki/Petri_net). Transitions only lead
 * to one state, though some transition come from multiple states. Only one
 * token is in the state machine so the actual state can be represented by
 * an enum value.
 *
 * This file is
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

#ifndef TRANSITIONS_H
#define TRANSITIONS_H

#include <libref.h>

/*!
 * Reference state transition function declarations
 */
bool refTransXXtoOF    (struct REF_mgr *ref_mgr);           //  0  ANY             to   OFF
bool refTransXXtoTO    (struct REF_mgr *ref_mgr);           //  1  ANY             to   TO_OFF
bool refTransXXtoTS    (struct REF_mgr *ref_mgr);           //  2  ANY             to   TO_STANDBY
bool refTransXXtoTC    (struct REF_mgr *ref_mgr);           //  3  ANY             to   TO_CYCLING
bool refTransXXtoTI    (struct REF_mgr *ref_mgr);           //  4  ANY             to   TO_IDLE
bool refTransXXtoDT    (struct REF_mgr *ref_mgr);           //  5  ANY             to   DIRECT
bool refTransOFtoPS    (struct REF_mgr *ref_mgr);           //  6  OFF             to   POL_SWITCHING
bool refTransOFtoDT    (struct REF_mgr *ref_mgr);           //  7  OFF             to   DIRECT
bool refTransOFtoTS    (struct REF_mgr *ref_mgr);           //  8  OFF             to   TO_STANDBY
bool refTransOFtoTC    (struct REF_mgr *ref_mgr);           //  9  OFF             to   TO_CYCLING
bool refTransOFtoTI    (struct REF_mgr *ref_mgr);           // 10  OFF             to   TO_IDLE
bool refTransPStoOF    (struct REF_mgr *ref_mgr);           // 11  POL_SWITCHING   to   OFF
bool refTransPStoDT    (struct REF_mgr *ref_mgr);           // 12  POL_SWITCHING   to   DIRECT
bool refTransPStoCY    (struct REF_mgr *ref_mgr);           // 13  POL_SWITCHING   to   CYCLING
bool refTransTStoSB    (struct REF_mgr *ref_mgr);           // 14  TO_STANDBY      to   ON_STANDBY
bool refTransTCtoCY    (struct REF_mgr *ref_mgr);           // 15  TO_CYCLING      to   CYCLING
bool refTransTItoIL    (struct REF_mgr *ref_mgr);           // 16  TO_IDLE         to   IDLE
bool refTransDTtoPS    (struct REF_mgr *ref_mgr);           // 17  DIRECT          to   POL_SWITCHING
bool refTransSBtoIL    (struct REF_mgr *ref_mgr);           // 18  ON_STANDBY      to   IDLE
bool refTransCYtoPS    (struct REF_mgr *ref_mgr);           // 19  CYCLING         to   POL_SWITCHING
bool refTransCYtoTC    (struct REF_mgr *ref_mgr);           // 20  CYCLING         to   TO_CYCLING
bool refTransCYtoEC    (struct REF_mgr *ref_mgr);           // 21  CYCLING         to   ECONOMY
bool refTransECtoCY    (struct REF_mgr *ref_mgr);           // 22  ECONOMY         to   CYCLING
bool refTransILtoAR    (struct REF_mgr *ref_mgr);           // 23  IDLE            to   ARMED
bool refTransARtoIL    (struct REF_mgr *ref_mgr);           // 24  ARMED           to   IDLE
bool refTransARtoRN    (struct REF_mgr *ref_mgr);           // 25  ARMED           to   RUNNING
bool refTransRNtoTI    (struct REF_mgr *ref_mgr);           // 26  RUNNING         to   TO_IDLE
bool refTransRNtoIL    (struct REF_mgr *ref_mgr);           // 27  RUNNING         to   IDLE
bool refTransRNtoPD    (struct REF_mgr *ref_mgr);           // 28  RUNNING         to   PAUSED
bool refTransPDtoRN    (struct REF_mgr *ref_mgr);           // 29  PAUSED          to   RUNNING

#endif

// EOF
