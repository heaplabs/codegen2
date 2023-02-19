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
	vector <string> identifier_list;
	#include <iostream>
	using std::cout;
	using std::endl;
	#include "graph.h"

	std::map<std::string, std::set<std::string>> table_relations;
%}

%union {
	struct Table * table;
	DataType datatype;
	string* identifier;
	int number;
	bool bboolean;
	string * text_val;
	vector<string> * identifier_list;
}

//%define api.value.type union
%define parse.lac full
%define parse.error verbose
%define lr.type ielr
//%token <std::string*> table_name identifier
//%token <datatype> datatype
%nterm <table>  create_table_stmt
%nterm <identifier_list>  identifier_list
%nterm <datatype>  datatype
//%nterm <DataType*>  data_type
%token <identifier> identifier
%token <text_val> TEXT_VAL
%token <bboolean> ON
%token <bboolean> OFF

%token CREATE TABLE '(' ')' ';' 
%token BIGINT TEXT TIMESTAMP WITH WITHOUT TIME ZONE INTEGER BOOLEAN
%token PRIMARY KEY FOREIGN REFERENCES SEARCH_KEY TENANT_ID 
%token now DEFAULT NOT NULLL UNIQUE
%token MAP PostgresToScala
%token SCALA_DATATYPE DB_CONV
%token SET
%token WARNING
%token SCHEMA
%token ALTER TO OWNER
%token SEQUENCE START INCREMENT BY NO MINVALUE MAXVALUE CACHE OWNED QUOTE HEAP CONSTRAINT CHECK LSQB RSQB
%token CHARACTER VARYING
%token JSONB CAST_TO_JSONB
%token EMPTY_JSON AS
%token DOUBLE PRECISION
%token ONLY COLUMN NEXTVAL CAST_TO_REG_CLASS
%token ADD BTREE INDEX USING
%token GREATEST LEAST LOWER WHERE IS 
%token GIN
%token FOREIGN
%token DELETE CASCADE
%left NE '='
%left OR
%left AND
%token JSON_FIELD_EXTRACTOR
%token <number> NUMBER
%token <bboolean> BBOOLEAN


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
	| create_index_stmt
	;

alter_seq_stmt:
	ALTER SEQUENCE identifier '.' identifier OWNED BY identifier '.' identifier '.' identifier ';'
	;

create_seq_stmt : 
	CREATE SEQUENCE identifier '.' identifier 
	START WITH NUMBER
	INCREMENT BY NUMBER
	NO MINVALUE
	NO MAXVALUE
	CACHE NUMBER ';'
	| CREATE SEQUENCE identifier '.' identifier 
	  AS datatype
	  START WITH NUMBER
	  INCREMENT BY NUMBER
	  NO MINVALUE
	  NO MAXVALUE
	  CACHE NUMBER ';'
	;

create_index_stmt:
	CREATE UNIQUE INDEX identifier ON identifier '.' identifier USING BTREE '(' expr_list ')' ';'
	| CREATE UNIQUE INDEX identifier ON identifier '.' identifier USING BTREE '(' expr_list ')' WHERE expr ';' 
	| CREATE INDEX identifier ON identifier '.' identifier USING BTREE '(' expr_list ')' WHERE expr ';'
	| CREATE INDEX identifier ON identifier '.' identifier USING BTREE '(' expr_list ')' ';'
	| CREATE INDEX identifier ON identifier '.' identifier USING GIN '(' expr_list identifier '.' identifier ')' ';'
	| CREATE INDEX identifier ON identifier '.' identifier USING GIN '(' expr_list identifier '.' identifier ')' WHERE expr ';'
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
	| ALTER TABLE ONLY identifier '.' identifier ALTER COLUMN identifier SET DEFAULT NEXTVAL '(' CAST_TO_REG_CLASS ')' ';'
	| ALTER TABLE ONLY identifier '.' identifier ADD CONSTRAINT identifier PRIMARY KEY '(' expr_list ')' ';'
	| ALTER TABLE ONLY identifier '.' identifier ADD CONSTRAINT identifier UNIQUE '(' expr_list ')' ';'
	| ALTER TABLE ONLY identifier '.' identifier ADD CONSTRAINT identifier FOREIGN KEY '(' identifier_list ')' REFERENCES identifier '.' identifier '(' identifier_list ')' ';' {
		string source_schema = *$4;
		string source_table = *$6;
		string target_schema = *$16;
		string target_table = *$18;
		vector<string> & source_table_keys = * $13;
		vector<string> & target_table_keys = * $20;
		cout << "got ALTER TABLE FOREIGN KEY"
			<< "source table : " << source_table 
			<< endl;
		cout << ", source keys : " << endl;
		for (map<string, Table *> ::iterator it = table_details.begin();
			it != table_details.end(); ++it) {
			cout << "table name: " << it->first 
				<< endl;
		}
		string source_key = source_schema + string(".") + source_table;
		cout << "source_key: " << source_key << endl;
		auto source_table_itr = table_details.find(source_key);
		if (source_table_itr == table_details.end() ) {
			cout << "source_table: " <<  source_table << " not found exiting" << endl;
			exit(0);
		}
		Table * source_table_ptr = source_table_itr->second;
		string target_key = target_schema + string(".") + target_table;
		auto target_table_itr = table_details.find(target_key);
		if (target_table_itr == table_details.end() ) {
			cout << "target_table: " << target_table << " not found exiting" << endl;
			exit(0);
		}
		Table * target_table_ptr = target_table_itr->second;

		for (int i = 0; i < source_table_keys.size(); ++i) {
			cout << source_table_keys[i] << endl;
		}
		cout << "target table : " << target_table;
		cout << ", target keys" << endl;
		for (int i = 0; i < target_table_keys.size(); ++i) {
			cout << target_table_keys[i] << endl;
		}
		if (table_relations.find(source_key) == table_relations.end()) {
			cout << "source_key: " << source_key << " not found in table_relations" << endl; 
		}
		if (table_relations.find(target_key) == table_relations.end()) {
			cout << "target_key: " << target_key << " not found in table_relations" << endl; 
		}
		// safe to pull stuff from table_relations
		std::set<string> dependencies = table_relations[source_key];
		dependencies.insert(target_key);
		table_relations[source_key] = dependencies;
		cout << " found relation : " 
			<< " source: " << source_key << " -> " << " dest: " << target_key 
			<< endl;

	}
	| ALTER TABLE ONLY identifier '.' identifier ADD CONSTRAINT identifier FOREIGN KEY '(' identifier_list ')' REFERENCES identifier '.' identifier '(' identifier_list ')' ON DELETE SET NULLL ';'
	| ALTER TABLE ONLY identifier '.' identifier ADD CONSTRAINT identifier FOREIGN KEY '(' identifier_list ')' REFERENCES identifier '.' identifier '(' identifier_list ')' ON DELETE CASCADE ';'
	;

	// we are going to discard these set statements 
set_stmt: 
	  SET identifier '=' NUMBER ';'
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
		/*
		std::set<string> dependencies;
		for (int i = 0; i < field_info_vec.size(); ++i) {
			FieldInfo * fi = field_info_vec[i];
			for (int j = 0; j < fi->flag_info_vec.size(); ++j) {
				FlagInfo * flg_inf = flag_info_vec[j];
				if (flg_inf->isForeignKey()) {
					ForeignKey * foreign_key = (ForeignKey *) flg_inf;
					string depends_on(foreign_key->table_name + string(".") + foreign_key->field_name);
					cout << "got dependency : " << depends_on << endl;
					dependencies.insert(depends_on) ;
				}
			}
		}
		*/
		std::set<string> dependencies = check_dependencies(field_info_vec); 

		table_relations.insert(std::pair<string, std::set<string>>{table_name, dependencies});
		field_info_vec.resize(0);
	}
	| CREATE TABLE identifier '.' identifier '(' field_defns ')' ';' {
		string * schema = $3;
		string * id = $5;
		string table_name( *schema + string(".") +  *id );
		Table * t = new Table(table_name, field_info_vec);
		$$ = t;
		table_details[table_name] = t;
		cout << "got new table: "
			<< table_name
			<< "field_info_vec sz: "
			<< field_info_vec.size()
			<< endl;
		std::set<string> dependencies = check_dependencies(field_info_vec); 
		table_relations.insert(std::pair<string, std::set<string>>{table_name, dependencies});
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
	  	DataType f_type = $2;
		cout << "parsing " << f_name 
			<< " without flags "
			<< " datatype: " << f_type
			<< endl;
		FieldInfo * a_field = new FieldInfo(f_name, f_type);
		field_info_vec.push_back(a_field);
	}
	| identifier datatype  flags {
	  	string f_name = *$1;
	  	DataType f_type = $2;
		cout << "parsing " << f_name
			<< " datatype: " << f_type
			<< " with flags "
			<< endl;
		FieldInfo * a_field = new FieldInfo(
			f_name, f_type, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	| identifier datatype LSQB RSQB  {
	  	string f_name = *$1;
		cout << "parsing " << f_name << " array without flags " << endl;
	  	DataType f_type = $2;
		FieldInfo * a_field = new FieldInfo(
			f_name, f_type, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	| identifier datatype LSQB RSQB  flags {
	  	string f_name = *$1;
		cout << "parsing " << f_name << " array with flags " << endl;
	  	DataType f_type = $2;
		FieldInfo * a_field = new FieldInfo(
			f_name, f_type, flag_info_vec);
		field_info_vec.push_back(a_field);
		flag_info_vec.resize(0);
	}
	| CONSTRAINT identifier CHECK expr
	;

identifier_list:
	  identifier {
		//identifier_list.push_back(*$1);
		vector<string> * ptr = new vector<string>;
		ptr->push_back(*$1);
		//vector<string> & v = *ptr;
		//v.push_back(*$1);
		$$ = ptr;
	}
	| identifier_list ',' identifier {
		identifier_list.push_back(*$3);
		$1->push_back(*$3);
		$$ = $1;
	}
	;

expr_list:
	 expr
	 | expr_list ',' expr
	 ;

expr :
     	identifier NE expr
     |  LEAST '(' expr_list ')'
     |  GREATEST '(' expr_list ')'
     |  LOWER '(' expr ')'
     |  identifier
     | identifier IS NULLL
     | identifier IS NOT NULLL
     | BBOOLEAN
     | NUMBER
     | identifier '=' expr
     | expr AND expr
     | expr OR expr
     | identifier JSON_FIELD_EXTRACTOR TEXT_VAL
     |  '(' expr ')'
     ;

datatype : BIGINT  { $$ = DataType::bigint; }
	 | TEXT { $$ = DataType::text; } 
	 | TIMESTAMP WITH TIME ZONE { $$ = DataType::date_time_with_timez; }
	 | TIMESTAMP WITHOUT TIME ZONE { $$ = DataType::date_time_with_timez; }
	 | INTEGER { $$ = DataType::integer; }
	 | BOOLEAN { $$ = DataType::boolean; }
	 | CHARACTER VARYING { $$ = DataType::text; }
	 | CHARACTER VARYING '(' NUMBER ')' { $$ = DataType::text; }
	 | JSONB { $$ = DataType::jsonb; }
	 | DOUBLE PRECISION
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
	| DEFAULT NUMBER  {
		flag_info_vec.push_back(new DefaultNumber($2));
	}
	| DEFAULT now '(' ')' {
		// todo incorrect
		flag_info_vec.push_back(new DefaultNow());
	}
	| DEFAULT EMPTY_JSON CAST_TO_JSONB {
		// todo incorrect
		flag_info_vec.push_back(new DefaultNow());
	}
	| DEFAULT TEXT_VAL   {
		// todo incorrect
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
