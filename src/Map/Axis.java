/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/* Copyright 2010 Kenneth 'Impaler' Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


public enum Axis {
    AXIS_Z ((byte)0),	// Vertical
    AXIS_Y ((byte)1),	// North/South
    AXIS_X ((byte)2);	// East/West

    private final byte value;
    Axis(byte Index) {
            this.value = Index;
    }
}

