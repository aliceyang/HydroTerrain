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

void print_endpoint(Halfedge_handle e, bool is_src) {
  std::cout << "\t";
  if ( is_src ) {
    if ( e->has_source() )  std::cout << e->source()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  } else {
    if ( e->has_target() )  std::cout << e->target()->point() << std::endl;
    else  std::cout << "point at infinity" << std::endl;
  }
}

int main()
{
  //std::ifstream ifs("data/data1.dt.cin");
  //assert( ifs );

	std::default_random_engine generator;
	std::poisson_distribution<float> distribution (6.5);

	std::vector<float> fifty;

	float test;

	for (int i = 0; i < 50; i++)
	{
		test = distribution(generator);
		fifty.push_back(test - 9.0);
	}

	for (int i = 0; i < 50; i++)
	{
		std::cout << fifty[i] << "  ";
	}
	std::cout << std::endl;

	

	Terrain tp;

	tp.AddPoint(0,0);
	tp.AddPoint(0,4);
	tp.AddPoint(4,4);
	tp.AddPoint(4,0);


	tp.DoVoronoi();
	tp.DoTriangulation();
	//tp.PrintAllPoints();

	//tp.PrintPolyFaces();

	for (int i = 0; i < tp.polyFaces.size(); i++)
	{
		//std::cout << tp.polyFaces[i].box.minX << " " << tp.polyFaces[i].box.minY << " " << tp.polyFaces[i].box.maxX << " " << tp.polyFaces[i].box.maxY << std::endl;
		//std::cout << tp.polyFaces[i].samples << std::endl;
	}
	//std::cout << tp.polyFaces.size();

	//std::cout << "FINISHED TESTING";
	std::string blah;
	std::cin >> blah;

  //ifq.close();



  return 0;
}
