#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <time.h>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>


#include "IDs.hpp"
#include "StoryBlock.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
namespace b = boost;


typedef b::adjacency_list<b::vecS, b::vecS, b::bidirectionalS, b::no_property, string> Graph;
typedef typename b::graph_traits<Graph>::vertex_descriptor Vertex;
typedef typename b::graph_traits<Graph>::edge_descriptor Edge;

class VertexTitleWriter {
public:
  VertexTitleWriter(SIDToName itn) : itn_(itn) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) {
    out << "[label=\"" << itn_[v].title_ << "\"]";
  }
private:
  SIDToName itn_;
};

class EdgeTitleWriter {
public:
  EdgeTitleWriter(std::map<Edge, EntityID> itn) : itn_(itn) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) {
    out << "[label=\"" << EntityIDs::ENTITY_IDS->getEntityByID(itn_[v]) << "\"]";
  }
private:
  std::map<Edge, EntityID> itn_;
};

struct GraphWriter {
  void operator()(std::ostream& out) const {
    out << "graph [bgcolor=lightgrey]" << std::endl;
    out << "node [shape=circle color=white]" << std::endl;
    out << "edge [style=dashed]" << std::endl;
  }
};
struct cycle_detector : public b::dfs_visitor<>
{
  cycle_detector(bool& has_cycle)
    : m_has_cycle(has_cycle) { }

  template <class Edge, class Graph>
  void back_edge(Edge, Graph&) { m_has_cycle = true; }
protected:
  bool& m_has_cycle;
};


class ParseException: public std::runtime_error {
  string message_;
  int lineNum_;
  string line_;
public:
  ParseException(const string& message, int lineNum, const string& line) throw () :
      std::runtime_error(message.c_str()), message_(message), lineNum_(lineNum), line_(line) {
  }

  char const* what() const throw () {
    std::stringstream ss;
    ss << "Parse error at line " << std::to_string(lineNum_) << ": " << message_ << std::endl << '\t' << line_;
    return ss.str().c_str();
  }
};

class StoryBlockCollection {
private:
  std::set<StoryID> storyIDs_;

  std::vector<StoryBlock> resolve(const std::set<StoryID>& storyIdList) {
    std::vector<StoryBlock> result;
    for(auto sid : storyIdList) {
      result.push_back(StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByID(sid));
    }
    return result;
  }
public:
  void add(const StoryBlock& sb) {
    StoryID id = StoryBlockIDs::STORY_BLOCK_IDS->addStoryBlock(sb);
    auto it = storyIDs_.find(id);
    if(it != storyIDs_.end()) {
      throw std::runtime_error("Attempt to add duplicate StoryBlock: " + sb.title_);
    } else {
      storyIDs_.insert(id);
    }
  }

  std::vector<StoryBlock> getAll() {
    return resolve(storyIDs_);
  }
};

enum Sections { BEGIN, TITLE, ITEMS_REQUIRED, ITEMS_PROVIDED, PRECONDITIONS, CONDITIONS_INITIATED, GOAL, STORY, END };

class ExpectedSection {
private:
  Sections current_ = BEGIN;
public:
  Sections next() {
    if(current_ == STORY)
      return END;
    else
      return (current_ = static_cast<Sections>(current_ + 1));
  }

  Sections current() {
    return current_;
  }

  void reset() {
    current_ = BEGIN;
  }
};

int main(int argc, char** argv) {
  std::ifstream ifs(argv[1]);
  string line;
  ExpectedSection expected;
  StoryBlockCollection storyBlockCollection;
  size_t lineNum = 0;
  StoryBlock* currentSB = NULL;
  while(std::getline(ifs, line)) {
    ++lineNum;

    boost::trim(line);
    if(line.empty() || boost::starts_with(line, "## ")) {
      continue;
    } else if (line.at(0) == '-') {
      expected.reset();
      continue;
    }

    if(boost::starts_with(line, "###")) {
      switch(expected.next()) {
	case BEGIN:
	  throw std::runtime_error("Reached section BEGIN");
	  break;
	case TITLE:
	  if(boost::starts_with(line, "### ")) {
	    if(currentSB != NULL) {
	      storyBlockCollection.add(*currentSB);
	      delete currentSB;
	    }
	    currentSB = new StoryBlock();
	    string s = line.substr(4);
	    boost::trim(s);
	    currentSB->title_ = s;
	  } else {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case ITEMS_REQUIRED:
	  if(!boost::starts_with(line, "#### Items Required")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case ITEMS_PROVIDED:
	  if(!boost::starts_with(line, "#### Items Provided")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case PRECONDITIONS:
	  if(!boost::starts_with(line, "#### Preconditions")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case CONDITIONS_INITIATED:
	  if(!boost::starts_with(line, "#### Conditions Initiated")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case GOAL:
	  if(!boost::starts_with(line, "#### Goal")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case STORY:
	  if(!boost::starts_with(line, "#### Story")) {
	    throw ParseException("Expected story title", lineNum, line);
	  }
	  break;
	case END:
	  throw std::runtime_error("Reached section END");
	  break;
      }
    } else if (boost::starts_with(line, "* ")) {
      switch(expected.current()) {
	case BEGIN:
	  throw std::runtime_error("Reached section BEGIN");
	  break;
	case TITLE:
	    throw ParseException("Found trailing list item in section title", lineNum, line);
	  break;
	case ITEMS_REQUIRED:
	  currentSB->add(StoryBlock::ITEMS_REQ , line.substr(2));
	  break;
	case ITEMS_PROVIDED:
	  currentSB->add(StoryBlock::ITEMS_PROV, line.substr(2));
	  break;
	case PRECONDITIONS:
	  currentSB->add(StoryBlock::PRE_COND , line.substr(2));
	  break;
	case CONDITIONS_INITIATED:
	  currentSB->add(StoryBlock::INIT_COND , line.substr(2));
	  break;
	case GOAL:
	  throw ParseException("Found list item in section goal", lineNum, line);
	  break;
	case STORY:
	  throw ParseException("Found list item in section story", lineNum, line);
	  break;
	case END:
	  throw std::runtime_error("Reached section END");
	  break;
      }
    } else {
      switch(expected.current()) {
	case BEGIN:
	  throw std::runtime_error("Reached section BEGIN");
	  break;
	case TITLE:
	    throw ParseException("Found trailing text in section title", lineNum, line);
	  break;
	case ITEMS_REQUIRED:
	    throw ParseException("Found non list item in section items required", lineNum, line);
	  break;
	case ITEMS_PROVIDED:
	    throw ParseException("Found non list item in section items provided", lineNum, line);
	  break;
	case PRECONDITIONS:
	    throw ParseException("Found non list item in section preconditions", lineNum, line);
	  break;
	case CONDITIONS_INITIATED:
	    throw ParseException("Found non list item in section conditions initiated", lineNum, line);
	  break;
	case GOAL:
	  currentSB->goal_ += (line + "\n");
	  break;
	case STORY:
	  currentSB->story_ += (line + "\n");
	  break;
	case END:
	  throw std::runtime_error("Reached section END");
	  break;
      }
    }
  }
  storyBlockCollection.add(*currentSB);

  Graph g;

  auto storyBlocks = storyBlockCollection.getAll();
  std::map<Edge, EntityID> edgeToEntityID;

  for(StoryBlock& sb1 : storyBlocks) {
    for(EntityID eid1: sb1.itemsProvided_) {
      for(StoryBlock& sb2 : storyBlocks) {
        for(EntityID eid2: sb2.itemsRequired_) {
          if(eid1 == eid2) {
            Edge e = b::add_edge(StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByName(sb1),
				 StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByName(sb2),
				 EntityIDs::ENTITY_IDS->getEntityByID(eid1), g).first;
            edgeToEntityID[e] = eid1;
          }
        }
      }
    }

    for(EntityID eid1: sb1.initiatedConditions_) {
      for(StoryBlock& sb2 : storyBlocks) {
	for(EntityID eid2: sb2.preconditions_) {
	  if(eid1 == eid2) {
	    Edge e = b::add_edge(StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByName(sb1),
			StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByName(sb2),
			EntityIDs::ENTITY_IDS->getEntityByID(eid1), g).first;
	    edgeToEntityID[e] = eid1;
	  }
	}
      }
    }
  }

  {
    typedef std::list<Vertex> SolveOrder;
    SolveOrder::iterator i;
    SolveOrder solve_order;

    topological_sort(g, std::front_inserter(solve_order));


    // Parallel ordering
    std::vector<int> time(solve_order.size(), 0);
    for (i = solve_order.begin(); i != solve_order.end(); ++i) {
      // Walk through the in_edges an calculate the maximum time.
      if (in_degree (*i, g) > 0) {
        Graph::in_edge_iterator j, j_end;
        int maxdist=0;
        // Through the order from topological sort, we are sure that every
        // time we are using here is already initialized.
        for (boost::tie(j, j_end) = in_edges(*i, g); j != j_end; ++j)
          maxdist=(std::max)(time[source(*j, g)], maxdist);
        time[*i]=maxdist+1;
      }
    }

    cerr << "Not fully satisfied: " << endl;
    for (i = solve_order.begin(); i != solve_order.end(); ++i) {
      StoryBlock sb = StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByID(*i);
      size_t reqcnt = sb.itemsRequired_.size() + sb.preconditions_.size();
      if (in_degree (*i, g) != reqcnt) {
	cerr << sb.title_ << endl;
      }
    }

    StoryBlockIDs copy = *StoryBlockIDs::STORY_BLOCK_IDS;
    for(auto& v : solve_order) {
      copy.removeStoryBlock(v);
    }

    cerr << endl << "Orphans: " << endl;
    for(auto& orphane : copy.getITN()) {
      cerr << orphane.second.title_ << endl;
    }

    cerr << endl << "Parallel ordering, story blocks with same group number can be made in parallel. lower numbers need to be solved first" << endl;
    {
      b::graph_traits<Graph>::vertex_iterator i, iend;
      for (boost::tie(i,iend) = vertices(g); i != iend; ++i)
        cerr << StoryBlockIDs::STORY_BLOCK_IDS->getStoryBlockByID(*i).title_ << " = " << time[*i] << endl;
    }

  }
  cerr << endl;

  b::write_graphviz(std::cout, g, VertexTitleWriter(StoryBlockIDs::STORY_BLOCK_IDS->getITN()), EdgeTitleWriter(edgeToEntityID), GraphWriter());

  return 0;
}
