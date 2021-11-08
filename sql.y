%{
	#include "common_defs.h"
	int yylex(void);
	using std::string;
	void yyerror(char const *);
	struct FieldInfo {
		string field_name;
		string data_type;
		FieldInfo(string f_name, string d_type):
			field_name(f_name), data_type(d_type)
		{ }
	};
	vector <FieldInfo> field_info_vec;
	struct Table {
		string table_name;
		vector<FieldInfo> * field_info;
	};
	map<string, Table*> table_details;
%}

%define api.value.type {std::string}

%token CREATE TABLE '(' ')' ';' table_name identifier


%%

stmt: create_stmt
	| stmt create_stmt
	;

create_stmt: CREATE TABLE identifier '('
	field_defns
	')' ';'

field_defns: 
	  identifier identifier {
	  	string f_name = $1;
	  	string f_type = $2;
		FieldInfo a_field = FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	| field_defns ',' identifier identifier {
	  	string f_name = $3;
	  	string f_type = $4;
		FieldInfo a_field = FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	;



%%

#include <iostream>

using namespace std;
void yyerror(char const * err)
{
	cout << "error while parsing: " << err << endl;
}

int main() {
	int status = yyparse();
	cout << " status: " << status << endl;
}
