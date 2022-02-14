// topological sort
//
#include <string>

#include <map>
#include <set>
#include <deque>
#include <iostream>

#include "graph.h"

using namespace std;

Graph::Graph( map<string, set<string>> p_node_relations):
		node_relations(p_node_relations)
	{ } 

void Graph::topological_sort_aux(
			string node,
			deque<pair<string, int> > & order,
			map<string, bool> & visited,
			deque<string> & visit_queue,
			int nest_level)
{
	cout << "ENTER topological_sort_aux: " 
		<< " nest_level: " << nest_level << endl;
	if (nest_level > 20) {
		return;
	}

		//string node = visit_queue.front();
		cout << "INFO topological_sort_aux visit_queue head: " << node
			<< endl;
		//visit_queue.pop_front();
		if (!visited[node]) {
			set<string>& nodes = node_relations[node];
			for (set<string>::const_iterator n_it = 
				nodes.begin(); n_it != nodes.end();
				++n_it) {
				if (!visited[*n_it]) {
					//visit_queue.push_back(*n_it);
					topological_sort_aux(*n_it, order, visited, visit_queue, nest_level+1);
					visited[*n_it] = true;
				}
			}
			//topological_sort_aux(order, visited, visit_queue, nest_level+1);
			cout << "adding node: " << node << " to the topological order" << endl;
			order.push_back({node, nest_level});
			visited[node] = true;
		}
}

deque<pair<string, int> > Graph::topological_sort() {
	deque<pair<string, int> >  order;
	map<string, bool>  visited;
	deque<string> visit_queue;
	for ( map<string, set<string> >::const_iterator it = 
		node_relations.begin(); it != node_relations.end(); ++it ) {
		visited.insert({it->first, false});
	}
	int nest_level = 1;
	cout << "INFO queue empty - searching for a starting node" << endl;
	for (map<string, set<string>>::iterator it = 
		node_relations.begin(); it != node_relations.end();
		++it) {
		if (!visited[it->first]) {
			cout << "processing node: " << it->first << ", nest_level: " << nest_level << endl;
			set<string>& nodes = it->second;
			for (set<string>::const_iterator n_it = 
				nodes.begin(); n_it != nodes.end();
				++n_it) {
				if (!visited[*n_it]) {
					cout << "pushing " << *n_it << " into the queue" << endl;
					//visit_queue.push_back(*n_it);
					topological_sort_aux(*n_it, order, visited, visit_queue, nest_level+1);
					visited[*n_it] = true;
				}
			}
			//topological_sort_aux(order, visited, visit_queue, nest_level+1);
			order.push_back({it->first, nest_level});
			visited[it->first] = true;
		}
	}
	return order;
}

int main() {
	map<string, set<string> > graph;
	set<string> empty;
	set<string> subjects;
	//subjects.insert("real_line");
	//subjects.insert("continuous_fns");
	//subjects.insert("differentiation");
	//subjects.insert("integration");
	set<string> highschool_algebra_deps;
	set<string> highschool_geometry_deps;
	set<string> real_line_deps;
	real_line_deps.insert("highschool_algebra");
	real_line_deps.insert("highschool_geometry");
	set<string> integration_deps;
	integration_deps.insert("real_line");
	integration_deps.insert("continuous_fns");
	set<string> differentiation_deps;
	integration_deps.insert("real_line");
	integration_deps.insert("differentiation");
	set<string> continuous_fns_deps;
	continuous_fns_deps.insert("real_line");
	set<string> bio1_deps;
	set<string> chem1_deps;
	set<string> bio2_deps;
	bio2_deps.insert("bio1");
	bio2_deps.insert("chem1");
	set<string> chem2_deps;
	chem2_deps.insert("bio1");
	chem2_deps.insert("chem1");

	graph.insert({"highschool_algebra", highschool_algebra_deps});
	graph.insert({"highschool_geometry", highschool_geometry_deps});
	graph.insert({"integration", integration_deps});
	graph.insert({"differentiation", differentiation_deps});
	graph.insert({"continuous_fns", continuous_fns_deps});
	graph.insert({"real_line", real_line_deps});
	graph.insert({"bio1", bio1_deps});
	graph.insert({"bio2", bio2_deps});
	graph.insert({"chem1", chem1_deps});
	graph.insert({"chem2", chem2_deps});
	Graph g(graph);

	deque<pair<string, int> >  order = g.topological_sort();

	for (int i = 0; i < order.size(); ++i) {
		cout << order[i].first << ", " << order[i].second << endl;
	}
}


