#ifndef SRC_IDS_HPP_
#define SRC_IDS_HPP_

#include <map>
#include <string>

using std::string;

typedef int64_t EntityID;
typedef int64_t StoryID;

class StoryBlock;

typedef std::map<string, EntityID> NameToEID;
typedef std::map<EntityID, string> EIDToName;
typedef std::map<StoryBlock, StoryID> NameToSB;
typedef std::map<StoryID, StoryBlock> SBToName;

class EntityIDs {
private:
  NameToEID nti_;
  EIDToName itn_;
  EntityID last_ = 0;
public:
  static EntityIDs* ENTITY_IDS;
  EntityIDs();

  EntityID addEntity(const string& name);
  EntityID getEntityIDByName(const string& name) const;
  string getEntityByID(const EntityID& id) const;
  EIDToName getITN() {
    return itn_;
  }
  NameToEID getNTI() {
    return nti_;
  }
};

class StoryBlockIDs {
private:
  NameToSB nts_;
  SBToName stn_;
  StoryID last_ = 0;
public:
  static StoryBlockIDs* STORY_BLOCK_IDS;
  StoryBlockIDs();

  StoryID addStoryBlock(const StoryBlock& sb);
  StoryID getStoryIDByName(const StoryBlock& sb) const;
  StoryBlock getStoryBlockByID(const StoryID& id) const;
  void removeStoryBlock(const StoryID& id);
  SBToName getSTN() {
    return stn_;
  }
  NameToSB getNTS() {
    return nts_;
  }
};


#endif /* SRC_IDS_HPP_ */
