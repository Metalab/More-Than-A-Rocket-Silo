#include "StoryBlock.hpp"


  std::vector<string> StoryBlock::resolve(const std::vector<EntityID>& entityList) {
    std::vector<string> result;
    for(auto eid : entityList) {
      result.push_back(EntityIDs::ENTITY_IDS->getEntityByID(eid));
    }
    return result;
  }


  void StoryBlock::add(const EntityType& type, const string& value) {
    switch(type) {
      case ITEMS_REQ:
	itemsRequired_.push_back(EntityIDs::ENTITY_IDS->addEntity(value));
	break;
      case ITEMS_PROV:
	itemsProvided_.push_back(EntityIDs::ENTITY_IDS->addEntity(value));
	break;
      case PRE_COND:
	preconditions_.push_back(EntityIDs::ENTITY_IDS->addEntity(value));
	break;
      case INIT_COND:
	initiatedConditions_.push_back(EntityIDs::ENTITY_IDS->addEntity(value));
	break;
    }
  }

  std::vector<string> StoryBlock::getAll(const EntityType& type) {
    switch(type) {
      case ITEMS_REQ:
	return resolve(itemsRequired_);
	break;
      case ITEMS_PROV:
	return resolve(itemsRequired_);
	break;
      case PRE_COND:
	return resolve(itemsRequired_);
	break;
      case INIT_COND:
	return resolve(itemsRequired_);
	break;
      default:
	throw std::runtime_error("This should never happen");
	break;
    }
  }

