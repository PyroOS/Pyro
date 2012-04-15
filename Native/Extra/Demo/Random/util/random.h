#ifndef __UTIL_RANDOM_H__
#define __UTIL_RANDOM_H__

#include <gui/font.h>
#include <gui/gfxtypes.h>
#include <gui/image.h>
#include <gui/point.h>
#include <gui/rect.h>

#include <util/string.h>
//#include <util/unit.h>
#include <util/datetime.h>

#include <storage/tempfile.h>

#include <cstdlib>
#include <time.h>
#include <float.h>

namespace os
{
class Random
{
public:
	enum
	{
		STRING_ALL,
		STRING_ALPHA,
		STRING_ALPHANUMERIC
	};
	
public:
	Random(bool bSeedRandom=true);
	
	os::font_properties AsFont(bool bAntiAlias=true, bool bShearing=false,bool bRotation=false);
	
	os::DateTime AsDateTime();
	os::String AsString(uint nType=STRING_ALPHANUMERIC,int nWidth=-1);
	os::Color32 AsColor(bool bAlpha=false);

	os::Point AsPoint(os::Point start, os::Point end);
	os::Rect AsRect(os::Rect start,os::Rect end);
	os::IPoint AsIPoint(os::IPoint start, os::IPoint end);
	os::IRect AsIRect(os::IRect start, os::IRect end);
	
		
	float AsFloat(float rangeStart=0.0,float rangeEnd=FLT_MAX);
	double AsDouble();
	int AsInt(int rangeStart=0,int rangeEnd=RAND_MAX);
	
public:	
	static void SeedRandom(int time=-1);	

private:

};

}

#endif











