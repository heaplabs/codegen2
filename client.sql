CREATE TABLE clients
(
	id bigserial: PRIMARY KEY,
	org_id bigint: TENANT_ID, 
	name text:  NOT NULL; UNIQUE,

	created_at TIMESTAMPTZ: DEFAULT now(),
	updated_at TIMESTAMPTZ: DEFAULT now(),
	website_url text,
	address text,
	email text
);

MAP PostgresToScala (
	double_precision DB_CONV double SCALA_DATATYPE Double,
	bigint DB_CONV long SCALA_DATATYPE Long ,
	bigserial DB_CONV long SCALA_DATATYPE Long ,
	integer DB_CONV int SCALA_DATATYPE Int,
	serial DB_CONV int SCALA_DATATYPE Int,
	real DB_CONV float SCALA_DATATYPE Float ,
	boolean DB_CONV boolean SCALA_DATATYPE Boolean,
	text DB_CONV string SCALA_DATATYPE String,
	TIMESTAMPTZ DB_CONV jodaDateTimeOpt SCALA_DATATYPE DateTime 
);

