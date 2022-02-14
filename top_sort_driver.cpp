
#include "graph.h"

// compile with 
// g++ top_sort_driver.cpp top_sort.cpp 
int main() {
	using namespace std;
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
