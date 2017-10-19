#include "IDs.hpp"
#include "StoryBlock.hpp"

EntityIDs* EntityIDs::ENTITY_IDS = new EntityIDs();
StoryBlockIDs* StoryBlockIDs::STORY_BLOCK_IDS = new StoryBlockIDs();

EntityIDs::EntityIDs() {}

EntityID EntityIDs::addEntity(const string& name) {
  auto it = nti_.find(name);
  if(it == nti_.end()) {
    nti_[name] = last_;
    itn_[last_] = name;
    return last_++;
  } else {
    return (*it).second;
  }
}

EntityID EntityIDs::getEntityByName(const string& name) const {
  auto it = nti_.find(name);
  if(it == nti_.end())
    return -1;
  else
    return (*it).second;
}

string EntityIDs::getEntityByID(const EntityID& id) const {
  auto it = itn_.find(id);
  if(it == itn_.end())
    return "";
  else
    return (*it).second;
}

StoryBlockIDs::StoryBlockIDs() {}

StoryID StoryBlockIDs::addStoryBlock(const StoryBlock& sb) {
  auto it = nti_.find(sb);
  if(it == nti_.end()) {
    nti_[sb] = last_;
    itn_[last_] = sb;
    return last_++;
  } else {
    return (*it).second;
  }
}

StoryID StoryBlockIDs::getStoryBlockByName(const StoryBlock& sb) const {
  auto it = nti_.find(sb);
  if(it == nti_.end())
    return -1;
  else
    return (*it).second;
}

StoryBlock StoryBlockIDs::getStoryBlockByID(const StoryID& id) const {
  auto it = itn_.find(id);
  if(it == itn_.end())
    throw std::runtime_error("Unknown StoryBlockID");
  else
    return (*it).second;
}

void StoryBlockIDs::removeStoryBlock(const StoryID& id) {
  auto it = itn_.find(id);
  if(it == itn_.end())
    throw std::runtime_error("Attempt to remove unknown StoryBlockID");
  else {
    nti_.erase((*it).second);
    itn_.erase(it);
  }
}

