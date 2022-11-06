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
    WITHOUT = 270,
    TIME = 271,
    ZONE = 272,
    INTEGER = 273,
    BOOLEAN = 274,
    PRIMARY = 275,
    KEY = 276,
    FOREIGN = 277,
    REFERENCES = 278,
    SEARCH_KEY = 279,
    TENANT_ID = 280,
    now = 281,
    DEFAULT = 282,
    NOT = 283,
    NULLL = 284,
    UNIQUE = 285,
    MAP = 286,
    PostgresToScala = 287,
    SCALA_DATATYPE = 288,
    DB_CONV = 289,
    SET = 290,
    WARNING = 291,
    SCHEMA = 292,
    ALTER = 293,
    TO = 294,
    OWNER = 295,
    SEQUENCE = 296,
    START = 297,
    INCREMENT = 298,
    BY = 299,
    NO = 300,
    MINVALUE = 301,
    MAXVALUE = 302,
    CACHE = 303,
    OWNED = 304,
    QUOTE = 305,
    HEAP = 306,
    CONSTRAINT = 307,
    CHECK = 308,
    LSQB = 309,
    RSQB = 310,
    CHARACTER = 311,
    VARYING = 312,
    JSONB = 313,
    CAST_TO_JSONB = 314,
    EMPTY_JSON = 315,
    AS = 316,
    DOUBLE = 317,
    PRECISION = 318,
    ONLY = 319,
    COLUMN = 320,
    NEXTVAL = 321,
    CAST_TO_REG_CLASS = 322,
    ADD = 323,
    BTREE = 324,
    INDEX = 325,
    USING = 326,
    GREATEST = 327,
    LEAST = 328,
    LOWER = 329,
    WHERE = 330,
    IS = 331,
    NE = 332,
    OR = 333,
    AND = 334
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

#line 146 "sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SQL_TAB_H_INCLUDED  */
