%{
	#include "common_defs.h"
	int yylex(void);
	using std::string;
	void yyerror(char const *);

	vector <FieldInfo> field_info_vec;

	map<string, Table*> table_details;
%}

%define api.value.type union
%token <std::string*> table_name identifier
%nterm <Table*>  create_stmt

%token CREATE TABLE '(' ')' ';' 


%%

stmt: create_stmt
	| stmt create_stmt
	;

create_stmt: CREATE TABLE identifier '('
	field_defns
	')' ';' {
		string * id = $3;
		string table_name(*id);
		Table * t = new Table(table_name, field_info_vec);
		$$ = t;
		table_details[table_name] = t;
	}

field_defns: 
	  identifier identifier {
	  	string f_name = *$1;
	  	string f_type = *$2;
		FieldInfo a_field = FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	| field_defns ',' identifier identifier {
	  	string f_name = *$3;
	  	string f_type = *$4;
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

void print_table_details( const map<string, Table*> & table_details)
{
	for(map<string, Table*>::const_iterator cit = 
		table_details.begin(); cit != table_details.end();
		++cit) {
		cout << cit->first << endl 
			<< cit->second->to_string() 
			<< endl;
	}
}

int main() {
	int status = yyparse();
	cout << " status: " << status << endl;
	print_table_details(table_details);
}
