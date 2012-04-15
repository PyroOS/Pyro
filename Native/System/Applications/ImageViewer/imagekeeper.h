// NView (C)opyright 2008 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _IMAGEKEEPER_H_
#define _IMAGEKEEPER_H_

#include <util/string.h>

class ImageKeeper
{
public:
	ImageKeeper();
	virtual ~ImageKeeper();

	virtual os::String GetNextImage();
	virtual os::String GetPreviousImage();

	virtual os::String GetCurrentImage();

	void RemoveCurrentImage();
	virtual void AddImage( os::String file );

protected:
	os::String _GetMime( os::String file );
	void _SetImageStartIndex( int index );
	int _GetImageStartIndex();
	void _AddImage( os::String file );
	void _FixCompletePath( os::String& file );
private:
	class _Private;
	_Private* _m;
};

class SingleImageKeeper : public ImageKeeper
{
public:
	SingleImageKeeper();
	SingleImageKeeper( os::String path );
	~SingleImageKeeper();	
	void AddImage( os::String file );
private:
};

class SeveralImageKeeper : public ImageKeeper
{
public:
	SeveralImageKeeper();
	SeveralImageKeeper( int no, char* path[] );
	~SeveralImageKeeper();
private:
};

#endif
