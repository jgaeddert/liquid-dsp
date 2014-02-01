#!/bin/sh
# Copyright (c) 2007 - 2014 Joseph Gaeddert
#
# This file is part of liquid.
#
# liquid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# liquid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with liquid.  If not, see <http://www.gnu.org/licenses/>.

# 
# bootstrap.sh
#
# This is the bootstrapping script to auto-generate a configure
# script for checking build environments, etc.
#

rm -f config.cache aclocal.m4
aclocal -I./scripts
autoconf
autoheader
#automake --foreign --add-missing
