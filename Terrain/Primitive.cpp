#include "Primitive.h"


Primitive::Primitive()
{
}

Primitive::Primitive(int nSamples)
{
	samples = 10;
	
	//radius = pow(2 * sqrt(3.0) * samples, -0.5);
}

Primitive::~Primitive()
{
}