#include <iostream>
#include <set>
#include <sstream>

#include "seed_db.h"
#include "common_defs.h"

using std::string;
using std::deque;
using std::pair;
using std::cout;
using std::endl;
using std::set;
using std::stringstream;


extern map<string, Table*> table_details;

extern std::map<std::string, std::set<std::string>> table_relations;

void generate_insert_for_table(
	Table * t,
	const map<string, Table*> & table_details,
	set<string> table_relations_set,
	const std::map<std::string, std::set<std::string>> & table_relations)
{
	cout << "Enter generate_insert_for_table " << t->table_name << endl;
	stringstream ss_ins;
	ss_ins << "insert into " << t->table_name << " ("  << endl;
	stringstream ss_field_names;
	for (int i = 0; i < t->field_info.size(); ++i) {
		FieldInfo * f = t->field_info[i];
		ss_field_names << f->field_name;	
		if (i < t->field_info.size() - 1) {
			ss_field_names << ", ";
		}
	}
	ss_ins << ss_field_names.str();
	ss_ins << ") values (" << endl;
	stringstream ss_field_values;
	for (int i = 0; i < t->field_info.size(); ++i) {
		FieldInfo * f = t->field_info[i];
		//if (f->data_type == bigint) {
		//	ss_field_values << "'" << "BIGINT" << "'" ;	
		//} else if (f->data_type {
		//} else {
		//	ss_field_values << "something_else";	
		//}
		switch(f->data_type) {
			case DataType::bigint:
				ss_field_values << "'" << "BIGINT" << "'";
				break;
			case DataType::text:
				ss_field_values << "'" << "TEXT" << "'";
				break;
			case DataType::boolean:
				ss_field_values << "'" << "BOOLEAN" << "'";
				break;
			case DataType::integer:
				ss_field_values << "'" << "INTEGER" << "'";
				break;
			case DataType::date_time_with_timez:
				ss_field_values << "'" << "date_time_with_timez" << "'";
				break;
			case DataType::date_time_without_timez:
				ss_field_values << "'" << "date_time_without_timez" << "'";
				break;
			case DataType::jsonb:
				ss_field_values << "'" << "jsonb" << "'";
				break;
			default:
				ss_field_values << "'" << "unhandled_field_type" << "'";
				break;
		}
		if (i < t->field_info.size() - 1) {
			ss_field_values << ", ";
		}
	}
	ss_ins << ss_field_values.str() << endl;
	ss_ins << "); " << endl;

	cout << ss_ins.str() << endl;

	cout << "Exit generate_insert_for_table" << endl;
}

void generate_insert(
	const string & table_name, 
	const map<string, Table*> & table_details,
	const std::map<std::string, std::set<std::string>> & table_relations)
{
	Table * t = table_details.find(table_name)->second;

	set<string> table_relations_set = table_relations.find(table_name)->second;
	generate_insert_for_table(t, table_details, table_relations_set, table_relations);
}

void generate_data_seeding(const deque<pair<string, int>> & order)  
{
	cout << "ENTER generate_data_seeding" << endl;
	for (int i = 0; i < order.size(); ++i) {
		string table_name = order[i].first;
		if (table_details.find(table_name)!= table_details.end() ) {
			cout << "found : " << table_name << " in table_details" << endl;
		}
		if (table_relations.find(table_name)!= table_relations.end() ) {
			cout << "found : " << table_name << " in table_relations" << endl;
		}
		generate_insert(table_name, table_details, table_relations);
	}
  
	cout << "EXIT generate_data_seeding" << endl;
}
