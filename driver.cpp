#include "common_defs.h"
#include "sql.tab.h"
#include "graph.h"
#include <algorithm>
#include "seed_db.h"

extern map<string, Table*> table_details;
extern std::map<std::string, std::set<std::string>> table_relations;

#include <iostream>

extern int n_lines;

using namespace std;
void yyerror(char const * err)
{
	cout << "error while parsing: " 
		<< err 
		<< ", n_lines: " << n_lines
		<< endl;
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
		if (!fs::exists(table_name.c_str())
			&& fs::create_directory(table_name.c_str())
			== false ) {
				cout << "cant create directory: "
					<< table_name << " for output files...exiting"
					<< endl;
				exit(3);
		}
		cout << "// ======= DAO " << cit->second->table_name <<  "  ====== " << endl;
		Table * t = cit->second;
		ClassAndTrait dao_dao_trait = generate_dao(t);
		{
			string dao_fname =  t->daoFileName();
			fstream dao(dao_fname, dao.out);
			dao << dao_dao_trait.defn;
		}
		{
			string dao_fname =  t->daoTraitFileName();
			fstream dao_trait(dao_fname, dao_trait.out);
			dao_trait << dao_dao_trait.defn_trait;
		}

		cout << "// ======= Controller " << cit->second->table_name <<  "  ====== " << endl;
		string cntrl_fname =  t->controllerFileName();
		fstream cntrl(cntrl_fname, cntrl.out);
		cntrl << generate_controller(cit->second);
		cout << "// ======= Models " << cit->second->table_name <<  "  ====== " << endl;
		string model_fname =  t->modelFileName();
		fstream model(model_fname, model.out);
		model << generate_models(cit->second);
		cout << "// ======= Service " << cit->second->table_name <<  "  ====== " << endl;
		ClassAndTrait service_service_trait = generate_service(t);
		{
			string service_fname =  t->serviceFileName();
			fstream service(service_fname, service.out);
			service << service_service_trait.defn;
		}
		{
			string service_fname =  t->serviceTraitFileName();
			fstream service_trait(service_fname, service_trait.out);
			service_trait << service_service_trait.defn_trait;
		}
	}
	
}

Graph build_table_relations_graph(const map<string, Table*> &table_details)
{
	map<string, set<string> > table_relations;
	for (map<string, Table*>::const_iterator ci = table_details.begin();
		ci != table_details.end(); ++ci) {
		set<string> deps;
		Table * t = ci->second;
		for (int i = 0; i < t->field_info.size(); ++i) {
			FieldInfo * f = t->field_info[i];
			for (int j =0; j < f->flag_info_vec.size(); ++j) {
				FlagInfo * flg_info = f->flag_info_vec[j];
				if (flg_info->isForeignKey()) {
					ForeignKey * fk_flag = (ForeignKey*) flg_info;
					deps.insert(fk_flag->table_name);
				}
			}
		}
		table_relations[t->table_name] = deps;
	}
	cout << "BEGIN build_table_relations_graph: deps between tables" << endl;
	for (map<string, set<string> > :: const_iterator ci = table_relations.begin();
			ci != table_relations.end(); ++ci) {
		cout << ci->first << ": " ;
		const set<string> & deps = ci->second;
		for (set<string>::const_iterator ci = deps.begin();
			ci != deps.end(); ++ci) {
			cout << " " << *ci; 
		}
		cout << endl;
	}
	cout << "END build_table_relations_graph: deps between tables" << endl;
	Graph g(table_relations);
	return g;

}

void print_table_details( const map<string, Table*> & table_details)
{
	cout << "ENTER print_table_details" << endl;
	for(map<string, Table*>::const_iterator cit =
		table_details.begin(); cit != table_details.end();
		++cit) {
		cout << cit->first << endl
			<< cit->second->to_string()
			<< endl;
	}
	cout << "EXIT print_table_details" << endl;
}

void print_map_details(
	const map<string, string> & postgres_to_scala_map)
{
	cout << "ENTER print_map_details" << endl;
	for (map<string, string>::const_iterator ci =
		postgres_to_scala_map.begin();
		ci != postgres_to_scala_map.end();
		++ci) {
		cout << ci->first << " : " << ci->second
			<< endl;
	}
	cout << "EXIT print_map_details" << endl;
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
	Graph g (table_relations);
	deque<pair<string, int> >  order = g.topological_sort();
	sort(order.begin(), order.end(), [] (const pair<string, int> & p1, const pair<string, int> & p2) {
		//if (p1.second < p2.second) {
		//		
		//}
		return p1.second > p2.second;
	});
	for (int i = 0; i < order.size(); ++i) {
		cout << order[i].first << ", " << order[i].second << endl;
	}

	generate_data_seeding(order);

	// print_table_details(table_details);
	// Graph g = build_table_relations_graph(table_details);
	// deque<pair<string, int> >  order = g.topological_sort();
	// cout << " TOPOLOGICAL ORDERING for tables is : " << endl;
	// for (int i = 0; i < order.size(); ++i) {
	// 	cout
	// 		<< "tab: " << order[i].first
	// 		<< ", ord: " << i 
	// 		<< endl;
	// }
	// cout << " END  TOPOLOGICAL ORDERING  " << endl;

	// extern map<string, string> postgres_to_scala_map;
	// print_map_details(postgres_to_scala_map);
	// generate_scala_play(table_details);
}

void generate_fromDB(Table * t, stringstream & ss)
{

	ss
		<< "def fromDB(" << endl
		<< "rs: WrappedResultSet" << endl
		<< "): Try["
		<< t->model()
		<< "] = Try {" << endl;
	ss << t->model() << "(" << endl;
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
		<< t->model()
		<< "]]"
		<< endl;
	return ss.str();
}


string gen_get_signature(Table *t)
{
	stringstream ss;
	ss
		<< "def" << " "
		<< " get" << t->model()
		<< "("
		<< endl
		<< t->tenant_and_id_params_scala()
		<< ")"
		<< ": Try[Option["
		<< t->model() << "]]"
		<< endl
		<< endl;
	return ss.str();
}

void generate_get(Table * t,
		stringstream & ss, stringstream & ss_trait)
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

void generate_create(Table * t, stringstream & ss,
		stringstream & ss_trait)
{
	ss
		<< gen_create_signature(t)
		<< " = Try {" << endl
		<< "DB autoCommit { implicit session =>" << endl;

	ss_trait
		<< gen_create_signature(t)
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

string gen_update_signature(Table *t)
{
	stringstream ss;
	ss
		<< "def" << " "
		<< t->fnUpdateDAO()
		<< "("
		<< endl
		<< t->model_as_method_param()
		<< ")"
		<< ": Try[Option["
		<< t->model() << "]]"
		<< endl
		<< endl;
	return ss.str();
}

void generate_update(Table * t,
		stringstream & ss,
		stringstream & ss_trait)
{
	string update_signature = gen_update_signature(t);

	ss
		<< update_signature
		<< " = Try {" << endl
		<< "DB.autoCommit { implicit session =>"
		<< endl;


	ss
		<< "sql\"\"\"" << endl

		<< "\t\tupdate  " << t->table_name << endl
		<< "\t\tSET " << endl
		<< t->update_values_withou_tenant_and_id_set_clause_sql()

		<< "\t\twhere " << endl
		<< "\t\t" << t->tenant_and_id_where_clause_update_sql()
		;

	ss
		<< "\treturning *;" << endl
		<< "\t" << "\"\"\"" << endl
		<< "\t.map(rs => fromDB(rs).get)" << endl
		<< "\t.single()" << endl
		<< "\t.apply()" << endl
		<< endl;

	ss
		<< " // closes autocommit " << endl
		<< "}"	
		<< endl;

	ss
		<< " // closes fn " << endl
		<< "}" << endl
		<< endl;
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


	ss
		<< "sql\"\"\"" << endl
		<< "\t\tdelete  from " << t->table_name << endl
		<< "\t\twhere " << endl
		<< "\t\t" << t->tenant_and_id_where_clause_sql()
		;

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
	ss
		<< "import "
		<< "api" << "." << t->table_name << "."
		<< "models" << ".{"
		<< t->model()
		<< ", "
		<< t->modelForCreate()
		<< "}"
		<< endl;
	ss << endl;
	ss << "import scalikejdbc.{AutoSession, DB, WrappedResultSet, scalikejdbcSQLInterpolationImplicitDef}" << endl;
	ss << "import scala.util.Try" << endl;

	ss << "class "
		<< t->daoClassName()
		<< endl
		<< " extends  "
		<< t->daoTraitName()
		<< " { " << endl;
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
		<< t->model()
		<< ", "
		<< t->modelForCreate()
		<< " }" 
		<< endl;

	ss_trait << "import scala.util.Try" << endl;

	ss_trait << "trait "
		<< t->daoTraitName()
		<< " {"
		<< endl;

	generate_fromDB(t, ss);

	generate_create(t, ss, ss_trait);
	generate_get(t, ss, ss_trait);
	generate_update(t, ss, ss_trait);
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
		<< "\t\t    " << t->createError()
		<< endl
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
		<< t->daoClassName()
		<< " }"
		<< endl;

	ss
		<< "import "
		<< "api" << "." << t->table_name
		<< "." << "models" << ".{"
		<< t->model()
		<< ", "
		<< t->modelForCreate()
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
		<< t->modelForCreate()
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
		<< t->updateError()
		<< endl
		<< endl;

	// == DeleteError
	ss
		<< "sealed trait" << " "
		<< t->deleteError()
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
		<< "class "
		<< t->serviceClassName()
		<< "(" << endl
		<< t->loweredCamelCase()
		<< "DAO"
		<< " : "
		<< t->daoClassName()
		<< endl
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

	ss
		<< "case class " << t->classNameModel() << "(" << endl
		<< t->params_scala()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "object " << t->classNameModel() << "{" << endl
		<< "\timplicit val writes = new Writes["
		<< t->classNameModel() << "] {" << endl
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
		<< "case class "
		<< t->createError()
		<< "(" << endl
		<< t->params_scala_without_primary_key()
		<< ")" << endl
		<< endl
		<< endl;

	ss
		<< "object "
		<< t->createError()
		<< " {" << endl
		<< "implicit val reads = Json.reads["
		<< t->modelForCreate()
		<< "]" << endl
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
		<< "request.body.validate["
		<< t->table_name
		<< "] match {" << endl
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


void generate_controller_get(Table * t, stringstream & ss)
{
	ss << "def" << " "
		<< t->controller_get_def()
		<< "("
		//<< " client_id: Long, org_id: Long"
		<< t->tenant_and_id_params_scala()
		<< "): Action[AnyContent]" << endl;
	ss << "  = Action.async { implicit request =>" << endl;
	ss << "    Logger.info(s\""
		<< t->controller_get_def()
		<< " recd req: ${request.body}\")" << endl;
	ss << "    val wrongRequest = new Exception(ERROR_INVALID_REQUEST)" << endl;
	ss << "" << endl;
	ss << "" << endl;
	ss << "        val "
		<< t->valModel()
		<< " = "
		<< t->serviceValName()
		<< "."
		<< t->service_get_by_id_def()
		<< "(" << endl;
	// params
	ss << "          clientId = clientId," << endl;
	ss << "          org_id = org_id"
	// params end
		<< ")" << endl;

	ss
		<< "        "
		<< t->valModel()
		<<" match {" << endl;
	ss << "          case Left("
		<< t->getError()
		<< ".SQLException(err)) =>"
		<< endl;
	ss << "            Future.failed(err)" << endl;
	ss << "" << endl;
	ss << "          case Right("
		<< t->valModel()
		<< ") => Future.successful(Ok(Json.toJson("
		<< t->valModel()
		<< ") ))" << endl;
	ss << "        }" << endl;
	ss << "" << endl;
	ss << "  }" << endl;
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

	//	============ BEGIN imports ======

	string capitalisedTableName = t->model();

	ss << "import api.CONSTANTS.API_MSGS.{ERROR_INVALID_REQUEST, ERROR_NOT_FOUND_ACCOUNT}" << endl;
	ss
		<< "import api.clients.models."
		<< t->modelForCreate()
		<< endl;
	ss << "import api.clients.services.{"
		<< t->createError()
		<< ", "
		<< t->getError()
		<< ", "
		<< t->serviceClassName()
		<< "}" << endl;
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
	generate_controller_get(t, ss);


	ss
		<< "// closes class" << endl
		<< "}";
	return ss.str();
}

