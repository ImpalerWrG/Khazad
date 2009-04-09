#include <stdafx.h>

#include <Plane.h>

class Vector3;

Plane::Plane()
{
	Normal.set(0,0,0);
	D = 0;
}

Plane::Plane(float x, float y, float z, float d)
{
	Normal.set(x, y, z);
	D = d;
}

Plane::~Plane()
{

}

void Plane::set3Points( Vector3 &v1,  Vector3 &v2,  Vector3 &v3)
{
	Vector3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	Normal = aux2 * aux1;

	Normal.normalize();
	Point = v2;
	D = -(Normal.innerProduct(Point));
}

float Plane::distance(Vector3 &p)
{
	return (D + Normal.innerProduct(p));
}

void Plane::set2Points( Vector3 &normal, Vector3 &point )
{
	Normal = normal;
	Point = point;

	Normal.normalize();
	D = -(Normal.innerProduct(point));
}
