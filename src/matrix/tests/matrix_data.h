/*
 * Copyright (c) 2013 Joseph Gaeddert
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
// autotest matrix data definitions
//

#ifndef __LIQUID_MATRIX_DATA_H__
#define __LIQUID_MATRIX_DATA_H__

// 
// single-precision floating-point data
//

// add
extern float matrixf_data_add_x[];
extern float matrixf_data_add_y[];
extern float matrixf_data_add_z[];

// aug
extern float matrixf_data_aug_x[];
extern float matrixf_data_aug_y[];
extern float matrixf_data_aug_z[];

// cgsolve
extern float matrixf_data_cgsolve_A[];
extern float matrixf_data_cgsolve_x[];
extern float matrixf_data_cgsolve_b[];

// chol
extern float matrixf_data_chol_A[];
extern float matrixf_data_chol_L[];

// gramschmidt
extern float matrixf_data_gramschmidt_A[];
extern float matrixf_data_gramschmidt_V[];

// inv
extern float matrixf_data_inv_x[];
extern float matrixf_data_inv_y[];

// linsolve
extern float matrixf_data_linsolve_A[];
extern float matrixf_data_linsolve_x[];
extern float matrixf_data_linsolve_b[];

// ludecomp
extern float matrixf_data_ludecomp_A[];

// mul
extern float matrixf_data_mul_x[];
extern float matrixf_data_mul_y[];
extern float matrixf_data_mul_z[];

// qrdecomp
extern float matrixf_data_qrdecomp_A[];
extern float matrixf_data_qrdecomp_Q[];
extern float matrixf_data_qrdecomp_R[];

#endif // __LIQUID_MATRIX_DATA_H__

