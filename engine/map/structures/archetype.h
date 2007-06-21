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

#ifndef FIFE_MAP_ARCHETYPE_H
#define FIFE_MAP_ARCHETYPE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map.h"

namespace FIFE { namespace map {

	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;

	class Map;
	typedef boost::shared_ptr<Map> MapPtr;

	class Archetype {
		public:
			Archetype(const std::string& type, const std::string& filename, MapPtr parent);
			virtual ~Archetype();

			/** Load an archetype
			 */
			static Archetype* load(const std::string& type, const std::string& filename, MapPtr map);

      /** Add a (nested) archetype
			 */
			void addArchetype(Archetype* archetype);

			/** Load a prototype of an object
			 *  Loading a prototype of an object will set the objects
			 *  data to default values, that can be overridden.
			 */
			virtual void loadPrototype(ObjectInfo* object, size_t proto_id);

			/** Add a tile to this archetype's parent map
			 * This function is used during the loading phase of the AT
			 */
			void addTile(size_t tileid, size_t imageid);

			/** Add a Prototype to this archetype's parent map
			 */
			size_t addPrototype(const std::string& proto_name);

			const std::string& getTypeName() const;
			const std::string& getFilename() const;

		protected:

			// the map that loaded this archetype
			MapPtr m_map;

			// archetypes can nest archetypes
			typedef std::list<Archetype*> type_archetypes;
			type_archetypes m_archetypes;
		
			std::string m_typename;
			std::string m_filename;
	};

	inline
	const std::string& Archetype::getTypeName() const {
		return m_typename;
	}

	inline
	const std::string& Archetype::getFilename() const {
		return m_filename;
	}

	inline
	void Archetype::addTile(size_t tile_id, size_t image_id) {
		m_map->addTile(tile_id, image_id);
	}

	inline
	size_t Archetype::addPrototype(const std::string& proto_name) {
		return m_map->addPrototype(this, proto_name);
	}

}}

#endif