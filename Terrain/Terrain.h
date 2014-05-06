// includes for defining the Voronoi diagram adaptor
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Delaunay_triangulation_2.h>

#include <vector>

#include "CGALHeaders.h"
#include "PolyStructs.h"
#include "clipper.hpp"


class Terrain
{
public:
	Terrain();
	
	~Terrain();

	//METHODS
	void AddPoint(double x, double y);
	bool DuplicatePoint(double x, double y);
	void DoVoronoi();
	void Terrain::print_endpoint(Halfedge_handle e, bool is_src);
	//void DoTriangulation();

	void PrintAllPoints();
	void PrintPolyFaces();

	void CreatePrimitives();

	//VARIABLES
	VD vd;  //THIS IS THE VORONOI OBJECT
	Polygon_set_2 initialPolySet;
	Polygon_2 initialPoly;
	CDT cdt;

	std::vector<PolyFace> polyFaces;

	std::vector<PolyPoint> initialPoints; //initial points that are fed into Voronoi diagram.
	std::vector<PolyPoint> allPoints;

	PolyFace contour;
	int iterations;


};

