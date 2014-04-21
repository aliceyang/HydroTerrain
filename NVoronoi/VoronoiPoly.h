#ifndef VoronoiPoly_h
#define VoronoiPoly_h


#include "VEdge.h"
#include "VPoint.h"
#include <vector>

typedef std::vector< VEdgePoint* > Vector2dVector;

//Will have one EdgeList for each VoronoiPoint that will hold the list of edges around that point.
class VoronoiPoly
{
public:
	VoronoiPoly();
	VoronoiPoly(VPoint* vPoint);
	~VoronoiPoly();

	VPoint* point;
	Vector2dVector triangles;


	std::vector<VEdge*> edges;
	std::vector<VEdgePoint*> vEdgePoints;
	void PrintEdgeList();
	void PrintEdgePoints();
	void CalculateEdgePoints();
	bool IsDuplicateEdgePoint(VEdgePoint* point);

	void Triangulate();
};

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.



class Triangulate
{
public:

  // triangulate a contour/polygon, places results in STL vector
  // as series of triangles.
  static bool Process( Vector2dVector &contour,
                      Vector2dVector &result);

  // compute area of a contour/polygon
  static float Area( Vector2dVector &contour);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py);


private:
  static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);

};

#endif