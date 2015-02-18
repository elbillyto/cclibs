/*!
 * @file      libref.h
 * @brief     Reference management library top-level header file
 *
 * <h2>Contact</h2>
 *
 * cclibs-devs@cern.ch
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

#ifndef LIBREF_H
#define LIBREF_H

// Libref enum constants

/*!
 * Reference state
 */
enum ref_state
{
    REF_OFF,
    REF_POL_SWITCHING,
    REF_TO_OFF,
    REF_TO_STANDBY,
    REF_TO_CYCLING,
    REF_TO_IDLE,
    REF_DIRECT,
    REF_ON_STANDBY,
    REF_CYCLING,
    REF_ECONOMY,
    REF_IDLE,
    REF_ARMED,
    REF_RUNNING,
};

// Include all libref header files



#endif // LIBREF_H

// EOF
