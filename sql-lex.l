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

[ \t]+ {
	// discard whitespace
}

\r\n|\n\r|\r|\n {
	// discard newlines - we'll count them later
}

[a-zA-Z][a-zA-Z0-9]* {
	return identifier;
}

.	return yytext[0];

%%

