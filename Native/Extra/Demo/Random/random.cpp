#include "util/random.h"

using namespace os;

static os::String m_cAlpha = os::String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
static os::String m_cAlphaNum = os::String ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
static os::String m_cAll = os::String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$%^&*()_+|-=\\~[]{};':\",./<>?");


Random::Random(bool bSeedRand)
{
	if (bSeedRand)
		SeedRandom();
}

os::Color32 Random::AsColor(bool bAlpha)
{
	os::Color32_s color;
	
	if (bAlpha)
		color.alpha = AsInt(0,256);
		
	color.red = AsInt(0,256);
	color.blue = AsInt(0,256);
	color.green = AsInt(0,256);
	return color;
}



void Random::SeedRandom(int nTime)
{
	if (nTime == -1)
	{
		srand(time(NULL));
	}
	else
		srand(nTime);
}


int Random::AsInt(int start,int end)
{
	return (rand()%end +start);
}


os::font_properties Random::AsFont(bool bAnti,bool bShear, bool bRotation)
{
	os::font_properties sProps;
	char family[FONT_FAMILY_LENGTH];
	char style[FONT_STYLE_LENGTH];
	uint32 flags=0;
		
	//the number of families
	int num_of_family = os::Font::GetFamilyCount();
	
	//a random family inside that
	int rand_family = AsInt(0,num_of_family);

	//get the family name and then place it in the properties
	os::Font::GetFamilyInfo(rand_family,family);
	sProps.m_cFamily = family;
	
	
	//the number of styles in the family
	int num_of_style=os::Font::GetStyleCount(family);
	
	//a random style
	int rand_style=AsInt(0,num_of_style);
	
	//get the style name and then place it in the properties
	os::Font::GetStyleInfo(family,rand_style,style,&flags);
	sProps.m_cStyle = style;
	
	
	//antialias
	if (bAnti)
		flags &= os::FPF_SMOOTHED;
	
	//shear
	if (bShear)
	{
		sProps.m_vShear = AsFloat(0,35);
	}	
	
	//rotation
	if (bRotation)
	{
		sProps.m_vRotation = AsFloat(0,90);
	}
	
	//set the size
	sProps.m_vSize = AsFloat(5.0,36.0);


	//set any flags(aka: antialias)
	sProps.m_nFlags = flags;

	return sProps;
}

float Random::AsFloat(float start, float end)
{
	float difference = end - start;
	float num = start + (difference * AsInt()/RAND_MAX);
	return (num);
}

os::String Random::AsString(uint nType, int nWidth)
{
	int nRandom;
	char nRandomChar;
	os::String cReturnString;
	
	if (nWidth == -1)
		nWidth = AsInt(0,255);
		
	for (int i=0;i<nWidth;i++)
	{
		if (STRING_ALPHA)
		{
			nRandom = AsInt(0,m_cAlpha.Length());
			nRandomChar = m_cAlpha[nRandom];	
		}
		else if (STRING_ALPHANUMERIC)
		{
			nRandom = AsInt(0,m_cAlphaNum.Length());
			nRandomChar = m_cAlpha[nRandom];
		}
		else if (STRING_ALL)
		{
			nRandom = AsInt(0,m_cAll.Length());
			nRandomChar = m_cAlpha[nRandom];
		}
		
		cReturnString += nRandomChar;	
	}
	return cReturnString;
}



os::Point Random::AsPoint(os::Point start, os::Point end)
{
	os::Point cReturn;
	cReturn.x = AsFloat(start.x,end.x);
	cReturn.y = AsFloat(start.y,end.y);
	return cReturn;
}

os::Rect Random::AsRect(os::Rect start, os::Rect end)
{
	os::Rect cReturn;
	
	cReturn.left	=	AsFloat(start.left, end.left);
	cReturn.right	=	AsFloat(start.right,end.right);
	cReturn.top		=	AsFloat(start.top,end.top);
	cReturn.bottom	=	AsFloat(start.bottom,end.bottom);
	
	return cReturn;
}

os::IPoint Random::AsIPoint(os::IPoint start, os::IPoint end)
{
	os::IPoint cReturn;
	cReturn.x = AsInt(start.x,end.x);
	cReturn.y = AsInt(start.y,end.y);
	return cReturn;
}

os::IRect Random::AsIRect(os::IRect start, os::IRect end)
{
	os::Rect cReturn;
	
	cReturn.left	=	AsInt(start.left, end.left);
	cReturn.right	=	AsInt(start.right,end.right);
	cReturn.top		=	AsInt(start.top,end.top);
	cReturn.bottom	=	AsInt(start.bottom,end.bottom);
	
	return cReturn;
}

os::DateTime Random::AsDateTime()
{
	int y = AsInt(0,2037);
	int m = AsInt(0,12);
	
	//datetime rolls over so we don't need testing it will just come out being that if the month it a 30/28 day month
	//that it will just roll over to the next month
	int d = AsInt(1,31);
	
	return os::DateTime(y,m,d);
}








