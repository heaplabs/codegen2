#ifndef common_defs_h
#define common_defs_h
#include <string>
#include <vector>
#include <map>
#include <sstream>
using std::string;
using std::vector;
using std::map;

struct error_pos {
	int row, col; string error_context;
	error_pos(int r, int c, string err_ctx)
		: row(r), col(c), error_context(err_ctx)
	{}
};

	struct FieldInfo {
		string field_name;
		string data_type;
		FieldInfo(string f_name, string d_type):
			field_name(f_name), data_type(d_type)
		{ }
	};
	struct Table {
		string table_name;
		vector<FieldInfo>  field_info;
		Table(std::string t_name
			, const vector<FieldInfo> & f_info
		):
			table_name(t_name)
			, field_info(f_info)
		{ }
		string to_string() {
			using std::stringstream;
			using std::endl;
			stringstream ss;
			ss << "table_name: " << table_name << endl;
			for (int i= 0; i < field_info.size();  ++i) {
				ss << "field_name: " << field_info[i].field_name
					<< ", field_type: " << field_info[i].data_type
					<< endl;
			}
			return ss.str();
		}
	};

#endif /* common_defs_h */

