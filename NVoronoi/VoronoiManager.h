#pragma once

#ifndef VoronoiManager_h
#define VoronoiManager_h

#include "Voronoi.h"
#include "VPoint.h"
#include "VoronoiPoly.h"

//typedef std::vector<EdgeList*> PointEdgeList;

class VoronoiManager
{
public:
	VoronoiManager();
	~VoronoiManager();

	void AddPoint(double x, double y);
	void GetEdges();
	void PrintEdges();
	void PrintEdgeLists();
	void BuildVPolys();

	vor::Voronoi * v;
	vor::Vertices * ver; // vrcholy
	vor::Vertices * dir; // smìry, kterými se pohybují
	vor::Edges * edg;	 // hrany diagramu

	std::vector<VoronoiPoly*> vPolys;
	//std::vector<VoronoiPoly*> VPList;

};


//class VoronoiPoly
//{
//	VoronoiPoly();
//	VoronoiPoly(VPoint* point);
//	~VoronoiPoly();
//
//	VPoint* center;
//	std::vector<VEdge*> edges;
//
//
//};

#endif