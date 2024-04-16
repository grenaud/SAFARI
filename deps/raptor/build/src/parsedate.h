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

#ifndef YY_RAPTOR_PARSEDATE_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_PARSEDATE_H_INCLUDED
# define YY_RAPTOR_PARSEDATE_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_PARSEDATE_H_INCLUDED
/* Debug traces.  */
#ifndef RAPTOR_PARSEDATE_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define RAPTOR_PARSEDATE_DEBUG 1
#  else
#   define RAPTOR_PARSEDATE_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define RAPTOR_PARSEDATE_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined RAPTOR_PARSEDATE_DEBUG */
#if RAPTOR_PARSEDATE_DEBUG
extern int raptor_parsedate_debug;
#endif

/* Token type.  */
#ifndef RAPTOR_PARSEDATE_TOKENTYPE
# define RAPTOR_PARSEDATE_TOKENTYPE
  enum raptor_parsedate_tokentype
  {
    tAGO = 258,
    tDAY = 259,
    tDAY_UNIT = 260,
    tDAYZONE = 261,
    tDST = 262,
    tHOUR_UNIT = 263,
    tID = 264,
    tTZONE = 265,
    tWZONE = 266,
    tZZONE = 267,
    tMERIDIAN = 268,
    tMINUTE_UNIT = 269,
    tMONTH = 270,
    tMONTH_UNIT = 271,
    tSEC_UNIT = 272,
    tSNUMBER = 273,
    tUNUMBER = 274,
    tYEAR_UNIT = 275,
    tZONE = 276
  };
#endif

/* Value type.  */



int raptor_parsedate_parse (struct date_yy *parm);

#endif /* !YY_RAPTOR_PARSEDATE_HOME_PROJECTS2_VGAN_ANCIENT_THIRD_VERSION_SAFARI_DEPS_RAPTOR_BUILD_SRC_PARSEDATE_H_INCLUDED  */
