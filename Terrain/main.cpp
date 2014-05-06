// standard includes
#include <iostream>
#include <fstream>
#include <cassert>

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

#include <random>
#include <vector>
#include <math.h>

#include "Terrain.h"
#include "Primitive.h"
#include "clipper.hpp"

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

// typedef for the result type of the point location
typedef AT::Site_2                    Site_2;
typedef AT::Point_2                   Point_2;

typedef VD::Locate_result             Locate_result;
typedef VD::Vertex_handle             Vertex_handle;
typedef VD::Face_handle               Face_handle;
typedef VD::Halfedge_handle           Halfedge_handle;
typedef VD::Ccb_halfedge_circulator   Ccb_halfedge_circulator;

//typedef signed long long long64;
//struct DoublePoint {long64 X; long64 Y;};
//typedef std::vector<DoublePoint> Path;
//typedef std::vector<Path> Paths;



int main()
{
  //std::ifstream ifs("data/data1.dt.cin");
  //assert( ifs );

	//std::default_random_engine generator;
	//std::poisson_distribution<float> distribution (6.5);

	//std::vector<float> fifty;

	//float test;

	//for (int i = 0; i < 50; i++)
	//{
	//	test = distribution(generator);
	//	fifty.push_back(test - 9.0);
	//}

	//for (int i = 0; i < 50; i++)
	//{
	//	std::cout << fifty[i] << "  ";
	//}
	//std::cout << std::endl;

	

	Terrain tp;


	//tp.AddPoint(0,0);
	//tp.AddPoint(0,20);
	//tp.AddPoint(20,20);
	//tp.AddPoint(20,0);

	tp.AddPoint(0,0);
	tp.AddPoint(0,20);
	tp.AddPoint(20,20);
	tp.AddPoint(20,0);
	tp.AddPoint(5,10);
	tp.AddPoint(15,10);

	PolyFace contour;
	contour.AddPoint(Point_2(0,0));
	contour.AddPoint(Point_2(0,20));
	contour.AddPoint(Point_2(20,20));
	contour.AddPoint(Point_2(20,0));

	tp.DoVoronoi();
//	tp.DoTriangulation();
	//tp.PrintAllPoints();

	//tp.PrintPolyFaces();

	//for (int i = 0; i < tp.polyFaces.size(); i++)
	//{
	//	std::cout << tp.polyFaces[i].box.minX << " " << tp.polyFaces[i].box.minY << " " << tp.polyFaces[i].box.maxX << " " << tp.polyFaces[i].box.maxY << std::endl;
	//	std::cout << tp.polyFaces[i].samples << std::endl;
	//}
	//std::cout << tp.polyFaces.size();

	//std::cout << "FINISHED TESTING";

	//PolyFace testContour;
	////testContour.AddPoint(Point_2(0,0));
	////testContour.AddPoint(Point_2(0,4));
	////testContour.AddPoint(Point_2(8,2));
	////testContour.AddPoint(Point_2(4,0));
	////testContour.AddPoint(Point_2(10,10));

	//testContour.AddPoint(Point_2(0,0));
	//testContour.AddPoint(Point_2(0,20));
	//testContour.AddPoint(Point_2(20,20));
	//testContour.AddPoint(Point_2(20,0));



	//testContour.sortedPointList = testContour.CreateConvexHull(testContour.pointList);

	//PolyFace testPoly;

	//testPoly.AddPoint(PolyPoint(0,0));
	//testPoly.AddPoint(PolyPoint(0,10));
	//testPoly.AddPoint(PolyPoint(10,10));
	//testPoly.AddPoint(PolyPoint(10,0));
	//testPoly.sortedPointList = testPoly.CreateConvexHull(testPoly.pointList);
	//testPoly.BuildSortedEdgeList();	

	///****TESTING CLIPPER ****////
	//ClipperLib::Clipper c;
	//ClipperLib::Paths subject(1), clip(1), solution;

	//subject[0] << ClipperLib::IntPoint(0,0);
	//subject[0] << ClipperLib::IntPoint(0,10);
	//subject[0] << ClipperLib::IntPoint(10,10);
	//subject[0] << ClipperLib::IntPoint(10,0);

	//clip[0] << ClipperLib::IntPoint(0,0);
	//clip[0] << ClipperLib::IntPoint(0,20);
	//clip[0] << ClipperLib::IntPoint(20,20);
	//clip[0] << ClipperLib::IntPoint(20,0);

	//c.AddPaths(subject, ClipperLib::ptSubject, true);
	//c.AddPaths(clip, ClipperLib::ptClip, true);
	//c.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	//std::cout << solution[0][0].X << " " << solution[0][0].Y << std::endl; 
	//std::cout << solution[0][1].X << " " << solution[0][1].Y << std::endl;
	//std::cout << solution[0][2].X << " " << solution[0][2].Y << std::endl;
	//std::cout << solution[0][3].X << " " << solution[0][3].Y << std::endl;

	//testPoly.ClipPolygon(testContour);

	//for (int i = 0; i < testPoly.constrainedPointList.size(); i++)
	//{
	//	std::cout << testPoly.constrainedPointList[i].xzPoint.x() << " " << testPoly.constrainedPointList[i].xzPoint.y() << std::endl;
	//}

	//for (int i = 0; i < testPoly.sortedEdgeList.size(); i++)
	//{
	//	std::cout << "Edge Start: " << testPoly.sortedEdgeList[i].start.xzPoint.x() << " " << testPoly.sortedEdgeList[i].start.xzPoint.y() 
	//			  << "   Edge End: "   << testPoly.sortedEdgeList[i].end.xzPoint.x() << " " << testPoly.sortedEdgeList[i].end.xzPoint.y() << std::endl << std::endl;
	//}


	///****TESTING CLIPPER ****////

	/***TESTING RAY/EDGE INTERSECTION***/

	//PolyEdge one(Point_2(5,0),Point_2(10,10));
	//PolyEdge two(Point_2(-4,10),Point_2(10,5));

	//double retX;
	//double retY;
	//testContour.RayEdgeIntersect(one,two, retX, retY);

	/***TESTING RAY/EDGE INTERSECTION***/





	//for (int i = 0; i < testContour.sortedEdgeList.size(); i++)
	//{
	//	std::cout << "Edge Start: " << testPoly.sortedEdgeList[i].start.xzPoint.x() << " " << testPoly.sortedEdgeList[i].start.xzPoint.y() 
	//			  << "   Edge End: "   << testPoly.sortedEdgeList[i].end.xzPoint.x() << " " << testPoly.sortedEdgeList[i].end.xzPoint.y() << std::endl << std::endl;
	//}

	//double crossX;
	//double crossY;
	///***for each edge in the sortedEdgeList, compare with each edge in the contour***/
	//for (int i = 0; i < testPoly.sortedEdgeList.size(); i++)
	//{
	//	//for each edge in the contour, find an intersection point
	//	for (int j = 0; j < testContour.sortedEdgeList.size(); j++)
	//	{
	//		bool intersect = testPoly.RayEdgeIntersect(testPoly.sortedEdgeList[i], testContour.sortedEdgeList[j], crossX, crossY);

	//		if (intersect)
	//		{
	//			Point_2 intersectPoint(crossX, crossY);
	//			testPoly.sortedEdgeList[i].end.xzPoint = intersectPoint; /* Change the end point to the intersect point*/
	//			testPoly.sortedEdgeList[i+1].start.xzPoint = intersectPoint; /* Change the start point of the next edge to the intersect point */
	//			break;
	//		}
	//	}
	//}

	////ONLY WORKS IF SORTED
	//for (int i = 0; i < a.size(); i++)
	//{
	//	for (int j = i + 1; j < a.size(); j++)
	//	{
	//		if (a[j].xzPoint.x() == a[i].xzPoint.x() && a[j].xzPoint.y() == a[i].xzPoint.y())
	//		{
	//			a.pop_back();
	//		}
	//	}
	//}

	std::cout << std::endl;

	//for (int i = 0; i < testPoly.sortedEdgeList.size(); i++)
	//{
	//	std::cout << "Edge Start: " << testPoly.sortedEdgeList[i].start.xzPoint.x() << " " << testPoly.sortedEdgeList[i].start.xzPoint.y() 
	//			  << "   Edge End: "   << testPoly.sortedEdgeList[i].end.xzPoint.x() << " " << testPoly.sortedEdgeList[i].end.xzPoint.y() << std::endl << std::endl;
	//}

	///***TESTING CROSS PRODUCT***/
	//Point_2 a(4,2);
	//Point_2 b(2,1);

	//if (tp.polyFaces[1].TwoDCross(a,b) == 0.0)
	//	std::cout << "IT'S ZERO";

	///***TESTING CROSS PRODUCT***/
	//tp.polyFaces[1].AddPrimitives();



	

  std::string a;
  std::cin >> a;
  return 0;
}
