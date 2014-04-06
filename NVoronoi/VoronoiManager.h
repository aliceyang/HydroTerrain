#pragma once

#ifndef VoronoiManager_h
#define VoronoiManager_h

#include "Voronoi.h"
#include "VPoint.h"

class VoronoiManager
{
public:
	VoronoiManager();
	~VoronoiManager();

	void AddPoint(double x, double y);
	void GetEdges();
	void PrintEdges();

	vor::Voronoi * v;
	vor::Vertices * ver; // vrcholy
	vor::Vertices * dir; // smìry, kterými se pohybují
	vor::Edges * edg;	 // hrany diagramu
};

#endif