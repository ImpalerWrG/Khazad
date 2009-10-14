#ifndef VECTOR3__HEADER
#define VECTOR3__HEADER


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
