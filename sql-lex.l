%{
#include "common_defs.h"
#include "sql.tab.h"
int n_lines = 1;
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

[bB][iI][gG][iI][nN][tT] {
	return BIGINT;
}

[tT][eE][Xx][Tt] {
	return TEXT;
}


[tT][iI][mM][eE][sS][tT][aA][mM][pP] {

	return TIMESTAMP;
}

[wW][iI][tT][hH] {
	return WITH;
}

[tT][iI][mM][eE] {
	return TIME;
}


[zZ][oO][Nn][eE] {
	return ZONE;
}

[iI][nN][tT][eE][gG][eE][rR] {
	return INTEGER;
}

[bB][oO][oO][Ll][eE][aA][nN] {
	return BOOLEAN;
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
	std::cout << "returning DEFAULT: " << std::endl;
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
	++n_lines;
}

true {
	string s(yytext); 
	if (s == "true") {
		yylval.bboolean = true;
	} else {
		yylval.bboolean = false;
	}
	std::cout << "returning bboolean: " << yylval.bboolean << std::endl;
	return BBOOLEAN;
}

false {
	string s(yytext); 
	if (s == "true") {
		yylval.bboolean = true;
	} else {
		yylval.bboolean = false;
	}
	std::cout << "returning bboolean: " << yylval.bboolean << std::endl;
	return BBOOLEAN;
}

	/*
	false {
		yylval.boolean = false;
		return boolean;
	}
	*/

[0-9]* {
	string num_str (yytext);
	yylval.number = atoi(num_str.c_str());
	return number;
}

--.*\n {
	// ignore comment, but track the line number
	++n_lines;
}

"'"[a-zA-Z_][_a-zA-Z0-9]*"'"::text {
	string * id = new string(yytext);
	yylval.text_val = id;
	return TEXT_VAL;
}

[a-zA-Z_][_a-zA-Z0-9]* {
	string * id = new string(yytext);
	yylval.identifier = id;
	return identifier;
}

.	return yytext[0];

%%

