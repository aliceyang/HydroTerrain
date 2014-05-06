#ifndef Primitive_h
#define Primitive_h

#include "CGALHeaders.h"
#include <math.h>

class Primitive
{
public:

	float radius;
	Point_2 center;

	Primitive();
	Primitive(double x, double y);
	Primitive(double x, double y, double nRadius);
	~Primitive();
};



#endif