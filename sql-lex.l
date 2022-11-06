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

SCHEMA {
	return SCHEMA;
}

ALTER {
	return ALTER;
}


OWNER {
	return OWNER;
}

OWNED {
	return OWNED;
}

TO {
	return TO;
}

SEQUENCE {
	return SEQUENCE;
}

START {
	return START;
}

INCREMENT {
	return INCREMENT;
}

BY {
	return BY;
}

NO {
	return NO;
}

MINVALUE {
	return MINVALUE;
}


MAXVALUE {
	return MAXVALUE;
}

CACHE {
	return CACHE;
}


[bB][iI][gG][iI][nN][tT] {
	return BIGINT;
}

[tT][eE][Xx][Tt] {
	return TEXT;
}


jsonb {
	return JSONB;
}

[tT][iI][mM][eE][sS][tT][aA][mM][pP] {

	return TIMESTAMP;
}

[wW][iI][tT][hH] {
	return WITH;
}

AS {
	return AS;
}


without {
	return WITHOUT;
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

double {
	return DOUBLE;
}

precision {
	return PRECISION;
}

ONLY {
	std::cout << "return ONLY " << std::endl;
	return ONLY;
}

COLUMN {
	return COLUMN;
}

nextval {
	return NEXTVAL;
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

on {
	yylval.bboolean = true;
	return BBOOLEAN;
}

off {
	yylval.bboolean = false;
	return BBOOLEAN;
}

content {
	string * id = new string(yytext);
	yylval.text_val = id;
	return TEXT_VAL;
}

heap {
	return HEAP;
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

CONSTRAINT {
	return CONSTRAINT;
}

CHECK {
	return CHECK;
}

character {
	return CHARACTER;
}

USING {
	return USING;
}

INDEX {
	return INDEX;
}

btree {
	return BTREE;
}

ADD {
	return ADD;
}

lower {
	return LOWER;
}

GREATEST {
	return GREATEST;
}

LEAST {
	return LEAST;
}

varying {
	return VARYING;
}


WHERE {
	return WHERE;
}

ON {
	return ON;
}

IS {
	return IS;
}


AND {
	return AND;
}

OR {
	return OR;
}

column {
	return COLUMN;
}


warning {
	return WARNING;
}

"<>" {
	return NE;
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

SET {
	return SET;
}

"[" {
	return LSQB;
}

"]" {
	return RSQB;
}


--.*\n {
	// ignore comment, but track the line number
	++n_lines;
}

"'{}'" {
	return EMPTY_JSON;
}

"'" {
	return QUOTE ;
}

::jsonb {
	return CAST_TO_JSONB;
}


"'"[a-zA-Z_][_a-zA-Z0-9]*"\."[a-zA-Z_][_a-zA-Z0-9]*"'"::regclass {
	string * id = new string(yytext);
	yylval.text_val = id;
	return CAST_TO_REG_CLASS;
}

"'"[a-zA-Z_][_a-zA-Z0-9]*"'"::text {
	string * id = new string(yytext);
	yylval.text_val = id;
	return TEXT_VAL;
}

"''"::text {
	string * id = new string(yytext);
	yylval.text_val = id;
	return TEXT_VAL;
}

"'"[a-zA-Z_][_a-zA-Z0-9]*"'" {
	string * id = new string(yytext);
	yylval.text_val = id;
	return TEXT_VAL;
}

[a-zA-Z_][_a-zA-Z0-9]* {
	string * id = new string(yytext);
	yylval.identifier = id;
	std::cout << "return identifier: " << *id << std::endl;
	return identifier;
}

.	return yytext[0];

%%

