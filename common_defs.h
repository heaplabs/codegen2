#ifndef common_defs_h
#define common_defs_h
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
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
};

struct PrimaryKey: public FlagInfo {
};

struct ForeignKey: public FlagInfo {
	string table_name;
	string field_name;
	ForeignKey(string t_name, string f_name):
		table_name(t_name), field_name(f_name)
	{ }
};

struct TenantId: public FlagInfo {
	TenantId() {}
};

struct SearchKey: public FlagInfo {
	SearchKey() {}
};



struct FieldInfo {
	string field_name;
	string data_type;
	vector<FlagInfo*> flag_info_vec;
	FieldInfo(string f_name, string d_type):
		field_name(f_name), data_type(d_type)
	{ }
};

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

	string to_string() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		ss << "table_name: " << table_name << endl;
		for (int i= 0; i < field_info.size();  ++i) {
			ss << "field_name: " << field_info[i]->field_name
				<< ", field_type: " << field_info[i]->data_type
				<< endl;
		}
		return ss.str();
	}

	string wrapped_result_to_classtype_scala() {
		using std::stringstream;
		using std::endl;
		using std::cout;
		//cout << "ENTER wrapped_result_to_classtype_scala"
		//	<< endl;
		stringstream ss;
		cout << "field_info.size(): " << field_info.size()
			<< endl;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << field_name << " = " 
				<< " rs ." << data_type
				<< "(\"" << field_name << "\")" << endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string params_scala() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << field_name << " : " 
				<< data_type << endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string insert_stmt_keys() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << field_name << endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

	string insert_stmt_values() {
		using std::stringstream;
		using std::endl;
		stringstream ss;
		for (int i= 0; i < field_info.size();  ++i) {
			string field_name = field_info[i]->field_name;
			string data_type = field_info[i]->data_type ;
			ss << "$" << field_name << endl;
			if (i != field_info.size() - 1) {
				ss << ",";
			}
		}
		return ss.str();
	}

};

void generate_scala_play( const map<string, 
	Table*> & table_details);

#endif /* common_defs_h */

