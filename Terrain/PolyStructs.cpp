#include "PolyStructs.h"

PolyFace::PolyFace()
{

}

PolyFace::PolyFace(Point_2 nInputPoint)
{
	inputPoint = nInputPoint;
	samples = 10;
}


void PolyFace::AddEdge(PolyEdge nEdge)
{
	edgeList.push_back(nEdge);
}

void PolyFace::AddPoint(Point_2 nPoint)
{
	pointList.push_back(nPoint);
	box.SetX(nPoint.x());
	box.SetY(nPoint.y());
}

void PolyFace::AddPrimitives()
{
	int counter = 0;
	while (primitiveList.size() != samples)
	{
		Primitive p(samples);
		//primitiveList.push_back(GeneratePoint());
	}


}

Point_2 PolyFace::GeneratePoint()
{
	std::default_random_engine xGen;
	std::default_random_engine yGen;

	//use the average of the bounds as the mean (THIS MIGHT NOT BE CORRECT)
	std::poisson_distribution<float> xDist((box.maxX + box.minX)/2.0);
	std::poisson_distribution<float> yDist((box.maxY + box.minY)/2.0);


	float xCoord = xDist(xGen) + box.minX; //add min since generator minimum point is 0;
	float yCoord = yDist(yGen) + box.minY; //add min since generator minimum point is 0;

	Point_2 sample(xCoord, yCoord);

	return sample;
}

void PolyFace::SetCenter(Point_2 nPoint)
{
	center = nPoint;
}

void PolyFace::SetIndex(int n)
{
	index = n;
}

void PolyFace::PrintEdges()
{
	std::cout << "Poly with input point at: " << inputPoint.x() << "," << inputPoint.y() << std::endl;
	for (int i = 0; i < edgeList.size(); i++)
	{
		std::cout << "Edge beginning at: " << edgeList[i].start.x() << "," << edgeList[i].start.y() << std::endl;
		std::cout << "Edge ending at: " << edgeList[i].end.y() << "," << edgeList[i].end.y() << std::endl;
		std::cout << std::endl;
	}
}

void PolyFace::BuildPointList()
{
	//loop through all edges in this poly
	for (int i = 0; i < edgeList.size(); i++)
	{
		Point_2 start = edgeList[i].start;
		Point_2 end = edgeList[i].end;

		//loop through all points currently in the list, if point is already there, move on.
		bool startExists = false;
		bool endExists = false;
		for (int j = 0; j < pointList.size(); j++)
		{
			if (pointList[j] == start)
				startExists = true;

			if (pointList[j] == end)
				endExists = true;

			if (startExists && endExists)
				break;
		}

		if (!startExists)
			AddPoint(start);

		if (!endExists)
			AddPoint(end);


		//pointList.push_back(e
	}
	
	//lastly, if the polygon is an initial point, add the inputPoint to the pointList.
	if (outer)
		AddPoint(inputPoint);
}


void PolyFace::PrintPointList()
{
	std::cout << "Poly inputPoint: " << inputPoint << " is outer? " << outer << std::endl;
	//for (int i = 0; i < pointList.size(); i++)
	//{
	//	std::cout << pointList[i].x() << "," << pointList[i].y() << std::endl;
	//}


}

