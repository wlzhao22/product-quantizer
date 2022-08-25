#ifndef PINFO_H
#define PINFO_H

class PInfo
{
public:
	PInfo(const int x, const int y,const int oct)
	{
	    this->x = x;
	    this->y = y;
	    this->octave = oct;
	    angle = 0;
	    scale = 0;
	}
	PInfo(int x, int y)
	{
	    this->x = x;
	    this->y = y;
	    angle = 0;
        scale = 0;
	}
	PInfo()
	{
	     x = 0;
	     y = 0;
	     octave = 0;
	     angle = 0;
	     scale = 0;
     }

	~PInfo()
	{	}
	int x;
	int y;
	int octave;
	float angle, scale;
	float score, iscale;
};

#endif
