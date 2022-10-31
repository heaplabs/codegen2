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
	int number;
	bool bboolean;
	string * text_val;
}

//%define api.value.type union
%define parse.lac full
%define parse.error verbose
%define lr.type ielr
//%token <std::string*> table_name identifier
//%token <datatype> datatype
%nterm <table>  create_table_stmt
%nterm <datatype>  datatype
//%nterm <DataType*>  data_type
%token <identifier> identifier
%token <number> number
%token <bboolean> BBOOLEAN
%token <text_val> TEXT_VAL
%token <bboolean> ON
%token <bboolean> OFF

%token CREATE TABLE '(' ')' ';' 
%token BIGINT TEXT TIMESTAMP WITH TIME ZONE INTEGER BOOLEAN
%token PRIMARY KEY FOREIGN REFERENCES SEARCH_KEY TENANT_ID 
%token now DEFAULT NOT NULLL UNIQUE
%token MAP PostgresToScala
%token SCALA_DATATYPE DB_CONV
%token SET
%token WARNING
%token SCHEMA
%token ALTER TO OWNER
%token SEQUENCE START INCREMENT BY NO MINVALUE MAXVALUE CACHE OWNED QUOTE HEAP CONSTRAINT CHECK NE LSQB RSQB


%%

stmts: stmt
     | stmts stmt
     ;

stmt: create_table_stmt
	//| map_types_stmt
	| set_stmt
	| create_schema_stmt
	| alter_schema_stmt
	| alter_table_stmt
	| create_seq_stmt 
	| alter_seq_stmt
	;

alter_seq_stmt:
	ALTER SEQUENCE identifier '.' identifier OWNED BY identifier '.' identifier '.' identifier ';'
	;

create_seq_stmt : 
	CREATE SEQUENCE identifier '.' identifier 
	START WITH number
	INCREMENT BY number
	NO MINVALUE
	NO MAXVALUE
	CACHE number ';'
	;

	// we are going to discard the create schema for now
create_schema_stmt :
	CREATE SCHEMA identifier ';'
	;

	// we are going to discard these alter schema  statements 
alter_schema_stmt:
	ALTER SCHEMA identifier OWNER TO identifier ';'
	;


alter_table_stmt:
	ALTER TABLE identifier '.' identifier OWNER TO identifier ';'
	;

	// we are going to discard these set statements 
set_stmt: 
	  SET identifier '=' number ';'
	| SET identifier '=' TEXT_VAL ';'
	| SET identifier '=' BBOOLEAN ';'
	| SET identifier '=' WARNING ';'
	| SET identifier '=' QUOTE QUOTE';'
	| SET identifier '=' HEAP ';'
	;

create_table_stmt: 
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
		cout << "parsing " << f_name << " without flags " << endl;
	  	//string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(f_name, $2);
		field_info_vec.push_back(a_field);
	}
	| identifier datatype  flags {
	  	string f_name = *$1;
		cout << "parsing " << f_name << " with flags " << endl;
	  	//string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(
			f_name, $2, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	| identifier datatype LSQB RSQB  flags {
	  	string f_name = *$1;
		cout << "parsing " << f_name << " with flags " << endl;
	  	//string f_type = *$2;
		FieldInfo * a_field = new FieldInfo(
			f_name, $2, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	| CONSTRAINT identifier CHECK expr
	;

expr :
     	expr NE expr
     |  '(' expr ')'
     |  identifier
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
	| DEFAULT BBOOLEAN  {
		flag_info_vec.push_back(new DefaultBoolean($2));
	}
	| DEFAULT number  {
		flag_info_vec.push_back(new DefaultNumber($2));
	}
	| DEFAULT now '(' ')' {
		flag_info_vec.push_back(new DefaultNow());
	}
	| DEFAULT TEXT_VAL   {
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
