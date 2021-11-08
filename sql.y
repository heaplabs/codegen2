%{
	#include "common_defs.h"
	int yylex(void);
	using std::string;
	void yyerror(char const *);

	vector <FlagInfo*> flag_info_vec;
	vector <FieldInfo*> field_info_vec;

	map<string, Table*> table_details;
	#include <iostream>
	using std::cout;
	using std::endl;
%}

%define api.value.type union
%token <std::string*> table_name identifier
%nterm <Table*>  create_stmt

%token CREATE TABLE '(' ')' ';' 
%token PRIMARY KEY FOREIGN REFERENCES SEARCH_KEY TENANT_ID


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
		cout << "got new table: "
			<< table_name
			<< "field_info_vec sz: "
			<< field_info_vec.size()
			<< endl;
	}

field_defns:
	  field_defn
	| field_defns ',' field_defn
	;

field_defn:
	identifier identifier {
	  	string f_name = *$1;
	  	string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	| identifier identifier ':' flags {
	  	string f_name = *$1;
	  	string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	;

flags:
	  flag
	| flags ';' flag
	;

flag:
	PRIMARY KEY {
		flag_info_vec.push_back(new PrimaryKey());
	}
	| FOREIGN KEY REFERENCES identifier '(' identifier ')' {
		string table_name = *$4;
		string field_name = *$6;
		flag_info_vec.push_back(
			new ForeignKey(table_name, field_name)
		);
	}
	| TENANT_ID
	| SEARCH_KEY
	;



%%
