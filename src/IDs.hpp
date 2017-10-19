#ifndef SRC_IDS_HPP_
#define SRC_IDS_HPP_

#include <map>
#include <string>

using std::string;

typedef int32_t EntityID;
typedef int32_t StoryID;

class StoryBlock;

typedef std::map<string, EntityID> NameToEID;
typedef std::map<EntityID, string> EIDToName;
typedef std::map<StoryBlock, StoryID> NameToSID;
typedef std::map<StoryID, StoryBlock> SIDToName;

class EntityIDs {
private:
  NameToEID nti_;
  EIDToName itn_;
  EntityID last_ = 0;
public:
  static EntityIDs* ENTITY_IDS;
  EntityIDs();

  EntityID addEntity(const string& name);
  EntityID getEntityByName(const string& name) const;
  string getEntityByID(const EntityID& id) const;
};

class StoryBlockIDs {
private:
  NameToSID nti_;
  SIDToName itn_;
  StoryID last_ = 0;
public:
  static StoryBlockIDs* STORY_BLOCK_IDS;
  StoryBlockIDs();

  StoryID addStoryBlock(const StoryBlock& sb);
  StoryID getStoryBlockByName(const StoryBlock& sb) const;
  StoryBlock getStoryBlockByID(const StoryID& id) const;
  void removeStoryBlock(const StoryID& id);
  SIDToName getITN() {
    return itn_;
  }
  NameToSID getNTI() {
    return nti_;
  }
};


#endif /* SRC_IDS_HPP_ */
