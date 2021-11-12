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
		<< "def"
		<< " "
		<< t->fnCreateDAO()
		<< "(" << endl
		//<< t->params_scala()
		<< t->dao_create_params()
		<< ")" << ": Try[Option["
		<< t->tableNameSingularCapitalised()
		<< "]]"
		<< endl;
	return ss.str();
}


string gen_get_signature(Table *t)
{
	stringstream ss;
	ss
		<< "def" << " "
		<< " get" << t->tableNameSingularCapitalised()
		<< "("
		<< endl
		<< t->tenant_and_id_params_scala()
		<< ")"
		<< ": Try[Option["
		<< t->tableNameSingularCapitalised() << "]]"
		<< endl
		<< endl;
	return ss.str();
}

void generate_get(Table * t, stringstream & ss, stringstream & ss_trait)
{
	ss
		<< gen_get_signature(t)
		<< " = Try {" << endl
		<< "DB readOnly { implicit session =>" << endl;
	ss_trait
		<< gen_get_signature(t)
		<< endl;
	ss
		<< "sql\"\"\"" << endl;
	ss
		<< "\t\tselect " << endl
		<< t->all_db_keys()
		<< endl
		<< "\t\tfrom " << t->table_name << endl
		<< "\t\twhere " << endl
		<< "\t\t" << t->tenant_and_id_where_clause_sql()
		<< endl;
	ss
		<< "\"\"\"" << endl
        	<< "\t\t.map(rs => fromDB(rs).get)" << endl
        	<< "\t\t.single()" << endl
        	<< "\t\t.apply()" << endl
		<< endl;

	ss
		<< " // closes readonly session " << endl
		<< "}"	
		<< endl;
	ss
		<< " // closes fn " << endl
		<< "}" << endl
		<< endl ;
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
		<< "DB autoCommit { implicit session =>" << endl;

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
		<< t->fnDeleteDAO()
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
		<< "DB.autoCommit { implicit session =>"
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
		<< ".models.{ "
		<< t->tableNameSingularCapitalised()
		<< ", "
		<< t->tableNameSingularCapitalised() << "ForCreate"
		<< " }" 
		<< endl;

	ss_trait << "import scala.util.Try" << endl;

	ss_trait << "trait "
		<< t->tableNameSingularCapitalised()
		<< "DAOTrait {"
		<< endl;

	generate_fromDB(t, ss);

	generate_create(t, ss, ss_trait);
	generate_get(t, ss, ss_trait);
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
		//<< "createNew" << t->tableNameSingularCapitalised()
		<< t->service_create_def()
		<< "(" << endl
		<< t->valModelForCreate()
		<< " : "
		<< t->modelForCreate() << endl
		<< ") : " 
		<< "Either["
		<< t->createError()
		<< ", "
		<< t->model()
		<< "] = {" << endl
		<< endl;

	ss
		<< "\t" << t->valDAO()
		<< "."
		<< t->fnCreateDAO()
		<< "(" << endl
		<< "\t\t" << t->valModelForCreate()
		<< " = "
		<< t->valModelForCreate()
		<< endl
		<< "\t) match {" << endl
		<< "\t\tcase Failure(exception) =>" << endl
		<< "\t\t  Left(" << endl
		<< "\t\t    " << t->tableNameSingularCapitalised() << "CreateError" << endl
		<< "\t\t      .SQLException(err = exception))" << endl
		<< "\t\tcase Success(Some("<< t->valModelForCreate() << ")) => " << endl
		<< "\t\t     Right(" << t->valModelForCreate() << ")" << endl
		<< "\t}" << endl;

	ss
		<< "}"
		<< endl
		<< endl;
}

string generate_service_get_by_id_signature(Table * t)
{
	stringstream ss;
	ss
		<< "def "<< t->service_get_by_id_def()
		<< "("
		<< endl
		<< t->tenant_and_id_params_scala()
		<< endl
		      << ") : "
		<< "Either["
		<< t->getError()<< ", "
		<< t->model() 
		<< "]"
		<< endl;
	return ss.str();
}

void generate_service_get_by_id (Table * t,
		stringstream & ss,
		stringstream & ss_trait)
{
	// ss << "def "<< t->service_get_by_id_def()
	// 	<< "("
	// 	<< endl
	//   << t->tenant_and_id_params_scala()
	//   << endl
	// 	<< ") : "
	//   << "Either["
	//   << t->getError()<< ", "
	//   << t->model() 
	//   << "]"
	//   << endl;
	ss
		<< generate_service_get_by_id_signature(t)
		<< "= {"
		<< endl;
	ss << "  "
		<< t->valDAO()
		<< "."
		<<  t->fnGetDAO()
		<< "("
		<< t->tenant_and_id_vars_scala()
		<< ") match {" << endl;
	ss
		<< "    case Failure(exception) => Left(" << endl;
	ss
		<< "      "
		<< t->getError()
		<< ".SQLException(err = exception))" << endl;
	ss
		<< "    case Success(None) =>" << endl;
	ss
		<< "      Left("
		<< t->getError()
		<< "."
		<< t->getClientNotFoundError()
		<< ")" << endl;
	ss
		<< "    "
		<< "case Success(Some("
		<< t->valModel()
		<< ")) =>" << endl;
	ss
		<< "      "
		<< "Right("
		<< t->valModel()
		<< ")" << endl;
	ss
		<< "  }" << endl;
	ss
		<< "// closes fn " << endl
		<< "}" << endl;
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
		<< "api" << "." << t->table_name
		<< "." << "models" << ".{"
		<< t->tableNameSingularCapitalised()
		<< ", "
		<< t->tableNameSingularCapitalised() << "ForCreate"
		<< "}"
		<< endl;
	ss
		<< "import cats.syntax.either._"
		<< endl;
	ss
		<< "// import utils.jodatimeutils.JodaTimeUtils"
		<< endl
		<< "import scala.util.{Failure, Success}"
		<< endl;

	// ERRORS
	//
	// == CreateError
	ss
		<< "sealed trait" << " "
		<< t->tableNameSingularCapitalised() << "CreateError"
		<< endl
		<< endl;

	ss
		<< "object "
		<< t->createError()
		<< " {" << endl;
	ss
		<< "\tcase class SQLException(err: Throwable) extends "
		<< t->createError()
		<< endl
		<< endl;

	ss
		<< "}" << endl;


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
		<< t->getError()
		<< endl
		<< endl;

	ss
		<< "object "
		<< t->getError()
		<< " {" << endl;
	ss
		<< "\tcase class SQLException(err: Throwable) extends "
		<< t->getError()
		<< endl
		<< endl;

  	ss
		<< "\tcase object "
		<< t->getClientNotFoundError()
		<< " extends "
		<< t->getError()
		<< endl
		<< endl;

	ss
		<< "}" << endl;

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
	generate_service_get_by_id(t, ss, ss_trait);
	

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
		<< ".models"
		<< endl
		<< endl;


	ss
		<< "import org.joda.time.DateTime" << endl
		<< "import play.api.libs.json.{JsValue, Json, Writes}"
		<< endl
		<< endl;

	//ss
	//	<< "import awscala.DateTime" << endl
	//	<< "import play.api.libs.json.Json" << endl
	//	<< endl
	//	;

	ss
		<< "case class " << t->classNameModel() << "(" << endl
		<< t->params_scala()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "object " << t->classNameModel() << "{" << endl
		<< "\timplicit val writes = new Writes["<< t->classNameModel() << "] {" << endl
		<< "\t\tdef writes ("
		<< t->valModel()
		<< ":"
		<< t->classNameModel()
		<< ") : JsValue = {" << endl
		<< "\t\tJson.obj(" << endl
		<< t->params_for_model_scala_to_json()
		<< "\t\t      )" << endl
		<< "\t\t}" << endl
		<< "\t}" << endl
		<< "}" << endl
		<< endl;

	ss
		<< "case class " << t->tableNameSingularCapitalised() << "ForCreate(" << endl
		<< t->params_scala_without_primary_key()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "object " << t->tableNameSingularCapitalised() << "ForCreate {" << endl
		<< "implicit val reads = Json.reads["<< t->tableNameSingularCapitalised() << "ForCreate" << "]" << endl
		<< "}" << endl
		<< endl
		<< endl;

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

void generate_controller_hello(Table * t, stringstream & ss)
{
	ss << "def hello(): Action[AnyContent] = Action {" << endl;
	ss << "    Logger.info(\"hello call for ClientController hello api/v1/support\")" << endl;
	ss << "    val json = Json.obj(\""<< t->classNameController() << "\" -> \"hello\")" << endl;
	ss << "" << endl;
	ss << "    Ok(json)" << endl;
	ss << "  }" << endl
		<< endl;
}


void generate_controller_create(Table * t, stringstream & ss)
{
	using std::stringstream;
	using std::endl;
	ss << "def " << t->controller_create_def() << "(): Action[JsValue] =  Action.async(parse.json){ implicit request =>" << endl;
	ss << "    Logger.info(s\"" << t->controller_create_def() << " recd req: ${request.body}\")" << endl;
	ss << "    val validateData =request.body.validate["<< t->modelForCreate() << "]" << endl;
	ss << "    val wrongRequest = new Exception(ERROR_INVALID_REQUEST)" << endl;
	ss << "    //val Res = request.Response" << endl;
	ss << "" << endl;
	ss << "    validateData match {" << endl;
	ss << "      case JsSuccess(value, _) =>" << endl;
	ss << "        val account = " << t->valService() << "." << t->service_create_def() << "(value)" << endl;
	ss << "        account match {" << endl;
	ss << "          case Left("<< t->createError() << ".SQLException(err)) =>" << endl;
	ss << "          //case Left( err) =>" << endl;
	ss << "            //val organizationJson = Json.toJson(err)" << endl;
	ss << "            Future.failed(err)" << endl;
	ss << "" << endl;
	ss << "          case Right(" << t->valModel() << ") => Future.successful(Ok(Json.toJson(" << t->valModel() << ") ))" << endl;
	ss << "        }" << endl;
	ss << "      case JsError(_) =>" << endl;
	ss << "        Logger.info(\"Failed parsing request\")" << endl;
	ss << "        Future.failed(wrongRequest)" << endl;
	ss << "    }" << endl;
	ss << "  }" << endl;
	ss << endl;
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

	//ss
	//	<< "import "
	//	<< "api"
	//	<< "."
	//	<< "{CONSTANTS, CacheService}" << endl;
	//ss
	//	<< "import "
	//	<< "api"
	//	<< "."
	//	<< "accounts.{PermType, PermissionUtils}"<< endl;

	//ss
	//	<< "import "
	//	<< "api"
	//	<< "."
	//	<< t->table_name
	//	<< ".models."
	//	<< t->table_name << "ForAdding" << endl;
	//ss
	//	<< "import "
	//	<< "api"
	//	<< "."
	//	<< t->table_name
	//	<< ".services.{"
	//	<< t->table_name << "AddError" << ", "
	//	<< t->table_name << "Service"
	//	<< "}"
	//	<< endl;
	//ss
	//	<< "import play.api.libs.json.{JsError, JsSuccess, JsValue, Json}" << endl
	//	<< "import play.api.mvc.{Action, Controller}" << endl
	//	<< "import utils.Helpers" << endl;
	//	============ BEGIN imports ======
	string capitalisedTableName = t->model();

	ss << "import api.CONSTANTS.API_MSGS.{ERROR_INVALID_REQUEST, ERROR_NOT_FOUND_ACCOUNT}" << endl;
	ss
		<< "import api.clients.models."
		<< t->modelForCreate()
		<< endl;
	ss << "import api.clients.services.{"<< capitalisedTableName << "CreateError, ClientService}" << endl;
	ss << "import play.api.Logger" << endl;
	ss << "import play.api.mvc.{Action, AnyContent, Controller}" << endl;
	ss << "import play.api.libs.json.{JsError, JsSuccess, JsValue, Json}" << endl;
	ss << "" << endl;
	ss << "import scala.concurrent.Future" << endl;
	ss << "import scala.util.{Failure, Success}" << endl;
	//	============ END imports ======
	

	ss << "class " << t->classNameController() << "(" << endl
		<< t->valService()
		<< " : " << capitalisedTableName
		<< "Service" << endl
		<< ") " << endl
		<< "extends Controller " << endl
		<< "// with PermissionUtils" << endl
		<< "{" << endl
		<< endl;

	//generate_controller_getall(t, ss);
	//generate_controller_addSingleEntry(t, ss);
	generate_controller_hello(t, ss);
	generate_controller_create(t, ss);


	ss
		<< "// closes class" << endl
		<< "}";
	return ss.str();
}

