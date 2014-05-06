#include "Terrain.h"


Terrain::Terrain()
{
	iterations = 4;
}

Terrain::~Terrain()
{
}

void Terrain::AddPoint(double x, double y)
{
	//Site_2 point = new Site_2(x,y);
	PolyPoint point(x,y);
	initialPoints.push_back(point);
	//contour.AddPoint(point);
	

	//if (!DuplicatePoint(x,y))
		//allPoints.push_back(point);


}

void Terrain::CreatePrimitives()
{
	//loop through each face and create primitives.
	for (int i = 0; i < polyFaces.size(); i++)
	{
		//polyFaces[i].AddPrimitives();
	}
}

bool Terrain::DuplicatePoint(double x, double y)
{
	for (int i = 0; i < allPoints.size(); i++)
	{
		if (x == allPoints[i].xzPoint.x() && y == allPoints[i].xzPoint.y())
		{
			return true;
		}
	}
	return false;
}

void Terrain::DoVoronoi()
{
	contour.sortedPointList = contour.CreateConvexHull(contour.pointList);

	//for (int i = 0; i < contour.sortedPointList.size(); i++)
	//{
	//
	//	std::cout << "Point: " << contour.sortedPointList[i].xzPoint.x() << " " << contour.sortedPointList[i].xzPoint.y() << std::endl;
	//
	//}

	for (int i = 0; i < initialPoints.size(); i++)
	{
		vd.insert(initialPoints[i].xzPoint);
		PolyFace tempFace(initialPoints[i]);
		tempFace.SetIndex(i);
		polyFaces.push_back(tempFace);
	}

	//add points generated from voronoi into allPoints.
	for (VD::Vertex_iterator i = vd.vertices_begin(); i != vd.vertices_end(); i++)
	{
		//std::cout << i->point() << std::endl;
		if (!DuplicatePoint(i->point().x(), i->point().y()))
			allPoints.push_back(i->point());
	}
	

	//loop through all edges, find the corresponding input point that the edge surrounds.
	for (VD::Edge_iterator e = vd.edges_begin(); e != vd.edges_end(); e++)
	{
		double xPoint;
		double yPoint;


		Point_2 temp(xPoint,yPoint);
		if (!DuplicatePoint(xPoint, yPoint))
			allPoints.push_back(temp);

		//add this edge to corresponding polyFaces
		for (int i = 0; i < polyFaces.size(); i++)
		{
			//if the up or down point is the inputPoint of a polygon, this edge belongs to that polygon.
			if (e->up()->point() == polyFaces[i].inputPoint.xzPoint ||
				e->down()->point() == polyFaces[i].inputPoint.xzPoint)
			{
				//add an edge with start at source, and end at intersection point found above, only if target is not found (meaning the edge ends at infinity)
				if (!e->has_target())
				{					
					xPoint = (e->up()->point().x() + e->down()->point().x()) / 2.0;
					yPoint = (e->up()->point().y() + e->down()->point().y()) / 2.0;


					//NEED TO CHECK IF EDGE TARGET IS PAST THE BOUNDING
					polyFaces[i].AddEdge(PolyEdge(PolyPoint(e->source()->point()), PolyPoint(xPoint,yPoint)));
					polyFaces[i].outerPoly = true;
				}

				//add an edge with end point at an intersection point, and a start point at the intersection of the contour and the line
				else if (!e->has_source())
				{					
					xPoint = (e->up()->point().x() + e->down()->point().x()) / 2.0;
					yPoint = (e->up()->point().y() + e->down()->point().y()) / 2.0;
					polyFaces[i].AddEdge(PolyEdge(Point_2(xPoint,yPoint), e->target()->point()));
					polyFaces[i].outerPoly = true;
				}

				//this edge is not infinite, so just add it to the list.
				else polyFaces[i].AddEdge(PolyEdge(e->source()->point(), e->target()->point()));
			}

		}

		//std::cout << "UP: " << e->up()->point().x() << "," << e->up()->point().y() << std::endl;
		//std::cout << "DOWN: " << e->down()->point().x() << "," << e->down()->point().y() << std::endl;


	}

	//Need to go through all polyFaces and build the vertices list.
	for (int j = 0; j < polyFaces.size(); j++)
	{
		polyFaces[j].BuildPointList();
		polyFaces[j].ClipPolygon(contour);
		polyFaces[j].AddPrimitives(iterations);
	}
	
	
}

void Terrain::print_endpoint(Halfedge_handle e, bool is_src) {
  std::cout << "\t";
  if ( is_src ) {
    if ( e->has_source() )  std::cout << e->source()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  } else {
    if ( e->has_target() )  std::cout << e->target()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  }
}

void Terrain::PrintAllPoints()
{
	for (int i = 0; i < allPoints.size(); i++)
	{
		//std::cout << "Point: " << allPoints[i].xzPoint.x() << "," << allPoints[i]..xzPoint.y() << std::endl;
	}
}

void Terrain::PrintPolyFaces()
{
	for (int i = 0; i < polyFaces.size(); i++)
	{
		polyFaces[i].PrintEdges();
		//polyFaces[i].PrintPointList();
		/*std::cout << "Poly centered: " << allPoints[i].x() << "," << allPoints[i].y() << std::endl;*/
	}
}
