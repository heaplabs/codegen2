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
string generate_models(Table * t);
string generate_controller(Table * t) ;

void generate_scala_play( const map<string, Table*> & table_details)
{
	for(map<string, Table*>::const_iterator cit = 
		table_details.begin(); cit != table_details.end();
		++cit) {
		cout << "// ======= DAO " << cit->second->table_name <<  "  ====== " << endl;
		cout << generate_dao(cit->second);
		cout << "// ======= Controller " << cit->second->table_name <<  "  ====== " << endl;
		cout << generate_controller(cit->second);
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

void print_map_details(
	const map<string, string> & postgres_to_scala_map)
{
	for (map<string, string>::const_iterator ci =
		postgres_to_scala_map.begin();
		ci != postgres_to_scala_map.end();
		++ci) {
		cout << ci->first << " : " << ci->second
			<< endl;
	}
}

int main() {
	int status = yyparse();
	cout << " status: " << status << endl;
	print_table_details(table_details);
	extern map<string, string> postgres_to_scala_map;
	print_map_details(postgres_to_scala_map);
	generate_scala_play(table_details);
}

void generate_fromDB(Table * t, stringstream & ss)
{

	ss << "private def fromDB(" << endl << "rs: WrappedResultSet" << endl
		<< "): Try[" << t->tableNameSingularCapitalised() << "] = Try {" << endl;
	ss << t->tableNameSingularCapitalised() << "(" << endl;
	ss << t->wrapped_result_to_classtype_scala();
	ss << ")" << endl;
	ss << "}" << endl << endl;
}

void generate_create(Table * t, stringstream & ss)
{
	ss << "final def" << " " << " create" << t->tableNameSingularCapitalised() << "("
		<< endl
		<< t->params_scala()
		<< ")" << ": Try[Option[" << t->tableNameSingularCapitalised() << "]] = Try {"
		<< "DB autocommit { implicit session =>" << endl;
	
	ss << "sql\"\"\"" << endl
		<< "insert into " << t->table_name << endl
		<< "(" << endl;
	ss << t->insert_stmt_keys();
	ss << ") values (" << endl;
	ss << t->insert_stmt_values();
	ss << ") on conflict do nothing" << endl
		<< "returning *;" << endl
		<< "\"\"\"" << endl
		<< ".map(rs => fromDB(rs).get)" << endl
		<< ".single" << endl
		<< ".apply()" << endl;

	ss 
		<< " // closes autocommit " << endl
		<< "}"	
		<< endl; 


	ss
		<< " // closes fn " << endl
		<< "}" << endl
		<< endl
		;

}

void generate_delete(Table * t, stringstream & ss)
{
	ss << "final def" << " " << " delete" << t->table_name << "("
		<< endl
		<< t->tenant_and_id_params_scala()
		<< ")" << ": Try[Option[" << t->table_name << "]] = Try {"
		<< "DB.autocommit { implicit session =>" << endl;
	
	ss << "sql\"\"\"" << endl
	

		<< "delete  from " << t->table_name << endl
		<< "where " << endl
		<< t->tenant_and_id_where_clause_sql()
		;
	//ss << t->insert_stmt_keys();
	//ss << ") values (" << endl;
	//ss << t->insert_stmt_values();
	//ss << ") on conflict do nothing" << endl

	ss
		<< "returning id;" << endl
		<< "\"\"\"" << endl
		<< ".map(rs => fromDB(rs).get)" << endl
		<< ".single()" << endl
		<< ".apply()" << endl
		<< ".get()" << endl;

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
	ss << "package " << "api" 
		<< "." 
		<< t->tableNameSingular()
		<< "." << "dao" << endl;
	ss << endl;
	ss << "import "  << "api" << "." << t->table_name << "." << "models" << "." << t->tableNameSingularCapitalised();
	ss << endl;
	ss << "import scalikejdbc.{AutoSession, DB, WrappedResultSet, scalikejdbcSQLInterpolationImplicitDef}" << endl;
	ss << "import scala.util.Try" << endl;

	ss << "class " << t->tableNameSingularCapitalised()
		<< "DAO" << endl 
		<< " extends  "
		<< t->tableNameSingularCapitalised() << "DAOTrait" << " { " << endl;
	ss << "private implicit val session: AutoSession = AutoSession" << endl;

	generate_fromDB(t, ss);

	generate_create(t, ss);
	generate_delete(t, ss);

	ss
		<< "// closes class " <<endl
		<< "}" << endl;

	return ss.str();
	
}

string generate_models(Table * t)
{
	using std::stringstream;
	stringstream ss;
	using std::endl;
}

void generate_controller_getall(Table * t, stringstream & ss)
{
	using std::stringstream;
	using std::endl;
	ss
		<< "def "
		<< "getAll" << t->table_name
		<< " = " 
		<< " isLoggedIn { request => " << endl
		<< "val res = request.Response" << endl
		<< t->table_name << "Service"
		<< "."
		<< "getAll" << t->table_name << "(" << endl
		<< "// fixme add tenant params here " << endl
		<< ") match {" << endl
		<< "case Left(err) =>" << endl
		<< "	res.ServerError(err = err)" << endl
		<< "case Right(" << t->table_name << ") => "
		<< "	res.Success("
		<< "\"" << t->table_name << "\"," << endl
		<< "data = Json.obj(" << endl
		<< "\"calendars\" -> " << t->table_name << endl
		<< ")" << endl
		<< "// close res.Success" << endl
		<< ")" << endl
		<< "// close match" << endl
		<< "}" << endl
		<< "// closes getAll" << t->table_name << endl
		<< "}" << endl;
}

void generate_controller_addSingleEntry(Table * t, stringstream & ss)
{
	using std::stringstream;
	using std::endl;
	ss
		<< "def " << "add" << t->table_name << "(" << endl
		<< "// FIXME - generate parms" << endl
		<< "): Action[JsValue] = checkPermission(" << endl
		<< "//fixme : add tidOpt param" << endl
		<< "permission = PermType.JUST_LOGGEDIN" << endl
		<< ")" << endl
		<< "(parse.json) { request => " << endl
		<< "val res = request.Response" << endl
		<< "val acc = request.loggedinAccount" << endl
		<< "request.body.validate[" << t->table_name << "] match {" << endl
		<< "case JsError(errors) => " << endl
		<< "  res.JsValidationError(errors = errors)" << endl
		<< endl
		<< "case JsSuccess("<<  t->table_name << ", path) => " << endl
		<< "\t" << t->table_name << "Service." << endl
		<< "\t\t" << "create" << t->table_name << "(" << endl
		<< "\t\t" << ")" << endl

		<< "// close match" << endl
		<< "}" << endl
		<< "// close fn" << endl
		<< "}";
}

string generate_controller(Table * t) {
	using std::stringstream;
	stringstream ss;
	ss << "package " 
		<< "api" << "."
		<< t->table_name
		<< "."
		<< "controllers"
		<< endl;

	ss 
		<< "import "
		<< "api"
		<< "."
		<< "{CONSTANTS, CacheService}" << endl;
	ss 
		<< "import "
		<< "api"
		<< "." 
		<< "accounts.{PermType, PermissionUtils}"<< endl;

	ss
		<< "import "
		<< "api"
		<< "."
		<< t->table_name
		<< ".models."
		<< t->table_name << "ForAdding" << endl;
	ss
		<< "import "
		<< "api"
		<< "."
		<< t->table_name 
		<< ".services.{"
		<< t->table_name << "AddError" << ", "
		<< t->table_name << "Service" 
		<< "}"
		<< endl;
	ss 
		<< "import play.api.libs.json.{JsError, JsSuccess, JsValue, Json}" << endl
		<< "import play.api.mvc.{Action, Controller}" << endl
		<< "import utils.Helpers" << endl;

	ss << "class " << t->table_name << "Controller("
		<< t->table_name << ":" << t->table_name << ","
  		<< "protected val cacheService: CacheService"
		<< ") extends Controller with PermissionUtils" << endl
		<< "{" << endl;

	generate_controller_getall(t, ss);
	generate_controller_addSingleEntry(t, ss);


	ss 
		<< "// closes class" << endl
		<< "}";
	return ss.str();
}

