/*!
 * @file  refTrans.c
 * @brief Converter Control Function Manager library state machine transition condition functions
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

bool refTransXXtoOF(struct REF_mgr *ref_mgr)
{
    return(*ref_mgr->u.pars.pc_state.value == REF_PC_OFF);
}



bool refTransXXtoTO(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoTS(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoTC(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoTI(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoDT(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoPS(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoDT(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTS(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTC(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTI(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoOF(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoDT(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoCY(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransTStoSB(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransTCtoCY(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransTItoIL(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransDTtoPS(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransSBtoIL(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoPS(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoTC(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoEC(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransECtoCY(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransILtoAR(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransARtoIL(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransARtoRN(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoTI(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoIL(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoPD(struct REF_mgr *ref_mgr)
{
    return(false);
}



bool refTransPDtoRN(struct REF_mgr *ref_mgr)
{
    return(false);
}

// EOF
