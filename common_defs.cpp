#include "common_defs.h"

string capitaliseSingular(string s)
{
	using std::endl;
	using std::cout;
	// assume suffix is "s" and we chop if off
	// obviously this is not going to work in a lot of
	// cases
	string s1 ( s.substr(0, s.size()-1));
	s1[0] = toupper(s1[0]);
	//cout << "tableNameSingularCapitalised: " << s1 << endl;
	return s1;
}


string capitalise(string s) {
	using std::endl;
	using std::cout;
	string s1 ( s);
	s1[0] = toupper(s1[0]);
	//cout << "tableNameSingularCapitalised: " << s1 << endl;
	return s1;
}

string singular(string s) 
{
	string s1 ( s.substr(0, s.size()-1));
	return s1;
}


std::set<std::string> check_dependencies(
		vector<FieldInfo*> & field_info_vec
		)
{
	using std::endl;
	using std::cout;
	std::set<string> dependencies;
	for (int i = 0; i < field_info_vec.size(); ++i) {
		FieldInfo * fi = field_info_vec[i];
		for (int j = 0; j < fi->flag_info_vec.size(); ++j) {
			FlagInfo * flg_inf = fi->flag_info_vec[j];
			if (flg_inf->isForeignKey()) {
				ForeignKey * foreign_key = (ForeignKey *) flg_inf;
				string depends_on(foreign_key->table_name + string(".") + foreign_key->field_name);
				cout << "got dependency : " << depends_on << endl;
				dependencies.insert(depends_on) ;
			}
		}
	}
	return dependencies;
}
