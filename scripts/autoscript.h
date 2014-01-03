/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#ifndef __LIQUID_AUTOSCRIPT_H__
#define __LIQUID_AUTOSCRIPT_H__

typedef struct autoscript_s * autoscript;

// create autoscript generator object
autoscript autoscript_create(char * _type,
                             char _delim);

// parse file
void autoscript_parse(autoscript _q,
                      char * _filename);

void autoscript_print(autoscript _q);

void autoscript_destroy(autoscript _q);


//
// internal methods
//

// parse filename
//  _q              :   generator object
//  _filename       :   name of file
//  _package_name   :   output package name (stripped filename)
void autoscript_parsefilename(autoscript _q,
                              char * _filename,
                              char * _package_name);

// parse file
//  _q              :   generator object
//  _filename       :   name of file
//  _package_name   :   input package name (stripped filename)
void autoscript_parsefile(autoscript _q,
                          char * _filename,
                          char * _package_name);

void autoscript_addpackage(autoscript _q,
                           char * _package_name,
                           char * _filename);

void autoscript_addscript(autoscript _q,
                          char * _package_name,
                          char * _script_name);

#endif // __LIQUID_AUTOSCRIPT_H__

