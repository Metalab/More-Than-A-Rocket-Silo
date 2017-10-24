/*
 * StoryBlock.hpp
 *
 *  Created on: Oct 19, 2017
 *      Author: elchaschab
 */

#ifndef SRC_STORYBLOCK_HPP_
#define SRC_STORYBLOCK_HPP_

#include <vector>
#include <string>
#include "IDs.hpp"

using std::string;

class StoryBlock {
private:
  std::vector<string> resolve(const std::vector<EntityID>& entityList);
public:
  enum EntityType { ITEMS_REQ, ITEMS_PROV, PRE_COND, INIT_COND };

  std::vector<EntityID> itemsRequired_;
  std::vector<EntityID> itemsProvided_;
  std::vector<EntityID> preconditions_;
  std::vector<EntityID> initiatedConditions_;

  string title_;
  string room_;
  string goal_;
  string story_;
  string chapter_;

  void add(const EntityType& type, const string& value);
  std::vector<string> getAll(const EntityType& type);

  friend bool operator<(const StoryBlock& l, const StoryBlock& r) {
      return l.title_ < r.title_;
  }

  string str() {
    return "(" + chapter_ + ")\t" + title_ + " at " + room_;
  }
};


#endif /* SRC_STORYBLOCK_HPP_ */
