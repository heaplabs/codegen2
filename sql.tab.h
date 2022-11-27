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
    TEXT_VAL = 259,
    ON = 260,
    OFF = 261,
    CREATE = 262,
    TABLE = 263,
    BIGINT = 264,
    TEXT = 265,
    TIMESTAMP = 266,
    WITH = 267,
    WITHOUT = 268,
    TIME = 269,
    ZONE = 270,
    INTEGER = 271,
    BOOLEAN = 272,
    PRIMARY = 273,
    KEY = 274,
    FOREIGN = 275,
    REFERENCES = 276,
    SEARCH_KEY = 277,
    TENANT_ID = 278,
    now = 279,
    DEFAULT = 280,
    NOT = 281,
    NULLL = 282,
    UNIQUE = 283,
    MAP = 284,
    PostgresToScala = 285,
    SCALA_DATATYPE = 286,
    DB_CONV = 287,
    SET = 288,
    WARNING = 289,
    SCHEMA = 290,
    ALTER = 291,
    TO = 292,
    OWNER = 293,
    SEQUENCE = 294,
    START = 295,
    INCREMENT = 296,
    BY = 297,
    NO = 298,
    MINVALUE = 299,
    MAXVALUE = 300,
    CACHE = 301,
    OWNED = 302,
    QUOTE = 303,
    HEAP = 304,
    CONSTRAINT = 305,
    CHECK = 306,
    LSQB = 307,
    RSQB = 308,
    CHARACTER = 309,
    VARYING = 310,
    JSONB = 311,
    CAST_TO_JSONB = 312,
    EMPTY_JSON = 313,
    AS = 314,
    DOUBLE = 315,
    PRECISION = 316,
    ONLY = 317,
    COLUMN = 318,
    NEXTVAL = 319,
    CAST_TO_REG_CLASS = 320,
    ADD = 321,
    BTREE = 322,
    INDEX = 323,
    USING = 324,
    GREATEST = 325,
    LEAST = 326,
    LOWER = 327,
    WHERE = 328,
    IS = 329,
    GIN = 330,
    DELETE = 331,
    CASCADE = 332,
    NE = 333,
    OR = 334,
    AND = 335,
    JSON_FIELD_EXTRACTOR = 336,
    NUMBER = 337,
    BBOOLEAN = 338
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 22 "sql.y"

	struct Table * table;
	DataType datatype;
	string* identifier;
	int number;
	bool bboolean;
	string * text_val;
	vector<string> * identifier_list;

#line 151 "sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SQL_TAB_H_INCLUDED  */
