#ifndef PolyEdge_h
#define PolyEdge_h


#include "CGALHeaders.h"
#include "Primitive.h"
#include "clipper.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>

struct PolyPoint
{
	Point_2 xzPoint;
	double height;

	PolyPoint()
	{
	}

	PolyPoint(Point_2 inPoint)
	{
		xzPoint = inPoint;
		height = 0.0;
	}

	PolyPoint(double x, double y)
	{
		Point_2 temp(x,y);
		xzPoint = temp;
		height = 0.0;
	}

	PolyPoint(Point_2 inPoint, double nHeight)
	{
		xzPoint = inPoint;
		height = nHeight;
	}

	bool operator <(const PolyPoint &p) const {
		return xzPoint.x() < p.xzPoint.x() || (xzPoint.x() == p.xzPoint.x() && xzPoint.y() < p.xzPoint.y());
	}
};

/** PolyEdge consists of two Points **/
struct PolyEdge
{
public:

	PolyPoint start;
	PolyPoint end;
	bool infinite;

	PolyEdge(PolyPoint nStart, PolyPoint nEnd)
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
	PolyPoint inputPoint; //the initial point (river network points)
	PolyPoint center; //might not be needed, is not necessarily the same location as inputPoint

	BoundingBox box; //gives you minX, minY, maxX, and maxY of the bounding box surrounding the polygon.

	bool outerPoly; //keeps track if the polygon is a polygon that's part of the contour, in this instance, the inputPoint is part of the pointList;
	std::vector<PolyEdge> edgeList;
	std::vector<PolyPoint> pointList;
	std::vector<PolyPoint> sortedPointList;
	std::vector<PolyEdge> sortedEdgeList;

	//USE THESE LISTS FOR POINTS AND EDGES!!!
	std::vector<PolyPoint> constrainedPointList; //FINAL LIST TO BE USED
	std::vector<PolyEdge> constrainedEdgeList;   //FINAL LIST TO BE USED

	std::vector<Primitive> primitiveList;

	int index;

	int samples;
	double diskDistance;

	PolyFace();
	PolyFace(PolyPoint nInputPoint);
	void AddEdge(PolyEdge nEdge);
	void AddPoint(PolyPoint nPoint);
	void AddPrimitives(int iterations);
	void SetCenter(PolyPoint nPoint);
	void SetIndex(int n);
	void BuildBoundingBox();
	void BuildPointList();
	void BuildSortedEdgeList();
	void BuildConstrainedEdgeList();
	bool RayEdgeIntersect(PolyEdge first, PolyEdge second, double &resultX, double &resultY);


	void ClipPolygon(PolyFace contour);

	//double TwoDCross(Point_2 one, Point_2 two);

	bool IsInPolygon(PolyPoint testPoint);
	PolyPoint GeneratePoissonPoint();

	void PrintEdges();	
	void PrintPointList();
	
	std::vector<PolyPoint> CreateConvexHull(std::vector<PolyPoint> P);
	double cross(const Point_2 &O, const Point_2 &A, const Point_2 &B);
	//bool PolyFace::compare(double x, double y, Point_2 &p);

	//NEEDED FOR CLIPPING
	ClipperLib::Clipper clipper;
	
};


#endif