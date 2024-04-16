/* raptor_config.h */

#define RAPTOR_VERSION_DECIMAL		20016
#define RAPTOR_MIN_VERSION_DECIMAL	20016

#define HAVE_ERRNO_H
#define HAVE_FCNTL_H
#define HAVE_GETOPT_H
#define HAVE_LIMITS_H
#define HAVE_MATH_H
#define HAVE_SETJMP_H
#define HAVE_STDDEF_H
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_UNISTD_H
#define HAVE_SYS_PARAM_H
#define HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H
#define HAVE_SYS_TIME_H

#define TIME_WITH_SYS_TIME

#define HAVE_ACCESS
/* #undef HAVE__ACCESS */
#define HAVE_GETOPT
#define HAVE_GETOPT_LONG
#define HAVE_GETTIMEOFDAY
#define HAVE_ISASCII
#define HAVE_SETJMP
#define HAVE_SNPRINTF
/* #undef HAVE__SNPRINTF */
#define HAVE_STAT
#define HAVE_STRCASECMP
/* #undef HAVE_STRICMP */
/* #undef HAVE__STRICMP */
#define HAVE_STRTOK_R
#define HAVE_VASPRINTF
#define HAVE_VSNPRINTF
/* #undef HAVE__VSNPRINTF */

#define HAVE___FUNCTION__

#define SIZEOF_UNSIGNED_CHAR		1
#define SIZEOF_UNSIGNED_SHORT		2
#define SIZEOF_UNSIGNED_INT		4
#define SIZEOF_UNSIGNED_LONG		8
#define SIZEOF_UNSIGNED_LONG_LONG	8

#define HAVE_XMLCTXTUSEOPTIONS
#define HAVE_XMLSAX2INTERNALSUBSET
#define RAPTOR_LIBXML_ENTITY_ETYPE
/* #undef RAPTOR_LIBXML_ENTITY_NAME_LENGTH */
#define RAPTOR_LIBXML_HTML_PARSE_NONET
#define RAPTOR_LIBXML_XMLSAXHANDLER_EXTERNALSUBSET
#define RAPTOR_LIBXML_XMLSAXHANDLER_INITIALIZED
#define RAPTOR_LIBXML_XML_PARSE_NONET

#define RAPTOR_STATIC
#define HAVE_RAPTOR_PARSE_DATE
#define RAPTOR_WWW_NONE
#define RAPTOR_XML_NONE
/* #undef RAPTOR_XML_1_1 */

/* #undef RAPTOR_PARSER_RDFXML */
#define RAPTOR_PARSER_NTRIPLES
#define RAPTOR_PARSER_TURTLE
#define RAPTOR_PARSER_TRIG
#define RAPTOR_PARSER_RSS
/* #undef RAPTOR_PARSER_GRDDL */
#define RAPTOR_PARSER_GUESS
#define RAPTOR_PARSER_RDFA
/* #undef RAPTOR_PARSER_JSON */
#define RAPTOR_PARSER_NQUADS

#define RAPTOR_SERIALIZER_RDFXML
#define RAPTOR_SERIALIZER_NTRIPLES
#define RAPTOR_SERIALIZER_RDFXML_ABBREV
#define RAPTOR_SERIALIZER_TURTLE
/* #undef RAPTOR_SERIALIZER_MKR */
#define RAPTOR_SERIALIZER_RSS_1_0
#define RAPTOR_SERIALIZER_ATOM
#define RAPTOR_SERIALIZER_DOT
#define RAPTOR_SERIALIZER_HTML
#define RAPTOR_SERIALIZER_JSON
#define RAPTOR_SERIALIZER_NQUADS

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  define _CRT_NONSTDC_NO_DEPRECATE
#  define _CRT_SECURE_NO_DEPRECATE

#  ifdef _MSC_VER
#    if _MSC_VER >= 1300
#      define __func__		__FUNCTION__
#    else
       /* better than nothing */
#      define raptor_str(s)	#s
#      define __func__		"func@" __FILE__ ":" raptor_str(__LINE__)
#    endif
#  endif

#  define RAPTOR_INLINE	__inline

#  define S_ISTYPE(mode, mask)	(((mode) & _S_IFMT) == (mask))
#  define S_ISDIR(mode)		S_ISTYPE((mode), _S_IFDIR)
#  define S_ISREG(mode)		S_ISTYPE((mode), _S_IFREG)

   /* Mode bits for access() */
#  define R_OK 04
#  define W_OK 02

#  if !defined(HAVE_ACCESS) && defined(HAVE__ACCESS)
#    define access(p,m)		_access(p,m)
#  endif
#  ifndef HAVE_STRCASECMP
#    if defined(HAVE__STRICMP)
#      define strcasecmp(a,b)	_stricmp(a,b)
#    elif defined(HAVE_STRICMP)
#      define strcasecmp(a,b)	stricmp(a,b)
#    endif
#  endif
#  if !defined(HAVE_SNPRINTF) && defined(HAVE__SNPRINTF)
#    define snprintf		_snprintf
#  endif
#  if !defined(HAVE_VSNPRINTF) && defined(HAVE__VSNPRINTF)
#    define vsnprintf		_vsnprintf
#  endif

   /* These prevent parsedate.c from declaring malloc() and free() */
#  define YYMALLOC malloc
#  define YYFREE   free
#endif

/* end raptor_config.h */
