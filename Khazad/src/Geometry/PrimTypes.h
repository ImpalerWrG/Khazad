/*
Peon - Win32 Games Programming Library
Copyright (C) 2002-2005 Erik Yuzwa

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Erik Yuzwa
peon AT wazooinc DOT com
*/

//#include "stdafx.h"

// This scene primitive type is our basic starting point for getting
// geometry rendered to our screen. 
struct DiffusePrim
{
	float XComponent, YComponent, ZComponent;
	float Red, Green, Blue, Alpha;
};

// This scene primitive is a starting point for displaying geometry
// that can contain texture coordinates.
struct DiffuseTexPrim
{
	float XComponent, YComponent, ZComponent;
	float Red, Green, Blue, Alpha;
	/** texture coordinates of this vertex */
	float tu, tv;
};

// This scene primitive is a starting point for displaying any type
// of geometry in our game world. It contains enough information to
// properly help lighting, blending and texture mapping calculations.
struct NormalDiffuseTexPrim
{
	float XComponent, YComponent, ZComponent;
	float Red, Green, Blue, Alpha;
	float XNormal, YNormal, ZNormal;
	/** texture coordinates of this vertex */
	float tu, tv;
};

