#ifndef Primitive_h
#define Primitive_h

#include "CGALHeaders.h"
#include <math.h>

class Primitive
{
public:

	float radius;
	int samples;


	Primitive();
	Primitive(int nSamples);
	~Primitive();
};



#endif