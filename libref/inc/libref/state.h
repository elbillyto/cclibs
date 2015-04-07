/*!
 * @file  libref/state.h
 * @brief Converter Control Reference Manager library reference state machine functions
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

#ifndef REF_STATE_H
#define REF_STATE_H

#include <libref.h>

/*!
 * Reference state machine
 *
 * This function will drive the reference state machine
 *
 * @param[out] reg_mgr              Pointer to reference manager structure.
 * @retval     true                 State changed on this iteration.
 * @retval     false                State remained unchanged on this iteration.
 */
bool refState(struct REF_mgr *ref_mgr);

#endif

// EOF
