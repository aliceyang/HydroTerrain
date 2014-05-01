#ifndef PolyEdge_h
#define PolyEdge_h


#include "CGALHeaders.h"
#include "Primitive.h"
#include <random>
#include <vector>
#include <math.h>


/** PolyEdge consists of two Points **/
struct PolyEdge
{
public:

	Point_2 start;
	Point_2 end;
	bool infinite;

	PolyEdge(Point_2 nStart, Point_2 nEnd)
	{
		start = nStart;
		end = nEnd;
	}
};

struct BoundingBox
{
public:
	double minX, minY, maxX, maxY;

	BoundingBox()
	{
		minX = minY = 1000000.0f;  // set min to a high value, so it is likely to be changed.
		maxX = maxY = -1000000.0f; // set max to a low value, so it is likely to be changed.
	}

	void SetX(double x)
	{
		if (x <= minX)
			minX = x;

		if (x >= maxX)
			maxX = x;
	}

	void SetY(double y)
	{
		if (y <= minY)
			minY = y;

		if (y >= maxY)
			maxY = y;
	}
};

/** PolyFace has a vector of its surrounding edges and the center voronoi point **/
/** A vector PolyFaces are kept in Terrain. **/
class PolyFace
{
public:
	Point_2 inputPoint;
	Point_2 center;

	BoundingBox box;

	bool outer;
	std::vector<PolyEdge> edgeList;
	std::vector<Point_2> pointList;
	std::vector<Primitive> primitiveList;
	int index;

	int samples;

	PolyFace();
	PolyFace(Point_2 nInputPoint);
	void AddEdge(PolyEdge nEdge);
	void AddPoint(Point_2 nPoint);
	void AddPrimitives();
	void SetCenter(Point_2 nPoint);
	void SetIndex(int n);
	void BuildPointList();
	Point_2 GeneratePoint();

	void PrintEdges();	
	void PrintPointList();



};

#endif