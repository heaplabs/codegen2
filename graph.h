#ifndef graph_h
#define graph_h

#include <string>

#include <map>
#include <set>
#include <deque>
#include <iostream>

struct Graph {
	Graph( std::map<std::string, std::set<std::string>> p_node_relations);
	//:
	//	node_relations(p_node_relations);
	//{ } 
	std::map<std::string, std::set<std::string>> node_relations;
	void topological_sort_aux(
				std::string node,
				std::deque<std::pair<std::string, int> > & order,
				std::map<std::string, bool> & visited,
				std::deque<std::string> & visit_queue,
				int nest_level);
	std::deque<std::pair<std::string, int> > topological_sort() ;
};

#endif /* graph_h */
