%{
#include "common_defs.h"
#include "sql.tab.h"
%}

%option noyywrap 8bit
%array

%%

[cC][rR][eE][aA][tT][eE] {
	return CREATE;
}

[tT][aA][bB][lL][eE] {
	return TABLE;
}

PRIMARY {
	return PRIMARY;
}

TENANT_ID {
	return TENANT_ID;
}

SEARCH_KEY {
	return SEARCH_KEY;
}


KEY {
	return KEY;
}

FOREIGN {
	return FOREIGN;
}

REFERENCES {
	return REFERENCES;
}

NOT {
	return NOT;
}

NULL {
	return NULLL;
}

DEFAULT {
	return DEFAULT;
}

UNIQUE {
	return UNIQUE;
}

now {
	return now;
}

MAP {
	return MAP;
}

DB_CONV {
	return DB_CONV;
}

SCALA_DATATYPE {
	return SCALA_DATATYPE;
}

PostgresToScala {
	return PostgresToScala;
}


[ \t]+ {
	// discard whitespace
}

\r\n|\n\r|\r|\n {
	// discard newlines - we'll count them later
}

[a-zA-Z_][_a-zA-Z0-9]* {
	string * id = new string(yytext);
	yylval.identifier = id;
	return identifier;
}

.	return yytext[0];

%%

