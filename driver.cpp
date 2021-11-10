#include "common_defs.h"
#include "sql.tab.h"

extern map<string, Table*> table_details;

#include <iostream>

using namespace std;
void yyerror(char const * err)
{
	cout << "error while parsing: " << err << endl;
}


struct ClassAndTrait {
	string defn;
	string defn_trait;
	ClassAndTrait(string p_defn, string p_defn_trait) :
		defn(p_defn), defn_trait(p_defn_trait)
	{ }
};

ClassAndTrait generate_dao(Table * t);
string generate_models(Table * t);
string generate_controller(Table * t) ;
ClassAndTrait generate_service(Table * t);

#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
//namespace filesystem = std::filesystem;
void generate_scala_play( const map<string, Table*> & table_details)
{
	for(map<string, Table*>::const_iterator cit =
		table_details.begin(); cit != table_details.end();
		++cit) {
		// mkdir
		string table_name = cit -> second -> table_name;
		if (!fs::exists(table_name.c_str()) && fs::create_directory(table_name.c_str()) == false ) {
				cout << "cant create directory: "
					<< table_name << " for output files...exiting"
					<< endl;
				exit(3);
		}
		cout << "// ======= DAO " << cit->second->table_name <<  "  ====== " << endl;
		Table * t = cit->second;
		ClassAndTrait dao_dao_trait = generate_dao(t);
		{
			string dao_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + "DAO.scala";
			fstream dao(dao_fname, dao.out);
			dao << dao_dao_trait.defn;
		}
		{
			string dao_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + "DAOTrait.scala";
			fstream dao_trait(dao_fname, dao_trait.out);
			dao_trait << dao_dao_trait.defn_trait;
		}

		cout << "// ======= Controller " << cit->second->table_name <<  "  ====== " << endl;
		string cntrl_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + "Controller.scala";
		fstream cntrl(cntrl_fname, cntrl.out);
		cntrl << generate_controller(cit->second);
		cout << "// ======= Models " << cit->second->table_name <<  "  ====== " << endl;
		string model_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + ".scala";
		fstream model(model_fname, model.out);
		model << generate_models(cit->second);
		cout << "// ======= Service " << cit->second->table_name <<  "  ====== " << endl;
		ClassAndTrait service_service_trait = generate_service(t);
		{
			string service_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + "Service.scala";
			fstream service(service_fname, service.out);
			service << service_service_trait.defn;
		}
		{
			string service_fname =  t->table_name + "/" + t->tableNameSingularCapitalised() + "ServiceTrait.scala";
			fstream service_trait(service_fname, service_trait.out);
			service_trait << service_service_trait.defn_trait;
		}
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

#include <sys/stat.h>
#include <sys/types.h>

int main() {
	int status = yyparse();
	cout << " status: " << status << endl;
	if (status != 0) {
		cout << "error parsing file"
			<< endl;
		exit(2);
	}

	print_table_details(table_details);
	extern map<string, string> postgres_to_scala_map;
	print_map_details(postgres_to_scala_map);
	generate_scala_play(table_details);
}

void generate_fromDB(Table * t, stringstream & ss)
{

	ss
		<< "def fromDB(" << endl
		<< "rs: WrappedResultSet" << endl
		<< "): Try["
		<< t->tableNameSingularCapitalised()
		<< "] = Try {" << endl;
	ss << t->tableNameSingularCapitalised() << "(" << endl;
	ss << t->wrapped_result_to_classtype_scala();
	ss << ")" << endl;
	ss << "}" << endl << endl;
}

string gen_create_signature(Table *t)
{
	stringstream ss;
	ss
		<< "final def"
		<< " "
		<< " create" << t->tableNameSingularCapitalised()
		<< "(" << endl
		//<< t->params_scala()
		<< t->dao_create_params()
		<< ")" << ": Try[Option["
		<< t->tableNameSingularCapitalised()
		<< "]]"
		<< endl;
	return ss.str();
}

void generate_create(Table * t, stringstream & ss, stringstream & ss_trait)
{
	ss
		//<< "final def" << " " << " create" << t->tableNameSingularCapitalised() << "("
		//<< endl
		//<< t->params_scala()
		//<< ")" << ": Try[Option["
		//<< t->tableNameSingularCapitalised()
		//<< "]]"
		<< gen_create_signature(t)
		<< " = Try {" << endl
		<< "DB autocommit { implicit session =>" << endl;

	ss_trait
		<< gen_create_signature(t)
		//<< "final def" << " " << " create" << t->tableNameSingularCapitalised() << "("
		//<< endl
		//<< t->params_scala()
		//<< ")" << ": Try[Option["
		//<< t->tableNameSingularCapitalised()
		//<< "]]" << endl
		<< endl;
	
	ss << "sql\"\"\"" << endl
		<< "\tinsert into " << t->table_name << endl
		<< "\t\t(" << endl;
	ss << t->insert_stmt_keys();
	ss << "\t\t) values (" << endl;
	ss << t->insert_stmt_values() << endl;
	ss << "\t\t) on conflict do nothing" << endl
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
		<< endl ;
}

string gen_delete_signature(Table *t)
{
	stringstream ss;
	ss
		<< "def" << " "
		<< " delete" << t->tableNameSingularCapitalised()
		<< "("
		<< endl
		<< t->tenant_and_id_params_scala()
		<< ")"
		<< ": Try[Option["
		<< "Long" << "]]"
		<< endl
		<< endl;
	return ss.str();
}

void generate_delete(Table * t,
		stringstream & ss,
		stringstream & ss_trait)
{
	string delete_signature = gen_delete_signature(t);

	ss_trait
		<< delete_signature;

	ss
		<< delete_signature
		<< " = Try {" << endl
		<< "DB.autocommit { implicit session =>"
		<< endl;

		//<< "final def" << " "
		//<< " delete" << t->tableNameSingularCapitalised()
		//<< "("
		//<< endl
		//<< t->tenant_and_id_params_scala()
		//<< ")"
		//<< ": Try[Option[" << t->table_name << "]]" << endl
		//<< " = Try {"
		//<< "DB.autocommit { implicit session =>"
		//<< endl
		//<< endl;


		//<< "final def" << " " << " delete" << t->table_name << "("
		//<< endl
		//<< t->tenant_and_id_params_scala()
		//<< ")" << ": Try[Option[" << t->table_name << "]]" << endl;
	
	ss
		<< "sql\"\"\"" << endl
		<< "\t\tdelete  from " << t->table_name << endl
		<< "\t\twhere " << endl
		<< "\t\t" << t->tenant_and_id_where_clause_sql()
		;
	//ss << t->insert_stmt_keys();
	//ss << ") values (" << endl;
	//ss << t->insert_stmt_values();
	//ss << ") on conflict do nothing" << endl

	ss
		<< "\treturning id;" << endl
		<< "\t" << "\"\"\"" << endl
		<< "\t.map(_.long(\"id\"))" << endl
		<< "\t.single()" << endl
		<< "\t.apply()" << endl
		<< endl;

	ss
		<< " // closes autocommit " << endl
		<< "}"	
		<< endl;


	ss
		<< " // closes fn " << endl
		<< "}" << endl;

}

ClassAndTrait generate_dao(Table * t)
{
	using std::stringstream;
	stringstream ss;
	stringstream ss_trait;
	ss << "package " << "api"
		<< "."
		//<< t->tableNameSingular()
		<< t->table_name
		<< "." << "dao" << endl;
	ss << endl;
	ss << "import "  << "api" << "." << t->table_name << "." << "models" << ".{"
		<< t->tableNameSingularCapitalised()
		<< ", "
		<< t->tableNameSingularCapitalised() << "ForCreate"
		<< "}"
		<< endl;
	ss << endl;
	ss << "import scalikejdbc.{AutoSession, DB, WrappedResultSet, scalikejdbcSQLInterpolationImplicitDef}" << endl;
	ss << "import scala.util.Try" << endl;

	ss << "class " << t->tableNameSingularCapitalised()
		<< "DAO" << endl
		<< " extends  "
		<< t->tableNameSingularCapitalised() << "DAOTrait" << " { " << endl;
	ss << "private implicit val session: AutoSession = AutoSession" << endl;

	ss_trait
		<< "package "
		<< "api."
		<< t->table_name
		<< ".dao"<< endl;

	ss_trait
		<< "import "
		<< "api."
		<< t->table_name
		<< ".models."
		<< t->tableNameSingularCapitalised()
		<< endl;

	ss_trait << "import scala.util.Try" << endl;

	ss_trait << "trait "
		<< t->tableNameSingularCapitalised()
		<< "DAOTrait {"
		<< endl;

	generate_fromDB(t, ss);

	generate_create(t, ss, ss_trait);
	generate_delete(t, ss, ss_trait);

	ss
		<< "// closes class " <<endl
		<< "}" << endl;

	ss_trait << "}" << endl;
	const string dao_op = ss.str();
	const string dao_trait_op = ss_trait.str();

	return ClassAndTrait(dao_op, dao_trait_op);
	
}

void generate_service_create(Table * t,
		stringstream & ss,
		stringstream & ss_trait)
{
	ss
		<< "def" << " "
		<< "createNew" << t->tableNameSingularCapitalised()
		<< "(" << endl
		<< t->valModelForCreate()
		<< " : "
		<< t->modelForCreate() << endl
		<< ") : " << endl
		<< "Either["
		<< t->tableNameSingularCapitalised() << "CreateError"
		<< ", "
		<< t->tableNameSingularCapitalised()
		<< "] = {" << endl
		<< endl;
	ss
		<< t->loweredCamelCase() << "DAO" << endl
		<< ".create" << t->tableNameSingularCapitalised()
		<< "(" << endl
		<< t->valModelForCreate() << " = " << t->valModelForCreate()
		<< endl
		<< ") match {" << endl
		<< "\tcase Failure(exception) =>" << endl
		<< "\t  Left(" << endl
		<< "\t    " << t->tableNameSingularCapitalised() << "CreateError" << endl
		<< "\t      .SQLException(err = exception)" << endl
		<< "\t case Success(Some("<< t->valModelForCreate() << ")) => " << endl
		<< "\t     Right(" << t->valModelForCreate() << ")"
		<< "\t   )" << endl
		<< "\t}" << endl;

	ss
		<< "}"
		<< endl
		<< endl;
}

ClassAndTrait  generate_service(Table * t)
{
	using std::stringstream;
	stringstream ss;
	stringstream ss_trait;

	ss
		<< "package "
		<< "api" << "."
		<< t->table_name
		<< "."
		<< "services" << endl
		<< endl;
	ss_trait
		<< "package "
		<< "api" << "."
		<< t->table_name
		<< "."
		<< "services" << endl
		<< endl;


	ss
		<< "import "
		<< "api"
		<< "."
		<< t->table_name
		<< "."
		<< "dao"
		<< ".{"
		<< t->tableNameSingularCapitalised() << "DAO"
		<< " }"
		<< endl;
	ss
		<< "import "
		<< "api"
		<< "."
		<< t->table_name
		<< "." << "models" << "."
		<< "{ "
		<< t->tableNameSingularCapitalised()
		<< " }"
		<< endl;
	ss
		<< "import cats.syntax.either._"
		<< endl;
	ss
		<< "import utils.jodatimeutils.JodaTimeUtils"
		<< endl
		<< endl;

	// ERRORS
	//
	// == CreateError
	ss
		<< "sealed trait" << " "
		<< t->tableNameSingularCapitalised() << "CreateError"
		<< endl
		<< endl;

	// == UpdateError
	ss
		<< "sealed trait" << " "
		<< t->tableNameSingularCapitalised() << "UpdateError"
		<< endl
		<< endl;

	// == DeleteError
	ss
		<< "sealed trait" << " "
		<< t->tableNameSingularCapitalised() << "DeleteError"
		<< endl
		<< endl;

	// == GetError
	ss
		<< "sealed trait" << " "
		<< t->tableNameSingularCapitalised() << "GetError"
		<< endl
		<< endl;

	ss
		<< "class " << t->tableNameSingularCapitalised()
		<< "Service"
		<< "(" << endl
		<< t->loweredCamelCase()
		<< "DAO"
		<< " : "
		<< t->tableNameSingularCapitalised()
		<< "DAO" << endl
		<< ") {" << endl
		<< endl;

	generate_service_create(t, ss, ss_trait);
	

	ss
		<< "}"
		<< endl;


	const string service_op = ss.str();
	const string service_trait_op = ss_trait.str();

	return ClassAndTrait(service_op, service_trait_op);
}

string generate_models(Table * t)
{
	using std::stringstream;
	stringstream ss;
	using std::endl;
	ss
		<< "package api." << t->table_name
		<< ".models" << endl;


	ss
		<< "import awscala.DateTime" << endl
		<< "import play.api.libs.json.Json" << endl
		<< endl
		;

	ss
		<< "case class " << t->tableNameSingularCapitalised() << "(" << endl
		<< t->params_scala()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "case class " << t->tableNameSingularCapitalised() << "ForCreate(" << endl
		<< t->params_scala_without_primary_key()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "object " << t->tableNameSingularCapitalised() << "{" << endl
		<< "implicit val writes = Json.writes["
		<< t->tableNameSingularCapitalised()
		<< "]" << endl
		<< "}" << endl;

	return ss.str();
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

