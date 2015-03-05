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

bool refTransXXtoOF(struct ref_mgr *ref_mgr)
{
    return(ref_mgr->pars.pc_on.value == REG_DISABLED);
}



bool refTransXXtoTO(struct ref_mgr *ref_mgr)
{
    return(ref_mgr->pars.);
}



bool refTransXXtoTS(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoTC(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoTI(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransXXtoDT(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoPS(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoDT(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTS(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTC(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransOFtoTI(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoOF(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoDT(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransPStoCY(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransTStoSB(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransTCtoCY(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransTItoIL(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransDTtoPS(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransSBtoIL(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoPS(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoTC(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransCYtoEC(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransECtoCY(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransILtoAR(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransARtoIL(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransARtoRN(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoTI(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoIL(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransRNtoPD(struct ref_mgr *ref_mgr)
{
    return(false);
}



bool refTransPDtoRN(struct ref_mgr *ref_mgr)
{
    return(false);
}

// EOF
