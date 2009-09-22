#include <stdafx.h>

#include <Vector3.h>


Vector3::Vector3( float x_, float y_, float z_ )
{
	x = x_;
	y = y_;
	z = z_;
}

Vector3::~Vector3()
{
}

void Vector3::set( float x_, float y_, float z_ )
{
	x = x_;
	y = y_;
	z = z_;
}

float Vector3::length( void )
{
	return((float) sqrt( x * x + y * y + z * z ));
}

void Vector3::normalize( void )
{
	float fLength = length();

	x = x / fLength;
	y = y / fLength;
	z = z / fLength;
}

// Static utility methods...

static float distance( const Vector3 &v1,  const Vector3 &v2 )
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;

	return (float)sqrt( dx * dx + dy * dy + dz * dz );
}

static float dotProduct( const Vector3 &v1,  const Vector3 &v2 )
{
	return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z  );
}

Vector3 Vector3::crossProduct( const Vector3 &other )
{
	Vector3 New;

	New.x = y * other.z - z * other.y;
	New.y = z * other.x - x * other.z;
	New.z = x * other.y - y * other.x;

	return New;
}

float Vector3::innerProduct( Vector3 &other )
{
	return (x * other.x + y * other.y + z * other.z);
}

// Operators...

Vector3 Vector3::operator + ( const Vector3 &other )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = x + other.x;
	vResult.y = y + other.y;
	vResult.z = z + other.z;

	return vResult;
}

Vector3 Vector3::operator + ( void ) const
{
	return *this;
}

Vector3 Vector3::operator - ( const Vector3 &other )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = x - other.x;
	vResult.y = y - other.y;
	vResult.z = z - other.z;

	return vResult;
}

Vector3 Vector3::operator - ( void ) const
{
	Vector3 vResult(-x, -y, -z);

	return vResult;
}

Vector3 Vector3::operator * ( const Vector3 &other )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = x * other.x;
	vResult.y = y * other.y;
	vResult.z = z * other.z;

	return vResult;
}

Vector3 Vector3::operator * ( const float scalar )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = x * scalar;
	vResult.y = y * scalar;
	vResult.z = z * scalar;

	return vResult;
}

Vector3 operator * ( const float scalar, const Vector3 &other )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = other.x * scalar;
	vResult.y = other.y * scalar;
	vResult.z = other.z * scalar;

	return vResult;
}

Vector3 Vector3::operator / ( const Vector3 &other )
{
	Vector3 vResult(0.0f, 0.0f, 0.0f);

	vResult.x = x / other.x;
	vResult.y = y / other.y;
	vResult.z = z / other.z;

	return vResult;
}

Vector3& Vector3::operator = ( const Vector3 &other )
{
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}

Vector3& Vector3::operator += ( const Vector3 &other )
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

Vector3& Vector3::operator -= ( const Vector3 &other )
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}
