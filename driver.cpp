#include "common_defs.h"
#include "sql.tab.h"

extern map<string, Table*> table_details;

#include <iostream>

using namespace std;
void yyerror(char const * err)
{
	cout << "error while parsing: " << err << endl;
}

string generate_dao(Table * t);

void generate_scala_play( const map<string, Table*> & table_details)
{
	for(map<string, Table*>::const_iterator cit = 
		table_details.begin(); cit != table_details.end();
		++cit) {
		cout << generate_dao(cit->second);
	}
	
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
	generate_scala_play(table_details);
}

void generate_fromDB(Table * t, stringstream & ss)
{

	ss << "private def fromDB(" << endl << "rs: WrappedResultSet" << endl
		<< "): Try[" << t->table_name << "] = Try {" << endl;
	ss << t->table_name << "(" << endl;
	ss << t->wrapped_result_to_classtype_scala();
	ss << ")" << endl;
	ss << "}" << endl;
}

void generate_create(Table * t, stringstream & ss)
{
	ss << "final def" << " " << " create" << t->table_name << "("
		<< endl
		<< t->params_scala()
		<< ")" << ": Try[Option[" << t->table_name << "]] = Try {"
		<< "DB.autocommit { implicit session =>" << endl;
	
	ss << "sql\"\"\"" << endl
		<< "insert into " << t->table_name << endl
		<< "(" << endl;
	ss << t->insert_stmt_keys();
	ss << ") values (" << endl;
	ss << t->insert_stmt_values();
	ss << ") on conflict do nothing" << endl
		<< "returning *;" << endl
		<< "\"\"\"" << endl
		<< "map(rs => fromDB(rs).get)" << endl
		<< ".single" << endl
		<< ".apply()" << endl;

	ss 
		<< " // closes autocommit " << endl
		<< "}"	
		<< endl; 


	ss
		<< " // closes fn " << endl
		<< "}" << endl;

}

string generate_dao(Table * t)
{
	using std::stringstream;
	stringstream ss;
	ss << "package " << "api" << "." << t->table_name << "." << "dao" << endl;
	ss << endl;
	ss << "import "  << "api" << "." << t->table_name << "." << "models" << "." << t->table_name;
	ss << endl;
	ss << "import scalikejdbc.{AutoSession, DB, WrappedResultSet, scalikejdbcSQLInterpolationImplicitDef}" << endl;
	ss << "import scala.util.Try" << endl;

	ss << "class " << t->table_name   << "DAO" << endl 
		<< " extends  " << t->table_name << "Trait" << " { " << endl;
	ss << "private implicit val session: AutoSession = AutoSession" << endl;

	generate_fromDB(t, ss);

	generate_create(t, ss);

	ss
		<< "// closes class " <<endl
		<< "}" << endl;

	return ss.str();
	
}
