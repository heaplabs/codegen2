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

