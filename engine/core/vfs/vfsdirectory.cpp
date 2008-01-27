/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <fstream>

// 3rd party library includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/raw/rawdatafile.h"
#include "util/logger.h"

#include "vfsdirectory.h"

namespace bfs = boost::filesystem;
namespace FIFE {
	static Logger _log(LM_VFS);

	VFSDirectory::VFSDirectory(const std::string& root) : m_root(root) {
		std::transform(m_root.begin(), m_root.end(), m_root.begin(), tolower);
		FL_DBG(_log, LMsg("VFSDirectory created with root path ") << m_root);
		if(!m_root.empty() && *(m_root.end() - 1) != '/')
			m_root.append(1,'/');
	}


	VFSDirectory::~VFSDirectory() {
	}


	bool VFSDirectory::fileExists(const std::string& name) const {
		std::string fullpath = m_root + name;
		std::ifstream file(fullpath.c_str());
		if (file)
			return true;

		return false;
	}

	RawData* VFSDirectory::open(const std::string& file) const {
		return new RawData(new RawDataFile(m_root + file));
	}

	std::set<std::string> VFSDirectory::listFiles(const std::string& path) const {
		std::string dir = m_root;
		// Avoid double slashes
		if(path[0] == '/' && m_root[m_root.size()-1] == '/') {
			dir.append(path.substr(1));
		}
		else {
			dir.append(path);
		}

		return list(dir, false);
	}

	std::set<std::string> VFSDirectory::listDirectories(const std::string& path) const {
		std::string dir = m_root;
		// Avoid double slashes
		if(path[0] == '/' && m_root[m_root.size()-1] == '/') {
			dir.append(path.substr(1));
		}
		else {
			dir.append(path);
		}

		return list(dir, true);
	}

	std::set<std::string> VFSDirectory::list(const std::string& path, bool directorys) const {
		std::set<std::string> list;
		bfs::path boost_path(m_root + path);
		if (!bfs::exists(boost_path) || !bfs::is_directory(boost_path))
			return list;

		bfs::directory_iterator end;
		for (bfs::directory_iterator i(boost_path); i != end; ++i) {
			if (bfs::is_directory(*i) != directorys)
				continue;

			list.insert(i->leaf());
		}

		return list;
	}
}
