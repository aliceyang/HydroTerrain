#include "PolyStructs.h"

ClipperLib::Paths subject(1), clip(1), solution;

PolyFace::PolyFace()
{
	srand(time(NULL));
	samples = 5;
}

PolyFace::PolyFace(PolyPoint nInputPoint)
{
	srand(time(NULL));
	inputPoint = nInputPoint;
	samples = 5;
}


void PolyFace::AddEdge(PolyEdge nEdge)
{
	edgeList.push_back(nEdge);
}

void PolyFace::AddPoint(PolyPoint nPoint)
{
	pointList.push_back(nPoint);
	box.SetX(nPoint.xzPoint.x());
	box.SetY(nPoint.xzPoint.y());
}

void PolyFace::AddPrimitives(int iterations)
{	
	double minDistanceX = (box.maxX - box.minX) / samples;
	double minDistanceY = (box.maxY - box.minY) / samples;

	double minDistance = sqrt(minDistanceX * minDistanceX + minDistanceY * minDistanceY);

	PolyPoint genPoint =  GeneratePoissonPoint();

	double xTest = genPoint.xzPoint.x();
	double yTest = genPoint.xzPoint.y();

	//while the point isn't in the polygon, keep creating until it is.
	while (!IsInPolygon(genPoint))
	{
		genPoint = GeneratePoissonPoint();
	}

	//First Primitive
	Primitive firstPrim(genPoint.xzPoint.x(), genPoint.xzPoint.y());	
	primitiveList.push_back(firstPrim);

	//Add rest of points, up to sample N or iterations
	int iterCount = 0;
	while (iterCount < iterations && primitiveList.size() < samples)
	{
		genPoint = GeneratePoissonPoint();
		while (!IsInPolygon(genPoint))
		{
			genPoint = GeneratePoissonPoint();
		}

		//Compare against other points in list, if too close to any, just end loop
		bool tooClose = false;
		for (int i = 0; i < primitiveList.size(); i++)
		{
			double distanceX = primitiveList[i].center.x() - genPoint.xzPoint.x();
			double distanceY = primitiveList[i].center.y() - genPoint.xzPoint.y();

			double distance = sqrt(distanceX * distanceX + distanceY * distanceY);
			if (distance < minDistance)
			{
				tooClose == true;
			}
		}

		if (!tooClose)
		{
			Primitive testPrim(genPoint.xzPoint.x(), genPoint.xzPoint.y());
			primitiveList.push_back(testPrim);
		}

		iterations++;

	}

	//for (int i = 0; i < primitiveList.size(); i++)
	//{
	//	std::cout << primitiveList[i].center.x() << " " << primitiveList[i].center.y() << std::endl;
	//}

	//std::cout << std::endl;


	//while (primitiveList.size() != samples)
	//{
	//	Primitive p(samples);
	//	//primitiveList.push_back(GeneratePoint());
	//}


}

double PolyFace::cross(const Point_2 &O, const Point_2 &A, const Point_2 &B)
{
	return (A.x() - O.x()) * (B.y() - O.y()) - (A.y() - O.y()) * (B.x() - O.x());
}

//bool PolyFace::compare(double x, double y, Point_2 &p)
//{
//	return x < p.x() || (x == p.x() && y < p.y());
//}

std::vector<PolyPoint> PolyFace::CreateConvexHull(std::vector<PolyPoint> P)
{
	int n = P.size(), k = 0;
	//std::vector<Point_2> H(2*n);
	std::vector<PolyPoint> H(2*n);

	sort(P.begin(), P.end());

	// Build lower hull
	for (int i = 0; i < n; ++i) 
	{
		while (k >= 2 && cross(H[k-2].xzPoint, H[k-1].xzPoint, P[i].xzPoint) <= 0)
			k--;
		H[k++] = P[i];
	}
 
	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) 
	{
		while (k >= t && cross(H[k-2].xzPoint, H[k-1].xzPoint, P[i].xzPoint) <= 0)
			k--;
		H[k++] = P[i];
	}

	H.resize(k);
	return H;
}

bool PolyFace::RayEdgeIntersect(PolyEdge first, PolyEdge second, double &resultX, double &resultY)
{
	double p0_x, p0_y, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y;

	p0_x = first.start.xzPoint.x();
	p0_y = first.start.xzPoint.y();
	p1_x = first.end.xzPoint.x();
	p1_y = first.end.xzPoint.y();

	p2_x = second.start.xzPoint.x();
	p2_y = second.start.xzPoint.y();
	p3_x = second.end.xzPoint.x();
	p3_y = second.end.xzPoint.y();


	//float s1_x = first.end.xzPoint.x() - first.start.xzPoint.x(); // x direction of first
	//float s1_y = first.end.xzPoint.y() - first.start.xzPoint.y(); // y direction of first

	//float s2_x = second.end.xzPoint.x() - second.start.xzPoint.x(); // x direction of first
	//float s2_y = second.end.xzPoint.y() - second.start.xzPoint.y(); // y direction of first

	float s1_x = p1_x - p0_x; // x direction of first
	float s1_y = first.end.xzPoint.y() - first.start.xzPoint.y(); // y direction of first

	float s2_x = second.end.xzPoint.x() - second.start.xzPoint.x(); // x direction of first
	float s2_y = second.end.xzPoint.y() - second.start.xzPoint.y(); // y direction of first

	float s, t;

	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
		Point_2 crossPoint(p0_x + (t * s1_x), p0_y + (t * s1_y));
		double x = crossPoint.x();
		double y = crossPoint.y();
		
		resultX = crossPoint.x();
		resultY = crossPoint.y();

        // Collision detected
        //if (i_x != NULL)
        //    *i_x = p0_x + (t * s1_x);
        //if (i_y != NULL)
        //    *i_y = p0_y + (t * s1_y);
        return true;
    }

	return false;
	/*Point_2 sd(first.end.xzPoint.x() - first.start.xzPoint.x(), direction.y() - direction.y());
	Point_2 edgeLine(edge.end.x() - edge.start.x(), edge.end.y() - edge.start.y());

	double edgeStartx = edge.start.x();
	double edgeStarty = edge.start.y();
	double edgeEndX = edge.end.x();
	double edgeEndY = edge.end.y();

	double startx = start.x();
	double starty = start.y();

	if (TwoDCross(sd, edgeLine) == 0)
	{
		return false;
	}

	Point_2 p = start;
	Point_2 r = direction;
	Point_2 q = edge.start;
	Point_2 s = edge.end;

	Point_2 qp(q.x() - p.x(), q.y() - p.y());

	double rsCross = TwoDCross(r,s);

	if (rsCross == 0.0)
		return false;

	double t = TwoDCross(qp, s) / rsCross;

	std::cout << "T VALUE: " << t;
	std::cout << std::endl;

	if (t > 0)
		return true;


*/
	return false; // No collision
	//float s1_x, s1_y, s2_x, s2_y;

	//p1_x = direction.x();
	//p1_y = direction.y();
	//s1_x = direction.x() - start.x();     
	//s1_y = direction.y() - start.y();
	//s2_x = edge.end.x() - edge.start.x();     
	//s2_y = edge.end.y() - edge.start.y();

	//float s, t;
	//s = (-s1_y * (start.x() - edge.start.x()) + s1_x * (start.y() - edge.start.y())) / (-s2_x * s1_y + s1_x * s2_y);
	//t = ( s2_x * (start.y() - edge.start.y()) - s2_y * (start.x() - edge.start.x())) / (-s2_x * s1_y + s1_x * s2_y);

	//if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	//{
	//	// Collision detected
	//	return 1;
	//}

	
	//}
}

void PolyFace::ClipPolygon(PolyFace contour)
{
	ClipperLib::Paths subject(1), clip(1), solution;

	//Build the clipping polygon from the contour
	for (int i = 0; i < contour.sortedPointList.size(); i++)
	{
		int xPt = (int)contour.sortedPointList[i].xzPoint.x();
		int yPt = (int)contour.sortedPointList[i].xzPoint.y();
		clip[0].push_back(ClipperLib::IntPoint(xPt, yPt));

	}

	//Build the subject polygon from the sortedPointList
	for (int i = 0; i < sortedPointList.size(); i++)
	{
		int xPt = (int)sortedPointList[i].xzPoint.x();
		int yPt = (int)sortedPointList[i].xzPoint.y();
		subject[0].push_back(ClipperLib::IntPoint(xPt, yPt));

		//std::cout << xPt << " " << yPt << std::endl;

	}

	clipper.AddPaths(subject, ClipperLib::ptSubject, true);
	clipper.AddPaths(clip, ClipperLib::ptClip, true);
	clipper.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	//Build the constrained point list.
	for (int i = 0; i < solution[0].size(); i++)
	{
		PolyPoint pt(solution[0][i].X, solution[0][i].Y);
		constrainedPointList.push_back(pt);
	}


	BuildBoundingBox();
	//GetVertexHeights();
	//BuildConstrainedEdgeList();

	//for (int i = 0; i < constrainedPointList.size(); i++)
	//{
	//	std::cout << constrainedPointList[i].xzPoint.x() << " " << constrainedPointList[i].xzPoint.y() << std::endl;
	//}

	//std::cout << box.minX << " " << box.minY << " " << box.maxX << " " << box.maxY;

	std::cout << std::endl;

}

void PolyFace::GetVertexHeights(std::vector<PolyPoint> originalPoints)
{
	//At this point, constrained point list should be made.
	for (int i = 0; i < constrainedPointList.size(); i++)
	{
		//PolyPoint A, B, C; //These will hold three closest points, but only really need their heights.
		PolyPoint closest;
		//double aDist, bDist, cDist = 100000.0; //some high value to easily be replaced.
		double aDist = 100000.0;

		double distanceSqr;
		double xDist;
		double yDist;

		//Compare against all original points
		for (int j = 0; j < originalPoints.size(); j++)
		{

			xDist = constrainedPointList[i].xzPoint.x() - originalPoints[j].xzPoint.x();
			yDist = constrainedPointList[i].xzPoint.y() - originalPoints[j].xzPoint.y();

			distanceSqr = xDist * xDist + yDist * yDist;

			if (distanceSqr < aDist)
			{
				aDist = distanceSqr;
				closest = originalPoints[j];
			}

		}

		constrainedPointList[i].height = closest.height + 2.0;


	}
}

void PolyFace::BuildConstrainedEdgeList()
{
	if (constrainedPointList.size() == 0)
		return;

	for (int i = 0; i < constrainedPointList.size(); i++)
	{
		if (i == constrainedPointList.size() - 1)
		{
			PolyPoint start = constrainedPointList[i];
			PolyPoint end = constrainedPointList[0];

			PolyEdge tempEdge(start,end);
			constrainedEdgeList.push_back(tempEdge);
			break;
		}

		PolyPoint start = constrainedPointList[i];
		PolyPoint end = constrainedPointList[i + 1];

		PolyEdge tempEdge(start,end);
		constrainedEdgeList.push_back(tempEdge);
		//Point_2 
	}

}


bool PolyFace::IsInPolygon(PolyPoint testPoint)
{
	//std::cout << xPoint << "   " << yPoint << std::endl;

	//PolyPoint tempPt(xPoint,yPoint);
	PolyEdge tempEdge(testPoint, PolyPoint(1,0));

	//Check if generated point is within primitve
	//Do this by sending ray from point and count how many edges are crossing.
	double crossX;
	double crossY;
	int crossCounter = 0;
	for (int i = 0; i < constrainedEdgeList.size(); i++)
	{
		if (RayEdgeIntersect(tempEdge, constrainedEdgeList[i], crossX, crossY))
		{
			crossCounter++;
		}
	}

	if (crossCounter % 2 == 1)
	{
		//std::cout << " IN POLY ";
		//point = tempPt;
		return true;
		//Primitive test;
	}

	return false;
}

PolyPoint PolyFace::GeneratePoissonPoint()
{

	//generate random X and Y points within limits of bounding box.
	double xPoint = (double)rand() / (double)RAND_MAX;
	xPoint = xPoint * (box.maxX - box.minX) + box.minX;

	double yPoint = (double)rand() / (double)RAND_MAX;
	yPoint = yPoint * (box.maxY - box.minY) + box.minY;

	return PolyPoint(xPoint, yPoint);
}

void PolyFace::SetCenter(PolyPoint nPoint)
{
	center = nPoint;
}

void PolyFace::SetIndex(int n)
{
	index = n;
}

void PolyFace::PrintEdges()
{
	std::cout << "Poly with input point at: " << inputPoint.xzPoint.x() << "," << inputPoint.xzPoint.y() << " outer: " << outerPoly << std::endl;
	for (int i = 0; i < sortedEdgeList.size(); i++)
	{
		std::cout << "Edge beginning at: " << edgeList[i].start.xzPoint.x() << "," << edgeList[i].start.xzPoint.y() << std::endl;
		std::cout << "Edge ending at: " << edgeList[i].end.xzPoint.y() << "," << edgeList[i].end.xzPoint.y() << std::endl;
		std::cout << std::endl;
	}
}

void PolyFace::BuildBoundingBox()
{
	box.minX = box.minY = 1000000.0f;  // set min to a high value, so it is likely to be changed.
	box.maxX = box.maxY = -1000000.0f; // set max to a low value, so it is likely to be changed.

	for (int i = 0; i < constrainedPointList.size(); i++)
	{
		box.SetX(constrainedPointList[i].xzPoint.x());
		box.SetY(constrainedPointList[i].xzPoint.y());
	}
}

void PolyFace::BuildPointList()
{
	//loop through all edges in this poly
	for (int i = 0; i < edgeList.size(); i++)
	{
		PolyPoint start = edgeList[i].start;
		PolyPoint end = edgeList[i].end;

		//if (start.xzPoint.x() == 20 & start.xzPoint.y() == 10)
		//{
		//	std::cout << "IN HERE" << std::endl;
		//}

		//if (end.xzPoint.x() == 20 & end.xzPoint.y() == 10)
		//{
		//	std::cout << "IN HERE" << std::endl;
		//}

		//loop through all points currently in the list, if point is already there, move on.
		bool startExists = false;
		bool endExists = false;
		for (int j = 0; j < pointList.size(); j++)
		{
			if (pointList[j].xzPoint == start.xzPoint)
				startExists = true;

			if (pointList[j].xzPoint == end.xzPoint)
				endExists = true;

			if (startExists && endExists)
				break;
		}

		if (!startExists)
			AddPoint(start);

		if (!endExists)
			AddPoint(end);


	}
	
	//lastly, if the polygon is an initial point, add the inputPoint to the pointList.
	if (outerPoly)
		AddPoint(inputPoint);

	//Set the distance each disk should be from each other
	double bbDiagonal = sqrt((box.maxX - box.minX) * (box.maxX - box.minX) + (box.maxY - box.minY) * (box.maxY - box.minY));
	diskDistance = bbDiagonal / samples;

	sortedPointList = CreateConvexHull(pointList);

	BuildSortedEdgeList();

	//for (int i = 0; i < testPoly.sortedEdgeList.size(); i++)
	//{
	//	std::cout << "Edge Start: " << testPoly.sortedEdgeList[i].start.xzPoint.x() << " " << testPoly.sortedEdgeList[i].start.xzPoint.y() 
	//			  << "   Edge End: "   << testPoly.sortedEdgeList[i].end.xzPoint.x() << " " << testPoly.sortedEdgeList[i].end.xzPoint.y() << std::endl << std::endl;
	//}


}

void PolyFace::BuildSortedEdgeList()
{
	if (sortedPointList.size() == 0)
		return;

	//Goes up to size - 1 because don't need to create an edge starting at the last point (which is the same as the first point)
	for (int i = 0; i < sortedPointList.size() - 1; i++)
	{
		PolyPoint start(sortedPointList[i].xzPoint);
		PolyPoint end(sortedPointList[i + 1].xzPoint);

		PolyEdge tempEdge(start,end);
		sortedEdgeList.push_back(tempEdge);
		//Point_2 
	}
}

void PolyFace::PrintPointList()
{
	//std::cout << "Poly inputPoint: " << inputPoint << " is outer? " << outerPoly << std::endl;
	for (int i = 0; i < pointList.size(); i++)
	{
		std::cout << pointList[i].xzPoint.x() << "," << pointList[i].xzPoint.y() << std::endl;
	}

	std::cout << std::endl;


}

