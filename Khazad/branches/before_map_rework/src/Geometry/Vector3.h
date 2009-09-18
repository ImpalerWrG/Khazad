#ifndef VECTOR3__HEADER
#define VECTOR3__HEADER

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

#include <stdafx.h>

class Vector3
{
public:

	float x;
	float y;
	float z;

	Vector3(float xArg = 0.0f, float yArg = 0.0f, float zArg = 0.0f);
	~Vector3();

	void set(float xArg, float yArg, float zArg);
	float length(void);
	void normalize(void);

	// Static utility methods
	static float distance(const Vector3 &v1, const Vector3 &v2);

	inline float DotProduct(const Vector3 &v2)		// Calculate The Angle Between The 2 Vectors
	{
		return x * v2.x + y * v2.y + z * v2.z;		// Return The Angle
	}

	Vector3 crossProduct(const Vector3 &v1);

	// Operators...
	Vector3 operator + (const Vector3 &other);
	Vector3 operator - (const Vector3 &other);
	Vector3 operator * (const Vector3 &other);
	Vector3 operator / (const Vector3 &other);

	Vector3 operator * (const float scalar);
	friend Vector3 operator * (const float scalar, const Vector3 &other);

	Vector3& operator = (const Vector3 &other);
	Vector3& operator += (const Vector3 &other);
	Vector3& operator -= (const Vector3 &other);

	Vector3 operator + (void) const;
	Vector3 operator - (void) const;

	float innerProduct(Vector3 &v);
};

#endif // VECTOR3__HEADER
