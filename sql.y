%{
	#include "common_defs.h"
	int yylex(void);
	using std::string;
	void yyerror(char const *);

	vector <FlagInfo*> flag_info_vec;
	vector <FieldInfo*> field_info_vec;

	map<string, Table*> table_details;
	map<string, string> postgres_to_scala_map;
	map<DataType, string> postgres_to_db_conv_map;
	#include <iostream>
	using std::cout;
	using std::endl;
%}

%union {
	struct Table * table;
	DataType datatype;
	string* identifier;
}

//%define api.value.type union
%define parse.lac full
%define parse.error verbose
%define lr.type ielr
//%token <std::string*> table_name identifier
//%token <datatype> datatype
%nterm <table>  create_stmt
%nterm <datatype>  datatype
//%nterm <DataType*>  data_type
%token <identifier>identifier

%token CREATE TABLE '(' ')' ';' 
%token BIGINT TEXT TIMESTAMP WITH TIME ZONE INTEGER BOOLEAN
%token PRIMARY KEY FOREIGN REFERENCES SEARCH_KEY TENANT_ID 
%token now DEFAULT NOT NULLL UNIQUE
%token MAP PostgresToScala
%token SCALA_DATATYPE DB_CONV


%%

stmts: stmt
     | stmts stmt
     ;

stmt: create_stmt
	//| map_types_stmt
	;

create_stmt: 
	CREATE TABLE identifier '(' field_defns ')' ';' {
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
		field_info_vec.resize(0);
	}
	| CREATE TABLE identifier '.' identifier '(' field_defns ')' ';' {
		string * id = $5;
		string table_name(*id);
		Table * t = new Table(table_name, field_info_vec);
		$$ = t;
		table_details[table_name] = t;
		cout << "got new table: "
			<< table_name
			<< "field_info_vec sz: "
			<< field_info_vec.size()
			<< endl;
		field_info_vec.resize(0);
	}
	;

	/*
	map_types_stmt:
		MAP PostgresToScala '(' map_defns ')' ';'
		;

	map_defns:
		map_defn
		| map_defns ',' map_defn
		;

	map_defn: identifier DB_CONV identifier SCALA_DATATYPE identifier {
			string postgres_data_type = *$1;
			string db_converter = *$3;
			string scala_data_type = *$5;
			postgres_to_scala_map[postgres_data_type] = scala_data_type;
			postgres_to_db_conv_map[postgres_data_type] = db_converter;
			using std::cout;
			using std::endl;
			cout << "adding type " 
				<< postgres_data_type << "->"
				<< scala_data_type 
				<< endl;
		}
		;
	*/

field_defns:
	  field_defn
	| field_defns ',' field_defn
	;

field_defn:
	identifier datatype {
	  	string f_name = *$1;
	  	//string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(f_name, $2);
		field_info_vec.push_back(a_field);
	}
	| identifier datatype  flags {
	  	string f_name = *$1;
	  	//string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(
			f_name, $2, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	;

datatype : BIGINT  //{ $$ = DataType.bigint }
	 | TEXT //{ $$ = DataType.text } 
	 | TIMESTAMP WITH TIME ZONE //{ $$ = DataType.date_time_with_timez }
	 | INTEGER //{ $$ = DataType.integer }
	 | BOOLEAN //{ $$ = DataType.boolean }
	 ;

flags:
	  flag
	| flags  flag
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
	| TENANT_ID {
		flag_info_vec.push_back(new TenantId());
	}
	| SEARCH_KEY {
		flag_info_vec.push_back(new SearchKey());
	}
	| DEFAULT now '(' ')' {
		flag_info_vec.push_back(new DefaultNow());
	}
	| NOT NULLL {
		flag_info_vec.push_back(new NotNull());
	}
	| UNIQUE {
		flag_info_vec.push_back(new UniqueKey());
	}
	;



%%
