/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SQL_TAB_H_INCLUDED
# define YY_YY_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    identifier = 258,
    number = 259,
    BBOOLEAN = 260,
    TEXT_VAL = 261,
    CREATE = 262,
    TABLE = 263,
    BIGINT = 264,
    TEXT = 265,
    TIMESTAMP = 266,
    WITH = 267,
    TIME = 268,
    ZONE = 269,
    INTEGER = 270,
    BOOLEAN = 271,
    PRIMARY = 272,
    KEY = 273,
    FOREIGN = 274,
    REFERENCES = 275,
    SEARCH_KEY = 276,
    TENANT_ID = 277,
    now = 278,
    DEFAULT = 279,
    NOT = 280,
    NULLL = 281,
    UNIQUE = 282,
    MAP = 283,
    PostgresToScala = 284,
    SCALA_DATATYPE = 285,
    DB_CONV = 286,
    SET = 287,
    WARNING = 288,
    SCHEMA = 289,
    ALTER = 290,
    TO = 291,
    OWNER = 292
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 18 "sql.y"

	struct Table * table;
	DataType datatype;
	string* identifier;
	int number;
	bool bboolean;
	string * text_val;

#line 104 "sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SQL_TAB_H_INCLUDED  */
