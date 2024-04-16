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

#ifndef YY_TURTLE_PARSER_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_TURTLE_PARSER_H_INCLUDED
# define YY_TURTLE_PARSER_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_TURTLE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef TURTLE_PARSER_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define TURTLE_PARSER_DEBUG 1
#  else
#   define TURTLE_PARSER_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define TURTLE_PARSER_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined TURTLE_PARSER_DEBUG */
#if TURTLE_PARSER_DEBUG
extern int turtle_parser_debug;
#endif

/* Token type.  */
#ifndef TURTLE_PARSER_TOKENTYPE
# define TURTLE_PARSER_TOKENTYPE
  enum turtle_parser_tokentype
  {
    A = 258,
    HAT = 259,
    DOT = 260,
    COMMA = 261,
    SEMICOLON = 262,
    LEFT_SQUARE = 263,
    RIGHT_SQUARE = 264,
    LEFT_ROUND = 265,
    RIGHT_ROUND = 266,
    LEFT_CURLY = 267,
    RIGHT_CURLY = 268,
    TRUE_TOKEN = 269,
    FALSE_TOKEN = 270,
    PREFIX = 271,
    BASE = 272,
    SPARQL_PREFIX = 273,
    SPARQL_BASE = 274,
    STRING_LITERAL = 275,
    IDENTIFIER = 276,
    LANGTAG = 277,
    INTEGER_LITERAL = 278,
    FLOATING_LITERAL = 279,
    DECIMAL_LITERAL = 280,
    BLANK_LITERAL = 281,
    URI_LITERAL = 282,
    GRAPH_NAME_LEFT_CURLY = 283,
    QNAME_LITERAL = 284,
    ERROR_TOKEN = 285
  };
#endif

/* Value type.  */
#if ! defined TURTLE_PARSER_STYPE && ! defined TURTLE_PARSER_STYPE_IS_DECLARED
union TURTLE_PARSER_STYPE
{
#line 143 "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/src/turtle_parser.y"

  unsigned char *string;
  raptor_term *identifier;
  raptor_sequence *sequence;
  raptor_uri *uri;

#line 103 "/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/src/turtle_parser.h"

};
typedef union TURTLE_PARSER_STYPE TURTLE_PARSER_STYPE;
# define TURTLE_PARSER_STYPE_IS_TRIVIAL 1
# define TURTLE_PARSER_STYPE_IS_DECLARED 1
#endif



int turtle_parser_parse (raptor_parser* rdf_parser, void* yyscanner);

#endif /* !YY_TURTLE_PARSER_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_TURTLE_PARSER_H_INCLUDED  */
