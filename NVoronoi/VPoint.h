#ifndef VPoint_h
#define VPoint_h

/*
	A structure that stores 2D point
*/

struct VPoint
{
public:
    
	double x, y;


	/*
		Constructor for structure; x, y - coordinates
	*/

    VPoint(double nx, double ny) 
	{
		x = nx; 
		y = ny;
	}

	double GetX(void) const { return x; };

	double GetY(void) const { return y; };

	void  Set(double nx,double ny)
	{
		x = nx;
		y = ny;
	};

	bool operator== (VPoint other)
	{
		if ((x == other.x) && (y == other.y))
		{
			return true;
		}

		return false;
	}

	bool operator!= (VPoint other)
	{
		if ((x != other.x) || (y != other.y))
		{
			return true;
		}

		return false;
	}
};

struct VEdgePoint
{
	double x,y;
	int index;

	VEdgePoint(double nx, double ny)
	{
		x = nx; 
		y = ny;
	}

	VEdgePoint(double nx, double ny, int nindex)
	{
		x = nx; 
		y = ny;
		index = nindex;
	}

	double GetX(void) const { return x; };

	double GetY(void) const { return y; };

	void  Set(double nx, double ny)
	{	
		x = nx;
		y = ny;
	};
};

#endif