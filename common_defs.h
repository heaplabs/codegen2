#ifndef common_defs_h
#define common_defs_h
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <cctype>
using std::string;
using std::vector;
using std::map;

struct error_pos {
	int row, col; string error_context;
	error_pos(int r, int c, string err_ctx)
		: row(r), col(c), error_context(err_ctx)
	{}
};

struct FlagInfo {
	FlagInfo() {}
	virtual bool isPrimaryKey() = 0;
	virtual bool isTenantKey() = 0;
	virtual string toString() = 0;
};

struct PrimaryKey: public FlagInfo {
	bool isPrimaryKey() { return true; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("PRIMARY KEY" );
	}
};

struct ForeignKey: public FlagInfo {
	string table_name;
	string field_name;
	ForeignKey(string t_name, string f_name):
		table_name(t_name), field_name(f_name)
	{ }
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("FOREIGN KEY REFERENCES  " + table_name + "(" + field_name + ")" );
	}
};

struct TenantId: public FlagInfo {
	TenantId() {}
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return true; }
	string toString() {
		return string("TENANT_ID ");
	}
};

struct SearchKey: public FlagInfo {
	SearchKey() {}
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("SEARCH_KEY ");
	}
};


struct DefaultNow: public FlagInfo {
	DefaultNow() {}
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("DEFAULT now()");
	}
};

struct NotNull: public FlagInfo {
	NotNull() {}
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("NOT NULL");
	}
};

struct UniqueKey: public FlagInfo {
	UniqueKey() {}
	bool isPrimaryKey() { return false; }
	bool isTenantKey() { return false; }
	string toString() {
		return string("UNIQUE");
	}
};

string capitaliseSingular(string s);
string capitalise(string s) ;
string singular(string s) ;

struct FieldInfo {
	string field_name;
	string data_type;
	vector<FlagInfo*> flag_info_vec;
	FieldInfo(string f_name, string d_type,
		vector<FlagInfo*> flag_info_v
		):
		field_name(f_name), data_type(d_type),
		flag_info_vec(flag_info_v)
	{ }
	FieldInfo(string f_name, string d_type
		):
		field_name(f_name), data_type(d_type),
		flag_info_vec()
	{ }
	bool isPrimaryKey() { 
		for (int i = 0; i < flag_info_vec.size();
				++i) {
			if (flag_info_vec[i]->isPrimaryKey()) {
				return true;
			}
		}
		return false;
	}
	bool isTenantKey() { 
		for (int i = 0; i < flag_info_vec.size();
				++i) {
			if (flag_info_vec[i]->isTenantKey()) {
				return true;
			}
		}
		return false;
	}
	string toString() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		ss << "field_name: " << field_name
			<< ", field_type: " << data_type ;
		for (int i = 0; i < flag_info_vec.size(); ++i) {
			ss << ' ' << flag_info_vec[i]->toString();
		}
		

		return ss.str();
		
	}

	string fieldSingularCapitalised() {
		//using std::endl;
		//using std::cout;
		//string s1 ( table_name.substr(0, table_name.size()-1));
		//s1[0] = toupper(s1[0]);
		//cout << "tableNameSingularCapitalised: " << s1 << endl;
		//return s1;
		return capitaliseSingular(field_name);
	}

};

//{
//	using std::endl;
//	using std::cout;
//	// assume suffix is "s" and we chop if off
//	// obviously this is not going to work in a lot of
//	// cases
//	string s1 ( s.substr(0, s.size()-1));
//	s1[0] = toupper(s1[0]);
//	//cout << "tableNameSingularCapitalised: " << s1 << endl;
//	return s1;
//}

struct Table {
	string table_name;
	vector<FieldInfo*>  field_info;
	Table(std::string t_name
		, const vector<FieldInfo*> & f_info
	):
		table_name(t_name)
		, field_info(f_info)
	{
		using std::cout;
		using std::endl;

		cout << "field_info.size() : "
			<< field_info.size() 
			<< endl;
		cout << to_string() << endl;
	}

	string tableNameCapitalised() {
		//using std::endl;
		//using std::cout;
		//string s1 ( table_name);
		//s1[0] = toupper(s1[0]);
		////cout << "tableNameSingularCapitalised: " << s1 << endl;
		//return s1;
		return capitalise(table_name);
	}

	string modelForCreate() {
		return capitaliseSingular(table_name) + "ForCreate";
	}

	//string valTableNameForCreate() {
	//	return loweredCamelCase() + "ForCreate";
	//}

	string valModelForCreate() {
		return singular(table_name) + "ForCreate";
	}

	string loweredCamelCase() {
		// FIXME
		return table_name;
	}

	string tableNameSingular() {
		using std::endl;
		using std::cout;
		string s1 ( table_name.substr(0, table_name.size()-1));
		return s1;
	}

	string tableNameSingularCapitalised() {
		//using std::endl;
		//using std::cout;
		//string s1 ( table_name.substr(0, table_name.size()-1));
		//s1[0] = toupper(s1[0]);
		//cout << "tableNameSingularCapitalised: " << s1 << endl;
		//return s1;
		return capitaliseSingular(table_name);
	}

	string to_string() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		ss << "table_name: " << table_name << endl;
		for (int i= 0; i < field_info.size();  ++i) {
			//ss << "field_name: " << field_info[i]->field_name
			//	<< ", field_type: " << field_info[i]->data_type
			//	<< endl;
			ss << field_info[i]->toString() << endl;
		}
		return ss.str();
	}

	string wrapped_result_to_classtype_scala() {
		using std::stringstream;
		using std::endl;
		using std::cout;
		//extern map<string, string> postgres_to_scala_map;
		extern map<string, string> postgres_to_db_conv_map;
		//cout << "ENTER wrapped_result_to_classtype_scala"
		//	<< endl;
		stringstream ss;
		//cout << "field_info.size(): " << field_info.size()
		//	<< endl;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << field_name << " = " 
				<< " rs ." 
				<< postgres_to_db_conv_map[data_type]
				<< "(\"" << field_name << "\")" << endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string dao_create_params() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		ss
			<< valModelForCreate()
			<< " : "
			<< modelForCreate() << endl;

		return ss.str();
	}

	string params_scala() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		extern map<string, string> postgres_to_scala_map;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << field_name << " : " 
				<< postgres_to_scala_map[data_type]
				<< endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string params_scala_without_primary_key() {
		using std::stringstream;
		using std::endl;
		using std::cout;
		stringstream ss;
		vector <FieldInfo*> filtered_rows;
		for (int i= 0; i < field_info.size();  ++i) {
			//ss << field_info[i]->toString() 
			//	<< endl;
			if (field_info[i]->isPrimaryKey()) {
			} else {
				filtered_rows.
					push_back(field_info[i]);
			}
		}
		extern map<string, string> postgres_to_scala_map;
		for (int i= 0; i < filtered_rows.size();  ++i) {
			string field_name = filtered_rows[i]->field_name;
			string data_type = filtered_rows[i]->data_type ;
			ss << field_name << " : " 
				<< postgres_to_scala_map[data_type]
				<< endl;
			if (i != filtered_rows.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string tenant_and_id_params_scala() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		vector<FieldInfo*> filtered_recs;
		for (int i= 0; i < field_info.size();  ++i) {
			// vector<FlagInfo*> flag_info_vec = 
			// 	field_info[i]->flag_info_vec;
			if (field_info[i]->isPrimaryKey() ||
				field_info[i]->isTenantKey())
			{
				//ss << field_name << " : " 
				//	<< data_type << endl;
				filtered_recs.push_back(
					field_info[i]);
			}

		}

		extern map<string, string> postgres_to_scala_map;
		for (int i= 0; i < filtered_recs.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss
				<< (field_info[i]->isPrimaryKey() ?
					(tableNameSingular() +
					capitalise(field_name))
						: field_name) 
				<< " : " 
				<< postgres_to_scala_map[data_type] << endl;
			if (i != filtered_recs.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string tenant_and_id_where_clause_sql() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		vector<FieldInfo*> filtered_recs;
		for (int i= 0; i < field_info.size();  ++i) {
			// vector<FlagInfo*> flag_info_vec = 
			// 	field_info[i]->flag_info_vec;
			if (field_info[i]->isPrimaryKey() ||
				field_info[i]->isTenantKey())
			{
				//ss << field_name << " : " 
				//	<< data_type << endl;
				filtered_recs.push_back(
					field_info[i]);
			}

		}

		for (int i= 0; i < filtered_recs.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			if (filtered_recs[i]->isPrimaryKey()) {
				ss << field_name << " = " 
					<< "$" << tableNameSingular() 
					<< capitalise(field_name)
					<< endl;
			} else {
				ss << field_name << " = " 
					<< "$" 
					<< field_name << endl;
			}
			if (i != filtered_recs.size() - 1) {
				ss << " AND ";
			}
		}
		return ss.str();
	}


	string insert_stmt_keys() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		vector<FieldInfo*> filtered_recs;
		for (int i= 0; i < field_info.size();  ++i) {
			// string field_name = field_info[i]->field_name;
			// string data_type = field_info[i]->data_type ;
			// ss << "\t\t\t" << field_name ;
			// if (i != field_info.size() - 1) {
			// 	ss << ",";
			// }
			// ss << endl;
			if (field_info[i]->isPrimaryKey() ) {
			} else {
				filtered_recs.push_back(field_info[i]);
			}
		}
		for (int i= 0; i < filtered_recs.size();  ++i) {
			string field_name = filtered_recs[i]->field_name;
			string data_type = filtered_recs[i]->data_type ;
			ss << "\t\t\t" << field_name ;
			if (i != filtered_recs.size() - 1) {
				ss << ",";
			}
			ss << endl;
		}
		return ss.str();
	}

	string insert_stmt_values() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		vector<FieldInfo*> filtered_recs;
		for (int i= 0; i < field_info.size();  ++i) {
			//string field_name = field_info[i]->field_name;
			//string data_type = field_info[i]->data_type ;
			//ss << "\t\t\t${"
			//	<< valModelForCreate()
			//	<< "."
			//	<< field_name << "}" ;
			//if (i != field_info.size() - 1) {
			//	ss << ","<< endl;
			//}
			if (field_info[i]->isPrimaryKey() ) {
			} else {
				filtered_recs.push_back(field_info[i]);
			}
		}
		for (int i= 0; i < filtered_recs.size();  ++i) {
			string field_name = filtered_recs[i]->field_name;
			string data_type = filtered_recs[i]->data_type ;
			ss << "\t\t\t${"
				<< valModelForCreate()
				<< "."
				<< field_name << "}" ;
			if (i != filtered_recs.size() - 1) {
				ss << ","<< endl;
			}
		}
		return ss.str();
	}

};

void generate_scala_play( const map<string, 
	Table*> & table_details);

#endif /* common_defs_h */

