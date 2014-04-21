#include "VoronoiPoly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sstream>


static const float EPSILON=0.0000000001f;

VoronoiPoly::VoronoiPoly()
{
}

VoronoiPoly::VoronoiPoly(VPoint* vPoint)
{
	this->point = vPoint;
}

void VoronoiPoly::PrintEdgeList()
{
	std::string edgeCounters;
	std::stringstream out;
	for (int i = 0; i < edges.size(); i++)
	{
		//VEdge* test = edges[i];
		out << edges[i]->index << ",";
		//edgeCounters += edges[i]->index + ",";
	}
	edgeCounters = out.str();

	//for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	//{
	//	(*i)->index = counter;
	//	counter++;

	//	std::cout << "START: " << (*i)->start->x << ", " << (*i)->start->y << ".    END: " << (*i)->end->x << ", " << (*i)->end->y << ".     INDEX: " << (*i)->index  << "\n";
	//}

	std::cout << "Point(" << point->x << "," << point->y << ") has edges with these counters: " << edgeCounters.c_str() << std::endl ;
}

void VoronoiPoly::CalculateEdgePoints()
{
	if (edges.size() == 0)
	{
		return;
	}

	else
	{
		int counter = 0;
		for (int i = 0; i < edges.size(); i++)
		{
			VEdgePoint* tempStart = new VEdgePoint(edges[i]->start->x, edges[i]->start->y);
			VEdgePoint* tempEnd = new VEdgePoint(edges[i]->end->x, edges[i]->end->y);

			//vEdgePoints.push_back(tempStart);
			//vEdgePoints.push_back(tempEnd);

			if (!IsDuplicateEdgePoint(tempStart))
			{
				vEdgePoints.push_back(tempStart);
			}

			if (!IsDuplicateEdgePoint(tempEnd))
			{
				vEdgePoints.push_back(tempEnd);
			}
			VEdge* test = edges[i];
			
			//edgeCounters += edges[i]->index + ",";
		}


	}
}

void VoronoiPoly::PrintEdgePoints()
{
	std::string pointList;
	std::stringstream out;
	int vepSize = vEdgePoints.size();
	for (int i = 0; i < vEdgePoints.size(); i++)
	{
		out << vEdgePoints[i]->x << "," << vEdgePoints[i]->y << "..." << std::endl;

	}

	pointList = out.str();

	std::cout << "Point(" << point->x << "," << point->y << ") has these edge points: " << pointList.c_str() << std::endl ;
}

bool VoronoiPoly::IsDuplicateEdgePoint(VEdgePoint* point)
{
	for (int i = 0; i < vEdgePoints.size(); i++)
	{
		if (point->x == vEdgePoints[i]->x &&
			point->y == vEdgePoints[i]->y )
		{
			return true;
		}
	}

	return false;
}

//NOW WORKING ON THIS
void VoronoiPoly::Triangulate()
{
	Vector2dVector a;

	for (int i = 0; i < vEdgePoints.size(); i++)
	{
		a.push_back(vEdgePoints[i]);
	}

	Triangulate::Process(a,triangles);

	int tcount = triangles.size()/3;

  for (int i=0; i<tcount; i++)
  {
    const VEdgePoint* p1 = triangles[i*3+0];
    const VEdgePoint* p2 = triangles[i*3+1];
    const VEdgePoint* p3 = triangles[i*3+2];
    //printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n",i+1,p1->GetX(),p1->GetY(),p2->GetX(),p2->GetY(),p3->GetX(),p3->GetY());

	delete p1, p2, p3;
  }
}


float Triangulate::Area(Vector2dVector &contour)
{

  int n = contour.size();

  float A=0.0f;

  for(int p=n-1,q=0; q<n; p=q++)
  {
    A+= contour[p]->GetX()*contour[q]->GetY() - contour[q]->GetX()*contour[p]->GetY();
  }
  return A*0.5f;
}

   /*
     InsideTriangle decides if a point P is Inside of the triangle
     defined by A, B, C.
   */
bool Triangulate::InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py)

{
  float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  float cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;

  return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
  int p;
  float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

  Ax = contour[V[u]]->GetX();
  Ay = contour[V[u]]->GetY();

  Bx = contour[V[v]]->GetX();
  By = contour[V[v]]->GetY();

  Cx = contour[V[w]]->GetX();
  Cy = contour[V[w]]->GetY();

  if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

  for (p=0;p<n;p++)
  {
    if( (p == u) || (p == v) || (p == w) ) continue;
    Px = contour[V[p]]->GetX();
    Py = contour[V[p]]->GetY();
    if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
  }

  return true;
}

bool Triangulate::Process(Vector2dVector &contour,Vector2dVector &result)
{
  /* allocate and initialize list of Vertices in polygon */

  int n = contour.size();
  if ( n < 3 ) return false;

  int *V = new int[n];

  /* we want a counter-clockwise polygon in V */

  if ( 0.0f < Area(contour) )
    for (int v=0; v<n; v++) V[v] = v;
  else
    for(int v=0; v<n; v++) V[v] = (n-1)-v;

  int nv = n;

  /*  remove nv-2 Vertices, creating 1 triangle every time */
  int count = 2*nv;   /* error detection */

  for(int m=0, v=nv-1; nv>2; )
  {
    /* if we loop, it is probably a non-simple polygon */
    if (0 >= (count--))
    {
      //** Triangulate: ERROR - probable bad polygon!
      return false;
    }

    /* three consecutive vertices in current polygon, <u,v,w> */
    int u = v  ; if (nv <= u) u = 0;     /* previous */
    v = u+1; if (nv <= v) v = 0;     /* new v    */
    int w = v+1; if (nv <= w) w = 0;     /* next     */

    if ( Snip(contour,u,v,w,nv,V) )
    {
      double a,b,c;
	  int s,t;

      /* true names of the vertices */
      a = V[u]; b = V[v]; c = V[w];

      /* output Triangle */
      result.push_back( contour[a] );
      result.push_back( contour[b] );
      result.push_back( contour[c] );

      m++;

      /* remove v from remaining polygon */
      for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

      /* resest error detection counter */
      count = 2*nv;
    }
  }



  delete V;

  return true;
}
