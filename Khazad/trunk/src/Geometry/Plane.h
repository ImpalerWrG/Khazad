#ifndef PLANE__HEADER
#define PLANE__HEADER

#include <stdafx.h>
#include <Vector3.h>

class  Plane
{
public:

	Vector3 Point;
	Vector3 Normal;
	float	D;

	Plane();
	Plane(float x, float y, float z, float d);
	~Plane();

	void set3Points( Vector3 &v1,  Vector3 &v2,  Vector3 &v3);
	void set2Points( Vector3 &normal, Vector3 &point );

	float distance(Vector3 &p);
};

#endif // PLANE__HEADER
