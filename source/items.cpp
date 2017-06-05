//////////////////////////////////////////////////////////////////////
// This file is part of Remere's Map Editor
//////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/items.hpp $
// $Id: items.hpp 310 2010-02-26 18:03:48Z admin $

#include "main.h"

#include "materials.h"
#include "gui.h"
#include <string.h> // memcpy

#include "items.h"
#include "item.h"
#include "pugicast.h"

ItemDatabase g_items;

ItemType::ItemType() :
	sprite(nullptr),
	id(0),
	clientID(0),
	brush(nullptr),
	doodad_brush(nullptr),
	raw_brush(nullptr),
	is_metaitem(false),
	has_raw(false),
	in_other_tileset(false),
	group(ITEM_GROUP_NONE),
	type(ITEM_TYPE_NONE),
	volume(0),
	maxTextLen(0),
	//writeOnceItemID(0),
	ground_equivalent(0),
	border_group(0),
	has_equivalent(false),
	wall_hate_me(false),
	name(""),
	description(""),
	weight(0.0f),
	attack(0),
	defense(0),
	armor(0),
	charges(0),
	client_chargeable(false),
	extra_chargeable(false),

	isVertical(false),
	isHorizontal(false),
	isHangable(false),
	canReadText(false),
	canWriteText(false),
	replaceable(true),
	decays(false),
	stackable(false),
	moveable(true),
	alwaysOnBottom(false),
	pickupable(false),
	rotable(false),
	isBorder(false),
	isOptionalBorder(false),
	isWall(false),
	isBrushDoor(false),
	isOpen(false),
	isTable(false),
	isCarpet(false),

	floorChangeDown(true),
	floorChangeNorth(false),
	floorChangeSouth(false),
	floorChangeEast(false),
	floorChangeWest(false),

	blockSolid(false),
	blockPickupable(false),
	blockProjectile(false),
	blockPathFind(false),

	alwaysOnTopOrder(0),
	rotateTo(0),
	border_alignment(BORDER_NONE)
{
	////
}

ItemType::~ItemType()
{
	////
}

ItemDatabase::ItemDatabase() :
	// Version information
	MajorVersion(3),
	MinorVersion(3),
	BuildNumber(0),

	// Count of GameSprite types
	item_count(0),
	effect_count(0),
	monster_count(0),
	distance_count(0),

	minclientID(0),
	maxclientID(0),

	max_item_id(0)
{
	////
}

ItemDatabase::~ItemDatabase()
{
	clear();
}

void ItemDatabase::clear()
{
	for(uint32_t i = 0; i < items.size(); i++) {
		delete items[i];
		items.set(i, nullptr);
	}
}

bool ItemDatabase::loadItemFromGameXml(pugi::xml_node itemNode, int id)
{
	ItemType* t = newd ItemType();

	t->id = id;
	t->clientID = id;

	if (max_item_id < t->id)
		max_item_id = t->id;

	if (items[t->id]) {
		delete items[t->id];
	}

	t->sprite = static_cast<GameSprite*>(g_gui.gfx.getSprite(t->clientID));

	items.set(t->id, t);

	ItemType& it = getItemType(id);

	it.name = itemNode.attribute("name").as_string();
	it.editorsuffix = itemNode.attribute("editorsuffix").as_string();

	pugi::xml_attribute attribute;
	for(pugi::xml_node itemAttributesNode = itemNode.first_child(); itemAttributesNode; itemAttributesNode = itemAttributesNode.next_sibling()) {
		if (attribute = itemAttributesNode.attribute("key")) {

			std::string key = attribute.as_string();
			to_lower_str(key);

			if (key == "type") {
				if (!(attribute = itemAttributesNode.attribute("value"))) {
					continue;
				}

				std::string typeValue = attribute.as_string();
				to_lower_str(key);
				if (typeValue == "magicfield") {
					it.group = ITEM_GROUP_MAGICFIELD;
					it.type = ITEM_TYPE_MAGICFIELD;
				} else if (typeValue == "key") {
					it.type = ITEM_TYPE_KEY;
				} else if (typeValue == "depot") {
					it.type = ITEM_TYPE_DEPOT;
				} else if (typeValue == "teleport") {
					it.type = ITEM_TYPE_TELEPORT;
				} else if (typeValue == "bed") {
					it.type = ITEM_TYPE_BED;
				} else if (typeValue == "door") {
					it.type = ITEM_TYPE_DOOR;
				} else {
					// We ignore many types, no need to complain
					//warnings.push_back("items.xml: Unknown type " + typeValue);
				}
			} else if (key == "name") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.name = attribute.as_string();
				}
			} else if (key == "description") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.description = attribute.as_string();
				}
			} else if (key == "runespellName") {
				/*if((attribute = itemAttributesNode.attribute("value"))) {
					it.runeSpellName = attribute.as_string();
				}*/
			} else if (key == "weight") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.weight = pugi::cast<int32_t>(attribute.value()) / 100.f;
				}
			} else if (key == "armor") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.armor = pugi::cast<int32_t>(attribute.value());
				}
			} else if (key == "defense") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.defense = pugi::cast<int32_t>(attribute.value());
				}
			} else if (key == "rotateto") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.rotateTo = pugi::cast<int32_t>(attribute.value());
				}
			} else if (key == "containersize") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.volume = pugi::cast<int32_t>(attribute.value());
				}
			} else if (key == "readable") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.canReadText = attribute.as_bool();
				}
			} else if (key == "writeable") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.canWriteText = it.canReadText = attribute.as_bool();
				}
			} else if (key == "decayto") {
				it.decays = true;
			} else if (key == "maxtextlen" || key == "maxtextlength") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.maxTextLen = pugi::cast<int32_t>(attribute.value());
					it.canReadText = it.maxTextLen > 0;
				}
			} else if (key == "writeonceitemid") {
				/*if((attribute = itemAttributesNode.attribute("value"))) {
					it.writeOnceItemId = pugi::cast<int32_t>(attribute.value());
				}*/
			} else if (key == "allowdistread") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.allowDistRead = attribute.as_bool();
				}
			} else if (key == "charges") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					it.charges = pugi::cast<int32_t>(attribute.value());
					it.extra_chargeable = true;
				}
			} else if (key == "flag") {
				if ((attribute = itemAttributesNode.attribute("value"))) {
					std::string flagName = attribute.as_string();
					to_lower_str(flagName);
					if (flagName == "bank") {
						it.group = ItemGroup_t::ITEM_GROUP_GROUND;
					} else if (flagName == "container") {
						it.type = ITEM_TYPE_CONTAINER;
						it.group = ItemGroup_t::ITEM_GROUP_CONTAINER;
					} else if (flagName == "liquidcontainer") {
						it.group = ItemGroup_t::ITEM_GROUP_FLUID;
					} else if (flagName == "liquidpool") {
						it.group = ItemGroup_t::ITEM_GROUP_SPLASH;
					} else if (flagName == "rune") {
						it.type = ITEM_TYPE_RUNE;
					} else if (flagName == "magicfield") {
						it.type = ITEM_TYPE_MAGICFIELD;
					} else if (flagName == "bed") {
						it.type = ITEM_TYPE_BED;
					} else if (flagName == "key") {
						it.type = ITEM_TYPE_KEY;
					} else if (flagName == "clip") {
						it.alwaysOnTopOrder = 1;
						it.alwaysOnBottom = true;
					} else if (flagName == "bottom") {
						it.alwaysOnTopOrder = 2;
						it.alwaysOnBottom = true;
					} else if (flagName == "top") {
						it.alwaysOnTopOrder = 3;
						it.alwaysOnBottom = true;
					} else if (flagName == "avoid") {
						it.blockPathFind = true;
					} else if (flagName == "unpass") {
						it.blockSolid = true;
					} else if (flagName == "unmove") {
						it.moveable = false;
					} else if (flagName == "unthrow") {
						it.blockProjectile = true;
					} else if (flagName == "take") {
						it.pickupable = true;
					} else if (flagName == "cumulative") {
						it.stackable = true;
					} else if (flagName == "text") {
						it.canReadText = true;
					} else if (flagName == "write") {
						it.canWriteText = it.canReadText = true;
					} else if (flagName == "hang") {
						it.isHangable = true;
					} else if (flagName == "hooksouth") {
						it.isVertical = true;
					} else if (flagName == "hookeast") {
						it.isHorizontal = true;
					} else if (flagName == "rotate") {
						it.rotable = true;
					} else if (flagName == "keydoor") {
						it.type = ITEM_TYPE_DOOR;
					} else if (flagName == "questdoor") {
						it.type = ITEM_TYPE_DOOR;
					} else if (flagName == "namedoor") {
						it.type = ITEM_TYPE_DOOR;
					} else if (flagName == "leveldoor") {
						it.type = ITEM_TYPE_DOOR;
					}
				}
			}
		}
	}
	return true;
}

bool ItemDatabase::loadFromGameXml(const FileName& identifier, wxString& error, wxArrayString& warnings)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(identifier.GetFullPath().mb_str());
	if(!result) {
		error = "Could not load objects.xml (Syntax error?)";
		return false;
	}

	pugi::xml_node node = doc.child("items");
	if(!node) {
		error = "objects.xml, invalid root node.";
		return false;
	}

	for(pugi::xml_node itemNode = node.first_child(); itemNode; itemNode = itemNode.next_sibling()) {
		if(as_lower_str(itemNode.name()) != "item") {
			continue;
		}

		int32_t fromId = pugi::cast<int32_t>(itemNode.attribute("fromid").value());
		int32_t toId = pugi::cast<int32_t>(itemNode.attribute("toid").value());
		if(pugi::xml_attribute attribute = itemNode.attribute("id")) {
			fromId = toId = pugi::cast<int32_t>(attribute.value());
		}

		if(fromId == 0 || toId == 0) {
			error = "Could not read item id from item node.";
			return false;
		}

		for(int32_t id = fromId; id <= toId; ++id) {
			if(!loadItemFromGameXml(itemNode, id)) {
				return false;
			}
		}
	}
	return true;
}

bool ItemDatabase::loadMetaItem(pugi::xml_node node)
{
	if(pugi::xml_attribute attribute = node.attribute("id")) {
		int32_t id = pugi::cast<int32_t>(attribute.value());
		if(items[id]) {
			//std::cout << "Occupied ID " << id << " : " << items[id]->id << ":" << items[id]->name << std::endl;
			return false;
		}
		items.set(id, newd ItemType());
		items[id]->is_metaitem = true;
		items[id]->id = id;
	} else {
		return false;
	}
	return true;
}

ItemType& ItemDatabase::getItemType(int id)
{
	ItemType* it = items[id];
	if(it)
		return *it;
	else {
		static ItemType dummyItemType; // use this for invalid ids
		return dummyItemType;
	}
}

bool ItemDatabase::typeExists(int id) const
{
	ItemType* it = items[id];
	return it != nullptr;
}
