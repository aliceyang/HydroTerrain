#include "VoronoiManager.h"

VoronoiManager::VoronoiManager()
{
	v = new vor::Voronoi();
	ver = new vor::Vertices();
	dir = new vor::Vertices();

}

VoronoiManager::~VoronoiManager()
{
}

void VoronoiManager::AddPoint(double x, double y)
{
	ver->push_back(new VPoint(x, y)); 
	dir->push_back(new VPoint(0, 0));
}

void VoronoiManager::GetEdges()
{
	edg = v->GetEdges(ver, 50, 50);
}

void VoronoiManager::PrintEdges()
{
	int counter = 0;
	for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	{
		(*i)->index = counter;
		counter++;

		std::cout << "START: " << (*i)->start->x << ", " << (*i)->start->y << ".    END: " << (*i)->end->x << ", " << (*i)->end->y << ".     INDEX: " << (*i)->index  << "\n";
	}
}
	