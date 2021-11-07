%{
	#include <string>
	#include "common_defs.h"
	int yylex(void);
	using std::string;
	void yyerror(char const *);
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
	  identifier identifier
	| field_defns ',' identifier identifier
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
