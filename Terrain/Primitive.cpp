#include "Primitive.h"


Primitive::Primitive()
{
}

Primitive::Primitive(double x, double y)
{
	center = Point_2(x,y);
}

Primitive::Primitive(double x, double y, double nRadius)
{
	center = Point_2(x,y);
	radius = nRadius;
}

Primitive::~Primitive()
{
}