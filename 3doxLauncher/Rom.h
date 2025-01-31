/**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */

#pragma once

#include "Global.h"
#include "IniFile.h"

class Rom
{
public:
	Rom();
	~Rom();

	bool Load(const string &szFilename);
	bool LoadFromCache(const string &szFilename, const string &szBoxArtFilename);

	string GetFileName();
	string GetBoxArtFilename();
	string GetComments();

	
private:
	string m_szFilename;
	string m_szBoxArtFilename;

	bool m_bLoaded;

};
