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
    ON = 262,
    OFF = 263,
    CREATE = 264,
    TABLE = 265,
    BIGINT = 266,
    TEXT = 267,
    TIMESTAMP = 268,
    WITH = 269,
    TIME = 270,
    ZONE = 271,
    INTEGER = 272,
    BOOLEAN = 273,
    PRIMARY = 274,
    KEY = 275,
    FOREIGN = 276,
    REFERENCES = 277,
    SEARCH_KEY = 278,
    TENANT_ID = 279,
    now = 280,
    DEFAULT = 281,
    NOT = 282,
    NULLL = 283,
    UNIQUE = 284,
    MAP = 285,
    PostgresToScala = 286,
    SCALA_DATATYPE = 287,
    DB_CONV = 288,
    SET = 289,
    WARNING = 290,
    SCHEMA = 291,
    ALTER = 292,
    TO = 293,
    OWNER = 294,
    SEQUENCE = 295,
    START = 296,
    INCREMENT = 297,
    BY = 298,
    NO = 299,
    MINVALUE = 300,
    MAXVALUE = 301,
    CACHE = 302,
    OWNED = 303,
    QUOTE = 304
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

#line 116 "sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SQL_TAB_H_INCLUDED  */
