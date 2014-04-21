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

	VoronoiPoly* tempEdgeList = new VoronoiPoly(new VPoint(x, y));
	vPolys.push_back(tempEdgeList);
	//edgeList->
}

void VoronoiManager::BuildVPolys()
{
	int counter = 0;
	//loop through all edges
	for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	{
		(*i)->index = counter;
		counter++;
		//loop through all the VPolys and add their edges.
		for (std::vector<VoronoiPoly*>::iterator j = vPolys.begin(); j!= vPolys.end(); ++j)
		{

			/*** FOR TESTING***/
			//std::cout << (*i)->left->x << "," << (*i)->left->y << "\n";
			//std::cout << (*j)->point->x << "," << (*j)->point->y << "\n";
			//std::cout << std::endl;


			//check if the left point of current edge matches a point in an edgeList.
			if (((*i)->left->x == (*j)->point->x) &&
				((*i)->left->y == (*j)->point->y))
			{
				(*j)->edges.push_back((*i));
				continue;
			}

			if (((*i)->right->x == (*j)->point->x) &&
				((*i)->right->y == (*j)->point->y))
			{
				(*j)->edges.push_back((*i));
				continue;
			}
		}		
	}

	//loop through all the VPolys and calculate edge points
	for (std::vector<VoronoiPoly*>::iterator j = vPolys.begin(); j!= vPolys.end(); ++j)
	{
		(*j)->CalculateEdgePoints();
	}		




}

void VoronoiManager::GetEdges()
{
	edg = v->GetEdges(ver, 50, 50);

	int counter = 0;
	for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	{
		(*i)->index = counter;
		counter++;
		if( (*i)->start == 0 )
		{
			std::cout << "chybi zacatek hrany!\n";
			continue;
		}
		if( (*i)->end == 0 )
		{
			std::cout << "chybi konec hrany!\n";
			continue;
		}
	}
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
	

void VoronoiManager::PrintEdgeLists()
{
	int counter = 0;
	for (int i = 0; i < vPolys.size(); i++)
	{
		vPolys[i]->PrintEdgeList();
	}
	//std::cout << counter;
}

