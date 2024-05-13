/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         base_yyparse
#define yylex           base_yylex
#define yyerror         base_yyerror
#define yydebug         base_yydebug
#define yynerrs         base_yynerrs

/* First part of user prologue.  */
#line 1 "gram.y"


/*#define YYDEBUG 1*/
/*-------------------------------------------------------------------------
 *
 * gram.y
 *    POSTGRESQL BISON rules/actions
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    src/backend/parser/gram.y
 *
 * HISTORY
 *    AUTHOR            DATE            MAJOR EVENT
 *    Andrew Yu         Sept, 1994      POSTQUEL to SQL conversion
 *    Andrew Yu         Oct, 1994       lispy code conversion
 *
 * NOTES
 *    CAPITALS are used to represent terminal symbols.
 *    non-capitals are used to represent non-terminals.
 *
 *    In general, nothing in this file should initiate database accesses
 *    nor depend on changeable state (such as SET variables).  If you do
 *    database accesses, your code will fail when we have aborted the
 *    current transaction and are just parsing commands to find the next
 *    ROLLBACK or COMMIT.  If you make use of SET variables, then you
 *    will do the wrong thing in multi-query strings like this:
 *          SET constraint_exclusion TO off; SELECT * FROM foo;
 *    because the entire string is parsed by gram.y before the SET gets
 *    executed.  Anything that depends on the database or changeable state
 *    should be handled during parse analysis so that it happens at the
 *    right time not the wrong time.
 *
 * WARNINGS
 *    If you use a list, make sure the datum is a node so that the printing
 *    routines work.
 *
 *    Sometimes we assign constants to makeStrings. Make sure we don't free
 *    those.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <ctype.h>
#include <limits.h>

#include "access/tableam.h"
#include "catalog/index.h"
#include "catalog/namespace.h"
#include "catalog/pg_am.h"
#include "catalog/pg_trigger.h"
#include "commands/defrem.h"
#include "commands/trigger.h"
#include "gramparse.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "parser/parser.h"
#include "storage/lmgr.h"
#include "utils/date.h"
#include "utils/datetime.h"
#include "utils/numeric.h"
#include "utils/xml.h"


/*
 * Location tracking support --- simpler than bison's default, since we only
 * want to track the start position not the end position of each nonterminal.
 */
#define YYLLOC_DEFAULT(Current, Rhs, N) \
    do { \
        if ((N) > 0) \
            (Current) = (Rhs)[1]; \
        else \
            (Current) = (-1); \
    } while (0)

/*
 * The above macro assigns -1 (unknown) as the parse location of any
 * nonterminal that was reduced from an empty rule, or whose leftmost
 * component was reduced from an empty rule.  This is problematic
 * for nonterminals defined like
 *      OptFooList: / * EMPTY * / { ... } | OptFooList Foo { ... } ;
 * because we'll set -1 as the location during the first reduction and then
 * copy it during each subsequent reduction, leaving us with -1 for the
 * location even when the list is not empty.  To fix that, do this in the
 * action for the nonempty rule(s):
 *      if (@$ < 0) @$ = @2;
 * (Although we have many nonterminals that follow this pattern, we only
 * bother with fixing @$ like this when the nonterminal's parse location
 * is actually referenced in some rule.)
 *
 * A cleaner answer would be to make YYLLOC_DEFAULT scan all the Rhs
 * locations until it's found one that's not -1.  Then we'd get a correct
 * location for any nonterminal that isn't entirely empty.  But this way
 * would add overhead to every rule reduction, and so far there's not been
 * a compelling reason to pay that overhead.
 */

/*
 * Bison doesn't allocate anything that needs to live across parser calls,
 * so we can easily have it use palloc instead of malloc.  This prevents
 * memory leaks if we error out during parsing.
 */
#define YYMALLOC palloc
#define YYFREE   pfree

/* Private struct for the result of privilege_target production */
typedef struct PrivTarget
{
    GrantTargetType targtype;
    ObjectType  objtype;
    List       *objs;
} PrivTarget;

/* Private struct for the result of import_qualification production */
typedef struct ImportQual
{
    ImportForeignSchemaType type;
    List       *table_names;
} ImportQual;

/* Private struct for the result of opt_select_limit production */
typedef struct SelectLimit
{
    Node       *limitOffset;
    Node       *limitCount;
    LimitOption limitOption;
} SelectLimit;

/* Private struct for the result of group_clause production */
typedef struct GroupClause
{
    bool        distinct;
    List       *list;
} GroupClause;

/* Private structs for the result of key_actions and key_action productions */
typedef struct KeyAction
{
    char        action;
    List       *cols;
} KeyAction;

typedef struct KeyActions
{
    KeyAction *updateAction;
    KeyAction *deleteAction;
} KeyActions;

/* ConstraintAttributeSpec yields an integer bitmask of these flags: */
#define CAS_NOT_DEFERRABLE          0x01
#define CAS_DEFERRABLE              0x02
#define CAS_INITIALLY_IMMEDIATE     0x04
#define CAS_INITIALLY_DEFERRED      0x08
#define CAS_NOT_VALID               0x10
#define CAS_NO_INHERIT              0x20


#define parser_yyerror(msg)  scanner_yyerror(msg, yyscanner)
#define parser_errposition(pos)  scanner_errposition(pos, yyscanner)

static void base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner,
                         const char *msg);
static RawStmt *makeRawStmt(Node *stmt, int stmt_location);
static void updateRawStmtEnd(RawStmt *rs, int end_location);
static Node *makeColumnRef(char *colname, List *indirection,
                           int location, core_yyscan_t yyscanner);
static Node *makeTypeCast(Node *arg, TypeName *typename, int location);
static Node *makeStringConstCast(char *str, int location, TypeName *typename);
static Node *makeIntConst(int val, int location);
static Node *makeFloatConst(char *str, int location);
static Node *makeBoolAConst(bool state, int location);
static Node *makeBitStringConst(char *str, int location);
static Node *makeNullAConst(int location);
static Node *makeAConst(Node *v, int location);
static RoleSpec *makeRoleSpec(RoleSpecType type, int location);
static void check_qualified_name(List *names, core_yyscan_t yyscanner);
static List *check_func_name(List *names, core_yyscan_t yyscanner);
static List *check_indirection(List *indirection, core_yyscan_t yyscanner);
static List *extractArgTypes(List *parameters);
static List *extractAggrArgTypes(List *aggrargs);
static List *makeOrderedSetArgs(List *directargs, List *orderedargs,
                                core_yyscan_t yyscanner);
static void insertSelectOptions(SelectStmt *stmt,
                                List *sortClause, List *lockingClause,
                                SelectLimit *limitClause,
                                WithClause *withClause,
                                core_yyscan_t yyscanner);
static Node *makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg);
static Node *doNegate(Node *n, int location);
static void doNegateFloat(Float *v);
static Node *makeAndExpr(Node *lexpr, Node *rexpr, int location);
static Node *makeOrExpr(Node *lexpr, Node *rexpr, int location);
static Node *makeNotExpr(Node *expr, int location);
static Node *makeAArrayExpr(List *elements, int location);
static Node *makeSQLValueFunction(SQLValueFunctionOp op, int32 typmod,
                                  int location);
static Node *makeXmlExpr(XmlExprOp op, char *name, List *named_args,
                         List *args, int location);
static List *mergeTableFuncParameters(List *func_args, List *columns);
static TypeName *TableFuncTypeName(List *columns);
static RangeVar *makeRangeVarFromAnyName(List *names, int position, core_yyscan_t yyscanner);
static RangeVar *makeRangeVarFromQualifiedName(char *name, List *namelist, int location,
                                               core_yyscan_t yyscanner);
static void SplitColQualList(List *qualList,
                             List **constraintList, CollateClause **collClause,
                             core_yyscan_t yyscanner);
static void processCASbits(int cas_bits, int location, const char *constrType,
               bool *deferrable, bool *initdeferred, bool *not_valid,
               bool *no_inherit, core_yyscan_t yyscanner);
static PartitionStrategy parsePartitionStrategy(char *strategy);
static void preprocess_pubobj_list(List *pubobjspec_list,
                                   core_yyscan_t yyscanner);
static Node *makeRecursiveViewSelect(char *relname, List *aliases, Node *query);


#line 297 "gram.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "gram.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDEcNT = 3,                      /* IDENT  */
  YYSYMBOL_UIDENT = 4,                     /* UIDENT  */
  YYSYMBOL_FCONST = 5,                     /* FCONST  */
  YYSYMBOL_SCONST = 6,                     /* SCONST  */
  YYSYMBOL_USCONST = 7,                    /* USCONST  */
  YYSYMBOL_BCONST = 8,                     /* BCONST  */
  YYSYMBOL_XCONST = 9,                     /* XCONST  */
  YYSYMBOL_Op = 10,                        /* Op  */
  YYSYMBOL_ICONST = 11,                    /* ICONST  */
  YYSYMBOL_PARAM = 12,                     /* PARAM  */
  YYSYMBOL_TYPECAST = 13,                  /* TYPECAST  */
  YYSYMBOL_DOT_DOT = 14,                   /* DOT_DOT  */
  YYSYMBOL_COLON_EQUALS = 15,              /* COLON_EQUALS  */
  YYSYMBOL_EQUALS_GREATER = 16,            /* EQUALS_GREATER  */
  YYSYMBOL_LESS_EQUALS = 17,               /* LESS_EQUALS  */
  YYSYMBOL_GREATER_EQUALS = 18,            /* GREATER_EQUALS  */
  YYSYMBOL_NOT_EQUALS = 19,                /* NOT_EQUALS  */
  YYSYMBOL_ABORT_P = 20,                   /* ABORT_P  */
  YYSYMBOL_ABSENT = 21,                    /* ABSENT  */
  YYSYMBOL_ABSOLUTE_P = 22,                /* ABSOLUTE_P  */
  YYSYMBOL_ACCESS = 23,                    /* ACCESS  */
  YYSYMBOL_ACTION = 24,                    /* ACTION  */
  YYSYMBOL_ADD_P = 25,                     /* ADD_P  */
  
  YYSYMBOL_qualified_name = 1248,          /* qualified_name  */
  YYSYMBOL_name_list = 1249,               /* name_list  */
  YYSYMBOL_name = 1250,                    /* name  */
  YYSYMBOL_attr_name = 1251,               /* attr_name  */
  YYSYMBOL_file_name = 1252,               /* file_name  */
  YYSYMBOL_func_name = 1253,               /* func_name  */
  YYSYMBOL_AexprConst = 1254,              /* AexprConst  */
  YYSYMBOL_Iconst = 1255,                  /* Iconst  */
  YYSYMBOL_Sconst = 1256,                  /* Sconst  */
  YYSYMBOL_SignedIconst = 1257,            /* SignedIconst  */
  YYSYMBOL_RoleId = 1258,                  /* RoleId  */
  YYSYMBOL_RoleSpec = 1259,                /* RoleSpec  */
  YYSYMBOL_role_list = 1260,               /* role_list  */
  YYSYMBOL_PLpgSQL_Expr = 1261,            /* PLpgSQL_Expr  */
  YYSYMBOL_PLAssignStmt = 1262,            /* PLAssignStmt  */
  YYSYMBOL_plassign_target = 1263,         /* plassign_target  */
  YYSYMBOL_plassign_equals = 1264,         /* plassign_equals  */
  YYSYMBOL_ColId = 1265,                   /* ColId  */
  YYSYMBOL_type_function_name = 1266,      /* type_function_name  */
  YYSYMBOL_NonReservedWord = 1267,         /* NonReservedWord  */
  YYSYMBOL_ColLabel = 1268,                /* ColLabel  */
  YYSYMBOL_BareColLabel = 1269,            /* BareColLabel  */
  YYSYMBOL_unreserved_keyword = 1270,      /* unreserved_keyword  */
  YYSYMBOL_col_name_keyword = 1271,        /* col_name_keyword  */
  YYSYMBOL_type_func_name_keyword = 1272,  /* type_func_name_keyword  */
  YYSYMBOL_reserved_keyword = 1273,        /* reserved_keyword  */
  YYSYMBOL_bare_label_keyword = 1274       /* bare_label_keyword  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  931
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   124553

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  542
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  733
/* YYNRULES -- Number of rules.  */
#define YYNRULES  3424
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  6499

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   779


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   531,     2,     2,
     536,   537,   529,   527,   540,   528,   538,   530,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   541,   539,
     524,   526,   525,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   534,     2,   535,   532,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   533
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   925,   925,   930,   934,   939,   947,   955,   975,   987,
    1001,  1002,  1006,  1007,  1008,  1009,  1010,  1011,  1012,  1013,
    1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,
    1024,  1025,  1026,  1027,  1028,  1029,  1030,  1031,  1032,  1033,
   
   18564, 18565, 18566, 18567, 18568, 18569, 18570, 18571, 18572, 18573,
   18574, 18575, 18576, 18577, 18578, 18579, 18580, 18581, 18582, 18583,
   18584, 18585, 18586, 18587, 18588, 18589, 18590, 18591, 18592, 18593,
   18594, 18595, 18596, 18597, 18598, 18599, 18600, 18601, 18602, 18603,
   18604, 18605, 18606, 18607, 18608
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENT", "UIDENT",
  "FCONST", "SCONST", "USCONST", "BCONST", "XCONST", "Op", "ICONST",
  "PARAM", "TYPECAST", "DOT_DOT", "COLON_EQUALS", "EQUALS_GREATER",
  "LESS_EQUALS", "GREATER_EQUALS", "NOT_EQUALS", "ABORT_P", "ABSENT",
  "ABSOLUTE_P", "ACCESS", "ACTION", "ADD_P", "ADMIN", "AFTER", "AGGREGATE",
  "ALL", "ALSO", "ALTER", "ALWAYS", "ANALYSE", "ANALYZE", "AND", "ANY",
  "roup_by_item",
  "empty_grouping_set", "rollup_clause", "cube_clause",
  "grouping_sets_clause", "having_clause", "for_locking_clause",
  "opt_for_locking_clause", "for_locking_items", "for_locking_item",
  "for_locking_strength", "locked_rels_list", "values_clause",
  "from_clause", "from_list", "table_ref", "joined_table", "alias_clause",
  "opt_alias_clause", "opt_alias_clause_for_join_using",
  "func_alias_clause", "join_type", "opt_outer", "join_qual",
  "relation_expr", "extended_relation_expr", "relation_expr_list",
  "relation_expr_opt_alias", "tablesample_clause", "opt_repeatable_clause",
  "func_table", "rowsfrom_item", "rowsfrom_list", "opt_col_def_list",
  "opt_ordinality", "where_clause", "where_or_current_clause",
  "OptTableFuncElementList", "TableFuncElementList", "TableFuncElement",
  "xmltable", "xmltable_column_list", "xmltable_column_el",
  "xmltable_column_option_list", "xmltable_column_option_el",
  "xml_namespace_list", "xml_namespace_el", "json_table",
  "json_table_path_name_opt", "json_table_column_definition_list",
  "json_table_column_definition", "path_opt",
  "json_table_column_path_clause_opt", "Typename", "opt_array_bounds",
  "SimpleTypename", "ConstTypename", "GenericType", "opt_type_modifiers",
  "Numeric", "opt_float", "Bit", "ConstBit", "BitWithLength",
  "BitWithoutLength", "Character", "ConstCharacter", "CharacterWithLength",
  "CharacterWithoutLength", "character", "opt_varying", "ConstDatetime",
  "ConstInterval", "opt_timezone", "opt_interval", "interval_second",
  "JsonType", "a_expr", "b_expr", "c_expr", "func_application",
  "func_expr", "func_expr_windowless", "func_expr_common_subexpr",
  "xml_root_version", "opt_xml_root_standalone", "xml_attributes",
  "xml_attribute_list", "xml_attribute_el", "document_or_content",
  "xml_indent_option", "xml_whitespace_option", "xmlexists_argument",
  "xml_passing_mech", "within_group_clause", "filter_clause",
  "window_clause", "window_definition_list", "window_definition",
  "over_clause", "window_specification", "opt_existing_window_name",
  "opt_partition_clause", "opt_frame_clause", "frame_extent",
  "frame_bound", "opt_window_exclusion_clause", "row", "explicit_row",
  "implicit_row", "sub_type", "all_Op", "MathOp", "qual_Op", "qual_all_Op",
  "subquery_Op", "expr_list", "func_arg_list", "func_arg_expr",
  "func_arg_list_opt", "type_list", "array_expr", "array_expr_list",
  "extract_list", "extract_arg", "unicode_normal_form", "overlay_list",
  "position_list", "substr_list", "trim_list", "in_expr", "case_expr",
  "when_clause_list", "when_clause", "case_default", "case_arg",
  "columnref", "indirection_el", "opt_slice_bound", "indirection",
  "opt_indirection", "opt_asymmetric", "json_passing_clause_opt",
  "json_arguments", "json_argument", "json_wrapper_behavior",
  "json_behavior", "json_behavior_type", "json_behavior_clause_opt",
  "json_on_error_clause_opt", "json_value_expr", "json_format_clause",
  "json_format_clause_opt", "json_quotes_clause_opt",
  "json_returning_clause_opt", "json_predicate_type_constraint",
  "json_key_uniqueness_constraint_opt", "json_name_and_value_list",
  "json_name_and_value", "json_object_constructor_null_clause_opt",
  "json_array_constructor_null_clause_opt", "json_value_expr_list",
  "json_aggregate_func", "json_array_aggregate_order_by_clause_opt",
  "opt_target_list", "target_list", "target_el", "qualified_name_list",
  "qualified_name", "name_list", "name", "attr_name", "file_name",
  "func_name", "AexprConst", "Iconst", "Sconst", "SignedIconst", "RoleId",
  "RoleSpec", "role_list", "PLpgSQL_Expr", "PLAssignStmt",
  "plassign_target", "plassign_equals", "ColId", "type_function_name",
  "NonReservedWord", "ColLabel", "BareColLabel", "unreserved_keyword",
  "col_name_keyword", "type_func_name_keyword", "reserved_keyword",
  "bare_label_keyword", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-5959)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3001)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const int yypact[] =
{
    8961,  1858, 19990, -5959, -5959,  1858, 73605, -5959, 83314,  1457,
     224,  1407,   226, 19802, 83825, 94556,   672,  1269, 20652,  1858,
   94556,  3182, 63895, 79226,  1121, 94556,  1280,  1068, 63895, 94556,
   95067,  1194,  1453,  1010, 95578, 84336, 78204,  1470, 94556,  1571,
     423, 96089, 84847,  1419, 85358,  1068, 58796,   460,  1436, 96600,
   94556,121555,   423, 85869, 85869, 85869,  2367,  1965,  1722, -5959,
   -5959, -5959, -5959, -5959, -5959, -5959, -5959, -5959, -5959, -5959,
  
    3406,  4896, -5959,  1982,  4638,  4641, 64918, -5959, -5959, -5959,
   -5959, -5959,  5050,   361,  4938, -5959, -5959,  1280,  1451,117551,
    4747,   857,  2699, -5959,  4581,  2701,   118, -5959,  1806, -5959,
    4280, -5959, -5959,  3567, -5959, -5959,   255, -5959,  4579,  4582,
   -5959,  2703, -5959, -5959, -5959, -5959, 36822, 66962,  2204,  4527,
   -5959, -5959,  4732,  1280,  2654,  4732, -5959, -5959, -5959,  1952,
   -5959, -5959,  4280, 73605,  4887,  4888, -5959, 64918,   939,  4594,
   94556,  4534, -5959,  1451, -5959,   238,  2654,  4595,  3406,  4541,
   73605, 73605, -5959,  4542, 94556,  4997,117551, -5959,  1451,   238,
    4581, -5959, 64918, -5959, -5959,  1280, -5959,  4544,  2714, -5959,
    1451, -5959,  2733,117551, -5959, -5959, -5959,  2734, -5959
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
    1731,  1470,   676,  1559,  1558,  1470,     0,   269,     0,  1570,
       0,  1470,   442,  1127,     0,     0,     0,     0,   676,  1470,
       0,  1731,     0,     0,     0,     0,     0,  1744,     0,     0,
       0,     0,     0,  1267,     0,     0,     0,  1470,     0,     0,
    1751,     0,     0,     0,     0,  1744,     0,  1572,     0,     0,
       0,     0,  1751,     0,     0,     0,     0,     0,     2,     9,
      10,    45,    80,    81,    38,    37,    98,    63,    24,    71,
     133,   132,   134,    50,    46,    88,    33,    35,    47,    51,
    ,  2378,  2367,
    1725,  1724,  2824,     0,     0,  1728,  1921,     0,  2370,     0,
       0,  1924,     0,  1871,     0,     0,     0,   559,   544,   399,
     606,   460,   595,   380,  1113,  1097,     0,  1432,     0,     0,
     817,     0,   813,   816,   818,   819,     0,     0,     0,     0,
    1914,  1915,  1924,     0,  2354,  1924,  1893,  1678,  1682,     0,
    1679,   464,   606,     0,     0,     0,   781,     0,  1755,     0,
       0,     0,  1910,  2368,  1923,  2380,  2354,     0,  1883,     0,
       0,     0,   814,     0,     0,     0,     0,  1918,  2368,  2380,
       0,  1098,   815,  1254,  1255,     0,  1727,     0,     0,  1916,
    2368,  1680,     0,     0,  1919,  1917,   782,     0,  1920
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -5959, -5959, -5959,  4152,  -441, -1729, -5160, -1131,  -881, -5959,
   -5959,  -969,   533,  3371, -1727, -5959, -5959, -5959,  1471, -5959,
   959,
   -3779,  3261,  3871,  -834, -5959, -5959,  -810, 10541, -5959, -5959,
   -5959, 10966, 12069, -5959, -5959, 12082, 13001, -5959,  -559, 13621,
    9811,  -797, -1949,   189, 13937, 19692, -1963,  2450,   159, -5959,
   -2995, -2381, -5959, -5959, -5959,  -261,  1140,   628, -5959, -5959,
   -4683, -4486, -5959,  2720, -1965, -5959,  -552,  1902,  -550, -5959,
   -5959, -5959, -2813, -5637, -2987,  2726, -5959, -5959, -5959,  -186,
   -5959, 19983, -3300, -5959,  -523, -1023, -1595,  3231, -3111, -1910,
   -5959, -5959, -5959, -3149, -5959, -5959, -5959,  -694,  1924, -5959,
   -5959,  2052, -5959, -5959, -5959,  -974,  2290,  -775, -1166,  1929,
   -4540, -5959,  -489, -4786, -5046, -5959, -5492, -1082, -1894, -1083,
   -3047, -5958, -1955,  1945, -3143, -5959, -1945,  2026,  1234, -5959,
   -1084, -5959,  4414,  -788,  2741,  -817, 17941,  -715, 10598, -1278,
   -5959,   399, -4219,  5961,  6709,    32,  -120,  8126,  -612,  2650,
    3058, -5959, -5959,    -6,  8794,  -631,  -573, -5959,  4992, 17514,
   15953, -3727, -5959
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    57,    58,    59,    60,  1111,  1116,  1915,  1934,    61,
      62,  1635,  3075,  2926,  3906,  3907,    63,    64,  1631,    65,
      66,    67,    68,  1545,    69,  1887,  3089,    70,  2758,   853,
     5545,  4086,
    4087,  6023,  3294,  3389,  4090,  3302,  3303,  4108,  4097,  3295,
    1379,  4099,  1387,  1388,  1381,  2023,   865,  5304,  1198,  1820,
     808,  1382,  1383,  1384,  1385,  1607,   971,  1165,  1166,   921,
     923,   924,  2680,   662,   654,   973,  3683,  2587,   663,   656,
     657,  1825,  2588
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     653,    71,    80,   175,    83,    84,   998,    85,    86,   929,
     
       0,   459,   460,   461,   462,     0,   463,   464,   465,   466,
     467,   468,     0,   469,   470,   471,     0,     0,   472,   473,
     474,     0,     0,   475,   476,   477,   478,   479,   480,   481,
     482,   483,   484,   485,   486,   487,   488,   489,   490,   491,
     492,   493,   494,     0,   495,   496,   497,   498,   499,   500,
     501,   502,   503,     0,   504,   505,   506,   507,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,   519,
       0,   520,   521,   522,   523,   524,   525,   526,   527,   528,
     529,   530,   531,   532,     0,   533,   534,   535,   536,   537,
     538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
     548,   549,   550,   551,     0,   552,   553,   554,   555,   556,
       0,   557,   558,   559,   560,   561,   562,   563,   564,   565,
     566,     0,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
       0,   585,   586,     0,     0,   587,   588,   589,   590,   591,
     592,   593,   594,     0,   595,   596,   597,     0,     0,   598,
     599,   600,   601,   602,     0,   603,   604,   605,   606,   607,
     608,   609,   610,   611,     0,     0,   612,   613,   614,   615,
     616,     0,     0,   617,   618,   619,   620,   621,   622,   623,
       0,   624,   625,   626,   627,   628,   629,     0,     0,   630,
       0,     0,   631,   632,   633,   634,   635,   636,   637,   638,
     639,   640,   641,   642,   643,   644,   645,   646,   647,   648,
     649,   650,   234,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   244,
     245,   246,     0,     0,     0,     0,     0,     0,     0,   247,
     248,   249,     0,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,     0,   263,   264,   265,
     266,   267,   268,   269,     0,     0,   270,   271,   272,   273,
     274,     0,   275,   276,   277,   278,   279,     0,   280,     0,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,     0,   292,   293,   294,   295,   296,   297,     0,   298,
     299,   300,   301,     0,     0,     0,   302,     0,     0,     0,
     303,   304,   305,   306,   307,   308,   309,   310,   311,     0,
     312,     0,   313,   314,   315,   316,   317,   318,   319,     0,
     320,   321,   322,   323,     0,     0,   324,   325,   326,   327,
     328,     0,   329,   330,   331,   332,     0,   333,   334,   335,
     336,     0,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,     0,   347,     0,   348,   349,   350,   351,   352,
       0,   353,     0,   354,   355,   356,     0,   357,   358,   359,
     360,   361,     0,   362,   363,     0,   364,   365,   366,     0,
     367,   368,   369,   370,  1179,   372,   373,   374,   375,   376,
       0,   377,   378,   379,   380,   381,   382,   383,   384,     0,
     385,   386,   387,   388,   389,   390,   391,   392,   393,     0,
     394,     0,   395,   396,   397,   398,   399,   400,   401,   402,
     403,   404,   405,   406,   407,   408,   409,   410,   411,   412,
     413,   414,   415,   416,   417,     0,     0,   418,   419,   420,
     421,   422,     0,   423,   424,   425,     0,     0,   426,   427,
     428,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,     0,   459,   460,   461,   462,     0,   463,   464,   465,
     466,   467,   468,     0,   469,   470,   471,     0,     0,   472,
     473,   474,     0,     0,   475,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,   492,   493,   494,     0,   495,   496,   497,   498,   499,
     500,   501,   502,   503,     0,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     519,     0,   520,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,     0,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   551,     0,   552,   553,   554,   555,
     556,     0,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,     0,   567,   568,   569,   570,   571,   572,   573,
     574,   575,   576,   577,   578,   579,   580,   581,   582,   583,
     584,     0,   585,   586,     0,     0,   587,   588,   589,   590,
     591,   592,   593,   594,     0,   595,   596,   597,     0,     0,
     598,   599,   600,   601,   602,     0,   603,   604,   605,   606,
     607,   608,   609,   610,   611,     0,     0,   612,   613,   614,
     615,   616,     0,     0,   617,   618,   619,   620,   621,   622,
     623,     0,   624,   625,   626,   627,   628,   629,     0,     0,
     630,     0,     0,   631,   632,   633,   634,   635,   636,   637,
     638,   639,   640,   641,   642,   643,   644,   645,   646,   647,
     648,   649,   650,   967,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  474,     0,     0,   475,   476,   477,     0,   479,     0,   481,
     482,   483,   484,   485,  2014,   487,   488,   489,   490,   491,
     492,   493,   494,     0,   495,    0,     0,     0,   624,   625,   
       0,   630,     0,     0,   631,   632,   633,   634,   635,   636,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   648,   649,   650
};

static const yytype_int16 yycheck[] =
{
       6,     0,     0,     0,     0,     0,   211,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,    23,  1041,    25,
    1050,     0,  1013,    29,    21,   211,   843,   980,    34,    35,
      36,   686,    38,  1228,   665,    41,    42,   942,    44,     8,
      46,  1660,    21,  1595,   759,  1505,    15,    53,    54,    55,
     400,   401,    -1,   403,   404,    -1,   406,   407,   408,   409,
     410,    -1,   412,    -1,   414,   415,   416,   417,   418,   419,
     420,   421,   422,   423,   424,   425,   426,   427,   428,   429,
      -1,   431,    -1,   433,   434,    -1,    -1,   437,   438,   439,
     440,   441,   442,   443,   444,    -1,   446,    -1,    -1,    -1,
      -1,   451,   452,    -1,   454,    -1,    -1,   457,   458,   459,
     460,   461,   462,   463,   464,   465,    -1,    -1,   468,   469,
     470,   471,   472,    -1,    -1,   475,   476,   477,   478,   479,
      -1,    -1,    -1,   483,   484,   485,   486,   487,   488,    -1,
      -1,   491,    -1,    -1,   494,   495,   496,   497,   498,   499,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   511,   512,   513
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,    20,    31,    33,    34,    51,    61,    73,    75,    76,
      82,    84,    97,    99,   116,   119,   134,   136,   140,   147,
     156,   158,   165,   183,   200,   254,   255,   260,   275,   294,
     343,   358,   364,   365,   367,   372,   378,   381,   388,   395,
     396,   403,   407,   419,   436,   457,   469,   475,   480,   493,
     517,   519,   520,   521,   522,   523,   536,   543,   544,   545,
     546,   551,   552,   558,   559,   561,   562,   563,   564,   566,
     569,   581,   586,   587,   590,   591,   592,   614,   617,   618,
     633,   687,   690,   691,   694,   697,   698,   699,   707,   714,
     716,   717,   720,   723,   724,   728,   737,   741,   742,   743,
     746,   748,   749,   750,   751,   759,   761,   781,   785,   787,
     788,   798,   800,   807,   808,   811,   812,   813,   814,   815,
       1248,   537,   640,
     312,  1265,   682,   677,   681,   125,   140,   342,  1250,    64,
      72,   256,  1045,   821,  1104,   860,   177,   778,   880,   537,
     540,   459,   147,   546,   892,   895,  1115,   416,   416,   537,
     282,   306,   766,   767,   768,   171,   771,   709,   288,   478,
     711,   712,  1250,   851,   540,    38,  1157,    37,   301,   308,
     537,   493,   495,  1225,  1226,  1228,   171,  1158,  1167,  1196,
     288,   512,   205,   537,  1196,   188,   355,   386,  1183,    58,
     129,   112,  1055,    38,  1128,  1114,  1111,   536,  1158,   537,
     540,    38,  1121,  1122,  1265,   537,  1249,  1196,   537,   536,
     404,   536,   537,   540,   540,    38,  1069,   479,  1250,   537,
     540,   293,   472,   265,   292,  1018,  1033,    35,  1036,  1036,
     845,   856,   550,  1250,   439,   861,   536,   897,   536,   866,
     603,   666,   362,   667,   667,   194,  1157,   474,   601,   403,
    1024,  1250,  1200,   171,   805,   805,  1200,  1200,  1157,   439,
    1196,  1196,   612,   613,  1267,   611,   595,   820,   821,   820,
     821,  1249,   550,   537,   540,   120,   529,   536,   577,   630,
    
       5,   776,   777,  1255,  1256,  1268,   536,   120,  1256,  1229,
    1130,   315,   157,   333,  1132,  1231,   537,  1040,   537,   322,
    1040,   684,   604,   775,   493,   493,   537,   540,  1197,  1254,
      38,    81,   537,  1132,  1256,  1225,  1132,  1013,   684,  1253,
     179,   179,   777,  1064,   474,  1250,   536,  1228,  1233,  1225,
     479,  1115,   536,   873,   873,   537,  1265,    81,  1129,  1228,
    1233,  1040,   776,   536,   537,  1228,   537,  1129,   537
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   542,   543,   543,   543,   543,   543,   543,   544,   544,
     545,   545,   546,   546,   546,   546,   546,   546,   546,   546,
     546,   546,   546,   546,   546,   546,   546,   546,   546,   546,
     546,   546,   546,   546,   546,   546,   546,   546,   546,   546,
     546,   546,   546,   546,   546,   546,   546,   546,   546,   546,
     
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,  1274,
    1274,  1274,  1274,  1274,  1274
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/*=====================================================================*/

enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, yyscanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, yyscanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, core_yyscan_t yyscanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (yyscanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, core_yyscan_t yyscanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, yyscanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, core_yyscan_t yyscanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), yyscanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, yyscanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, core_yyscan_t yyscanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (yyscanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*==================== THE PARSER (~230000 lines) =========================*/

/*----------.
| yyparse.  |
`----------*/

int
yyparse (core_yyscan_t yyscanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /*=========== THE STACKS =========================================*/

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

    /*================================================================*/

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, yyscanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* parse_toplevel: stmtmulti  */
#line 926 "gram.y"
                        {
                pg_yyget_extra(yyscanner)->parsetree = (yyvsp[0].list);
                (void) yynerrs;     /* suppress compiler warning */
            }
#line 31062 "gram.c"
    break;

  case 3: /* parse_toplevel: MODE_TYPE_NAME Typename  */
#line 931 "gram.y"
                        {
                pg_yyget_extra(yyscanner)->parsetree = list_make1((yyvsp[0].typnam));
            }
#line 31070 "gram.c"
    break;

  case 4: /* parse_toplevel: MODE_PLPGSQL_EXPR PLpgSQL_Expr  */
#line 935 "gram.y"
                        {
                pg_yyget_extra(yyscanner)->parsetree =
                    list_make1(makeRawStmt((yyvsp[0].node), 0));
            }
#line 31079 "gram.c"
    break;

  case 5: /* parse_toplevel: MODE_PLPGSQL_ASSIGN1 PLAssignStmt  */
#line 940 "gram.y"
                        {
                PLAssignStmt *n = (PLAssignStmt *) (yyvsp[0].node);

                n->nnames = 1;
                pg_yyget_extra(yyscanner)->parsetree =
                    list_make1(makeRawStmt((Node *) n, 0));
            }
#line 31091 "gram.c"
    break;

  case 6: /* parse_toplevel: MODE_PLPGSQL_ASSIGN2 PLAssignStmt  */
#line 948 "gram.y"
                        {
                PLAssignStmt *n = (PLAssignStmt *) (yyvsp[0].node);

                n->nnames = 2;
                pg_yyget_extra(yyscanner)->parsetree =
                    list_make1(makeRawStmt((Node *) n, 0));
            }
#line 31103 "gram.c"
    break;

  case 7: /* parse_toplevel: MODE_PLPGSQL_ASSIGN3 PLAssignStmt  */
#line 956 "gram.y"
                        {
                PLAssignStmt *n = (PLAssignStmt *) (yyvsp[0].node);

                n->nnames = 3;
                pg_yyget_extra(yyscanner)->parsetree =
                    list_make1(makeRawStmt((Node *) n, 0));
            }
#line 31115 "gram.c"
    break;

  case 8: /* stmtmulti: stmtmulti ';' toplevel_stmt  */
#line 976 "gram.y"
                                {
                    if ((yyvsp[-2].list) != NIL)
                    {
                        /* update length of previous stmt */
                        updateRawStmtEnd(llast_node(RawStmt, (yyvsp[-2].list)), (yylsp[-1]));
                    }
                    if ((yyvsp[0].node) != NULL)
                        (yyval.list) = lappend((yyvsp[-2].list), makeRawStmt((yyvsp[0].node), (yylsp[-1]) + 1));
                    else
                        (yyval.list) = (yyvsp[-2].list);
                }
#line 31131 "gram.c"
    break;

  case 9: /* stmtmulti: toplevel_stmt  */
#line 988 "gram.y"
                                {
                    if ((yyvsp[0].node) != NULL)
                        (yyval.list) = list_make1(makeRawStmt((yyvsp[0].node), 0));
                    else
                        (yyval.list) = NIL;
                }
#line 31142 "gram.c"
    break;

  case 136: /* stmt: %empty  */
#line 1131 "gram.y"
                                { (yyval.node) = NULL; }
#line 31148 "gram.c"
    break;

  case 137: /* opt_single_name: ColId  */
#line 1138 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 31154 "gram.c"
    break;

  case 138: /* opt_single_name: %empty  */
#line 1139 "gram.y"
                                                                        { (yyval.str) = NULL; }
#line 31160 "gram.c"
    break;

  case 139: /* opt_qualified_name: any_name  */
#line 1143 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 31166 "gram.c"
    break;

  case 140: /* opt_qualified_name: %empty  */
#line 1144 "gram.y"
                                                                                { (yyval.list) = NIL; }
#line 31172 "gram.c"
    break;

  case 141: /* opt_concurrently: CONCURRENTLY  */
#line 1148 "gram.y"
                                                                        { (yyval.boolean) = true; }
#line 31178 "gram.c"
    break;

  case 142: /* opt_concurrently: %empty  */
#line 1149 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 31184 "gram.c"
    break;

  case 143: /* opt_drop_behavior: CASCADE  */
#line 1153 "gram.y"
                                                                                { (yyval.dbehavior) = DROP_CASCADE; }
#line 31190 "gram.c"
    break;

  case 144: /* opt_drop_behavior: RESTRICT  */
#line 1154 "gram.y"
                                                                                { (yyval.dbehavior) = DROP_RESTRICT; }
#line 31196 "gram.c"
    break;

  case 145: /* opt_drop_behavior: %empty  */
#line 1155 "gram.y"
                                                                        { (yyval.dbehavior) = DROP_RESTRICT; /* default */ }
#line 31202 "gram.c"
    break;

  case 146: /* CallStmt: CALL func_application  */
#line 1165 "gram.y"
                                {
                    CallStmt   *n = makeNode(CallStmt);

                    n->funccall = castNode(FuncCall, (yyvsp[0].node));
                    (yyval.node) = (Node *) n;
                }
#line 31213 "gram.c"
    break;

  case 147: /* CreateRoleStmt: CREATE ROLE RoleId opt_with OptRoleList  */
#line 1181 "gram.y"
                                {
                    CreateRoleStmt *n = makeNode(CreateRoleStmt);

                    n->stmt_type = ROLESTMT_ROLE;
                    n->role = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31226 "gram.c"
    break;

  case 151: /* OptRoleList: OptRoleList CreateOptRoleElem  */
#line 1203 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 31232 "gram.c"
    break;

  case 152: /* OptRoleList: %empty  */
#line 1204 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 31238 "gram.c"
    break;

  case 153: /* AlterOptRoleList: AlterOptRoleList AlterOptRoleElem  */
#line 1208 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 31244 "gram.c"
    break;

  case 154: /* AlterOptRoleList: %empty  */
#line 1209 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 31250 "gram.c"
    break;

  case 155: /* AlterOptRoleElem: PASSWORD Sconst  */
#line 1214 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("password",
                                     (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 31259 "gram.c"
    break;

  case 156: /* AlterOptRoleElem: PASSWORD NULL_P  */
#line 1219 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("password", NULL, (yylsp[-1]));
                }
#line 31267 "gram.c"
    break;

  case 157: /* AlterOptRoleElem: ENCRYPTED PASSWORD Sconst  */
#line 1223 "gram.y"
                                {
                    /*
                     * These days, passwords are always stored in encrypted
                     * form, so there is no difference between PASSWORD and
                     * ENCRYPTED PASSWORD.
                     */
                    (yyval.defelt) = makeDefElem("password",
                                     (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 31281 "gram.c"
    break;

  case 158: /* AlterOptRoleElem: UNENCRYPTED PASSWORD Sconst  */
#line 1233 "gram.y"
                                {
                    ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("UNENCRYPTED PASSWORD is no longer supported"),
                             errhint("Remove UNENCRYPTED to store the password in encrypted form instead."),
                             parser_errposition((yylsp[-2]))));
                }
#line 31293 "gram.c"
    break;

  case 159: /* AlterOptRoleElem: INHERIT  */
#line 1241 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("inherit", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 31301 "gram.c"
    break;

  case 160: /* AlterOptRoleElem: CONNECTION LIMIT SignedIconst  */
#line 1245 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("connectionlimit", (Node *) makeInteger((yyvsp[0].ival)), (yylsp[-2]));
                }
#line 31309 "gram.c"
    break;

  case 161: /* AlterOptRoleElem: VALID UNTIL Sconst  */
#line 1249 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("validUntil", (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 31317 "gram.c"
    break;

  case 162: /* AlterOptRoleElem: USER role_list  */
#line 1254 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("rolemembers", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 31325 "gram.c"
    break;

  case 163: /* AlterOptRoleElem: IDENT  */
#line 1258 "gram.y"
                                {
                    /*
                     * We handle identifiers that aren't parser keywords with
                     * the following special-case codes, to avoid bloating the
                     * size of the main parser.
                     */
                    if (strcmp((yyvsp[0].str), "superuser") == 0)
                        (yyval.defelt) = makeDefElem("superuser", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "nosuperuser") == 0)
                        (yyval.defelt) = makeDefElem("superuser", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "createrole") == 0)
                        (yyval.defelt) = makeDefElem("createrole", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "nocreaterole") == 0)
                        (yyval.defelt) = makeDefElem("createrole", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "replication") == 0)
                        (yyval.defelt) = makeDefElem("isreplication", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "noreplication") == 0)
                        (yyval.defelt) = makeDefElem("isreplication", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "createdb") == 0)
                        (yyval.defelt) = makeDefElem("createdb", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "nocreatedb") == 0)
                        (yyval.defelt) = makeDefElem("createdb", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "login") == 0)
                        (yyval.defelt) = makeDefElem("canlogin", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "nologin") == 0)
                        (yyval.defelt) = makeDefElem("canlogin", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "bypassrls") == 0)
                        (yyval.defelt) = makeDefElem("bypassrls", (Node *) makeBoolean(true), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "nobypassrls") == 0)
                        (yyval.defelt) = makeDefElem("bypassrls", (Node *) makeBoolean(false), (yylsp[0]));
                    else if (strcmp((yyvsp[0].str), "noinherit") == 0)
                    {
                        /*
                         * Note that INHERIT is a keyword, so it's handled by main parser, but
                         * NOINHERIT is handled here.
                         */
                        (yyval.defelt) = makeDefElem("inherit", (Node *) makeBoolean(false), (yylsp[0]));
                    }
                    else
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("unrecognized role option \"%s\"", (yyvsp[0].str)),
                                     parser_errposition((yylsp[0]))));
                }
#line 31374 "gram.c"
    break;

  case 164: /* CreateOptRoleElem: AlterOptRoleElem  */
#line 1305 "gram.y"
                                                                { (yyval.defelt) = (yyvsp[0].defelt); }
#line 31380 "gram.c"
    break;

  case 165: /* CreateOptRoleElem: SYSID Iconst  */
#line 1308 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("sysid", (Node *) makeInteger((yyvsp[0].ival)), (yylsp[-1]));
                }
#line 31388 "gram.c"
    break;

  case 166: /* CreateOptRoleElem: ADMIN role_list  */
#line 1312 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("adminmembers", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 31396 "gram.c"
    break;

  case 167: /* CreateOptRoleElem: ROLE role_list  */
#line 1316 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("rolemembers", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 31404 "gram.c"
    break;

  case 168: /* CreateOptRoleElem: IN_P ROLE role_list  */
#line 1320 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("addroleto", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 31412 "gram.c"
    break;

  case 169: /* CreateOptRoleElem: IN_P GROUP_P role_list  */
#line 1324 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("addroleto", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 31420 "gram.c"
    break;

  case 170: /* CreateUserStmt: CREATE USER RoleId opt_with OptRoleList  */
#line 1338 "gram.y"
                                {
                    CreateRoleStmt *n = makeNode(CreateRoleStmt);

                    n->stmt_type = ROLESTMT_USER;
                    n->role = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31433 "gram.c"
    break;

  case 171: /* AlterRoleStmt: ALTER ROLE RoleSpec opt_with AlterOptRoleList  */
#line 1357 "gram.y"
                                 {
                    AlterRoleStmt *n = makeNode(AlterRoleStmt);

                    n->role = (yyvsp[-2].rolespec);
                    n->action = +1; /* add, if there are members */
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                 }
#line 31446 "gram.c"
    break;

  case 172: /* AlterRoleStmt: ALTER USER RoleSpec opt_with AlterOptRoleList  */
#line 1366 "gram.y"
                                 {
                    AlterRoleStmt *n = makeNode(AlterRoleStmt);

                    n->role = (yyvsp[-2].rolespec);
                    n->action = +1; /* add, if there are members */
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                 }
#line 31459 "gram.c"
    break;

  case 173: /* opt_in_database: %empty  */
#line 1377 "gram.y"
                                                                        { (yyval.str) = NULL; }
#line 31465 "gram.c"
    break;

  case 174: /* opt_in_database: IN_P DATABASE name  */
#line 1378 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 31471 "gram.c"
    break;

  case 175: /* AlterRoleSetStmt: ALTER ROLE RoleSpec opt_in_database SetResetClause  */
#line 1383 "gram.y"
                                {
                    AlterRoleSetStmt *n = makeNode(AlterRoleSetStmt);

                    n->role = (yyvsp[-2].rolespec);
                    n->database = (yyvsp[-1].str);
                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 31484 "gram.c"
    break;

  case 176: /* AlterRoleSetStmt: ALTER ROLE ALL opt_in_database SetResetClause  */
#line 1392 "gram.y"
                                {
                    AlterRoleSetStmt *n = makeNode(AlterRoleSetStmt);

                    n->role = NULL;
                    n->database = (yyvsp[-1].str);
                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 31497 "gram.c"
    break;

  case 177: /* AlterRoleSetStmt: ALTER USER RoleSpec opt_in_database SetResetClause  */
#line 1401 "gram.y"
                                {
                    AlterRoleSetStmt *n = makeNode(AlterRoleSetStmt);

                    n->role = (yyvsp[-2].rolespec);
                    n->database = (yyvsp[-1].str);
                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 31510 "gram.c"
    break;

  case 178: /* AlterRoleSetStmt: ALTER USER ALL opt_in_database SetResetClause  */
#line 1410 "gram.y"
                                {
                    AlterRoleSetStmt *n = makeNode(AlterRoleSetStmt);

                    n->role = NULL;
                    n->database = (yyvsp[-1].str);
                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 31523 "gram.c"
    break;

  case 179: /* DropRoleStmt: DROP ROLE role_list  */
#line 1432 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->missing_ok = false;
                    n->roles = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31535 "gram.c"
    break;

  case 180: /* DropRoleStmt: DROP ROLE IF_P EXISTS role_list  */
#line 1440 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->missing_ok = true;
                    n->roles = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31547 "gram.c"
    break;

  case 181: /* DropRoleStmt: DROP USER role_list  */
#line 1448 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->missing_ok = false;
                    n->roles = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31559 "gram.c"
    break;

  case 182: /* DropRoleStmt: DROP USER IF_P EXISTS role_list  */
#line 1456 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->roles = (yyvsp[0].list);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 31571 "gram.c"
    break;

  case 183: /* DropRoleStmt: DROP GROUP_P role_list  */
#line 1464 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->missing_ok = false;
                    n->roles = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31583 "gram.c"
    break;

  case 184: /* DropRoleStmt: DROP GROUP_P IF_P EXISTS role_list  */
#line 1472 "gram.y"
                                {
                    DropRoleStmt *n = makeNode(DropRoleStmt);

                    n->missing_ok = true;
                    n->roles = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31595 "gram.c"
    break;

  case 185: /* CreateGroupStmt: CREATE GROUP_P RoleId opt_with OptRoleList  */
#line 1490 "gram.y"
                                {
                    CreateRoleStmt *n = makeNode(CreateRoleStmt);

                    n->stmt_type = ROLESTMT_GROUP;
                    n->role = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 31608 "gram.c"
    break;

  case 186: /* AlterGroupStmt: ALTER GROUP_P RoleSpec add_drop USER role_list  */
#line 1509 "gram.y"
                                {
                    AlterRoleStmt *n = makeNode(AlterRoleStmt);

                    n->role = (yyvsp[-3].rolespec);
                    n->action = (yyvsp[-2].ival);
                    n->options = list_make1(makeDefElem("rolemembers",
                                                        (Node *) (yyvsp[0].list), (yylsp[0])));
                    (yyval.node) = (Node *) n;
                }
#line 31622 "gram.c"
    break;

  case 187: /* add_drop: ADD_P  */
#line 1520 "gram.y"
                                                                                        { (yyval.ival) = +1; }
#line 31628 "gram.c"
    break;

  case 188: /* add_drop: DROP  */
#line 1521 "gram.y"
                                                                                                { (yyval.ival) = -1; }
#line 31634 "gram.c"
    break;

  case 189: /* CreateSchemaStmt: CREATE SCHEMA opt_single_name AUTHORIZATION RoleSpec OptSchemaEltList  */
#line 1533 "gram.y"
                                {
                    CreateSchemaStmt *n = makeNode(CreateSchemaStmt);

                    /* One can omit the schema name or the authorization id. */
                    n->schemaname = (yyvsp[-3].str);
                    n->authrole = (yyvsp[-1].rolespec);
                    n->schemaElts = (yyvsp[0].list);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 31649 "gram.c"
    break;

  case 190: /* CreateSchemaStmt: CREATE SCHEMA ColId OptSchemaEltList  */
#line 1544 "gram.y"
                                {
                    CreateSchemaStmt *n = makeNode(CreateSchemaStmt);

                    /* ...but not both */
                    n->schemaname = (yyvsp[-1].str);
                    n->authrole = NULL;
                    n->schemaElts = (yyvsp[0].list);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 31664 "gram.c"
    break;

  case 191: /* CreateSchemaStmt: CREATE SCHEMA IF_P NOT EXISTS opt_single_name AUTHORIZATION RoleSpec OptSchemaEltList  */
#line 1555 "gram.y"
                                {
                    CreateSchemaStmt *n = makeNode(CreateSchemaStmt);

                    /* schema name can be omitted here, too */
                    n->schemaname = (yyvsp[-3].str);
                    n->authrole = (yyvsp[-1].rolespec);
                    if ((yyvsp[0].list) != NIL)
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("CREATE SCHEMA IF NOT EXISTS cannot include schema elements"),
                                 parser_errposition((yylsp[0]))));
                    n->schemaElts = (yyvsp[0].list);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 31684 "gram.c"
    break;

  case 192: /* CreateSchemaStmt: CREATE SCHEMA IF_P NOT EXISTS ColId OptSchemaEltList  */
#line 1571 "gram.y"
                                {
                    CreateSchemaStmt *n = makeNode(CreateSchemaStmt);

                    /* ...but not here */
                    n->schemaname = (yyvsp[-1].str);
                    n->authrole = NULL;
                    if ((yyvsp[0].list) != NIL)
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("CREATE SCHEMA IF NOT EXISTS cannot include schema elements"),
                                 parser_errposition((yylsp[0]))));
                    n->schemaElts = (yyvsp[0].list);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 31704 "gram.c"
    break;

  case 193: /* OptSchemaEltList: OptSchemaEltList schema_stmt  */
#line 1590 "gram.y"
                                {
                    if ((yyloc) < 0)            /* see comments for YYLLOC_DEFAULT */
                        (yyloc) = (yylsp[0]);
                    (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node));
                }
#line 31714 "gram.c"
    break;

  case 194: /* OptSchemaEltList: %empty  */
#line 1596 "gram.y"
                                { (yyval.list) = NIL; }
#line 31720 "gram.c"
    break;

  case 201: /* VariableSetStmt: SET set_rest  */
#line 1624 "gram.y"
                                {
                    VariableSetStmt *n = (yyvsp[0].vsetstmt);

                    n->is_local = false;
                    (yyval.node) = (Node *) n;
                }
#line 31731 "gram.c"
    break;

  case 202: /* VariableSetStmt: SET LOCAL set_rest  */
#line 1631 "gram.y"
                                {
                    VariableSetStmt *n = (yyvsp[0].vsetstmt);

                    n->is_local = true;
                    (yyval.node) = (Node *) n;
                }
#line 31742 "gram.c"
    break;

  case 203: /* VariableSetStmt: SET SESSION set_rest  */
#line 1638 "gram.y"
                                {
                    VariableSetStmt *n = (yyvsp[0].vsetstmt);

                    n->is_local = false;
                    (yyval.node) = (Node *) n;
                }
#line 31753 "gram.c"
    break;

  case 204: /* set_rest: TRANSACTION transaction_mode_list  */
#line 1648 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_MULTI;
                    n->name = "TRANSACTION";
                    n->args = (yyvsp[0].list);
                    (yyval.vsetstmt) = n;
                }
#line 31766 "gram.c"
    break;

  case 205: /* set_rest: SESSION CHARACTERISTICS AS TRANSACTION transaction_mode_list  */
#line 1657 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_MULTI;
                    n->name = "SESSION CHARACTERISTICS";
                    n->args = (yyvsp[0].list);
                    (yyval.vsetstmt) = n;
                }
#line 31779 "gram.c"
    break;

  case 207: /* generic_set: var_name TO var_list  */
#line 1670 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = (yyvsp[-2].str);
                    n->args = (yyvsp[0].list);
                    (yyval.vsetstmt) = n;
                }
#line 31792 "gram.c"
    break;

  case 208: /* generic_set: var_name '=' var_list  */
#line 1679 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = (yyvsp[-2].str);
                    n->args = (yyvsp[0].list);
                    (yyval.vsetstmt) = n;
                }
#line 31805 "gram.c"
    break;

  case 209: /* generic_set: var_name TO DEFAULT  */
#line 1688 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_DEFAULT;
                    n->name = (yyvsp[-2].str);
                    (yyval.vsetstmt) = n;
                }
#line 31817 "gram.c"
    break;

  case 210: /* generic_set: var_name '=' DEFAULT  */
#line 1696 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_DEFAULT;
                    n->name = (yyvsp[-2].str);
                    (yyval.vsetstmt) = n;
                }
#line 31829 "gram.c"
    break;

  case 211: /* set_rest_more: generic_set  */
#line 1706 "gram.y"
                                                                                        {(yyval.vsetstmt) = (yyvsp[0].vsetstmt);}
#line 31835 "gram.c"
    break;

  case 212: /* set_rest_more: var_name FROM CURRENT_P  */
#line 1708 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_CURRENT;
                    n->name = (yyvsp[-2].str);
                    (yyval.vsetstmt) = n;
                }
#line 31847 "gram.c"
    break;

  case 213: /* set_rest_more: TIME ZONE zone_value  */
#line 1717 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "timezone";
                    if ((yyvsp[0].node) != NULL)
                        n->args = list_make1((yyvsp[0].node));
                    else
                        n->kind = VAR_SET_DEFAULT;
                    (yyval.vsetstmt) = n;
                }
#line 31863 "gram.c"
    break;

  case 214: /* set_rest_more: CATALOG_P Sconst  */
#line 1729 "gram.y"
                                {
                    ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("current database cannot be changed"),
                             parser_errposition((yylsp[0]))));
                    (yyval.vsetstmt) = NULL; /*not reached*/
                }
#line 31875 "gram.c"
    break;

  case 215: /* set_rest_more: SCHEMA Sconst  */
#line 1737 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "search_path";
                    n->args = list_make1(makeStringConst((yyvsp[0].str), (yylsp[0])));
                    (yyval.vsetstmt) = n;
                }
#line 31888 "gram.c"
    break;

  case 216: /* set_rest_more: NAMES opt_encoding  */
#line 1746 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "client_encoding";
                    if ((yyvsp[0].str) != NULL)
                        n->args = list_make1(makeStringConst((yyvsp[0].str), (yylsp[0])));
                    else
                        n->kind = VAR_SET_DEFAULT;
                    (yyval.vsetstmt) = n;
                }
#line 31904 "gram.c"
    break;

  case 217: /* set_rest_more: ROLE NonReservedWord_or_Sconst  */
#line 1758 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "role";
                    n->args = list_make1(makeStringConst((yyvsp[0].str), (yylsp[0])));
                    (yyval.vsetstmt) = n;
                }
#line 31917 "gram.c"
    break;

  case 218: /* set_rest_more: SESSION AUTHORIZATION NonReservedWord_or_Sconst  */
#line 1767 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "session_authorization";
                    n->args = list_make1(makeStringConst((yyvsp[0].str), (yylsp[0])));
                    (yyval.vsetstmt) = n;
                }
#line 31930 "gram.c"
    break;

  case 219: /* set_rest_more: SESSION AUTHORIZATION DEFAULT  */
#line 1776 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_DEFAULT;
                    n->name = "session_authorization";
                    (yyval.vsetstmt) = n;
                }
#line 31942 "gram.c"
    break;

  case 220: /* set_rest_more: XML_P OPTION document_or_content  */
#line 1784 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_VALUE;
                    n->name = "xmloption";
                    n->args = list_make1(makeStringConst((yyvsp[0].ival) == XMLOPTION_DOCUMENT ? "DOCUMENT" : "CONTENT", (yylsp[0])));
                    (yyval.vsetstmt) = n;
                }
#line 31955 "gram.c"
    break;

  case 221: /* set_rest_more: TRANSACTION SNAPSHOT Sconst  */
#line 1794 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_SET_MULTI;
                    n->name = "TRANSACTION SNAPSHOT";
                    n->args = list_make1(makeStringConst((yyvsp[0].str), (yylsp[0])));
                    (yyval.vsetstmt) = n;
                }
#line 31968 "gram.c"
    break;

  case 222: /* var_name: ColId  */
#line 1804 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 31974 "gram.c"
    break;

  case 223: /* var_name: var_name '.' ColId  */
#line 1806 "gram.y"
                                { (yyval.str) = psprintf("%s.%s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 31980 "gram.c"
    break;

  case 224: /* var_list: var_value  */
#line 1809 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 31986 "gram.c"
    break;

  case 225: /* var_list: var_list ',' var_value  */
#line 1810 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 31992 "gram.c"
    break;

  case 226: /* var_value: opt_boolean_or_string  */
#line 1814 "gram.y"
                                { (yyval.node) = makeStringConst((yyvsp[0].str), (yylsp[0])); }
#line 31998 "gram.c"
    break;

  case 227: /* var_value: NumericOnly  */
#line 1816 "gram.y"
                                { (yyval.node) = makeAConst((yyvsp[0].node), (yylsp[0])); }
#line 32004 "gram.c"
    break;

  case 228: /* iso_level: READ UNCOMMITTED  */
#line 1819 "gram.y"
                                                                                { (yyval.str) = "read uncommitted"; }
#line 32010 "gram.c"
    break;

  case 229: /* iso_level: READ COMMITTED  */
#line 1820 "gram.y"
                                                                                        { (yyval.str) = "read committed"; }
#line 32016 "gram.c"
    break;

  case 230: /* iso_level: REPEATABLE READ  */
#line 1821 "gram.y"
                                                                                        { (yyval.str) = "repeatable read"; }
#line 32022 "gram.c"
    break;

  case 231: /* iso_level: SERIALIZABLE  */
#line 1822 "gram.y"
                                                                                        { (yyval.str) = "serializable"; }
#line 32028 "gram.c"
    break;

  case 232: /* opt_boolean_or_string: TRUE_P  */
#line 1826 "gram.y"
                                                                                                { (yyval.str) = "true"; }
#line 32034 "gram.c"
    break;

  case 233: /* opt_boolean_or_string: FALSE_P  */
#line 1827 "gram.y"
                                                                                                { (yyval.str) = "false"; }
#line 32040 "gram.c"
    break;

  case 234: /* opt_boolean_or_string: ON  */
#line 1828 "gram.y"
                                                                                                { (yyval.str) = "on"; }
#line 32046 "gram.c"
    break;

  case 235: /* opt_boolean_or_string: NonReservedWord_or_Sconst  */
#line 1834 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 32052 "gram.c"
    break;

  case 236: /* zone_value: Sconst  */
#line 1847 "gram.y"
                                {
                    (yyval.node) = makeStringConst((yyvsp[0].str), (yylsp[0]));
                }
#line 32060 "gram.c"
    break;

  case 237: /* zone_value: IDENT  */
#line 1851 "gram.y"
                                {
                    (yyval.node) = makeStringConst((yyvsp[0].str), (yylsp[0]));
                }
#line 32068 "gram.c"
    break;

  case 238: /* zone_value: ConstInterval Sconst opt_interval  */
#line 1855 "gram.y"
                                {
                    TypeName   *t = (yyvsp[-2].typnam);

                    if ((yyvsp[0].list) != NIL)
                    {
                        A_Const    *n = (A_Const *) linitial((yyvsp[0].list));

                        if ((n->val.ival.ival & ~(INTERVAL_MASK(HOUR) | INTERVAL_MASK(MINUTE))) != 0)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("time zone interval must be HOUR or HOUR TO MINUTE"),
                                     parser_errposition((yylsp[0]))));
                    }
                    t->typmods = (yyvsp[0].list);
                    (yyval.node) = makeStringConstCast((yyvsp[-1].str), (yylsp[-1]), t);
                }
#line 32089 "gram.c"
    break;

  case 239: /* zone_value: ConstInterval '(' Iconst ')' Sconst  */
#line 1872 "gram.y"
                                {
                    TypeName   *t = (yyvsp[-4].typnam);

                    t->typmods = list_make2(makeIntConst(INTERVAL_FULL_RANGE, -1),
                                            makeIntConst((yyvsp[-2].ival), (yylsp[-2])));
                    (yyval.node) = makeStringConstCast((yyvsp[0].str), (yylsp[0]), t);
                }
#line 32101 "gram.c"
    break;

  case 240: /* zone_value: NumericOnly  */
#line 1879 "gram.y"
                                                                                        { (yyval.node) = makeAConst((yyvsp[0].node), (yylsp[0])); }
#line 32107 "gram.c"
    break;

  case 241: /* zone_value: DEFAULT  */
#line 1880 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 32113 "gram.c"
    break;

  case 242: /* zone_value: LOCAL  */
#line 1881 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 32119 "gram.c"
    break;

  case 243: /* opt_encoding: Sconst  */
#line 1885 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 32125 "gram.c"
    break;

  case 244: /* opt_encoding: DEFAULT  */
#line 1886 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 32131 "gram.c"
    break;

  case 245: /* opt_encoding: %empty  */
#line 1887 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 32137 "gram.c"
    break;

  case 246: /* NonReservedWord_or_Sconst: NonReservedWord  */
#line 1891 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 32143 "gram.c"
    break;

  case 247: /* NonReservedWord_or_Sconst: Sconst  */
#line 1892 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 32149 "gram.c"
    break;

  case 248: /* VariableResetStmt: RESET reset_rest  */
#line 1896 "gram.y"
                                                                                        { (yyval.node) = (Node *) (yyvsp[0].vsetstmt); }
#line 32155 "gram.c"
    break;

  case 249: /* reset_rest: generic_reset  */
#line 1900 "gram.y"
                                                                                        { (yyval.vsetstmt) = (yyvsp[0].vsetstmt); }
#line 32161 "gram.c"
    break;

  case 250: /* reset_rest: TIME ZONE  */
#line 1902 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_RESET;
                    n->name = "timezone";
                    (yyval.vsetstmt) = n;
                }
#line 32173 "gram.c"
    break;

  case 251: /* reset_rest: TRANSACTION ISOLATION LEVEL  */
#line 1910 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_RESET;
                    n->name = "transaction_isolation";
                    (yyval.vsetstmt) = n;
                }
#line 32185 "gram.c"
    break;

  case 252: /* reset_rest: SESSION AUTHORIZATION  */
#line 1918 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_RESET;
                    n->name = "session_authorization";
                    (yyval.vsetstmt) = n;
                }
#line 32197 "gram.c"
    break;

  case 253: /* generic_reset: var_name  */
#line 1929 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_RESET;
                    n->name = (yyvsp[0].str);
                    (yyval.vsetstmt) = n;
                }
#line 32209 "gram.c"
    break;

  case 254: /* generic_reset: ALL  */
#line 1937 "gram.y"
                                {
                    VariableSetStmt *n = makeNode(VariableSetStmt);

                    n->kind = VAR_RESET_ALL;
                    (yyval.vsetstmt) = n;
                }
#line 32220 "gram.c"
    break;

  case 255: /* SetResetClause: SET set_rest  */
#line 1947 "gram.y"
                                                                        { (yyval.vsetstmt) = (yyvsp[0].vsetstmt); }
#line 32226 "gram.c"
    break;

  case 256: /* SetResetClause: VariableResetStmt  */
#line 1948 "gram.y"
                                                                        { (yyval.vsetstmt) = (VariableSetStmt *) (yyvsp[0].node); }
#line 32232 "gram.c"
    break;

  case 257: /* FunctionSetResetClause: SET set_rest_more  */
#line 1953 "gram.y"
                                                                        { (yyval.vsetstmt) = (yyvsp[0].vsetstmt); }
#line 32238 "gram.c"
    break;

  case 258: /* FunctionSetResetClause: VariableResetStmt  */
#line 1954 "gram.y"
                                                                        { (yyval.vsetstmt) = (VariableSetStmt *) (yyvsp[0].node); }
#line 32244 "gram.c"
    break;

  case 259: /* VariableShowStmt: SHOW var_name  */
#line 1960 "gram.y"
                                {
                    VariableShowStmt *n = makeNode(VariableShowStmt);

                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 32255 "gram.c"
    break;

  case 260: /* VariableShowStmt: SHOW TIME ZONE  */
#line 1967 "gram.y"
                                {
                    VariableShowStmt *n = makeNode(VariableShowStmt);

                    n->name = "timezone";
                    (yyval.node) = (Node *) n;
                }
#line 32266 "gram.c"
    break;

  case 261: /* VariableShowStmt: SHOW TRANSACTION ISOLATION LEVEL  */
#line 1974 "gram.y"
                                {
                    VariableShowStmt *n = makeNode(VariableShowStmt);

                    n->name = "transaction_isolation";
                    (yyval.node) = (Node *) n;
                }
#line 32277 "gram.c"
    break;

  case 262: /* VariableShowStmt: SHOW SESSION AUTHORIZATION  */
#line 1981 "gram.y"
                                {
                    VariableShowStmt *n = makeNode(VariableShowStmt);

                    n->name = "session_authorization";
                    (yyval.node) = (Node *) n;
                }
#line 32288 "gram.c"
    break;

  case 263: /* VariableShowStmt: SHOW ALL  */
#line 1988 "gram.y"
                                {
                    VariableShowStmt *n = makeNode(VariableShowStmt);

                    n->name = "all";
                    (yyval.node) = (Node *) n;
                }
#line 32299 "gram.c"
    break;

  case 264: /* ConstraintsSetStmt: SET CONSTRAINTS constraints_set_list constraints_set_mode  */
#line 1999 "gram.y"
                                {
                    ConstraintsSetStmt *n = makeNode(ConstraintsSetStmt);

                    n->constraints = (yyvsp[-1].list);
                    n->deferred = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32311 "gram.c"
    break;

  case 265: /* constraints_set_list: ALL  */
#line 2009 "gram.y"
                                                                                                        { (yyval.list) = NIL; }
#line 32317 "gram.c"
    break;

  case 266: /* constraints_set_list: qualified_name_list  */
#line 2010 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 32323 "gram.c"
    break;

  case 267: /* constraints_set_mode: DEFERRED  */
#line 2014 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 32329 "gram.c"
    break;

  case 268: /* constraints_set_mode: IMMEDIATE  */
#line 2015 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 32335 "gram.c"
    break;

  case 269: /* CheckPointStmt: CHECKPOINT  */
#line 2024 "gram.y"
                                {
                    CheckPointStmt *n = makeNode(CheckPointStmt);

                    (yyval.node) = (Node *) n;
                }
#line 32345 "gram.c"
    break;

  case 270: /* DiscardStmt: DISCARD ALL  */
#line 2040 "gram.y"
                                {
                    DiscardStmt *n = makeNode(DiscardStmt);

                    n->target = DISCARD_ALL;
                    (yyval.node) = (Node *) n;
                }
#line 32356 "gram.c"
    break;

  case 271: /* DiscardStmt: DISCARD TEMP  */
#line 2047 "gram.y"
                                {
                    DiscardStmt *n = makeNode(DiscardStmt);

                    n->target = DISCARD_TEMP;
                    (yyval.node) = (Node *) n;
                }
#line 32367 "gram.c"
    break;

  case 272: /* DiscardStmt: DISCARD TEMPORARY  */
#line 2054 "gram.y"
                                {
                    DiscardStmt *n = makeNode(DiscardStmt);

                    n->target = DISCARD_TEMP;
                    (yyval.node) = (Node *) n;
                }
#line 32378 "gram.c"
    break;

  case 273: /* DiscardStmt: DISCARD PLANS  */
#line 2061 "gram.y"
                                {
                    DiscardStmt *n = makeNode(DiscardStmt);

                    n->target = DISCARD_PLANS;
                    (yyval.node) = (Node *) n;
                }
#line 32389 "gram.c"
    break;

  case 274: /* DiscardStmt: DISCARD SEQUENCES  */
#line 2068 "gram.y"
                                {
                    DiscardStmt *n = makeNode(DiscardStmt);

                    n->target = DISCARD_SEQUENCES;
                    (yyval.node) = (Node *) n;
                }
#line 32400 "gram.c"
    break;

  case 275: /* AlterTableStmt: ALTER TABLE relation_expr alter_table_cmds  */
#line 2088 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_TABLE;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32414 "gram.c"
    break;

  case 276: /* AlterTableStmt: ALTER TABLE IF_P EXISTS relation_expr alter_table_cmds  */
#line 2098 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_TABLE;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32428 "gram.c"
    break;

  case 277: /* AlterTableStmt: ALTER TABLE relation_expr partition_cmd  */
#line 2108 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = list_make1((yyvsp[0].node));
                    n->objtype = OBJECT_TABLE;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32442 "gram.c"
    break;

  case 278: /* AlterTableStmt: ALTER TABLE IF_P EXISTS relation_expr partition_cmd  */
#line 2118 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = list_make1((yyvsp[0].node));
                    n->objtype = OBJECT_TABLE;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32456 "gram.c"
    break;

  case 279: /* AlterTableStmt: ALTER TABLE ALL IN_P TABLESPACE name SET TABLESPACE name opt_nowait  */
#line 2128 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-4].str);
                    n->objtype = OBJECT_TABLE;
                    n->roles = NIL;
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32472 "gram.c"
    break;

  case 280: /* AlterTableStmt: ALTER TABLE ALL IN_P TABLESPACE name OWNED BY role_list SET TABLESPACE name opt_nowait  */
#line 2140 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-7].str);
                    n->objtype = OBJECT_TABLE;
                    n->roles = (yyvsp[-4].list);
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32488 "gram.c"
    break;

  case 281: /* AlterTableStmt: ALTER INDEX qualified_name alter_table_cmds  */
#line 2152 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_INDEX;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32502 "gram.c"
    break;

  case 282: /* AlterTableStmt: ALTER INDEX IF_P EXISTS qualified_name alter_table_cmds  */
#line 2162 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_INDEX;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32516 "gram.c"
    break;

  case 283: /* AlterTableStmt: ALTER INDEX qualified_name index_partition_cmd  */
#line 2172 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = list_make1((yyvsp[0].node));
                    n->objtype = OBJECT_INDEX;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32530 "gram.c"
    break;

  case 284: /* AlterTableStmt: ALTER INDEX ALL IN_P TABLESPACE name SET TABLESPACE name opt_nowait  */
#line 2182 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-4].str);
                    n->objtype = OBJECT_INDEX;
                    n->roles = NIL;
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32546 "gram.c"
    break;

  case 285: /* AlterTableStmt: ALTER INDEX ALL IN_P TABLESPACE name OWNED BY role_list SET TABLESPACE name opt_nowait  */
#line 2194 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-7].str);
                    n->objtype = OBJECT_INDEX;
                    n->roles = (yyvsp[-4].list);
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32562 "gram.c"
    break;

  case 286: /* AlterTableStmt: ALTER SEQUENCE qualified_name alter_table_cmds  */
#line 2206 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_SEQUENCE;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32576 "gram.c"
    break;

  case 287: /* AlterTableStmt: ALTER SEQUENCE IF_P EXISTS qualified_name alter_table_cmds  */
#line 2216 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_SEQUENCE;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32590 "gram.c"
    break;

  case 288: /* AlterTableStmt: ALTER VIEW qualified_name alter_table_cmds  */
#line 2226 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_VIEW;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32604 "gram.c"
    break;

  case 289: /* AlterTableStmt: ALTER VIEW IF_P EXISTS qualified_name alter_table_cmds  */
#line 2236 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_VIEW;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32618 "gram.c"
    break;

  case 290: /* AlterTableStmt: ALTER MATERIALIZED VIEW qualified_name alter_table_cmds  */
#line 2246 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_MATVIEW;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32632 "gram.c"
    break;

  case 291: /* AlterTableStmt: ALTER MATERIALIZED VIEW IF_P EXISTS qualified_name alter_table_cmds  */
#line 2256 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_MATVIEW;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32646 "gram.c"
    break;

  case 292: /* AlterTableStmt: ALTER MATERIALIZED VIEW ALL IN_P TABLESPACE name SET TABLESPACE name opt_nowait  */
#line 2266 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-4].str);
                    n->objtype = OBJECT_MATVIEW;
                    n->roles = NIL;
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32662 "gram.c"
    break;

  case 293: /* AlterTableStmt: ALTER MATERIALIZED VIEW ALL IN_P TABLESPACE name OWNED BY role_list SET TABLESPACE name opt_nowait  */
#line 2278 "gram.y"
                                {
                    AlterTableMoveAllStmt *n =
                        makeNode(AlterTableMoveAllStmt);

                    n->orig_tablespacename = (yyvsp[-7].str);
                    n->objtype = OBJECT_MATVIEW;
                    n->roles = (yyvsp[-4].list);
                    n->new_tablespacename = (yyvsp[-1].str);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 32678 "gram.c"
    break;

  case 294: /* AlterTableStmt: ALTER FOREIGN TABLE relation_expr alter_table_cmds  */
#line 2290 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_FOREIGN_TABLE;
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32692 "gram.c"
    break;

  case 295: /* AlterTableStmt: ALTER FOREIGN TABLE IF_P EXISTS relation_expr alter_table_cmds  */
#line 2300 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    n->relation = (yyvsp[-1].range);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_FOREIGN_TABLE;
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32706 "gram.c"
    break;

  case 296: /* alter_table_cmds: alter_table_cmd  */
#line 2312 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 32712 "gram.c"
    break;

  case 297: /* alter_table_cmds: alter_table_cmds ',' alter_table_cmd  */
#line 2313 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 32718 "gram.c"
    break;

  case 298: /* partitions_list: SinglePartitionSpec  */
#line 2317 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].singlepartspec)); }
#line 32724 "gram.c"
    break;

  case 299: /* partitions_list: partitions_list ',' SinglePartitionSpec  */
#line 2318 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].singlepartspec)); }
#line 32730 "gram.c"
    break;

  case 300: /* SinglePartitionSpec: PARTITION qualified_name PartitionBoundSpec  */
#line 2323 "gram.y"
                                {
                    SinglePartitionSpec *n = makeNode(SinglePartitionSpec);

                    n->name = (yyvsp[-1].range);
                    n->bound = (yyvsp[0].partboundspec);

                    (yyval.singlepartspec) = n;
                }
#line 32743 "gram.c"
    break;

  case 301: /* partition_cmd: ATTACH PARTITION qualified_name PartitionBoundSpec  */
#line 2336 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_AttachPartition;
                    cmd->name = (yyvsp[-1].range);
                    cmd->bound = (yyvsp[0].partboundspec);
                    cmd->partlist = NULL;
                    cmd->concurrent = false;
                    n->def = (Node *) cmd;

                    (yyval.node) = (Node *) n;
                }
#line 32761 "gram.c"
    break;

  case 302: /* partition_cmd: DETACH PARTITION qualified_name opt_concurrently  */
#line 2351 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_DetachPartition;
                    cmd->name = (yyvsp[-1].range);
                    cmd->bound = NULL;
                    cmd->partlist = NULL;
                    cmd->concurrent = (yyvsp[0].boolean);
                    n->def = (Node *) cmd;

                    (yyval.node) = (Node *) n;
                }
#line 32779 "gram.c"
    break;

  case 303: /* partition_cmd: DETACH PARTITION qualified_name FINALIZE  */
#line 2365 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_DetachPartitionFinalize;
                    cmd->name = (yyvsp[-1].range);
                    cmd->bound = NULL;
                    cmd->partlist = NULL;
                    cmd->concurrent = false;
                    n->def = (Node *) cmd;
                    (yyval.node) = (Node *) n;
                }
#line 32796 "gram.c"
    break;

  case 304: /* partition_cmd: SPLIT PARTITION qualified_name INTO '(' partitions_list ')'  */
#line 2379 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_SplitPartition;
                    cmd->name = (yyvsp[-4].range);
                    cmd->bound = NULL;
                    cmd->partlist = (yyvsp[-1].list);
                    cmd->concurrent = false;
                    n->def = (Node *) cmd;
                    (yyval.node) = (Node *) n;
                }
#line 32813 "gram.c"
    break;

  case 305: /* partition_cmd: MERGE PARTITIONS '(' qualified_name_list ')' INTO qualified_name  */
#line 2393 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_MergePartitions;
                    cmd->name = (yyvsp[0].range);
                    cmd->bound = NULL;
                    cmd->partlist = (yyvsp[-3].list);
                    cmd->concurrent = false;
                    n->def = (Node *) cmd;
                    (yyval.node) = (Node *) n;
                }
#line 32830 "gram.c"
    break;

  case 306: /* index_partition_cmd: ATTACH PARTITION qualified_name  */
#line 2410 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    PartitionCmd *cmd = makeNode(PartitionCmd);

                    n->subtype = AT_AttachPartition;
                    cmd->name = (yyvsp[0].range);
                    cmd->bound = NULL;
                    cmd->partlist = NULL;
                    cmd->concurrent = false;
                    n->def = (Node *) cmd;

                    (yyval.node) = (Node *) n;
                }
#line 32848 "gram.c"
    break;

  case 307: /* alter_table_cmd: ADD_P columnDef  */
#line 2428 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddColumn;
                    n->def = (yyvsp[0].node);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32861 "gram.c"
    break;

  case 308: /* alter_table_cmd: ADD_P IF_P NOT EXISTS columnDef  */
#line 2438 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddColumn;
                    n->def = (yyvsp[0].node);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32874 "gram.c"
    break;

  case 309: /* alter_table_cmd: ADD_P COLUMN columnDef  */
#line 2448 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddColumn;
                    n->def = (yyvsp[0].node);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 32887 "gram.c"
    break;

  case 310: /* alter_table_cmd: ADD_P COLUMN IF_P NOT EXISTS columnDef  */
#line 2458 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddColumn;
                    n->def = (yyvsp[0].node);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32900 "gram.c"
    break;

  case 311: /* alter_table_cmd: ALTER opt_column ColId alter_column_default  */
#line 2468 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ColumnDefault;
                    n->name = (yyvsp[-1].str);
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 32913 "gram.c"
    break;

  case 312: /* alter_table_cmd: ALTER opt_column ColId DROP NOT NULL_P  */
#line 2478 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropNotNull;
                    n->name = (yyvsp[-3].str);
                    (yyval.node) = (Node *) n;
                }
#line 32925 "gram.c"
    break;

  case 313: /* alter_table_cmd: ALTER opt_column ColId SET NOT NULL_P  */
#line 2487 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetNotNull;
                    n->name = (yyvsp[-3].str);
                    (yyval.node) = (Node *) n;
                }
#line 32937 "gram.c"
    break;

  case 314: /* alter_table_cmd: ALTER opt_column ColId SET EXPRESSION AS '(' a_expr ')'  */
#line 2496 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetExpression;
                    n->name = (yyvsp[-6].str);
                    n->def = (yyvsp[-1].node);
                    (yyval.node) = (Node *) n;
                }
#line 32950 "gram.c"
    break;

  case 315: /* alter_table_cmd: ALTER opt_column ColId DROP EXPRESSION  */
#line 2506 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropExpression;
                    n->name = (yyvsp[-2].str);
                    (yyval.node) = (Node *) n;
                }
#line 32962 "gram.c"
    break;

  case 316: /* alter_table_cmd: ALTER opt_column ColId DROP EXPRESSION IF_P EXISTS  */
#line 2515 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropExpression;
                    n->name = (yyvsp[-4].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 32975 "gram.c"
    break;

  case 317: /* alter_table_cmd: ALTER opt_column ColId SET STATISTICS set_statistics_value  */
#line 2525 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetStatistics;
                    n->name = (yyvsp[-3].str);
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 32988 "gram.c"
    break;

  case 318: /* alter_table_cmd: ALTER opt_column Iconst SET STATISTICS set_statistics_value  */
#line 2535 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    if ((yyvsp[-3].ival) <= 0 || (yyvsp[-3].ival) > PG_INT16_MAX)
                        ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                 errmsg("column number must be in range from 1 to %d", PG_INT16_MAX),
                                 parser_errposition((yylsp[-3]))));

                    n->subtype = AT_SetStatistics;
                    n->num = (int16) (yyvsp[-3].ival);
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 33007 "gram.c"
    break;

  case 319: /* alter_table_cmd: ALTER opt_column ColId SET reloptions  */
#line 2551 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetOptions;
                    n->name = (yyvsp[-2].str);
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33020 "gram.c"
    break;

  case 320: /* alter_table_cmd: ALTER opt_column ColId RESET reloptions  */
#line 2561 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ResetOptions;
                    n->name = (yyvsp[-2].str);
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33033 "gram.c"
    break;

  case 321: /* alter_table_cmd: ALTER opt_column ColId SET column_storage  */
#line 2571 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetStorage;
                    n->name = (yyvsp[-2].str);
                    n->def = (Node *) makeString((yyvsp[0].str));
                    (yyval.node) = (Node *) n;
                }
#line 33046 "gram.c"
    break;

  case 322: /* alter_table_cmd: ALTER opt_column ColId SET column_compression  */
#line 2581 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetCompression;
                    n->name = (yyvsp[-2].str);
                    n->def = (Node *) makeString((yyvsp[0].str));
                    (yyval.node) = (Node *) n;
                }
#line 33059 "gram.c"
    break;

  case 323: /* alter_table_cmd: ALTER opt_column ColId ADD_P GENERATED generated_when AS IDENTITY_P OptParenthesizedSeqOptList  */
#line 2591 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    Constraint *c = makeNode(Constraint);

                    c->contype = CONSTR_IDENTITY;
                    c->generated_when = (yyvsp[-3].ival);
                    c->options = (yyvsp[0].list);
                    c->location = (yylsp[-4]);

                    n->subtype = AT_AddIdentity;
                    n->name = (yyvsp[-6].str);
                    n->def = (Node *) c;

                    (yyval.node) = (Node *) n;
                }
#line 33079 "gram.c"
    break;

  case 324: /* alter_table_cmd: ALTER opt_column ColId alter_identity_column_option_list  */
#line 2608 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetIdentity;
                    n->name = (yyvsp[-1].str);
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33092 "gram.c"
    break;

  case 325: /* alter_table_cmd: ALTER opt_column ColId DROP IDENTITY_P  */
#line 2618 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropIdentity;
                    n->name = (yyvsp[-2].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 33105 "gram.c"
    break;

  case 326: /* alter_table_cmd: ALTER opt_column ColId DROP IDENTITY_P IF_P EXISTS  */
#line 2628 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropIdentity;
                    n->name = (yyvsp[-4].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 33118 "gram.c"
    break;

  case 327: /* alter_table_cmd: DROP opt_column IF_P EXISTS ColId opt_drop_behavior  */
#line 2638 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropColumn;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 33132 "gram.c"
    break;

  case 328: /* alter_table_cmd: DROP opt_column ColId opt_drop_behavior  */
#line 2649 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropColumn;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 33146 "gram.c"
    break;

  case 329: /* alter_table_cmd: ALTER opt_column ColId opt_set_data TYPE_P Typename opt_collate_clause alter_using  */
#line 2663 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    ColumnDef *def = makeNode(ColumnDef);

                    n->subtype = AT_AlterColumnType;
                    n->name = (yyvsp[-5].str);
                    n->def = (Node *) def;
                    /* We only use these fields of the ColumnDef node */
                    def->typeName = (yyvsp[-2].typnam);
                    def->collClause = (CollateClause *) (yyvsp[-1].node);
                    def->raw_default = (yyvsp[0].node);
                    def->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 33165 "gram.c"
    break;

  case 330: /* alter_table_cmd: ALTER opt_column ColId alter_generic_options  */
#line 2679 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AlterColumnGenericOptions;
                    n->name = (yyvsp[-1].str);
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33178 "gram.c"
    break;

  case 331: /* alter_table_cmd: ADD_P TableConstraint  */
#line 2689 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddConstraint;
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 33190 "gram.c"
    break;

  case 332: /* alter_table_cmd: ALTER CONSTRAINT name ConstraintAttributeSpec  */
#line 2698 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    Constraint *c = makeNode(Constraint);

                    n->subtype = AT_AlterConstraint;
                    n->def = (Node *) c;
                    c->contype = CONSTR_FOREIGN; /* others not supported, yet */
                    c->conname = (yyvsp[-1].str);
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "ALTER CONSTRAINT statement",
                                    &c->deferrable,
                                    &c->initdeferred,
                                    NULL, NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 33209 "gram.c"
    break;

  case 333: /* alter_table_cmd: VALIDATE CONSTRAINT name  */
#line 2714 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ValidateConstraint;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33221 "gram.c"
    break;

  case 334: /* alter_table_cmd: DROP CONSTRAINT IF_P EXISTS name opt_drop_behavior  */
#line 2723 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropConstraint;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 33235 "gram.c"
    break;

  case 335: /* alter_table_cmd: DROP CONSTRAINT name opt_drop_behavior  */
#line 2734 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropConstraint;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 33249 "gram.c"
    break;

  case 336: /* alter_table_cmd: SET WITHOUT OIDS  */
#line 2745 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropOids;
                    (yyval.node) = (Node *) n;
                }
#line 33260 "gram.c"
    break;

  case 337: /* alter_table_cmd: CLUSTER ON name  */
#line 2753 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ClusterOn;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33272 "gram.c"
    break;

  case 338: /* alter_table_cmd: SET WITHOUT CLUSTER  */
#line 2762 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropCluster;
                    n->name = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 33284 "gram.c"
    break;

  case 339: /* alter_table_cmd: SET LOGGED  */
#line 2771 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetLogged;
                    (yyval.node) = (Node *) n;
                }
#line 33295 "gram.c"
    break;

  case 340: /* alter_table_cmd: SET UNLOGGED  */
#line 2779 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetUnLogged;
                    (yyval.node) = (Node *) n;
                }
#line 33306 "gram.c"
    break;

  case 341: /* alter_table_cmd: ENABLE_P TRIGGER name  */
#line 2787 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableTrig;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33318 "gram.c"
    break;

  case 342: /* alter_table_cmd: ENABLE_P ALWAYS TRIGGER name  */
#line 2796 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableAlwaysTrig;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33330 "gram.c"
    break;

  case 343: /* alter_table_cmd: ENABLE_P REPLICA TRIGGER name  */
#line 2805 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableReplicaTrig;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33342 "gram.c"
    break;

  case 344: /* alter_table_cmd: ENABLE_P TRIGGER ALL  */
#line 2814 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableTrigAll;
                    (yyval.node) = (Node *) n;
                }
#line 33353 "gram.c"
    break;

  case 345: /* alter_table_cmd: ENABLE_P TRIGGER USER  */
#line 2822 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableTrigUser;
                    (yyval.node) = (Node *) n;
                }
#line 33364 "gram.c"
    break;

  case 346: /* alter_table_cmd: DISABLE_P TRIGGER name  */
#line 2830 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DisableTrig;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33376 "gram.c"
    break;

  case 347: /* alter_table_cmd: DISABLE_P TRIGGER ALL  */
#line 2839 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DisableTrigAll;
                    (yyval.node) = (Node *) n;
                }
#line 33387 "gram.c"
    break;

  case 348: /* alter_table_cmd: DISABLE_P TRIGGER USER  */
#line 2847 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DisableTrigUser;
                    (yyval.node) = (Node *) n;
                }
#line 33398 "gram.c"
    break;

  case 349: /* alter_table_cmd: ENABLE_P RULE name  */
#line 2855 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableRule;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33410 "gram.c"
    break;

  case 350: /* alter_table_cmd: ENABLE_P ALWAYS RULE name  */
#line 2864 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableAlwaysRule;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33422 "gram.c"
    break;

  case 351: /* alter_table_cmd: ENABLE_P REPLICA RULE name  */
#line 2873 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableReplicaRule;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33434 "gram.c"
    break;

  case 352: /* alter_table_cmd: DISABLE_P RULE name  */
#line 2882 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DisableRule;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33446 "gram.c"
    break;

  case 353: /* alter_table_cmd: INHERIT qualified_name  */
#line 2891 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddInherit;
                    n->def = (Node *) (yyvsp[0].range);
                    (yyval.node) = (Node *) n;
                }
#line 33458 "gram.c"
    break;

  case 354: /* alter_table_cmd: NO INHERIT qualified_name  */
#line 2900 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropInherit;
                    n->def = (Node *) (yyvsp[0].range);
                    (yyval.node) = (Node *) n;
                }
#line 33470 "gram.c"
    break;

  case 355: /* alter_table_cmd: OF any_name  */
#line 2909 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    TypeName   *def = makeTypeNameFromNameList((yyvsp[0].list));

                    def->location = (yylsp[0]);
                    n->subtype = AT_AddOf;
                    n->def = (Node *) def;
                    (yyval.node) = (Node *) n;
                }
#line 33484 "gram.c"
    break;

  case 356: /* alter_table_cmd: NOT OF  */
#line 2920 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropOf;
                    (yyval.node) = (Node *) n;
                }
#line 33495 "gram.c"
    break;

  case 357: /* alter_table_cmd: OWNER TO RoleSpec  */
#line 2928 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ChangeOwner;
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 33507 "gram.c"
    break;

  case 358: /* alter_table_cmd: SET ACCESS METHOD set_access_method_name  */
#line 2937 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetAccessMethod;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33519 "gram.c"
    break;

  case 359: /* alter_table_cmd: SET TABLESPACE name  */
#line 2946 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetTableSpace;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33531 "gram.c"
    break;

  case 360: /* alter_table_cmd: SET reloptions  */
#line 2955 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_SetRelOptions;
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33543 "gram.c"
    break;

  case 361: /* alter_table_cmd: RESET reloptions  */
#line 2964 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ResetRelOptions;
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33555 "gram.c"
    break;

  case 362: /* alter_table_cmd: REPLICA IDENTITY_P replica_identity  */
#line 2973 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ReplicaIdentity;
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 33567 "gram.c"
    break;

  case 363: /* alter_table_cmd: ENABLE_P ROW LEVEL SECURITY  */
#line 2982 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_EnableRowSecurity;
                    (yyval.node) = (Node *) n;
                }
#line 33578 "gram.c"
    break;

  case 364: /* alter_table_cmd: DISABLE_P ROW LEVEL SECURITY  */
#line 2990 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DisableRowSecurity;
                    (yyval.node) = (Node *) n;
                }
#line 33589 "gram.c"
    break;

  case 365: /* alter_table_cmd: FORCE ROW LEVEL SECURITY  */
#line 2998 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_ForceRowSecurity;
                    (yyval.node) = (Node *) n;
                }
#line 33600 "gram.c"
    break;

  case 366: /* alter_table_cmd: NO FORCE ROW LEVEL SECURITY  */
#line 3006 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_NoForceRowSecurity;
                    (yyval.node) = (Node *) n;
                }
#line 33611 "gram.c"
    break;

  case 367: /* alter_table_cmd: alter_generic_options  */
#line 3013 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_GenericOptions;
                    n->def = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 33623 "gram.c"
    break;

  case 368: /* alter_column_default: SET DEFAULT a_expr  */
#line 3023 "gram.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 33629 "gram.c"
    break;

  case 369: /* alter_column_default: DROP DEFAULT  */
#line 3024 "gram.y"
                                                                { (yyval.node) = NULL; }
#line 33635 "gram.c"
    break;

  case 370: /* opt_collate_clause: COLLATE any_name  */
#line 3029 "gram.y"
                                {
                    CollateClause *n = makeNode(CollateClause);

                    n->arg = NULL;
                    n->collname = (yyvsp[0].list);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 33648 "gram.c"
    break;

  case 371: /* opt_collate_clause: %empty  */
#line 3037 "gram.y"
                                                                { (yyval.node) = NULL; }
#line 33654 "gram.c"
    break;

  case 372: /* alter_using: USING a_expr  */
#line 3041 "gram.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 33660 "gram.c"
    break;

  case 373: /* alter_using: %empty  */
#line 3042 "gram.y"
                                                                { (yyval.node) = NULL; }
#line 33666 "gram.c"
    break;

  case 374: /* replica_identity: NOTHING  */
#line 3047 "gram.y"
                                {
                    ReplicaIdentityStmt *n = makeNode(ReplicaIdentityStmt);

                    n->identity_type = REPLICA_IDENTITY_NOTHING;
                    n->name = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 33678 "gram.c"
    break;

  case 375: /* replica_identity: FULL  */
#line 3055 "gram.y"
                                {
                    ReplicaIdentityStmt *n = makeNode(ReplicaIdentityStmt);

                    n->identity_type = REPLICA_IDENTITY_FULL;
                    n->name = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 33690 "gram.c"
    break;

  case 376: /* replica_identity: DEFAULT  */
#line 3063 "gram.y"
                                {
                    ReplicaIdentityStmt *n = makeNode(ReplicaIdentityStmt);

                    n->identity_type = REPLICA_IDENTITY_DEFAULT;
                    n->name = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 33702 "gram.c"
    break;

  case 377: /* replica_identity: USING INDEX name  */
#line 3071 "gram.y"
                                {
                    ReplicaIdentityStmt *n = makeNode(ReplicaIdentityStmt);

                    n->identity_type = REPLICA_IDENTITY_INDEX;
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 33714 "gram.c"
    break;

  case 378: /* reloptions: '(' reloption_list ')'  */
#line 3081 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 33720 "gram.c"
    break;

  case 379: /* opt_reloptions: WITH reloptions  */
#line 3084 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 33726 "gram.c"
    break;

  case 380: /* opt_reloptions: %empty  */
#line 3085 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 33732 "gram.c"
    break;

  case 381: /* reloption_list: reloption_elem  */
#line 3089 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 33738 "gram.c"
    break;

  case 382: /* reloption_list: reloption_list ',' reloption_elem  */
#line 3090 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 33744 "gram.c"
    break;

  case 383: /* reloption_elem: ColLabel '=' def_arg  */
#line 3096 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 33752 "gram.c"
    break;

  case 384: /* reloption_elem: ColLabel  */
#line 3100 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[0].str), NULL, (yylsp[0]));
                }
#line 33760 "gram.c"
    break;

  case 385: /* reloption_elem: ColLabel '.' ColLabel '=' def_arg  */
#line 3104 "gram.y"
                                {
                    (yyval.defelt) = makeDefElemExtended((yyvsp[-4].str), (yyvsp[-2].str), (Node *) (yyvsp[0].node),
                                             DEFELEM_UNSPEC, (yylsp[-4]));
                }
#line 33769 "gram.c"
    break;

  case 386: /* reloption_elem: ColLabel '.' ColLabel  */
#line 3109 "gram.y"
                                {
                    (yyval.defelt) = makeDefElemExtended((yyvsp[-2].str), (yyvsp[0].str), NULL, DEFELEM_UNSPEC, (yylsp[-2]));
                }
#line 33777 "gram.c"
    break;

  case 387: /* alter_identity_column_option_list: alter_identity_column_option  */
#line 3116 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 33783 "gram.c"
    break;

  case 388: /* alter_identity_column_option_list: alter_identity_column_option_list alter_identity_column_option  */
#line 3118 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 33789 "gram.c"
    break;

  case 389: /* alter_identity_column_option: RESTART  */
#line 3123 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("restart", NULL, (yylsp[0]));
                }
#line 33797 "gram.c"
    break;

  case 390: /* alter_identity_column_option: RESTART opt_with NumericOnly  */
#line 3127 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("restart", (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 33805 "gram.c"
    break;

  case 391: /* alter_identity_column_option: SET SeqOptElem  */
#line 3131 "gram.y"
                                {
                    if (strcmp((yyvsp[0].defelt)->defname, "as") == 0 ||
                        strcmp((yyvsp[0].defelt)->defname, "restart") == 0 ||
                        strcmp((yyvsp[0].defelt)->defname, "owned_by") == 0)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("sequence option \"%s\" not supported here", (yyvsp[0].defelt)->defname),
                                 parser_errposition((yylsp[0]))));
                    (yyval.defelt) = (yyvsp[0].defelt);
                }
#line 33820 "gram.c"
    break;

  case 392: /* alter_identity_column_option: SET GENERATED generated_when  */
#line 3142 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("generated", (Node *) makeInteger((yyvsp[0].ival)), (yylsp[-2]));
                }
#line 33828 "gram.c"
    break;

  case 393: /* set_statistics_value: SignedIconst  */
#line 3148 "gram.y"
                                                                        { (yyval.node) = (Node *) makeInteger((yyvsp[0].ival)); }
#line 33834 "gram.c"
    break;

  case 394: /* set_statistics_value: DEFAULT  */
#line 3149 "gram.y"
                                                                                { (yyval.node) = NULL; }
#line 33840 "gram.c"
    break;

  case 395: /* set_access_method_name: ColId  */
#line 3153 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 33846 "gram.c"
    break;

  case 396: /* set_access_method_name: DEFAULT  */
#line 3154 "gram.y"
                                                                                { (yyval.str) = NULL; }
#line 33852 "gram.c"
    break;

  case 397: /* PartitionBoundSpec: FOR VALUES WITH '(' hash_partbound ')'  */
#line 3160 "gram.y"
                                {
                    ListCell   *lc;
                    PartitionBoundSpec *n = makeNode(PartitionBoundSpec);

                    n->strategy = PARTITION_STRATEGY_HASH;
                    n->modulus = n->remainder = -1;

                    foreach (lc, (yyvsp[-1].list))
                    {
                        DefElem    *opt = lfirst_node(DefElem, lc);

                        if (strcmp(opt->defname, "modulus") == 0)
                        {
                            if (n->modulus != -1)
                                ereport(ERROR,
                                        (errcode(ERRCODE_DUPLICATE_OBJECT),
                                         errmsg("modulus for hash partition provided more than once"),
                                         parser_errposition(opt->location)));
                            n->modulus = defGetInt32(opt);
                        }
                        else if (strcmp(opt->defname, "remainder") == 0)
                        {
                            if (n->remainder != -1)
                                ereport(ERROR,
                                        (errcode(ERRCODE_DUPLICATE_OBJECT),
                                         errmsg("remainder for hash partition provided more than once"),
                                         parser_errposition(opt->location)));
                            n->remainder = defGetInt32(opt);
                        }
                        else
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("unrecognized hash partition bound specification \"%s\"",
                                            opt->defname),
                                     parser_errposition(opt->location)));
                    }

                    if (n->modulus == -1)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("modulus for hash partition must be specified")));
                    if (n->remainder == -1)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("remainder for hash partition must be specified")));

                    n->location = (yylsp[-3]);

                    (yyval.partboundspec) = n;
                }
#line 33907 "gram.c"
    break;

  case 398: /* PartitionBoundSpec: FOR VALUES IN_P '(' expr_list ')'  */
#line 3213 "gram.y"
                                {
                    PartitionBoundSpec *n = makeNode(PartitionBoundSpec);

                    n->strategy = PARTITION_STRATEGY_LIST;
                    n->is_default = false;
                    n->listdatums = (yyvsp[-1].list);
                    n->location = (yylsp[-3]);

                    (yyval.partboundspec) = n;
                }
#line 33922 "gram.c"
    break;

  case 399: /* PartitionBoundSpec: FOR VALUES FROM '(' expr_list ')' TO '(' expr_list ')'  */
#line 3226 "gram.y"
                                {
                    PartitionBoundSpec *n = makeNode(PartitionBoundSpec);

                    n->strategy = PARTITION_STRATEGY_RANGE;
                    n->is_default = false;
                    n->lowerdatums = (yyvsp[-5].list);
                    n->upperdatums = (yyvsp[-1].list);
                    n->location = (yylsp[-7]);

                    (yyval.partboundspec) = n;
                }
#line 33938 "gram.c"
    break;

  case 400: /* PartitionBoundSpec: DEFAULT  */
#line 3240 "gram.y"
                                {
                    PartitionBoundSpec *n = makeNode(PartitionBoundSpec);

                    n->is_default = true;
                    n->location = (yylsp[0]);

                    (yyval.partboundspec) = n;
                }
#line 33951 "gram.c"
    break;

  case 401: /* hash_partbound_elem: NonReservedWord Iconst  */
#line 3252 "gram.y"
                        {
                (yyval.defelt) = makeDefElem((yyvsp[-1].str), (Node *) makeInteger((yyvsp[0].ival)), (yylsp[-1]));
            }
#line 33959 "gram.c"
    break;

  case 402: /* hash_partbound: hash_partbound_elem  */
#line 3259 "gram.y"
                        {
                (yyval.list) = list_make1((yyvsp[0].defelt));
            }
#line 33967 "gram.c"
    break;

  case 403: /* hash_partbound: hash_partbound ',' hash_partbound_elem  */
#line 3263 "gram.y"
                        {
                (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt));
            }
#line 33975 "gram.c"
    break;

  case 404: /* AlterCompositeTypeStmt: ALTER TYPE_P any_name alter_type_cmds  */
#line 3277 "gram.y"
                                {
                    AlterTableStmt *n = makeNode(AlterTableStmt);

                    /* can't use qualified_name, sigh */
                    n->relation = makeRangeVarFromAnyName((yyvsp[-1].list), (yylsp[-1]), yyscanner);
                    n->cmds = (yyvsp[0].list);
                    n->objtype = OBJECT_TYPE;
                    (yyval.node) = (Node *) n;
                }
#line 33989 "gram.c"
    break;

  case 405: /* alter_type_cmds: alter_type_cmd  */
#line 3289 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 33995 "gram.c"
    break;

  case 406: /* alter_type_cmds: alter_type_cmds ',' alter_type_cmd  */
#line 3290 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 34001 "gram.c"
    break;

  case 407: /* alter_type_cmd: ADD_P ATTRIBUTE TableFuncElement opt_drop_behavior  */
#line 3296 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_AddColumn;
                    n->def = (yyvsp[-1].node);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 34014 "gram.c"
    break;

  case 408: /* alter_type_cmd: DROP ATTRIBUTE IF_P EXISTS ColId opt_drop_behavior  */
#line 3306 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropColumn;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 34028 "gram.c"
    break;

  case 409: /* alter_type_cmd: DROP ATTRIBUTE ColId opt_drop_behavior  */
#line 3317 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);

                    n->subtype = AT_DropColumn;
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 34042 "gram.c"
    break;

  case 410: /* alter_type_cmd: ALTER ATTRIBUTE ColId opt_set_data TYPE_P Typename opt_collate_clause opt_drop_behavior  */
#line 3328 "gram.y"
                                {
                    AlterTableCmd *n = makeNode(AlterTableCmd);
                    ColumnDef *def = makeNode(ColumnDef);

                    n->subtype = AT_AlterColumnType;
                    n->name = (yyvsp[-5].str);
                    n->def = (Node *) def;
                    n->behavior = (yyvsp[0].dbehavior);
                    /* We only use these fields of the ColumnDef node */
                    def->typeName = (yyvsp[-2].typnam);
                    def->collClause = (CollateClause *) (yyvsp[-1].node);
                    def->raw_default = NULL;
                    def->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 34062 "gram.c"
    break;

  case 411: /* ClosePortalStmt: CLOSE cursor_name  */
#line 3355 "gram.y"
                                {
                    ClosePortalStmt *n = makeNode(ClosePortalStmt);

                    n->portalname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 34073 "gram.c"
    break;

  case 412: /* ClosePortalStmt: CLOSE ALL  */
#line 3362 "gram.y"
                                {
                    ClosePortalStmt *n = makeNode(ClosePortalStmt);

                    n->portalname = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 34084 "gram.c"
    break;

  case 413: /* CopyStmt: COPY opt_binary qualified_name opt_column_list copy_from opt_program copy_file_name copy_delimiter opt_with copy_options where_clause  */
#line 3398 "gram.y"
                                {
                    CopyStmt *n = makeNode(CopyStmt);

                    n->relation = (yyvsp[-8].range);
                    n->query = NULL;
                    n->attlist = (yyvsp[-7].list);
                    n->is_from = (yyvsp[-6].boolean);
                    n->is_program = (yyvsp[-5].boolean);
                    n->filename = (yyvsp[-4].str);
                    n->whereClause = (yyvsp[0].node);

                    if (n->is_program && n->filename == NULL)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("STDIN/STDOUT not allowed with PROGRAM"),
                                 parser_errposition((yylsp[-3]))));

                    if (!n->is_from && n->whereClause != NULL)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("WHERE clause not allowed with COPY TO"),
                                 parser_errposition((yylsp[0]))));

                    n->options = NIL;
                    /* Concatenate user-supplied flags */
                    if ((yyvsp[-9].defelt))
                        n->options = lappend(n->options, (yyvsp[-9].defelt));
                    if ((yyvsp[-3].defelt))
                        n->options = lappend(n->options, (yyvsp[-3].defelt));
                    if ((yyvsp[-1].list))
                        n->options = list_concat(n->options, (yyvsp[-1].list));
                    (yyval.node) = (Node *) n;
                }
#line 34122 "gram.c"
    break;

  case 414: /* CopyStmt: COPY '(' PreparableStmt ')' TO opt_program copy_file_name opt_with copy_options  */
#line 3432 "gram.y"
                                {
                    CopyStmt *n = makeNode(CopyStmt);

                    n->relation = NULL;
                    n->query = (yyvsp[-6].node);
                    n->attlist = NIL;
                    n->is_from = false;
                    n->is_program = (yyvsp[-3].boolean);
                    n->filename = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);

                    if (n->is_program && n->filename == NULL)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("STDIN/STDOUT not allowed with PROGRAM"),
                                 parser_errposition((yylsp[-4]))));

                    (yyval.node) = (Node *) n;
                }
#line 34146 "gram.c"
    break;

  case 415: /* copy_from: FROM  */
#line 3454 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 34152 "gram.c"
    break;

  case 416: /* copy_from: TO  */
#line 3455 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 34158 "gram.c"
    break;

  case 417: /* opt_program: PROGRAM  */
#line 3459 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 34164 "gram.c"
    break;

  case 418: /* opt_program: %empty  */
#line 3460 "gram.y"
                                                                                        { (yyval.boolean) = false; }
#line 34170 "gram.c"
    break;

  case 419: /* copy_file_name: Sconst  */
#line 3469 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 34176 "gram.c"
    break;

  case 420: /* copy_file_name: STDIN  */
#line 3470 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 34182 "gram.c"
    break;

  case 421: /* copy_file_name: STDOUT  */
#line 3471 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 34188 "gram.c"
    break;

  case 422: /* copy_options: copy_opt_list  */
#line 3474 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 34194 "gram.c"
    break;

  case 423: /* copy_options: '(' copy_generic_opt_list ')'  */
#line 3475 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 34200 "gram.c"
    break;

  case 424: /* copy_opt_list: copy_opt_list copy_opt_item  */
#line 3480 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 34206 "gram.c"
    break;

  case 425: /* copy_opt_list: %empty  */
#line 3481 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 34212 "gram.c"
    break;

  case 426: /* copy_opt_item: BINARY  */
#line 3486 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("format", (Node *) makeString("binary"), (yylsp[0]));
                }
#line 34220 "gram.c"
    break;

  case 427: /* copy_opt_item: FREEZE  */
#line 3490 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("freeze", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 34228 "gram.c"
    break;

  case 428: /* copy_opt_item: DELIMITER opt_as Sconst  */
#line 3494 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("delimiter", (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 34236 "gram.c"
    break;

  case 429: /* copy_opt_item: NULL_P opt_as Sconst  */
#line 3498 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("null", (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 34244 "gram.c"
    break;

  case 430: /* copy_opt_item: CSV  */
#line 3502 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("format", (Node *) makeString("csv"), (yylsp[0]));
                }
#line 34252 "gram.c"
    break;

  case 431: /* copy_opt_item: HEADER_P  */
#line 3506 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("header", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 34260 "gram.c"
    break;

  case 432: /* copy_opt_item: QUOTE opt_as Sconst  */
#line 3510 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("quote", (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 34268 "gram.c"
    break;

  case 433: /* copy_opt_item: ESCAPE opt_as Sconst  */
#line 3514 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("escape", (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 34276 "gram.c"
    break;

  case 434: /* copy_opt_item: FORCE QUOTE columnList  */
#line 3518 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_quote", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 34284 "gram.c"
    break;

  case 435: /* copy_opt_item: FORCE QUOTE '*'  */
#line 3522 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_quote", (Node *) makeNode(A_Star), (yylsp[-2]));
                }
#line 34292 "gram.c"
    break;

  case 436: /* copy_opt_item: FORCE NOT NULL_P columnList  */
#line 3526 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_not_null", (Node *) (yyvsp[0].list), (yylsp[-3]));
                }
#line 34300 "gram.c"
    break;

  case 437: /* copy_opt_item: FORCE NOT NULL_P '*'  */
#line 3530 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_not_null", (Node *) makeNode(A_Star), (yylsp[-3]));
                }
#line 34308 "gram.c"
    break;

  case 438: /* copy_opt_item: FORCE NULL_P columnList  */
#line 3534 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_null", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 34316 "gram.c"
    break;

  case 439: /* copy_opt_item: FORCE NULL_P '*'  */
#line 3538 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force_null", (Node *) makeNode(A_Star), (yylsp[-2]));
                }
#line 34324 "gram.c"
    break;

  case 440: /* copy_opt_item: ENCODING Sconst  */
#line 3542 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("encoding", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 34332 "gram.c"
    break;

  case 441: /* opt_binary: BINARY  */
#line 3551 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("format", (Node *) makeString("binary"), (yylsp[0]));
                }
#line 34340 "gram.c"
    break;

  case 442: /* opt_binary: %empty  */
#line 3554 "gram.y"
                                                                                                { (yyval.defelt) = NULL; }
#line 34346 "gram.c"
    break;

  case 443: /* copy_delimiter: opt_using DELIMITERS Sconst  */
#line 3559 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("delimiter", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 34354 "gram.c"
    break;

  case 444: /* copy_delimiter: %empty  */
#line 3562 "gram.y"
                                                                                                { (yyval.defelt) = NULL; }
#line 34360 "gram.c"
    break;

  case 447: /* copy_generic_opt_list: copy_generic_opt_elem  */
#line 3573 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].defelt));
                }
#line 34368 "gram.c"
    break;

  case 448: /* copy_generic_opt_list: copy_generic_opt_list ',' copy_generic_opt_elem  */
#line 3577 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt));
                }
#line 34376 "gram.c"
    break;

  case 449: /* copy_generic_opt_elem: ColLabel copy_generic_opt_arg  */
#line 3584 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-1].str), (yyvsp[0].node), (yylsp[-1]));
                }
#line 34384 "gram.c"
    break;

  case 450: /* copy_generic_opt_arg: opt_boolean_or_string  */
#line 3590 "gram.y"
                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 34390 "gram.c"
    break;

  case 451: /* copy_generic_opt_arg: NumericOnly  */
#line 3591 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].node); }
#line 34396 "gram.c"
    break;

  case 452: /* copy_generic_opt_arg: '*'  */
#line 3592 "gram.y"
                                                                                { (yyval.node) = (Node *) makeNode(A_Star); }
#line 34402 "gram.c"
    break;

  case 453: /* copy_generic_opt_arg: DEFAULT  */
#line 3593 "gram.y"
                                                        { (yyval.node) = (Node *) makeString("default"); }
#line 34408 "gram.c"
    break;

  case 454: /* copy_generic_opt_arg: '(' copy_generic_opt_arg_list ')'  */
#line 3594 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[-1].list); }
#line 34414 "gram.c"
    break;

  case 455: /* copy_generic_opt_arg: %empty  */
#line 3595 "gram.y"
                                                                        { (yyval.node) = NULL; }
#line 34420 "gram.c"
    break;

  case 456: /* copy_generic_opt_arg_list: copy_generic_opt_arg_list_item  */
#line 3600 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 34428 "gram.c"
    break;

  case 457: /* copy_generic_opt_arg_list: copy_generic_opt_arg_list ',' copy_generic_opt_arg_list_item  */
#line 3604 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 34436 "gram.c"
    break;

  case 458: /* copy_generic_opt_arg_list_item: opt_boolean_or_string  */
#line 3611 "gram.y"
                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 34442 "gram.c"
    break;

  case 459: /* CreateStmt: CREATE OptTemp TABLE qualified_name '(' OptTableElementList ')' OptInherit OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3625 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-9].range)->relpersistence = (yyvsp[-11].ival);
                    n->relation = (yyvsp[-9].range);
                    n->tableElts = (yyvsp[-7].list);
                    n->inhRelations = (yyvsp[-5].list);
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = NULL;
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 34464 "gram.c"
    break;

  case 460: /* CreateStmt: CREATE OptTemp TABLE IF_P NOT EXISTS qualified_name '(' OptTableElementList ')' OptInherit OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3645 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-9].range)->relpersistence = (yyvsp[-14].ival);
                    n->relation = (yyvsp[-9].range);
                    n->tableElts = (yyvsp[-7].list);
                    n->inhRelations = (yyvsp[-5].list);
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = NULL;
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 34486 "gram.c"
    break;

  case 461: /* CreateStmt: CREATE OptTemp TABLE qualified_name OF any_name OptTypedTableElementList OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3665 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-8].range)->relpersistence = (yyvsp[-10].ival);
                    n->relation = (yyvsp[-8].range);
                    n->tableElts = (yyvsp[-5].list);
                    n->inhRelations = NIL;
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = makeTypeNameFromNameList((yyvsp[-6].list));
                    n->ofTypename->location = (yylsp[-6]);
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 34509 "gram.c"
    break;

  case 462: /* CreateStmt: CREATE OptTemp TABLE IF_P NOT EXISTS qualified_name OF any_name OptTypedTableElementList OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3686 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-8].range)->relpersistence = (yyvsp[-13].ival);
                    n->relation = (yyvsp[-8].range);
                    n->tableElts = (yyvsp[-5].list);
                    n->inhRelations = NIL;
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = makeTypeNameFromNameList((yyvsp[-6].list));
                    n->ofTypename->location = (yylsp[-6]);
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 34532 "gram.c"
    break;

  case 463: /* CreateStmt: CREATE OptTemp TABLE qualified_name PARTITION OF qualified_name OptTypedTableElementList PartitionBoundSpec OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3707 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-10].range)->relpersistence = (yyvsp[-12].ival);
                    n->relation = (yyvsp[-10].range);
                    n->tableElts = (yyvsp[-6].list);
                    n->inhRelations = list_make1((yyvsp[-7].range));
                    n->partbound = (yyvsp[-5].partboundspec);
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = NULL;
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 34555 "gram.c"
    break;

  case 464: /* CreateStmt: CREATE OptTemp TABLE IF_P NOT EXISTS qualified_name PARTITION OF qualified_name OptTypedTableElementList PartitionBoundSpec OptPartitionSpec table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 3728 "gram.y"
                                {
                    CreateStmt *n = makeNode(CreateStmt);

                    (yyvsp[-10].range)->relpersistence = (yyvsp[-15].ival);
                    n->relation = (yyvsp[-10].range);
                    n->tableElts = (yyvsp[-6].list);
                    n->inhRelations = list_make1((yyvsp[-7].range));
                    n->partbound = (yyvsp[-5].partboundspec);
                    n->partspec = (yyvsp[-4].partspec);
                    n->ofTypename = NULL;
                    n->constraints = NIL;
                    n->accessMethod = (yyvsp[-3].str);
                    n->options = (yyvsp[-2].list);
                    n->oncommit = (yyvsp[-1].oncommit);
                    n->tablespacename = (yyvsp[0].str);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 34578 "gram.c"
    break;

  case 465: /* OptTemp: TEMPORARY  */
#line 3759 "gram.y"
                                                                { (yyval.ival) = RELPERSISTENCE_TEMP; }
#line 34584 "gram.c"
    break;

  case 466: /* OptTemp: TEMP  */
#line 3760 "gram.y"
                                                                        { (yyval.ival) = RELPERSISTENCE_TEMP; }
#line 34590 "gram.c"
    break;

  case 467: /* OptTemp: LOCAL TEMPORARY  */
#line 3761 "gram.y"
                                                                { (yyval.ival) = RELPERSISTENCE_TEMP; }
#line 34596 "gram.c"
    break;

  case 468: /* OptTemp: LOCAL TEMP  */
#line 3762 "gram.y"
                                                                { (yyval.ival) = RELPERSISTENCE_TEMP; }
#line 34602 "gram.c"
    break;

  case 469: /* OptTemp: GLOBAL TEMPORARY  */
#line 3764 "gram.y"
                                {
                    ereport(WARNING,
                            (errmsg("GLOBAL is deprecated in temporary table creation"),
                             parser_errposition((yylsp[-1]))));
                    (yyval.ival) = RELPERSISTENCE_TEMP;
                }
#line 34613 "gram.c"
    break;

  case 470: /* OptTemp: GLOBAL TEMP  */
#line 3771 "gram.y"
                                {
                    ereport(WARNING,
                            (errmsg("GLOBAL is deprecated in temporary table creation"),
                             parser_errposition((yylsp[-1]))));
                    (yyval.ival) = RELPERSISTENCE_TEMP;
                }
#line 34624 "gram.c"
    break;

  case 471: /* OptTemp: UNLOGGED  */
#line 3777 "gram.y"
                                                                        { (yyval.ival) = RELPERSISTENCE_UNLOGGED; }
#line 34630 "gram.c"
    break;

  case 472: /* OptTemp: %empty  */
#line 3778 "gram.y"
                                                                        { (yyval.ival) = RELPERSISTENCE_PERMANENT; }
#line 34636 "gram.c"
    break;

  case 473: /* OptTableElementList: TableElementList  */
#line 3782 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 34642 "gram.c"
    break;

  case 474: /* OptTableElementList: %empty  */
#line 3783 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 34648 "gram.c"
    break;

  case 475: /* OptTypedTableElementList: '(' TypedTableElementList ')'  */
#line 3787 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 34654 "gram.c"
    break;

  case 476: /* OptTypedTableElementList: %empty  */
#line 3788 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 34660 "gram.c"
    break;

  case 477: /* TableElementList: TableElement  */
#line 3793 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 34668 "gram.c"
    break;

  case 478: /* TableElementList: TableElementList ',' TableElement  */
#line 3797 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 34676 "gram.c"
    break;

  case 479: /* TypedTableElementList: TypedTableElement  */
#line 3804 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 34684 "gram.c"
    break;

  case 480: /* TypedTableElementList: TypedTableElementList ',' TypedTableElement  */
#line 3808 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 34692 "gram.c"
    break;

  case 481: /* TableElement: columnDef  */
#line 3814 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 34698 "gram.c"
    break;

  case 482: /* TableElement: TableLikeClause  */
#line 3815 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 34704 "gram.c"
    break;

  case 483: /* TableElement: TableConstraint  */
#line 3816 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 34710 "gram.c"
    break;

  case 484: /* TypedTableElement: columnOptions  */
#line 3820 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 34716 "gram.c"
    break;

  case 485: /* TypedTableElement: TableConstraint  */
#line 3821 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 34722 "gram.c"
    break;

  case 486: /* columnDef: ColId Typename opt_column_storage opt_column_compression create_generic_options ColQualList  */
#line 3825 "gram.y"
                                {
                    ColumnDef *n = makeNode(ColumnDef);

                    n->colname = (yyvsp[-5].str);
                    n->typeName = (yyvsp[-4].typnam);
                    n->storage_name = (yyvsp[-3].str);
                    n->compression = (yyvsp[-2].str);
                    n->inhcount = 0;
                    n->is_local = true;
                    n->is_not_null = false;
                    n->is_from_type = false;
                    n->storage = 0;
                    n->raw_default = NULL;
                    n->cooked_default = NULL;
                    n->collOid = InvalidOid;
                    n->fdwoptions = (yyvsp[-1].list);
                    SplitColQualList((yyvsp[0].list), &n->constraints, &n->collClause,
                                     yyscanner);
                    n->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 34748 "gram.c"
    break;

  case 487: /* columnOptions: ColId ColQualList  */
#line 3849 "gram.y"
                                {
                    ColumnDef *n = makeNode(ColumnDef);

                    n->colname = (yyvsp[-1].str);
                    n->typeName = NULL;
                    n->inhcount = 0;
                    n->is_local = true;
                    n->is_not_null = false;
                    n->is_from_type = false;
                    n->storage = 0;
                    n->raw_default = NULL;
                    n->cooked_default = NULL;
                    n->collOid = InvalidOid;
                    SplitColQualList((yyvsp[0].list), &n->constraints, &n->collClause,
                                     yyscanner);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 34771 "gram.c"
    break;

  case 488: /* columnOptions: ColId WITH OPTIONS ColQualList  */
#line 3868 "gram.y"
                                {
                    ColumnDef *n = makeNode(ColumnDef);

                    n->colname = (yyvsp[-3].str);
                    n->typeName = NULL;
                    n->inhcount = 0;
                    n->is_local = true;
                    n->is_not_null = false;
                    n->is_from_type = false;
                    n->storage = 0;
                    n->raw_default = NULL;
                    n->cooked_default = NULL;
                    n->collOid = InvalidOid;
                    SplitColQualList((yyvsp[0].list), &n->constraints, &n->collClause,
                                     yyscanner);
                    n->location = (yylsp[-3]);
                    (yyval.node) = (Node *) n;
                }
#line 34794 "gram.c"
    break;

  case 489: /* column_compression: COMPRESSION ColId  */
#line 3889 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 34800 "gram.c"
    break;

  case 490: /* column_compression: COMPRESSION DEFAULT  */
#line 3890 "gram.y"
                                                                                { (yyval.str) = pstrdup("default"); }
#line 34806 "gram.c"
    break;

  case 491: /* opt_column_compression: column_compression  */
#line 3894 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 34812 "gram.c"
    break;

  case 492: /* opt_column_compression: %empty  */
#line 3895 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 34818 "gram.c"
    break;

  case 493: /* column_storage: STORAGE ColId  */
#line 3899 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 34824 "gram.c"
    break;

  case 494: /* column_storage: STORAGE DEFAULT  */
#line 3900 "gram.y"
                                                                                        { (yyval.str) = pstrdup("default"); }
#line 34830 "gram.c"
    break;

  case 495: /* opt_column_storage: column_storage  */
#line 3904 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 34836 "gram.c"
    break;

  case 496: /* opt_column_storage: %empty  */
#line 3905 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 34842 "gram.c"
    break;

  case 497: /* ColQualList: ColQualList ColConstraint  */
#line 3909 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 34848 "gram.c"
    break;

  case 498: /* ColQualList: %empty  */
#line 3910 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 34854 "gram.c"
    break;

  case 499: /* ColConstraint: CONSTRAINT name ColConstraintElem  */
#line 3915 "gram.y"
                                {
                    Constraint *n = castNode(Constraint, (yyvsp[0].node));

                    n->conname = (yyvsp[-1].str);
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 34866 "gram.c"
    break;

  case 500: /* ColConstraint: ColConstraintElem  */
#line 3922 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 34872 "gram.c"
    break;

  case 501: /* ColConstraint: ConstraintAttr  */
#line 3923 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 34878 "gram.c"
    break;

  case 502: /* ColConstraint: COLLATE any_name  */
#line 3925 "gram.y"
                                {
                    /*
                     * Note: the CollateClause is momentarily included in
                     * the list built by ColQualList, but we split it out
                     * again in SplitColQualList.
                     */
                    CollateClause *n = makeNode(CollateClause);

                    n->arg = NULL;
                    n->collname = (yyvsp[0].list);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 34896 "gram.c"
    break;

  case 503: /* ColConstraintElem: NOT NULL_P opt_no_inherit  */
#line 3957 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_NOTNULL;
                    n->location = (yylsp[-2]);
                    n->is_no_inherit = (yyvsp[0].boolean);
                    n->skip_validation = false;
                    n->initially_valid = true;
                    (yyval.node) = (Node *) n;
                }
#line 34911 "gram.c"
    break;

  case 504: /* ColConstraintElem: NULL_P  */
#line 3968 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_NULL;
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 34923 "gram.c"
    break;

  case 505: /* ColConstraintElem: UNIQUE opt_unique_null_treatment opt_definition OptConsTableSpace  */
#line 3976 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_UNIQUE;
                    n->location = (yylsp[-3]);
                    n->nulls_not_distinct = !(yyvsp[-2].boolean);
                    n->keys = NULL;
                    n->options = (yyvsp[-1].list);
                    n->indexname = NULL;
                    n->indexspace = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 34940 "gram.c"
    break;

  case 506: /* ColConstraintElem: PRIMARY KEY opt_definition OptConsTableSpace  */
#line 3989 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_PRIMARY;
                    n->location = (yylsp[-3]);
                    n->keys = NULL;
                    n->options = (yyvsp[-1].list);
                    n->indexname = NULL;
                    n->indexspace = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 34956 "gram.c"
    break;

  case 507: /* ColConstraintElem: CHECK '(' a_expr ')' opt_no_inherit  */
#line 4001 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_CHECK;
                    n->location = (yylsp[-4]);
                    n->is_no_inherit = (yyvsp[0].boolean);
                    n->raw_expr = (yyvsp[-2].node);
                    n->cooked_expr = NULL;
                    n->skip_validation = false;
                    n->initially_valid = true;
                    (yyval.node) = (Node *) n;
                }
#line 34973 "gram.c"
    break;

  case 508: /* ColConstraintElem: DEFAULT b_expr  */
#line 4014 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_DEFAULT;
                    n->location = (yylsp[-1]);
                    n->raw_expr = (yyvsp[0].node);
                    n->cooked_expr = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 34987 "gram.c"
    break;

  case 509: /* ColConstraintElem: GENERATED generated_when AS IDENTITY_P OptParenthesizedSeqOptList  */
#line 4024 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_IDENTITY;
                    n->generated_when = (yyvsp[-3].ival);
                    n->options = (yyvsp[0].list);
                    n->location = (yylsp[-4]);
                    (yyval.node) = (Node *) n;
                }
#line 35001 "gram.c"
    break;

  case 510: /* ColConstraintElem: GENERATED generated_when AS '(' a_expr ')' STORED  */
#line 4034 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_GENERATED;
                    n->generated_when = (yyvsp[-5].ival);
                    n->raw_expr = (yyvsp[-2].node);
                    n->cooked_expr = NULL;
                    n->location = (yylsp[-6]);

                    /*
                     * Can't do this in the grammar because of shift/reduce
                     * conflicts.  (IDENTITY allows both ALWAYS and BY
                     * DEFAULT, but generated columns only allow ALWAYS.)  We
                     * can also give a more useful error message and location.
                     */
                    if ((yyvsp[-5].ival) != ATTRIBUTE_IDENTITY_ALWAYS)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("for a generated column, GENERATED ALWAYS must be specified"),
                                 parser_errposition((yylsp[-5]))));

                    (yyval.node) = (Node *) n;
                }
#line 35029 "gram.c"
    break;

  case 511: /* ColConstraintElem: REFERENCES qualified_name opt_column_list key_match key_actions  */
#line 4058 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_FOREIGN;
                    n->location = (yylsp[-4]);
                    n->pktable = (yyvsp[-3].range);
                    n->fk_attrs = NIL;
                    n->pk_attrs = (yyvsp[-2].list);
                    n->fk_matchtype = (yyvsp[-1].ival);
                    n->fk_upd_action = ((yyvsp[0].keyactions))->updateAction->action;
                    n->fk_del_action = ((yyvsp[0].keyactions))->deleteAction->action;
                    n->fk_del_set_cols = ((yyvsp[0].keyactions))->deleteAction->cols;
                    n->skip_validation = false;
                    n->initially_valid = true;
                    (yyval.node) = (Node *) n;
                }
#line 35050 "gram.c"
    break;

  case 512: /* opt_unique_null_treatment: NULLS_P DISTINCT  */
#line 4077 "gram.y"
                                                        { (yyval.boolean) = true; }
#line 35056 "gram.c"
    break;

  case 513: /* opt_unique_null_treatment: NULLS_P NOT DISTINCT  */
#line 4078 "gram.y"
                                                { (yyval.boolean) = false; }
#line 35062 "gram.c"
    break;

  case 514: /* opt_unique_null_treatment: %empty  */
#line 4079 "gram.y"
                                                                { (yyval.boolean) = true; }
#line 35068 "gram.c"
    break;

  case 515: /* generated_when: ALWAYS  */
#line 4083 "gram.y"
                                                { (yyval.ival) = ATTRIBUTE_IDENTITY_ALWAYS; }
#line 35074 "gram.c"
    break;

  case 516: /* generated_when: BY DEFAULT  */
#line 4084 "gram.y"
                                        { (yyval.ival) = ATTRIBUTE_IDENTITY_BY_DEFAULT; }
#line 35080 "gram.c"
    break;

  case 517: /* ConstraintAttr: DEFERRABLE  */
#line 4104 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_ATTR_DEFERRABLE;
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 35092 "gram.c"
    break;

  case 518: /* ConstraintAttr: NOT DEFERRABLE  */
#line 4112 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_ATTR_NOT_DEFERRABLE;
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 35104 "gram.c"
    break;

  case 519: /* ConstraintAttr: INITIALLY DEFERRED  */
#line 4120 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_ATTR_DEFERRED;
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 35116 "gram.c"
    break;

  case 520: /* ConstraintAttr: INITIALLY IMMEDIATE  */
#line 4128 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_ATTR_IMMEDIATE;
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 35128 "gram.c"
    break;

  case 521: /* TableLikeClause: LIKE qualified_name TableLikeOptionList  */
#line 4140 "gram.y"
                                {
                    TableLikeClause *n = makeNode(TableLikeClause);

                    n->relation = (yyvsp[-1].range);
                    n->options = (yyvsp[0].ival);
                    n->relationOid = InvalidOid;
                    (yyval.node) = (Node *) n;
                }
#line 35141 "gram.c"
    break;

  case 522: /* TableLikeOptionList: TableLikeOptionList INCLUDING TableLikeOption  */
#line 4151 "gram.y"
                                                                                { (yyval.ival) = (yyvsp[-2].ival) | (yyvsp[0].ival); }
#line 35147 "gram.c"
    break;

  case 523: /* TableLikeOptionList: TableLikeOptionList EXCLUDING TableLikeOption  */
#line 4152 "gram.y"
                                                                                { (yyval.ival) = (yyvsp[-2].ival) & ~(yyvsp[0].ival); }
#line 35153 "gram.c"
    break;

  case 524: /* TableLikeOptionList: %empty  */
#line 4153 "gram.y"
                                                                                        { (yyval.ival) = 0; }
#line 35159 "gram.c"
    break;

  case 525: /* TableLikeOption: COMMENTS  */
#line 4157 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_COMMENTS; }
#line 35165 "gram.c"
    break;

  case 526: /* TableLikeOption: COMPRESSION  */
#line 4158 "gram.y"
                                                        { (yyval.ival) = CREATE_TABLE_LIKE_COMPRESSION; }
#line 35171 "gram.c"
    break;

  case 527: /* TableLikeOption: CONSTRAINTS  */
#line 4159 "gram.y"
                                                        { (yyval.ival) = CREATE_TABLE_LIKE_CONSTRAINTS; }
#line 35177 "gram.c"
    break;

  case 528: /* TableLikeOption: DEFAULTS  */
#line 4160 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_DEFAULTS; }
#line 35183 "gram.c"
    break;

  case 529: /* TableLikeOption: IDENTITY_P  */
#line 4161 "gram.y"
                                                        { (yyval.ival) = CREATE_TABLE_LIKE_IDENTITY; }
#line 35189 "gram.c"
    break;

  case 530: /* TableLikeOption: GENERATED  */
#line 4162 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_GENERATED; }
#line 35195 "gram.c"
    break;

  case 531: /* TableLikeOption: INDEXES  */
#line 4163 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_INDEXES; }
#line 35201 "gram.c"
    break;

  case 532: /* TableLikeOption: STATISTICS  */
#line 4164 "gram.y"
                                                        { (yyval.ival) = CREATE_TABLE_LIKE_STATISTICS; }
#line 35207 "gram.c"
    break;

  case 533: /* TableLikeOption: STORAGE  */
#line 4165 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_STORAGE; }
#line 35213 "gram.c"
    break;

  case 534: /* TableLikeOption: ALL  */
#line 4166 "gram.y"
                                                                { (yyval.ival) = CREATE_TABLE_LIKE_ALL; }
#line 35219 "gram.c"
    break;

  case 535: /* TableConstraint: CONSTRAINT name ConstraintElem  */
#line 4176 "gram.y"
                                {
                    Constraint *n = castNode(Constraint, (yyvsp[0].node));

                    n->conname = (yyvsp[-1].str);
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 35231 "gram.c"
    break;

  case 536: /* TableConstraint: ConstraintElem  */
#line 4183 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 35237 "gram.c"
    break;

  case 537: /* ConstraintElem: CHECK '(' a_expr ')' ConstraintAttributeSpec  */
#line 4188 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_CHECK;
                    n->location = (yylsp[-4]);
                    n->raw_expr = (yyvsp[-2].node);
                    n->cooked_expr = NULL;
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "CHECK",
                                   NULL, NULL, &n->skip_validation,
                                   &n->is_no_inherit, yyscanner);
                    n->initially_valid = !n->skip_validation;
                    (yyval.node) = (Node *) n;
                }
#line 35255 "gram.c"
    break;

  case 538: /* ConstraintElem: NOT NULL_P ColId ConstraintAttributeSpec  */
#line 4202 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_NOTNULL;
                    n->location = (yylsp[-3]);
                    n->keys = list_make1(makeString((yyvsp[-1].str)));
                    /* no NOT VALID support yet */
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "NOT NULL",
                                   NULL, NULL, NULL,
                                   &n->is_no_inherit, yyscanner);
                    n->initially_valid = true;
                    (yyval.node) = (Node *) n;
                }
#line 35273 "gram.c"
    break;

  case 539: /* ConstraintElem: UNIQUE opt_unique_null_treatment '(' columnList opt_without_overlaps ')' opt_c_include opt_definition OptConsTableSpace ConstraintAttributeSpec  */
#line 4217 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_UNIQUE;
                    n->location = (yylsp[-9]);
                    n->nulls_not_distinct = !(yyvsp[-8].boolean);
                    n->keys = (yyvsp[-6].list);
                    n->without_overlaps = (yyvsp[-5].boolean);
                    n->including = (yyvsp[-3].list);
                    n->options = (yyvsp[-2].list);
                    n->indexname = NULL;
                    n->indexspace = (yyvsp[-1].str);
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "UNIQUE",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 35295 "gram.c"
    break;

  case 540: /* ConstraintElem: UNIQUE ExistingIndex ConstraintAttributeSpec  */
#line 4235 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_UNIQUE;
                    n->location = (yylsp[-2]);
                    n->keys = NIL;
                    n->including = NIL;
                    n->options = NIL;
                    n->indexname = (yyvsp[-1].str);
                    n->indexspace = NULL;
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "UNIQUE",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 35315 "gram.c"
    break;

  case 541: /* ConstraintElem: PRIMARY KEY '(' columnList opt_without_overlaps ')' opt_c_include opt_definition OptConsTableSpace ConstraintAttributeSpec  */
#line 4252 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_PRIMARY;
                    n->location = (yylsp[-9]);
                    n->keys = (yyvsp[-6].list);
                    n->without_overlaps = (yyvsp[-5].boolean);
                    n->including = (yyvsp[-3].list);
                    n->options = (yyvsp[-2].list);
                    n->indexname = NULL;
                    n->indexspace = (yyvsp[-1].str);
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "PRIMARY KEY",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 35336 "gram.c"
    break;

  case 542: /* ConstraintElem: PRIMARY KEY ExistingIndex ConstraintAttributeSpec  */
#line 4269 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_PRIMARY;
                    n->location = (yylsp[-3]);
                    n->keys = NIL;
                    n->including = NIL;
                    n->options = NIL;
                    n->indexname = (yyvsp[-1].str);
                    n->indexspace = NULL;
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "PRIMARY KEY",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 35356 "gram.c"
    break;

  case 543: /* ConstraintElem: EXCLUDE access_method_clause '(' ExclusionConstraintList ')' opt_c_include opt_definition OptConsTableSpace OptWhereClause ConstraintAttributeSpec  */
#line 4287 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_EXCLUSION;
                    n->location = (yylsp[-9]);
                    n->access_method = (yyvsp[-8].str);
                    n->exclusions = (yyvsp[-6].list);
                    n->including = (yyvsp[-4].list);
                    n->options = (yyvsp[-3].list);
                    n->indexname = NULL;
                    n->indexspace = (yyvsp[-2].str);
                    n->where_clause = (yyvsp[-1].node);
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "EXCLUDE",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 35378 "gram.c"
    break;

  case 544: /* ConstraintElem: FOREIGN KEY '(' columnList optionalPeriodName ')' REFERENCES qualified_name opt_column_and_period_list key_match key_actions ConstraintAttributeSpec  */
#line 4306 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_FOREIGN;
                    n->location = (yylsp[-11]);
                    n->pktable = (yyvsp[-4].range);
                    n->fk_attrs = (yyvsp[-8].list);
                    if ((yyvsp[-7].node))
                    {
                        n->fk_attrs = lappend(n->fk_attrs, (yyvsp[-7].node));
                        n->fk_with_period = true;
                    }
                    n->pk_attrs = linitial((yyvsp[-3].list));
                    if (lsecond((yyvsp[-3].list)))
                    {
                        n->pk_attrs = lappend(n->pk_attrs, lsecond((yyvsp[-3].list)));
                        n->pk_with_period = true;
                    }
                    n->fk_matchtype = (yyvsp[-2].ival);
                    n->fk_upd_action = ((yyvsp[-1].keyactions))->updateAction->action;
                    n->fk_del_action = ((yyvsp[-1].keyactions))->deleteAction->action;
                    n->fk_del_set_cols = ((yyvsp[-1].keyactions))->deleteAction->cols;
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "FOREIGN KEY",
                                   &n->deferrable, &n->initdeferred,
                                   &n->skip_validation, NULL,
                                   yyscanner);
                    n->initially_valid = !n->skip_validation;
                    (yyval.node) = (Node *) n;
                }
#line 35412 "gram.c"
    break;

  case 545: /* DomainConstraint: CONSTRAINT name DomainConstraintElem  */
#line 4350 "gram.y"
                                {
                    Constraint *n = castNode(Constraint, (yyvsp[0].node));

                    n->conname = (yyvsp[-1].str);
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 35424 "gram.c"
    break;

  case 546: /* DomainConstraint: DomainConstraintElem  */
#line 4357 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 35430 "gram.c"
    break;

  case 547: /* DomainConstraintElem: CHECK '(' a_expr ')' ConstraintAttributeSpec  */
#line 4362 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_CHECK;
                    n->location = (yylsp[-4]);
                    n->raw_expr = (yyvsp[-2].node);
                    n->cooked_expr = NULL;
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "CHECK",
                                   NULL, NULL, &n->skip_validation,
                                   &n->is_no_inherit, yyscanner);
                    n->initially_valid = !n->skip_validation;
                    (yyval.node) = (Node *) n;
                }
#line 35448 "gram.c"
    break;

  case 548: /* DomainConstraintElem: NOT NULL_P ConstraintAttributeSpec  */
#line 4376 "gram.y"
                                {
                    Constraint *n = makeNode(Constraint);

                    n->contype = CONSTR_NOTNULL;
                    n->location = (yylsp[-2]);
                    n->keys = list_make1(makeString("value"));
                    /* no NOT VALID support yet */
                    processCASbits((yyvsp[0].ival), (yylsp[0]), "NOT NULL",
                                   NULL, NULL, NULL,
                                   &n->is_no_inherit, yyscanner);
                    n->initially_valid = true;
                    (yyval.node) = (Node *) n;
                }
#line 35466 "gram.c"
    break;

  case 549: /* opt_no_inherit: NO INHERIT  */
#line 4391 "gram.y"
                                                                                {  (yyval.boolean) = true; }
#line 35472 "gram.c"
    break;

  case 550: /* opt_no_inherit: %empty  */
#line 4392 "gram.y"
                                                                                        {  (yyval.boolean) = false; }
#line 35478 "gram.c"
    break;

  case 551: /* opt_without_overlaps: WITHOUT OVERLAPS  */
#line 4396 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 35484 "gram.c"
    break;

  case 552: /* opt_without_overlaps: %empty  */
#line 4397 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 35490 "gram.c"
    break;

  case 553: /* opt_column_list: '(' columnList ')'  */
#line 4401 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 35496 "gram.c"
    break;

  case 554: /* opt_column_list: %empty  */
#line 4402 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 35502 "gram.c"
    break;

  case 555: /* columnList: columnElem  */
#line 4406 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 35508 "gram.c"
    break;

  case 556: /* columnList: columnList ',' columnElem  */
#line 4407 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 35514 "gram.c"
    break;

  case 557: /* optionalPeriodName: ',' PERIOD columnElem  */
#line 4411 "gram.y"
                                              { (yyval.node) = (yyvsp[0].node); }
#line 35520 "gram.c"
    break;

  case 558: /* optionalPeriodName: %empty  */
#line 4412 "gram.y"
                                                  { (yyval.node) = NULL; }
#line 35526 "gram.c"
    break;

  case 559: /* opt_column_and_period_list: '(' columnList optionalPeriodName ')'  */
#line 4416 "gram.y"
                                                                                { (yyval.list) = list_make2((yyvsp[-2].list), (yyvsp[-1].node)); }
#line 35532 "gram.c"
    break;

  case 560: /* opt_column_and_period_list: %empty  */
#line 4417 "gram.y"
                                                                                                { (yyval.list) = list_make2(NIL, NULL); }
#line 35538 "gram.c"
    break;

  case 561: /* columnElem: ColId  */
#line 4421 "gram.y"
                                {
                    (yyval.node) = (Node *) makeString((yyvsp[0].str));
                }
#line 35546 "gram.c"
    break;

  case 562: /* opt_c_include: INCLUDE '(' columnList ')'  */
#line 4426 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 35552 "gram.c"
    break;

  case 563: /* opt_c_include: %empty  */
#line 4427 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 35558 "gram.c"
    break;

  case 564: /* key_match: MATCH FULL  */
#line 4431 "gram.y"
                        {
                (yyval.ival) = FKCONSTR_MATCH_FULL;
            }
#line 35566 "gram.c"
    break;

  case 565: /* key_match: MATCH PARTIAL  */
#line 4435 "gram.y"
                        {
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("MATCH PARTIAL not yet implemented"),
                         parser_errposition((yylsp[-1]))));
                (yyval.ival) = FKCONSTR_MATCH_PARTIAL;
            }
#line 35578 "gram.c"
    break;

  case 566: /* key_match: MATCH SIMPLE  */
#line 4443 "gram.y"
                        {
                (yyval.ival) = FKCONSTR_MATCH_SIMPLE;
            }
#line 35586 "gram.c"
    break;

  case 567: /* key_match: %empty  */
#line 4447 "gram.y"
                        {
                (yyval.ival) = FKCONSTR_MATCH_SIMPLE;
            }
#line 35594 "gram.c"
    break;

  case 568: /* ExclusionConstraintList: ExclusionConstraintElem  */
#line 4453 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].list)); }
#line 35600 "gram.c"
    break;

  case 569: /* ExclusionConstraintList: ExclusionConstraintList ',' ExclusionConstraintElem  */
#line 4455 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].list)); }
#line 35606 "gram.c"
    break;

  case 570: /* ExclusionConstraintElem: index_elem WITH any_operator  */
#line 4459 "gram.y"
                        {
                (yyval.list) = list_make2((yyvsp[-2].ielem), (yyvsp[0].list));
            }
#line 35614 "gram.c"
    break;

  case 571: /* ExclusionConstraintElem: index_elem WITH OPERATOR '(' any_operator ')'  */
#line 4464 "gram.y"
                        {
                (yyval.list) = list_make2((yyvsp[-5].ielem), (yyvsp[-1].list));
            }
#line 35622 "gram.c"
    break;

  case 572: /* OptWhereClause: WHERE '(' a_expr ')'  */
#line 4470 "gram.y"
                                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 35628 "gram.c"
    break;

  case 573: /* OptWhereClause: %empty  */
#line 4471 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 35634 "gram.c"
    break;

  case 574: /* key_actions: key_update  */
#line 4476 "gram.y"
                                {
                    KeyActions *n = palloc(sizeof(KeyActions));

                    n->updateAction = (yyvsp[0].keyaction);
                    n->deleteAction = palloc(sizeof(KeyAction));
                    n->deleteAction->action = FKCONSTR_ACTION_NOACTION;
                    n->deleteAction->cols = NIL;
                    (yyval.keyactions) = n;
                }
#line 35648 "gram.c"
    break;

  case 575: /* key_actions: key_delete  */
#line 4486 "gram.y"
                                {
                    KeyActions *n = palloc(sizeof(KeyActions));

                    n->updateAction = palloc(sizeof(KeyAction));
                    n->updateAction->action = FKCONSTR_ACTION_NOACTION;
                    n->updateAction->cols = NIL;
                    n->deleteAction = (yyvsp[0].keyaction);
                    (yyval.keyactions) = n;
                }
#line 35662 "gram.c"
    break;

  case 576: /* key_actions: key_update key_delete  */
#line 4496 "gram.y"
                                {
                    KeyActions *n = palloc(sizeof(KeyActions));

                    n->updateAction = (yyvsp[-1].keyaction);
                    n->deleteAction = (yyvsp[0].keyaction);
                    (yyval.keyactions) = n;
                }
#line 35674 "gram.c"
    break;

  case 577: /* key_actions: key_delete key_update  */
#line 4504 "gram.y"
                                {
                    KeyActions *n = palloc(sizeof(KeyActions));

                    n->updateAction = (yyvsp[0].keyaction);
                    n->deleteAction = (yyvsp[-1].keyaction);
                    (yyval.keyactions) = n;
                }
#line 35686 "gram.c"
    break;

  case 578: /* key_actions: %empty  */
#line 4512 "gram.y"
                                {
                    KeyActions *n = palloc(sizeof(KeyActions));

                    n->updateAction = palloc(sizeof(KeyAction));
                    n->updateAction->action = FKCONSTR_ACTION_NOACTION;
                    n->updateAction->cols = NIL;
                    n->deleteAction = palloc(sizeof(KeyAction));
                    n->deleteAction->action = FKCONSTR_ACTION_NOACTION;
                    n->deleteAction->cols = NIL;
                    (yyval.keyactions) = n;
                }
#line 35702 "gram.c"
    break;

  case 579: /* key_update: ON UPDATE key_action  */
#line 4526 "gram.y"
                                {
                    if (((yyvsp[0].keyaction))->cols)
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("a column list with %s is only supported for ON DELETE actions",
                                        ((yyvsp[0].keyaction))->action == FKCONSTR_ACTION_SETNULL ? "SET NULL" : "SET DEFAULT"),
                                 parser_errposition((yylsp[-2]))));
                    (yyval.keyaction) = (yyvsp[0].keyaction);
                }
#line 35716 "gram.c"
    break;

  case 580: /* key_delete: ON DELETE_P key_action  */
#line 4538 "gram.y"
                                {
                    (yyval.keyaction) = (yyvsp[0].keyaction);
                }
#line 35724 "gram.c"
    break;

  case 581: /* key_action: NO ACTION  */
#line 4545 "gram.y"
                                {
                    KeyAction *n = palloc(sizeof(KeyAction));

                    n->action = FKCONSTR_ACTION_NOACTION;
                    n->cols = NIL;
                    (yyval.keyaction) = n;
                }
#line 35736 "gram.c"
    break;

  case 582: /* key_action: RESTRICT  */
#line 4553 "gram.y"
                                {
                    KeyAction *n = palloc(sizeof(KeyAction));

                    n->action = FKCONSTR_ACTION_RESTRICT;
                    n->cols = NIL;
                    (yyval.keyaction) = n;
                }
#line 35748 "gram.c"
    break;

  case 583: /* key_action: CASCADE  */
#line 4561 "gram.y"
                                {
                    KeyAction *n = palloc(sizeof(KeyAction));

                    n->action = FKCONSTR_ACTION_CASCADE;
                    n->cols = NIL;
                    (yyval.keyaction) = n;
                }
#line 35760 "gram.c"
    break;

  case 584: /* key_action: SET NULL_P opt_column_list  */
#line 4569 "gram.y"
                                {
                    KeyAction *n = palloc(sizeof(KeyAction));

                    n->action = FKCONSTR_ACTION_SETNULL;
                    n->cols = (yyvsp[0].list);
                    (yyval.keyaction) = n;
                }
#line 35772 "gram.c"
    break;

  case 585: /* key_action: SET DEFAULT opt_column_list  */
#line 4577 "gram.y"
                                {
                    KeyAction *n = palloc(sizeof(KeyAction));

                    n->action = FKCONSTR_ACTION_SETDEFAULT;
                    n->cols = (yyvsp[0].list);
                    (yyval.keyaction) = n;
                }
#line 35784 "gram.c"
    break;

  case 586: /* OptInherit: INHERITS '(' qualified_name_list ')'  */
#line 4586 "gram.y"
                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 35790 "gram.c"
    break;

  case 587: /* OptInherit: %empty  */
#line 4587 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 35796 "gram.c"
    break;

  case 588: /* OptPartitionSpec: PartitionSpec  */
#line 4591 "gram.y"
                                { (yyval.partspec) = (yyvsp[0].partspec); }
#line 35802 "gram.c"
    break;

  case 589: /* OptPartitionSpec: %empty  */
#line 4592 "gram.y"
                                                        { (yyval.partspec) = NULL; }
#line 35808 "gram.c"
    break;

  case 590: /* PartitionSpec: PARTITION BY ColId '(' part_params ')'  */
#line 4596 "gram.y"
                                {
                    PartitionSpec *n = makeNode(PartitionSpec);

                    n->strategy = parsePartitionStrategy((yyvsp[-3].str));
                    n->partParams = (yyvsp[-1].list);
                    n->location = (yylsp[-5]);

                    (yyval.partspec) = n;
                }
#line 35822 "gram.c"
    break;

  case 591: /* part_params: part_elem  */
#line 4607 "gram.y"
                                                                        { (yyval.list) = list_make1((yyvsp[0].partelem)); }
#line 35828 "gram.c"
    break;

  case 592: /* part_params: part_params ',' part_elem  */
#line 4608 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].partelem)); }
#line 35834 "gram.c"
    break;

  case 593: /* part_elem: ColId opt_collate opt_qualified_name  */
#line 4612 "gram.y"
                                {
                    PartitionElem *n = makeNode(PartitionElem);

                    n->name = (yyvsp[-2].str);
                    n->expr = NULL;
                    n->collation = (yyvsp[-1].list);
                    n->opclass = (yyvsp[0].list);
                    n->location = (yylsp[-2]);
                    (yyval.partelem) = n;
                }
#line 35849 "gram.c"
    break;

  case 594: /* part_elem: func_expr_windowless opt_collate opt_qualified_name  */
#line 4623 "gram.y"
                                {
                    PartitionElem *n = makeNode(PartitionElem);

                    n->name = NULL;
                    n->expr = (yyvsp[-2].node);
                    n->collation = (yyvsp[-1].list);
                    n->opclass = (yyvsp[0].list);
                    n->location = (yylsp[-2]);
                    (yyval.partelem) = n;
                }
#line 35864 "gram.c"
    break;

  case 595: /* part_elem: '(' a_expr ')' opt_collate opt_qualified_name  */
#line 4634 "gram.y"
                                {
                    PartitionElem *n = makeNode(PartitionElem);

                    n->name = NULL;
                    n->expr = (yyvsp[-3].node);
                    n->collation = (yyvsp[-1].list);
                    n->opclass = (yyvsp[0].list);
                    n->location = (yylsp[-4]);
                    (yyval.partelem) = n;
                }
#line 35879 "gram.c"
    break;

  case 596: /* table_access_method_clause: USING name  */
#line 4647 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 35885 "gram.c"
    break;

  case 597: /* table_access_method_clause: %empty  */
#line 4648 "gram.y"
                                                                                        { (yyval.str) = NULL; }
#line 35891 "gram.c"
    break;

  case 598: /* OptWith: WITH reloptions  */
#line 4653 "gram.y"
                                                                { (yyval.list) = (yyvsp[0].list); }
#line 35897 "gram.c"
    break;

  case 599: /* OptWith: WITHOUT OIDS  */
#line 4654 "gram.y"
                                                                { (yyval.list) = NIL; }
#line 35903 "gram.c"
    break;

  case 600: /* OptWith: %empty  */
#line 4655 "gram.y"
                                                                        { (yyval.list) = NIL; }
#line 35909 "gram.c"
    break;

  case 601: /* OnCommitOption: ON COMMIT DROP  */
#line 4658 "gram.y"
                                                        { (yyval.oncommit) = ONCOMMIT_DROP; }
#line 35915 "gram.c"
    break;

  case 602: /* OnCommitOption: ON COMMIT DELETE_P ROWS  */
#line 4659 "gram.y"
                                                                { (yyval.oncommit) = ONCOMMIT_DELETE_ROWS; }
#line 35921 "gram.c"
    break;

  case 603: /* OnCommitOption: ON COMMIT PRESERVE ROWS  */
#line 4660 "gram.y"
                                                                { (yyval.oncommit) = ONCOMMIT_PRESERVE_ROWS; }
#line 35927 "gram.c"
    break;

  case 604: /* OnCommitOption: %empty  */
#line 4661 "gram.y"
                                                                                { (yyval.oncommit) = ONCOMMIT_NOOP; }
#line 35933 "gram.c"
    break;

  case 605: /* OptTableSpace: TABLESPACE name  */
#line 4664 "gram.y"
                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 35939 "gram.c"
    break;

  case 606: /* OptTableSpace: %empty  */
#line 4665 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 35945 "gram.c"
    break;

  case 607: /* OptConsTableSpace: USING INDEX TABLESPACE name  */
#line 4668 "gram.y"
                                                        { (yyval.str) = (yyvsp[0].str); }
#line 35951 "gram.c"
    break;

  case 608: /* OptConsTableSpace: %empty  */
#line 4669 "gram.y"
                                                                                                { (yyval.str) = NULL; }
#line 35957 "gram.c"
    break;

  case 609: /* ExistingIndex: USING INDEX name  */
#line 4672 "gram.y"
                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 35963 "gram.c"
    break;

  case 610: /* CreateStatsStmt: CREATE STATISTICS opt_qualified_name opt_name_list ON stats_params FROM from_list  */
#line 4694 "gram.y"
                                {
                    CreateStatsStmt *n = makeNode(CreateStatsStmt);

                    n->defnames = (yyvsp[-5].list);
                    n->stat_types = (yyvsp[-4].list);
                    n->exprs = (yyvsp[-2].list);
                    n->relations = (yyvsp[0].list);
                    n->stxcomment = NULL;
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 35979 "gram.c"
    break;

  case 611: /* CreateStatsStmt: CREATE STATISTICS IF_P NOT EXISTS any_name opt_name_list ON stats_params FROM from_list  */
#line 4707 "gram.y"
                                {
                    CreateStatsStmt *n = makeNode(CreateStatsStmt);

                    n->defnames = (yyvsp[-5].list);
                    n->stat_types = (yyvsp[-4].list);
                    n->exprs = (yyvsp[-2].list);
                    n->relations = (yyvsp[0].list);
                    n->stxcomment = NULL;
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 35995 "gram.c"
    break;

  case 612: /* stats_params: stats_param  */
#line 4727 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].selem)); }
#line 36001 "gram.c"
    break;

  case 613: /* stats_params: stats_params ',' stats_param  */
#line 4728 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].selem)); }
#line 36007 "gram.c"
    break;

  case 614: /* stats_param: ColId  */
#line 4732 "gram.y"
                                {
                    (yyval.selem) = makeNode(StatsElem);
                    (yyval.selem)->name = (yyvsp[0].str);
                    (yyval.selem)->expr = NULL;
                }
#line 36017 "gram.c"
    break;

  case 615: /* stats_param: func_expr_windowless  */
#line 4738 "gram.y"
                                {
                    (yyval.selem) = makeNode(StatsElem);
                    (yyval.selem)->name = NULL;
                    (yyval.selem)->expr = (yyvsp[0].node);
                }
#line 36027 "gram.c"
    break;

  case 616: /* stats_param: '(' a_expr ')'  */
#line 4744 "gram.y"
                                {
                    (yyval.selem) = makeNode(StatsElem);
                    (yyval.selem)->name = NULL;
                    (yyval.selem)->expr = (yyvsp[-1].node);
                }
#line 36037 "gram.c"
    break;

  case 617: /* AlterStatsStmt: ALTER STATISTICS any_name SET STATISTICS set_statistics_value  */
#line 4761 "gram.y"
                                {
                    AlterStatsStmt *n = makeNode(AlterStatsStmt);

                    n->defnames = (yyvsp[-3].list);
                    n->missing_ok = false;
                    n->stxstattarget = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 36050 "gram.c"
    break;

  case 618: /* AlterStatsStmt: ALTER STATISTICS IF_P EXISTS any_name SET STATISTICS set_statistics_value  */
#line 4770 "gram.y"
                                {
                    AlterStatsStmt *n = makeNode(AlterStatsStmt);

                    n->defnames = (yyvsp[-3].list);
                    n->missing_ok = true;
                    n->stxstattarget = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 36063 "gram.c"
    break;

  case 619: /* CreateAsStmt: CREATE OptTemp TABLE create_as_target AS SelectStmt opt_with_data  */
#line 4792 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);

                    ctas->query = (yyvsp[-1].node);
                    ctas->into = (yyvsp[-3].into);
                    ctas->objtype = OBJECT_TABLE;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = false;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-3].into)->rel->relpersistence = (yyvsp[-5].ival);
                    (yyvsp[-3].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 36081 "gram.c"
    break;

  case 620: /* CreateAsStmt: CREATE OptTemp TABLE IF_P NOT EXISTS create_as_target AS SelectStmt opt_with_data  */
#line 4806 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);

                    ctas->query = (yyvsp[-1].node);
                    ctas->into = (yyvsp[-3].into);
                    ctas->objtype = OBJECT_TABLE;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = true;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-3].into)->rel->relpersistence = (yyvsp[-8].ival);
                    (yyvsp[-3].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 36099 "gram.c"
    break;

  case 621: /* create_as_target: qualified_name opt_column_list table_access_method_clause OptWith OnCommitOption OptTableSpace  */
#line 4824 "gram.y"
                                {
                    (yyval.into) = makeNode(IntoClause);
                    (yyval.into)->rel = (yyvsp[-5].range);
                    (yyval.into)->colNames = (yyvsp[-4].list);
                    (yyval.into)->accessMethod = (yyvsp[-3].str);
                    (yyval.into)->options = (yyvsp[-2].list);
                    (yyval.into)->onCommit = (yyvsp[-1].oncommit);
                    (yyval.into)->tableSpaceName = (yyvsp[0].str);
                    (yyval.into)->viewQuery = NULL;
                    (yyval.into)->skipData = false;     /* might get changed later */
                }
#line 36115 "gram.c"
    break;

  case 622: /* opt_with_data: WITH DATA_P  */
#line 4838 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 36121 "gram.c"
    break;

  case 623: /* opt_with_data: WITH NO DATA_P  */
#line 4839 "gram.y"
                                                                                        { (yyval.boolean) = false; }
#line 36127 "gram.c"
    break;

  case 624: /* opt_with_data: %empty  */
#line 4840 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 36133 "gram.c"
    break;

  case 625: /* CreateMatViewStmt: CREATE OptNoLog MATERIALIZED VIEW create_mv_target AS SelectStmt opt_with_data  */
#line 4853 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);

                    ctas->query = (yyvsp[-1].node);
                    ctas->into = (yyvsp[-3].into);
                    ctas->objtype = OBJECT_MATVIEW;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = false;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-3].into)->rel->relpersistence = (yyvsp[-6].ival);
                    (yyvsp[-3].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 36151 "gram.c"
    break;

  case 626: /* CreateMatViewStmt: CREATE OptNoLog MATERIALIZED VIEW IF_P NOT EXISTS create_mv_target AS SelectStmt opt_with_data  */
#line 4867 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);

                    ctas->query = (yyvsp[-1].node);
                    ctas->into = (yyvsp[-3].into);
                    ctas->objtype = OBJECT_MATVIEW;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = true;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-3].into)->rel->relpersistence = (yyvsp[-9].ival);
                    (yyvsp[-3].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 36169 "gram.c"
    break;

  case 627: /* create_mv_target: qualified_name opt_column_list table_access_method_clause opt_reloptions OptTableSpace  */
#line 4884 "gram.y"
                                {
                    (yyval.into) = makeNode(IntoClause);
                    (yyval.into)->rel = (yyvsp[-4].range);
                    (yyval.into)->colNames = (yyvsp[-3].list);
                    (yyval.into)->accessMethod = (yyvsp[-2].str);
                    (yyval.into)->options = (yyvsp[-1].list);
                    (yyval.into)->onCommit = ONCOMMIT_NOOP;
                    (yyval.into)->tableSpaceName = (yyvsp[0].str);
                    (yyval.into)->viewQuery = NULL;     /* filled at analysis time */
                    (yyval.into)->skipData = false;     /* might get changed later */
                }
#line 36185 "gram.c"
    break;

  case 628: /* OptNoLog: UNLOGGED  */
#line 4897 "gram.y"
                                                                { (yyval.ival) = RELPERSISTENCE_UNLOGGED; }
#line 36191 "gram.c"
    break;

  case 629: /* OptNoLog: %empty  */
#line 4898 "gram.y"
                                                                        { (yyval.ival) = RELPERSISTENCE_PERMANENT; }
#line 36197 "gram.c"
    break;

  case 630: /* RefreshMatViewStmt: REFRESH MATERIALIZED VIEW opt_concurrently qualified_name opt_with_data  */
#line 4911 "gram.y"
                                {
                    RefreshMatViewStmt *n = makeNode(RefreshMatViewStmt);

                    n->concurrent = (yyvsp[-2].boolean);
                    n->relation = (yyvsp[-1].range);
                    n->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) n;
                }
#line 36210 "gram.c"
    break;

  case 631: /* CreateSeqStmt: CREATE OptTemp SEQUENCE qualified_name OptSeqOptList  */
#line 4932 "gram.y"
                                {
                    CreateSeqStmt *n = makeNode(CreateSeqStmt);

                    (yyvsp[-1].range)->relpersistence = (yyvsp[-3].ival);
                    n->sequence = (yyvsp[-1].range);
                    n->options = (yyvsp[0].list);
                    n->ownerId = InvalidOid;
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 36225 "gram.c"
    break;

  case 632: /* CreateSeqStmt: CREATE OptTemp SEQUENCE IF_P NOT EXISTS qualified_name OptSeqOptList  */
#line 4943 "gram.y"
                                {
                    CreateSeqStmt *n = makeNode(CreateSeqStmt);

                    (yyvsp[-1].range)->relpersistence = (yyvsp[-6].ival);
                    n->sequence = (yyvsp[-1].range);
                    n->options = (yyvsp[0].list);
                    n->ownerId = InvalidOid;
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 36240 "gram.c"
    break;

  case 633: /* AlterSeqStmt: ALTER SEQUENCE qualified_name SeqOptList  */
#line 4957 "gram.y"
                                {
                    AlterSeqStmt *n = makeNode(AlterSeqStmt);

                    n->sequence = (yyvsp[-1].range);
                    n->options = (yyvsp[0].list);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 36253 "gram.c"
    break;

  case 634: /* AlterSeqStmt: ALTER SEQUENCE IF_P EXISTS qualified_name SeqOptList  */
#line 4966 "gram.y"
                                {
                    AlterSeqStmt *n = makeNode(AlterSeqStmt);

                    n->sequence = (yyvsp[-1].range);
                    n->options = (yyvsp[0].list);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 36266 "gram.c"
    break;

  case 635: /* OptSeqOptList: SeqOptList  */
#line 4977 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 36272 "gram.c"
    break;

  case 636: /* OptSeqOptList: %empty  */
#line 4978 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 36278 "gram.c"
    break;

  case 637: /* OptParenthesizedSeqOptList: '(' SeqOptList ')'  */
#line 4981 "gram.y"
                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 36284 "gram.c"
    break;

  case 638: /* OptParenthesizedSeqOptList: %empty  */
#line 4982 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 36290 "gram.c"
    break;

  case 639: /* SeqOptList: SeqOptElem  */
#line 4985 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 36296 "gram.c"
    break;

  case 640: /* SeqOptList: SeqOptList SeqOptElem  */
#line 4986 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 36302 "gram.c"
    break;

  case 641: /* SeqOptElem: AS SimpleTypename  */
#line 4990 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("as", (Node *) (yyvsp[0].typnam), (yylsp[-1]));
                }
#line 36310 "gram.c"
    break;

  case 642: /* SeqOptElem: CACHE NumericOnly  */
#line 4994 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("cache", (Node *) (yyvsp[0].node), (yylsp[-1]));
                }
#line 36318 "gram.c"
    break;

  case 643: /* SeqOptElem: CYCLE  */
#line 4998 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("cycle", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 36326 "gram.c"
    break;

  case 644: /* SeqOptElem: NO CYCLE  */
#line 5002 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("cycle", (Node *) makeBoolean(false), (yylsp[-1]));
                }
#line 36334 "gram.c"
    break;

  case 645: /* SeqOptElem: INCREMENT opt_by NumericOnly  */
#line 5006 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("increment", (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 36342 "gram.c"
    break;

  case 646: /* SeqOptElem: MAXVALUE NumericOnly  */
#line 5010 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("maxvalue", (Node *) (yyvsp[0].node), (yylsp[-1]));
                }
#line 36350 "gram.c"
    break;

  case 647: /* SeqOptElem: MINVALUE NumericOnly  */
#line 5014 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("minvalue", (Node *) (yyvsp[0].node), (yylsp[-1]));
                }
#line 36358 "gram.c"
    break;

  case 648: /* SeqOptElem: NO MAXVALUE  */
#line 5018 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("maxvalue", NULL, (yylsp[-1]));
                }
#line 36366 "gram.c"
    break;

  case 649: /* SeqOptElem: NO MINVALUE  */
#line 5022 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("minvalue", NULL, (yylsp[-1]));
                }
#line 36374 "gram.c"
    break;

  case 650: /* SeqOptElem: OWNED BY any_name  */
#line 5026 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("owned_by", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 36382 "gram.c"
    break;

  case 651: /* SeqOptElem: SEQUENCE NAME_P any_name  */
#line 5030 "gram.y"
                                {
                    /* not documented, only used by pg_dump */
                    (yyval.defelt) = makeDefElem("sequence_name", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 36391 "gram.c"
    break;

  case 652: /* SeqOptElem: START opt_with NumericOnly  */
#line 5035 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("start", (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 36399 "gram.c"
    break;

  case 653: /* SeqOptElem: RESTART  */
#line 5039 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("restart", NULL, (yylsp[0]));
                }
#line 36407 "gram.c"
    break;

  case 654: /* SeqOptElem: RESTART opt_with NumericOnly  */
#line 5043 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("restart", (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 36415 "gram.c"
    break;

  case 657: /* NumericOnly: FCONST  */
#line 5053 "gram.y"
                                                                                        { (yyval.node) = (Node *) makeFloat((yyvsp[0].str)); }
#line 36421 "gram.c"
    break;

  case 658: /* NumericOnly: '+' FCONST  */
#line 5054 "gram.y"
                                                                                { (yyval.node) = (Node *) makeFloat((yyvsp[0].str)); }
#line 36427 "gram.c"
    break;

  case 659: /* NumericOnly: '-' FCONST  */
#line 5056 "gram.y"
                                {
                    Float      *f = makeFloat((yyvsp[0].str));

                    doNegateFloat(f);
                    (yyval.node) = (Node *) f;
                }
#line 36438 "gram.c"
    break;

  case 660: /* NumericOnly: SignedIconst  */
#line 5062 "gram.y"
                                                                                { (yyval.node) = (Node *) makeInteger((yyvsp[0].ival)); }
#line 36444 "gram.c"
    break;

  case 661: /* NumericOnly_list: NumericOnly  */
#line 5065 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 36450 "gram.c"
    break;

  case 662: /* NumericOnly_list: NumericOnly_list ',' NumericOnly  */
#line 5066 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 36456 "gram.c"
    break;

  case 663: /* CreatePLangStmt: CREATE opt_or_replace opt_trusted opt_procedural LANGUAGE name  */
#line 5079 "gram.y"
                        {
                /*
                 * We now interpret parameterless CREATE LANGUAGE as
                 * CREATE EXTENSION.  "OR REPLACE" is silently translated
                 * to "IF NOT EXISTS", which isn't quite the same, but
                 * seems more useful than throwing an error.  We just
                 * ignore TRUSTED, as the previous code would have too.
                 */
                CreateExtensionStmt *n = makeNode(CreateExtensionStmt);

                n->if_not_exists = (yyvsp[-4].boolean);
                n->extname = (yyvsp[0].str);
                n->options = NIL;
                (yyval.node) = (Node *) n;
            }
#line 36476 "gram.c"
    break;

  case 664: /* CreatePLangStmt: CREATE opt_or_replace opt_trusted opt_procedural LANGUAGE name HANDLER handler_name opt_inline_handler opt_validator  */
#line 5096 "gram.y"
                        {
                CreatePLangStmt *n = makeNode(CreatePLangStmt);

                n->replace = (yyvsp[-8].boolean);
                n->plname = (yyvsp[-4].str);
                n->plhandler = (yyvsp[-2].list);
                n->plinline = (yyvsp[-1].list);
                n->plvalidator = (yyvsp[0].list);
                n->pltrusted = (yyvsp[-7].boolean);
                (yyval.node) = (Node *) n;
            }
#line 36492 "gram.c"
    break;

  case 665: /* opt_trusted: TRUSTED  */
#line 5110 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 36498 "gram.c"
    break;

  case 666: /* opt_trusted: %empty  */
#line 5111 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 36504 "gram.c"
    break;

  case 667: /* handler_name: name  */
#line 5119 "gram.y"
                                                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 36510 "gram.c"
    break;

  case 668: /* handler_name: name attrs  */
#line 5120 "gram.y"
                                                                { (yyval.list) = lcons(makeString((yyvsp[-1].str)), (yyvsp[0].list)); }
#line 36516 "gram.c"
    break;

  case 669: /* opt_inline_handler: INLINE_P handler_name  */
#line 5124 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 36522 "gram.c"
    break;

  case 670: /* opt_inline_handler: %empty  */
#line 5125 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 36528 "gram.c"
    break;

  case 671: /* validator_clause: VALIDATOR handler_name  */
#line 5129 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 36534 "gram.c"
    break;

  case 672: /* validator_clause: NO VALIDATOR  */
#line 5130 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 36540 "gram.c"
    break;

  case 673: /* opt_validator: validator_clause  */
#line 5134 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 36546 "gram.c"
    break;

  case 674: /* opt_validator: %empty  */
#line 5135 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 36552 "gram.c"
    break;

  case 677: /* CreateTableSpaceStmt: CREATE TABLESPACE name OptTableSpaceOwner LOCATION Sconst opt_reloptions  */
#line 5151 "gram.y"
                                {
                    CreateTableSpaceStmt *n = makeNode(CreateTableSpaceStmt);

                    n->tablespacename = (yyvsp[-4].str);
                    n->owner = (yyvsp[-3].rolespec);
                    n->location = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36566 "gram.c"
    break;

  case 678: /* OptTableSpaceOwner: OWNER RoleSpec  */
#line 5162 "gram.y"
                                                { (yyval.rolespec) = (yyvsp[0].rolespec); }
#line 36572 "gram.c"
    break;

  case 679: /* OptTableSpaceOwner: %empty  */
#line 5163 "gram.y"
                                                                { (yyval.rolespec) = NULL; }
#line 36578 "gram.c"
    break;

  case 680: /* DropTableSpaceStmt: DROP TABLESPACE name  */
#line 5177 "gram.y"
                                {
                    DropTableSpaceStmt *n = makeNode(DropTableSpaceStmt);

                    n->tablespacename = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 36590 "gram.c"
    break;

  case 681: /* DropTableSpaceStmt: DROP TABLESPACE IF_P EXISTS name  */
#line 5185 "gram.y"
                                {
                    DropTableSpaceStmt *n = makeNode(DropTableSpaceStmt);

                    n->tablespacename = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 36602 "gram.c"
    break;

  case 682: /* CreateExtensionStmt: CREATE EXTENSION name opt_with create_extension_opt_list  */
#line 5203 "gram.y"
                                {
                    CreateExtensionStmt *n = makeNode(CreateExtensionStmt);

                    n->extname = (yyvsp[-2].str);
                    n->if_not_exists = false;
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36615 "gram.c"
    break;

  case 683: /* CreateExtensionStmt: CREATE EXTENSION IF_P NOT EXISTS name opt_with create_extension_opt_list  */
#line 5212 "gram.y"
                                {
                    CreateExtensionStmt *n = makeNode(CreateExtensionStmt);

                    n->extname = (yyvsp[-2].str);
                    n->if_not_exists = true;
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36628 "gram.c"
    break;

  case 684: /* create_extension_opt_list: create_extension_opt_list create_extension_opt_item  */
#line 5224 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 36634 "gram.c"
    break;

  case 685: /* create_extension_opt_list: %empty  */
#line 5226 "gram.y"
                                { (yyval.list) = NIL; }
#line 36640 "gram.c"
    break;

  case 686: /* create_extension_opt_item: SCHEMA name  */
#line 5231 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("schema", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 36648 "gram.c"
    break;

  case 687: /* create_extension_opt_item: VERSION_P NonReservedWord_or_Sconst  */
#line 5235 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("new_version", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 36656 "gram.c"
    break;

  case 688: /* create_extension_opt_item: FROM NonReservedWord_or_Sconst  */
#line 5239 "gram.y"
                                {
                    ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("CREATE EXTENSION ... FROM is no longer supported"),
                             parser_errposition((yylsp[-1]))));
                }
#line 36667 "gram.c"
    break;

  case 689: /* create_extension_opt_item: CASCADE  */
#line 5246 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("cascade", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 36675 "gram.c"
    break;

  case 690: /* AlterExtensionStmt: ALTER EXTENSION name UPDATE alter_extension_opt_list  */
#line 5258 "gram.y"
                                {
                    AlterExtensionStmt *n = makeNode(AlterExtensionStmt);

                    n->extname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36687 "gram.c"
    break;

  case 691: /* alter_extension_opt_list: alter_extension_opt_list alter_extension_opt_item  */
#line 5269 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 36693 "gram.c"
    break;

  case 692: /* alter_extension_opt_list: %empty  */
#line 5271 "gram.y"
                                { (yyval.list) = NIL; }
#line 36699 "gram.c"
    break;

  case 693: /* alter_extension_opt_item: TO NonReservedWord_or_Sconst  */
#line 5276 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("new_version", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 36707 "gram.c"
    break;

  case 694: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop object_type_name name  */
#line 5289 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = (yyvsp[-1].objtype);
                    n->object = (Node *) makeString((yyvsp[0].str));
                    (yyval.node) = (Node *) n;
                }
#line 36721 "gram.c"
    break;

  case 695: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop object_type_any_name any_name  */
#line 5299 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = (yyvsp[-1].objtype);
                    n->object = (Node *) (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36735 "gram.c"
    break;

  case 696: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop AGGREGATE aggregate_with_argtypes  */
#line 5309 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[0].objwithargs);
                    (yyval.node) = (Node *) n;
                }
#line 36749 "gram.c"
    break;

  case 697: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop CAST '(' Typename AS Typename ')'  */
#line 5319 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-7].str);
                    n->action = (yyvsp[-6].ival);
                    n->objtype = OBJECT_CAST;
                    n->object = (Node *) list_make2((yyvsp[-3].typnam), (yyvsp[-1].typnam));
                    (yyval.node) = (Node *) n;
                }
#line 36763 "gram.c"
    break;

  case 698: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop DOMAIN_P Typename  */
#line 5329 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[0].typnam);
                    (yyval.node) = (Node *) n;
                }
#line 36777 "gram.c"
    break;

  case 699: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop FUNCTION function_with_argtypes  */
#line 5339 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[0].objwithargs);
                    (yyval.node) = (Node *) n;
                }
#line 36791 "gram.c"
    break;

  case 700: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop OPERATOR operator_with_argtypes  */
#line 5349 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_OPERATOR;
                    n->object = (Node *) (yyvsp[0].objwithargs);
                    (yyval.node) = (Node *) n;
                }
#line 36805 "gram.c"
    break;

  case 701: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop OPERATOR CLASS any_name USING name  */
#line 5359 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-6].str);
                    n->action = (yyvsp[-5].ival);
                    n->objtype = OBJECT_OPCLASS;
                    n->object = (Node *) lcons(makeString((yyvsp[0].str)), (yyvsp[-2].list));
                    (yyval.node) = (Node *) n;
                }
#line 36819 "gram.c"
    break;

  case 702: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop OPERATOR FAMILY any_name USING name  */
#line 5369 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-6].str);
                    n->action = (yyvsp[-5].ival);
                    n->objtype = OBJECT_OPFAMILY;
                    n->object = (Node *) lcons(makeString((yyvsp[0].str)), (yyvsp[-2].list));
                    (yyval.node) = (Node *) n;
                }
#line 36833 "gram.c"
    break;

  case 703: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop PROCEDURE function_with_argtypes  */
#line 5379 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[0].objwithargs);
                    (yyval.node) = (Node *) n;
                }
#line 36847 "gram.c"
    break;

  case 704: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop ROUTINE function_with_argtypes  */
#line 5389 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[0].objwithargs);
                    (yyval.node) = (Node *) n;
                }
#line 36861 "gram.c"
    break;

  case 705: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop TRANSFORM FOR Typename LANGUAGE name  */
#line 5399 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-6].str);
                    n->action = (yyvsp[-5].ival);
                    n->objtype = OBJECT_TRANSFORM;
                    n->object = (Node *) list_make2((yyvsp[-2].typnam), makeString((yyvsp[0].str)));
                    (yyval.node) = (Node *) n;
                }
#line 36875 "gram.c"
    break;

  case 706: /* AlterExtensionContentsStmt: ALTER EXTENSION name add_drop TYPE_P Typename  */
#line 5409 "gram.y"
                                {
                    AlterExtensionContentsStmt *n = makeNode(AlterExtensionContentsStmt);

                    n->extname = (yyvsp[-3].str);
                    n->action = (yyvsp[-2].ival);
                    n->objtype = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[0].typnam);
                    (yyval.node) = (Node *) n;
                }
#line 36889 "gram.c"
    break;

  case 707: /* CreateFdwStmt: CREATE FOREIGN DATA_P WRAPPER name opt_fdw_options create_generic_options  */
#line 5428 "gram.y"
                                {
                    CreateFdwStmt *n = makeNode(CreateFdwStmt);

                    n->fdwname = (yyvsp[-2].str);
                    n->func_options = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36902 "gram.c"
    break;

  case 708: /* fdw_option: HANDLER handler_name  */
#line 5439 "gram.y"
                                                                        { (yyval.defelt) = makeDefElem("handler", (Node *) (yyvsp[0].list), (yylsp[-1])); }
#line 36908 "gram.c"
    break;

  case 709: /* fdw_option: NO HANDLER  */
#line 5440 "gram.y"
                                                                                { (yyval.defelt) = makeDefElem("handler", NULL, (yylsp[-1])); }
#line 36914 "gram.c"
    break;

  case 710: /* fdw_option: VALIDATOR handler_name  */
#line 5441 "gram.y"
                                                                        { (yyval.defelt) = makeDefElem("validator", (Node *) (yyvsp[0].list), (yylsp[-1])); }
#line 36920 "gram.c"
    break;

  case 711: /* fdw_option: NO VALIDATOR  */
#line 5442 "gram.y"
                                                                                { (yyval.defelt) = makeDefElem("validator", NULL, (yylsp[-1])); }
#line 36926 "gram.c"
    break;

  case 712: /* fdw_options: fdw_option  */
#line 5446 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 36932 "gram.c"
    break;

  case 713: /* fdw_options: fdw_options fdw_option  */
#line 5447 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 36938 "gram.c"
    break;

  case 714: /* opt_fdw_options: fdw_options  */
#line 5451 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 36944 "gram.c"
    break;

  case 715: /* opt_fdw_options: %empty  */
#line 5452 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 36950 "gram.c"
    break;

  case 716: /* AlterFdwStmt: ALTER FOREIGN DATA_P WRAPPER name opt_fdw_options alter_generic_options  */
#line 5463 "gram.y"
                                {
                    AlterFdwStmt *n = makeNode(AlterFdwStmt);

                    n->fdwname = (yyvsp[-2].str);
                    n->func_options = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 36963 "gram.c"
    break;

  case 717: /* AlterFdwStmt: ALTER FOREIGN DATA_P WRAPPER name fdw_options  */
#line 5472 "gram.y"
                                {
                    AlterFdwStmt *n = makeNode(AlterFdwStmt);

                    n->fdwname = (yyvsp[-1].str);
                    n->func_options = (yyvsp[0].list);
                    n->options = NIL;
                    (yyval.node) = (Node *) n;
                }
#line 36976 "gram.c"
    break;

  case 718: /* create_generic_options: OPTIONS '(' generic_option_list ')'  */
#line 5484 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 36982 "gram.c"
    break;

  case 719: /* create_generic_options: %empty  */
#line 5485 "gram.y"
                                                                                                        { (yyval.list) = NIL; }
#line 36988 "gram.c"
    break;

  case 720: /* generic_option_list: generic_option_elem  */
#line 5490 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].defelt));
                }
#line 36996 "gram.c"
    break;

  case 721: /* generic_option_list: generic_option_list ',' generic_option_elem  */
#line 5494 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt));
                }
#line 37004 "gram.c"
    break;

  case 722: /* alter_generic_options: OPTIONS '(' alter_generic_option_list ')'  */
#line 5501 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 37010 "gram.c"
    break;

  case 723: /* alter_generic_option_list: alter_generic_option_elem  */
#line 5506 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].defelt));
                }
#line 37018 "gram.c"
    break;

  case 724: /* alter_generic_option_list: alter_generic_option_list ',' alter_generic_option_elem  */
#line 5510 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt));
                }
#line 37026 "gram.c"
    break;

  case 725: /* alter_generic_option_elem: generic_option_elem  */
#line 5517 "gram.y"
                                {
                    (yyval.defelt) = (yyvsp[0].defelt);
                }
#line 37034 "gram.c"
    break;

  case 726: /* alter_generic_option_elem: SET generic_option_elem  */
#line 5521 "gram.y"
                                {
                    (yyval.defelt) = (yyvsp[0].defelt);
                    (yyval.defelt)->defaction = DEFELEM_SET;
                }
#line 37043 "gram.c"
    break;

  case 727: /* alter_generic_option_elem: ADD_P generic_option_elem  */
#line 5526 "gram.y"
                                {
                    (yyval.defelt) = (yyvsp[0].defelt);
                    (yyval.defelt)->defaction = DEFELEM_ADD;
                }
#line 37052 "gram.c"
    break;

  case 728: /* alter_generic_option_elem: DROP generic_option_name  */
#line 5531 "gram.y"
                                {
                    (yyval.defelt) = makeDefElemExtended(NULL, (yyvsp[0].str), NULL, DEFELEM_DROP, (yylsp[0]));
                }
#line 37060 "gram.c"
    break;

  case 729: /* generic_option_elem: generic_option_name generic_option_arg  */
#line 5538 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-1].str), (yyvsp[0].node), (yylsp[-1]));
                }
#line 37068 "gram.c"
    break;

  case 730: /* generic_option_name: ColLabel  */
#line 5544 "gram.y"
                                                                { (yyval.str) = (yyvsp[0].str); }
#line 37074 "gram.c"
    break;

  case 731: /* generic_option_arg: Sconst  */
#line 5549 "gram.y"
                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 37080 "gram.c"
    break;

  case 732: /* CreateForeignServerStmt: CREATE SERVER name opt_type opt_foreign_server_version FOREIGN DATA_P WRAPPER name create_generic_options  */
#line 5561 "gram.y"
                                {
                    CreateForeignServerStmt *n = makeNode(CreateForeignServerStmt);

                    n->servername = (yyvsp[-7].str);
                    n->servertype = (yyvsp[-6].str);
                    n->version = (yyvsp[-5].str);
                    n->fdwname = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 37096 "gram.c"
    break;

  case 733: /* CreateForeignServerStmt: CREATE SERVER IF_P NOT EXISTS name opt_type opt_foreign_server_version FOREIGN DATA_P WRAPPER name create_generic_options  */
#line 5574 "gram.y"
                                {
                    CreateForeignServerStmt *n = makeNode(CreateForeignServerStmt);

                    n->servername = (yyvsp[-7].str);
                    n->servertype = (yyvsp[-6].str);
                    n->version = (yyvsp[-5].str);
                    n->fdwname = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 37112 "gram.c"
    break;

  case 734: /* opt_type: TYPE_P Sconst  */
#line 5588 "gram.y"
                                                        { (yyval.str) = (yyvsp[0].str); }
#line 37118 "gram.c"
    break;

  case 735: /* opt_type: %empty  */
#line 5589 "gram.y"
                                                                { (yyval.str) = NULL; }
#line 37124 "gram.c"
    break;

  case 736: /* foreign_server_version: VERSION_P Sconst  */
#line 5594 "gram.y"
                                                        { (yyval.str) = (yyvsp[0].str); }
#line 37130 "gram.c"
    break;

  case 737: /* foreign_server_version: VERSION_P NULL_P  */
#line 5595 "gram.y"
                                                        { (yyval.str) = NULL; }
#line 37136 "gram.c"
    break;

  case 738: /* opt_foreign_server_version: foreign_server_version  */
#line 5599 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 37142 "gram.c"
    break;

  case 739: /* opt_foreign_server_version: %empty  */
#line 5600 "gram.y"
                                                                { (yyval.str) = NULL; }
#line 37148 "gram.c"
    break;

  case 740: /* AlterForeignServerStmt: ALTER SERVER name foreign_server_version alter_generic_options  */
#line 5611 "gram.y"
                                {
                    AlterForeignServerStmt *n = makeNode(AlterForeignServerStmt);

                    n->servername = (yyvsp[-2].str);
                    n->version = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    n->has_version = true;
                    (yyval.node) = (Node *) n;
                }
#line 37162 "gram.c"
    break;

  case 741: /* AlterForeignServerStmt: ALTER SERVER name foreign_server_version  */
#line 5621 "gram.y"
                                {
                    AlterForeignServerStmt *n = makeNode(AlterForeignServerStmt);

                    n->servername = (yyvsp[-1].str);
                    n->version = (yyvsp[0].str);
                    n->has_version = true;
                    (yyval.node) = (Node *) n;
                }
#line 37175 "gram.c"
    break;

  case 742: /* AlterForeignServerStmt: ALTER SERVER name alter_generic_options  */
#line 5630 "gram.y"
                                {
                    AlterForeignServerStmt *n = makeNode(AlterForeignServerStmt);

                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37187 "gram.c"
    break;

  case 743: /* CreateForeignTableStmt: CREATE FOREIGN TABLE qualified_name '(' OptTableElementList ')' OptInherit SERVER name create_generic_options  */
#line 5650 "gram.y"
                                {
                    CreateForeignTableStmt *n = makeNode(CreateForeignTableStmt);

                    (yyvsp[-7].range)->relpersistence = RELPERSISTENCE_PERMANENT;
                    n->base.relation = (yyvsp[-7].range);
                    n->base.tableElts = (yyvsp[-5].list);
                    n->base.inhRelations = (yyvsp[-3].list);
                    n->base.ofTypename = NULL;
                    n->base.constraints = NIL;
                    n->base.options = NIL;
                    n->base.oncommit = ONCOMMIT_NOOP;
                    n->base.tablespacename = NULL;
                    n->base.if_not_exists = false;
                    /* FDW-specific data */
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37210 "gram.c"
    break;

  case 744: /* CreateForeignTableStmt: CREATE FOREIGN TABLE IF_P NOT EXISTS qualified_name '(' OptTableElementList ')' OptInherit SERVER name create_generic_options  */
#line 5671 "gram.y"
                                {
                    CreateForeignTableStmt *n = makeNode(CreateForeignTableStmt);

                    (yyvsp[-7].range)->relpersistence = RELPERSISTENCE_PERMANENT;
                    n->base.relation = (yyvsp[-7].range);
                    n->base.tableElts = (yyvsp[-5].list);
                    n->base.inhRelations = (yyvsp[-3].list);
                    n->base.ofTypename = NULL;
                    n->base.constraints = NIL;
                    n->base.options = NIL;
                    n->base.oncommit = ONCOMMIT_NOOP;
                    n->base.tablespacename = NULL;
                    n->base.if_not_exists = true;
                    /* FDW-specific data */
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37233 "gram.c"
    break;

  case 745: /* CreateForeignTableStmt: CREATE FOREIGN TABLE qualified_name PARTITION OF qualified_name OptTypedTableElementList PartitionBoundSpec SERVER name create_generic_options  */
#line 5692 "gram.y"
                                {
                    CreateForeignTableStmt *n = makeNode(CreateForeignTableStmt);

                    (yyvsp[-8].range)->relpersistence = RELPERSISTENCE_PERMANENT;
                    n->base.relation = (yyvsp[-8].range);
                    n->base.inhRelations = list_make1((yyvsp[-5].range));
                    n->base.tableElts = (yyvsp[-4].list);
                    n->base.partbound = (yyvsp[-3].partboundspec);
                    n->base.ofTypename = NULL;
                    n->base.constraints = NIL;
                    n->base.options = NIL;
                    n->base.oncommit = ONCOMMIT_NOOP;
                    n->base.tablespacename = NULL;
                    n->base.if_not_exists = false;
                    /* FDW-specific data */
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37257 "gram.c"
    break;

  case 746: /* CreateForeignTableStmt: CREATE FOREIGN TABLE IF_P NOT EXISTS qualified_name PARTITION OF qualified_name OptTypedTableElementList PartitionBoundSpec SERVER name create_generic_options  */
#line 5714 "gram.y"
                                {
                    CreateForeignTableStmt *n = makeNode(CreateForeignTableStmt);

                    (yyvsp[-8].range)->relpersistence = RELPERSISTENCE_PERMANENT;
                    n->base.relation = (yyvsp[-8].range);
                    n->base.inhRelations = list_make1((yyvsp[-5].range));
                    n->base.tableElts = (yyvsp[-4].list);
                    n->base.partbound = (yyvsp[-3].partboundspec);
                    n->base.ofTypename = NULL;
                    n->base.constraints = NIL;
                    n->base.options = NIL;
                    n->base.oncommit = ONCOMMIT_NOOP;
                    n->base.tablespacename = NULL;
                    n->base.if_not_exists = true;
                    /* FDW-specific data */
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37281 "gram.c"
    break;

  case 747: /* ImportForeignSchemaStmt: IMPORT_P FOREIGN SCHEMA name import_qualification FROM SERVER name INTO name create_generic_options  */
#line 5747 "gram.y"
                        {
                ImportForeignSchemaStmt *n = makeNode(ImportForeignSchemaStmt);

                n->server_name = (yyvsp[-3].str);
                n->remote_schema = (yyvsp[-7].str);
                n->local_schema = (yyvsp[-1].str);
                n->list_type = (yyvsp[-6].importqual)->type;
                n->table_list = (yyvsp[-6].importqual)->table_names;
                n->options = (yyvsp[0].list);
                (yyval.node) = (Node *) n;
            }
#line 37297 "gram.c"
    break;

  case 748: /* import_qualification_type: LIMIT TO  */
#line 5761 "gram.y"
                                                        { (yyval.ival) = FDW_IMPORT_SCHEMA_LIMIT_TO; }
#line 37303 "gram.c"
    break;

  case 749: /* import_qualification_type: EXCEPT  */
#line 5762 "gram.y"
                                                        { (yyval.ival) = FDW_IMPORT_SCHEMA_EXCEPT; }
#line 37309 "gram.c"
    break;

  case 750: /* import_qualification: import_qualification_type '(' relation_expr_list ')'  */
#line 5767 "gram.y"
                        {
                ImportQual *n = (ImportQual *) palloc(sizeof(ImportQual));

                n->type = (yyvsp[-3].ival);
                n->table_names = (yyvsp[-1].list);
                (yyval.importqual) = n;
            }
#line 37321 "gram.c"
    break;

  case 751: /* import_qualification: %empty  */
#line 5775 "gram.y"
                        {
                ImportQual *n = (ImportQual *) palloc(sizeof(ImportQual));
                n->type = FDW_IMPORT_SCHEMA_ALL;
                n->table_names = NIL;
                (yyval.importqual) = n;
            }
#line 37332 "gram.c"
    break;

  case 752: /* CreateUserMappingStmt: CREATE USER MAPPING FOR auth_ident SERVER name create_generic_options  */
#line 5791 "gram.y"
                                {
                    CreateUserMappingStmt *n = makeNode(CreateUserMappingStmt);

                    n->user = (yyvsp[-3].rolespec);
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    n->if_not_exists = false;
                    (yyval.node) = (Node *) n;
                }
#line 37346 "gram.c"
    break;

  case 753: /* CreateUserMappingStmt: CREATE USER MAPPING IF_P NOT EXISTS FOR auth_ident SERVER name create_generic_options  */
#line 5801 "gram.y"
                                {
                    CreateUserMappingStmt *n = makeNode(CreateUserMappingStmt);

                    n->user = (yyvsp[-3].rolespec);
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 37360 "gram.c"
    break;

  case 754: /* auth_ident: RoleSpec  */
#line 5813 "gram.y"
                                        { (yyval.rolespec) = (yyvsp[0].rolespec); }
#line 37366 "gram.c"
    break;

  case 755: /* auth_ident: USER  */
#line 5814 "gram.y"
                                                        { (yyval.rolespec) = makeRoleSpec(ROLESPEC_CURRENT_USER, (yylsp[0])); }
#line 37372 "gram.c"
    break;

  case 756: /* DropUserMappingStmt: DROP USER MAPPING FOR auth_ident SERVER name  */
#line 5827 "gram.y"
                                {
                    DropUserMappingStmt *n = makeNode(DropUserMappingStmt);

                    n->user = (yyvsp[-2].rolespec);
                    n->servername = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 37385 "gram.c"
    break;

  case 757: /* DropUserMappingStmt: DROP USER MAPPING IF_P EXISTS FOR auth_ident SERVER name  */
#line 5836 "gram.y"
                                {
                    DropUserMappingStmt *n = makeNode(DropUserMappingStmt);

                    n->user = (yyvsp[-2].rolespec);
                    n->servername = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 37398 "gram.c"
    break;

  case 758: /* AlterUserMappingStmt: ALTER USER MAPPING FOR auth_ident SERVER name alter_generic_options  */
#line 5854 "gram.y"
                                {
                    AlterUserMappingStmt *n = makeNode(AlterUserMappingStmt);

                    n->user = (yyvsp[-3].rolespec);
                    n->servername = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 37411 "gram.c"
    break;

  case 759: /* CreatePolicyStmt: CREATE POLICY name ON qualified_name RowSecurityDefaultPermissive RowSecurityDefaultForCmd RowSecurityDefaultToRole RowSecurityOptionalExpr RowSecurityOptionalWithCheck  */
#line 5881 "gram.y"
                                {
                    CreatePolicyStmt *n = makeNode(CreatePolicyStmt);

                    n->policy_name = (yyvsp[-7].str);
                    n->table = (yyvsp[-5].range);
                    n->permissive = (yyvsp[-4].boolean);
                    n->cmd_name = (yyvsp[-3].str);
                    n->roles = (yyvsp[-2].list);
                    n->qual = (yyvsp[-1].node);
                    n->with_check = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 37428 "gram.c"
    break;

  case 760: /* AlterPolicyStmt: ALTER POLICY name ON qualified_name RowSecurityOptionalToRole RowSecurityOptionalExpr RowSecurityOptionalWithCheck  */
#line 5898 "gram.y"
                                {
                    AlterPolicyStmt *n = makeNode(AlterPolicyStmt);

                    n->policy_name = (yyvsp[-5].str);
                    n->table = (yyvsp[-3].range);
                    n->roles = (yyvsp[-2].list);
                    n->qual = (yyvsp[-1].node);
                    n->with_check = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 37443 "gram.c"
    break;

  case 761: /* RowSecurityOptionalExpr: USING '(' a_expr ')'  */
#line 5911 "gram.y"
                                                { (yyval.node) = (yyvsp[-1].node); }
#line 37449 "gram.c"
    break;

  case 762: /* RowSecurityOptionalExpr: %empty  */
#line 5912 "gram.y"
                                                        { (yyval.node) = NULL; }
#line 37455 "gram.c"
    break;

  case 763: /* RowSecurityOptionalWithCheck: WITH CHECK '(' a_expr ')'  */
#line 5916 "gram.y"
                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 37461 "gram.c"
    break;

  case 764: /* RowSecurityOptionalWithCheck: %empty  */
#line 5917 "gram.y"
                                                                        { (yyval.node) = NULL; }
#line 37467 "gram.c"
    break;

  case 765: /* RowSecurityDefaultToRole: TO role_list  */
#line 5921 "gram.y"
                                                        { (yyval.list) = (yyvsp[0].list); }
#line 37473 "gram.c"
    break;

  case 766: /* RowSecurityDefaultToRole: %empty  */
#line 5922 "gram.y"
                                                        { (yyval.list) = list_make1(makeRoleSpec(ROLESPEC_PUBLIC, -1)); }
#line 37479 "gram.c"
    break;

  case 767: /* RowSecurityOptionalToRole: TO role_list  */
#line 5926 "gram.y"
                                                        { (yyval.list) = (yyvsp[0].list); }
#line 37485 "gram.c"
    break;

  case 768: /* RowSecurityOptionalToRole: %empty  */
#line 5927 "gram.y"
                                                        { (yyval.list) = NULL; }
#line 37491 "gram.c"
    break;

  case 769: /* RowSecurityDefaultPermissive: AS IDENT  */
#line 5932 "gram.y"
                                {
                    if (strcmp((yyvsp[0].str), "permissive") == 0)
                        (yyval.boolean) = true;
                    else if (strcmp((yyvsp[0].str), "restrictive") == 0)
                        (yyval.boolean) = false;
                    else
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("unrecognized row security option \"%s\"", (yyvsp[0].str)),
                                 errhint("Only PERMISSIVE or RESTRICTIVE policies are supported currently."),
                                 parser_errposition((yylsp[0]))));

                }
#line 37509 "gram.c"
    break;

  case 770: /* RowSecurityDefaultPermissive: %empty  */
#line 5945 "gram.y"
                                                        { (yyval.boolean) = true; }
#line 37515 "gram.c"
    break;

  case 771: /* RowSecurityDefaultForCmd: FOR row_security_cmd  */
#line 5949 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 37521 "gram.c"
    break;

  case 772: /* RowSecurityDefaultForCmd: %empty  */
#line 5950 "gram.y"
                                                        { (yyval.str) = "all"; }
#line 37527 "gram.c"
    break;

  case 773: /* row_security_cmd: ALL  */
#line 5954 "gram.y"
                                                        { (yyval.str) = "all"; }
#line 37533 "gram.c"
    break;

  case 774: /* row_security_cmd: SELECT  */
#line 5955 "gram.y"
                                                { (yyval.str) = "select"; }
#line 37539 "gram.c"
    break;

  case 775: /* row_security_cmd: INSERT  */
#line 5956 "gram.y"
                                                { (yyval.str) = "insert"; }
#line 37545 "gram.c"
    break;

  case 776: /* row_security_cmd: UPDATE  */
#line 5957 "gram.y"
                                                { (yyval.str) = "update"; }
#line 37551 "gram.c"
    break;

  case 777: /* row_security_cmd: DELETE_P  */
#line 5958 "gram.y"
                                                { (yyval.str) = "delete"; }
#line 37557 "gram.c"
    break;

  case 778: /* CreateAmStmt: CREATE ACCESS METHOD name TYPE_P am_type HANDLER handler_name  */
#line 5969 "gram.y"
                                {
                    CreateAmStmt *n = makeNode(CreateAmStmt);

                    n->amname = (yyvsp[-4].str);
                    n->handler_name = (yyvsp[0].list);
                    n->amtype = (yyvsp[-2].chr);
                    (yyval.node) = (Node *) n;
                }
#line 37570 "gram.c"
    break;

  case 779: /* am_type: INDEX  */
#line 5980 "gram.y"
                                                { (yyval.chr) = AMTYPE_INDEX; }
#line 37576 "gram.c"
    break;

  case 780: /* am_type: TABLE  */
#line 5981 "gram.y"
                                                { (yyval.chr) = AMTYPE_TABLE; }
#line 37582 "gram.c"
    break;

  case 781: /* CreateTrigStmt: CREATE opt_or_replace TRIGGER name TriggerActionTime TriggerEvents ON qualified_name TriggerReferencing TriggerForSpec TriggerWhen EXECUTE FUNCTION_or_PROCEDURE func_name '(' TriggerFuncArgs ')'  */
#line 5995 "gram.y"
                                {
                    CreateTrigStmt *n = makeNode(CreateTrigStmt);

                    n->replace = (yyvsp[-15].boolean);
                    n->isconstraint = false;
                    n->trigname = (yyvsp[-13].str);
                    n->relation = (yyvsp[-9].range);
                    n->funcname = (yyvsp[-3].list);
                    n->args = (yyvsp[-1].list);
                    n->row = (yyvsp[-7].boolean);
                    n->timing = (yyvsp[-12].ival);
                    n->events = intVal(linitial((yyvsp[-11].list)));
                    n->columns = (List *) lsecond((yyvsp[-11].list));
                    n->whenClause = (yyvsp[-6].node);
                    n->transitionRels = (yyvsp[-8].list);
                    n->deferrable = false;
                    n->initdeferred = false;
                    n->constrrel = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 37607 "gram.c"
    break;

  case 782: /* CreateTrigStmt: CREATE opt_or_replace CONSTRAINT TRIGGER name AFTER TriggerEvents ON qualified_name OptConstrFromTable ConstraintAttributeSpec FOR EACH ROW TriggerWhen EXECUTE FUNCTION_or_PROCEDURE func_name '(' TriggerFuncArgs ')'  */
#line 6019 "gram.y"
                                {
                    CreateTrigStmt *n = makeNode(CreateTrigStmt);

                    n->replace = (yyvsp[-19].boolean);
                    if (n->replace) /* not supported, see CreateTrigger */
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("CREATE OR REPLACE CONSTRAINT TRIGGER is not supported")));
                    n->isconstraint = true;
                    n->trigname = (yyvsp[-16].str);
                    n->relation = (yyvsp[-12].range);
                    n->funcname = (yyvsp[-3].list);
                    n->args = (yyvsp[-1].list);
                    n->row = true;
                    n->timing = TRIGGER_TYPE_AFTER;
                    n->events = intVal(linitial((yyvsp[-14].list)));
                    n->columns = (List *) lsecond((yyvsp[-14].list));
                    n->whenClause = (yyvsp[-6].node);
                    n->transitionRels = NIL;
                    processCASbits((yyvsp[-10].ival), (yylsp[-10]), "TRIGGER",
                                   &n->deferrable, &n->initdeferred, NULL,
                                   NULL, yyscanner);
                    n->constrrel = (yyvsp[-11].range);
                    (yyval.node) = (Node *) n;
                }
#line 37637 "gram.c"
    break;

  case 783: /* TriggerActionTime: BEFORE  */
#line 6047 "gram.y"
                                                                                        { (yyval.ival) = TRIGGER_TYPE_BEFORE; }
#line 37643 "gram.c"
    break;

  case 784: /* TriggerActionTime: AFTER  */
#line 6048 "gram.y"
                                                                                        { (yyval.ival) = TRIGGER_TYPE_AFTER; }
#line 37649 "gram.c"
    break;

  case 785: /* TriggerActionTime: INSTEAD OF  */
#line 6049 "gram.y"
                                                                                { (yyval.ival) = TRIGGER_TYPE_INSTEAD; }
#line 37655 "gram.c"
    break;

  case 786: /* TriggerEvents: TriggerOneEvent  */
#line 6054 "gram.y"
                                { (yyval.list) = (yyvsp[0].list); }
#line 37661 "gram.c"
    break;

  case 787: /* TriggerEvents: TriggerEvents OR TriggerOneEvent  */
#line 6056 "gram.y"
                                {
                    int         events1 = intVal(linitial((yyvsp[-2].list)));
                    int         events2 = intVal(linitial((yyvsp[0].list)));
                    List       *columns1 = (List *) lsecond((yyvsp[-2].list));
                    List       *columns2 = (List *) lsecond((yyvsp[0].list));

                    if (events1 & events2)
                        parser_yyerror("duplicate trigger events specified");
                    /*
                     * concat'ing the columns lists loses information about
                     * which columns went with which event, but so long as
                     * only UPDATE carries columns and we disallow multiple
                     * UPDATE items, it doesn't matter.  Command execution
                     * should just ignore the columns for non-UPDATE events.
                     */
                    (yyval.list) = list_make2(makeInteger(events1 | events2),
                                    list_concat(columns1, columns2));
                }
#line 37684 "gram.c"
    break;

  case 788: /* TriggerOneEvent: INSERT  */
#line 6078 "gram.y"
                                { (yyval.list) = list_make2(makeInteger(TRIGGER_TYPE_INSERT), NIL); }
#line 37690 "gram.c"
    break;

  case 789: /* TriggerOneEvent: DELETE_P  */
#line 6080 "gram.y"
                                { (yyval.list) = list_make2(makeInteger(TRIGGER_TYPE_DELETE), NIL); }
#line 37696 "gram.c"
    break;

  case 790: /* TriggerOneEvent: UPDATE  */
#line 6082 "gram.y"
                                { (yyval.list) = list_make2(makeInteger(TRIGGER_TYPE_UPDATE), NIL); }
#line 37702 "gram.c"
    break;

  case 791: /* TriggerOneEvent: UPDATE OF columnList  */
#line 6084 "gram.y"
                                { (yyval.list) = list_make2(makeInteger(TRIGGER_TYPE_UPDATE), (yyvsp[0].list)); }
#line 37708 "gram.c"
    break;

  case 792: /* TriggerOneEvent: TRUNCATE  */
#line 6086 "gram.y"
                                { (yyval.list) = list_make2(makeInteger(TRIGGER_TYPE_TRUNCATE), NIL); }
#line 37714 "gram.c"
    break;

  case 793: /* TriggerReferencing: REFERENCING TriggerTransitions  */
#line 6090 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 37720 "gram.c"
    break;

  case 794: /* TriggerReferencing: %empty  */
#line 6091 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 37726 "gram.c"
    break;

  case 795: /* TriggerTransitions: TriggerTransition  */
#line 6095 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 37732 "gram.c"
    break;

  case 796: /* TriggerTransitions: TriggerTransitions TriggerTransition  */
#line 6096 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 37738 "gram.c"
    break;

  case 797: /* TriggerTransition: TransitionOldOrNew TransitionRowOrTable opt_as TransitionRelName  */
#line 6101 "gram.y"
                                {
                    TriggerTransition *n = makeNode(TriggerTransition);

                    n->name = (yyvsp[0].str);
                    n->isNew = (yyvsp[-3].boolean);
                    n->isTable = (yyvsp[-2].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 37751 "gram.c"
    break;

  case 798: /* TransitionOldOrNew: NEW  */
#line 6112 "gram.y"
                                                                                                        { (yyval.boolean) = true; }
#line 37757 "gram.c"
    break;

  case 799: /* TransitionOldOrNew: OLD  */
#line 6113 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 37763 "gram.c"
    break;

  case 800: /* TransitionRowOrTable: TABLE  */
#line 6117 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 37769 "gram.c"
    break;

  case 801: /* TransitionRowOrTable: ROW  */
#line 6126 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 37775 "gram.c"
    break;

  case 802: /* TransitionRelName: ColId  */
#line 6130 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 37781 "gram.c"
    break;

  case 803: /* TriggerForSpec: FOR TriggerForOptEach TriggerForType  */
#line 6135 "gram.y"
                                {
                    (yyval.boolean) = (yyvsp[0].boolean);
                }
#line 37789 "gram.c"
    break;

  case 804: /* TriggerForSpec: %empty  */
#line 6139 "gram.y"
                                {
                    /*
                     * If ROW/STATEMENT not specified, default to
                     * STATEMENT, per SQL
                     */
                    (yyval.boolean) = false;
                }
#line 37801 "gram.c"
    break;

  case 807: /* TriggerForType: ROW  */
#line 6154 "gram.y"
                                                                                                        { (yyval.boolean) = true; }
#line 37807 "gram.c"
    break;

  case 808: /* TriggerForType: STATEMENT  */
#line 6155 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 37813 "gram.c"
    break;

  case 809: /* TriggerWhen: WHEN '(' a_expr ')'  */
#line 6159 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 37819 "gram.c"
    break;

  case 810: /* TriggerWhen: %empty  */
#line 6160 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 37825 "gram.c"
    break;

  case 813: /* TriggerFuncArgs: TriggerFuncArg  */
#line 6169 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 37831 "gram.c"
    break;

  case 814: /* TriggerFuncArgs: TriggerFuncArgs ',' TriggerFuncArg  */
#line 6170 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 37837 "gram.c"
    break;

  case 815: /* TriggerFuncArgs: %empty  */
#line 6171 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 37843 "gram.c"
    break;

  case 816: /* TriggerFuncArg: Iconst  */
#line 6176 "gram.y"
                                {
                    (yyval.node) = (Node *) makeString(psprintf("%d", (yyvsp[0].ival)));
                }
#line 37851 "gram.c"
    break;

  case 817: /* TriggerFuncArg: FCONST  */
#line 6179 "gram.y"
                                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 37857 "gram.c"
    break;

  case 818: /* TriggerFuncArg: Sconst  */
#line 6180 "gram.y"
                                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 37863 "gram.c"
    break;

  case 819: /* TriggerFuncArg: ColLabel  */
#line 6181 "gram.y"
                                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 37869 "gram.c"
    break;

  case 820: /* OptConstrFromTable: FROM qualified_name  */
#line 6185 "gram.y"
                                                                                        { (yyval.range) = (yyvsp[0].range); }
#line 37875 "gram.c"
    break;

  case 821: /* OptConstrFromTable: %empty  */
#line 6186 "gram.y"
                                                                                                { (yyval.range) = NULL; }
#line 37881 "gram.c"
    break;

  case 822: /* ConstraintAttributeSpec: %empty  */
#line 6191 "gram.y"
                                { (yyval.ival) = 0; }
#line 37887 "gram.c"
    break;

  case 823: /* ConstraintAttributeSpec: ConstraintAttributeSpec ConstraintAttributeElem  */
#line 6193 "gram.y"
                                {
                    /*
                     * We must complain about conflicting options.
                     * We could, but choose not to, complain about redundant
                     * options (ie, where $2's bit is already set in $1).
                     */
                    int     newspec = (yyvsp[-1].ival) | (yyvsp[0].ival);

                    /* special message for this case */
                    if ((newspec & (CAS_NOT_DEFERRABLE | CAS_INITIALLY_DEFERRED)) == (CAS_NOT_DEFERRABLE | CAS_INITIALLY_DEFERRED))
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("constraint declared INITIALLY DEFERRED must be DEFERRABLE"),
                                 parser_errposition((yylsp[0]))));
                    /* generic message for other conflicts */
                    if ((newspec & (CAS_NOT_DEFERRABLE | CAS_DEFERRABLE)) == (CAS_NOT_DEFERRABLE | CAS_DEFERRABLE) ||
                        (newspec & (CAS_INITIALLY_IMMEDIATE | CAS_INITIALLY_DEFERRED)) == (CAS_INITIALLY_IMMEDIATE | CAS_INITIALLY_DEFERRED))
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("conflicting constraint properties"),
                                 parser_errposition((yylsp[0]))));
                    (yyval.ival) = newspec;
                }
#line 37915 "gram.c"
    break;

  case 824: /* ConstraintAttributeElem: NOT DEFERRABLE  */
#line 6219 "gram.y"
                                                                        { (yyval.ival) = CAS_NOT_DEFERRABLE; }
#line 37921 "gram.c"
    break;

  case 825: /* ConstraintAttributeElem: DEFERRABLE  */
#line 6220 "gram.y"
                                                                        { (yyval.ival) = CAS_DEFERRABLE; }
#line 37927 "gram.c"
    break;

  case 826: /* ConstraintAttributeElem: INITIALLY IMMEDIATE  */
#line 6221 "gram.y"
                                                                { (yyval.ival) = CAS_INITIALLY_IMMEDIATE; }
#line 37933 "gram.c"
    break;

  case 827: /* ConstraintAttributeElem: INITIALLY DEFERRED  */
#line 6222 "gram.y"
                                                                { (yyval.ival) = CAS_INITIALLY_DEFERRED; }
#line 37939 "gram.c"
    break;

  case 828: /* ConstraintAttributeElem: NOT VALID  */
#line 6223 "gram.y"
                                                                                { (yyval.ival) = CAS_NOT_VALID; }
#line 37945 "gram.c"
    break;

  case 829: /* ConstraintAttributeElem: NO INHERIT  */
#line 6224 "gram.y"
                                                                        { (yyval.ival) = CAS_NO_INHERIT; }
#line 37951 "gram.c"
    break;

  case 830: /* CreateEventTrigStmt: CREATE EVENT TRIGGER name ON ColLabel EXECUTE FUNCTION_or_PROCEDURE func_name '(' ')'  */
#line 6239 "gram.y"
                                {
                    CreateEventTrigStmt *n = makeNode(CreateEventTrigStmt);

                    n->trigname = (yyvsp[-7].str);
                    n->eventname = (yyvsp[-5].str);
                    n->whenclause = NULL;
                    n->funcname = (yyvsp[-2].list);
                    (yyval.node) = (Node *) n;
                }
#line 37965 "gram.c"
    break;

  case 831: /* CreateEventTrigStmt: CREATE EVENT TRIGGER name ON ColLabel WHEN event_trigger_when_list EXECUTE FUNCTION_or_PROCEDURE func_name '(' ')'  */
#line 6251 "gram.y"
                                {
                    CreateEventTrigStmt *n = makeNode(CreateEventTrigStmt);

                    n->trigname = (yyvsp[-9].str);
                    n->eventname = (yyvsp[-7].str);
                    n->whenclause = (yyvsp[-5].list);
                    n->funcname = (yyvsp[-2].list);
                    (yyval.node) = (Node *) n;
                }
#line 37979 "gram.c"
    break;

  case 832: /* event_trigger_when_list: event_trigger_when_item  */
#line 6264 "gram.y"
                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 37985 "gram.c"
    break;

  case 833: /* event_trigger_when_list: event_trigger_when_list AND event_trigger_when_item  */
#line 6266 "gram.y"
                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 37991 "gram.c"
    break;

  case 834: /* event_trigger_when_item: ColId IN_P '(' event_trigger_value_list ')'  */
#line 6271 "gram.y"
                        { (yyval.defelt) = makeDefElem((yyvsp[-4].str), (Node *) (yyvsp[-1].list), (yylsp[-4])); }
#line 37997 "gram.c"
    break;

  case 835: /* event_trigger_value_list: SCONST  */
#line 6276 "gram.y"
                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 38003 "gram.c"
    break;

  case 836: /* event_trigger_value_list: event_trigger_value_list ',' SCONST  */
#line 6278 "gram.y"
                        { (yyval.list) = lappend((yyvsp[-2].list), makeString((yyvsp[0].str))); }
#line 38009 "gram.c"
    break;

  case 837: /* AlterEventTrigStmt: ALTER EVENT TRIGGER name enable_trigger  */
#line 6283 "gram.y"
                                {
                    AlterEventTrigStmt *n = makeNode(AlterEventTrigStmt);

                    n->trigname = (yyvsp[-1].str);
                    n->tgenabled = (yyvsp[0].chr);
                    (yyval.node) = (Node *) n;
                }
#line 38021 "gram.c"
    break;

  case 838: /* enable_trigger: ENABLE_P  */
#line 6293 "gram.y"
                                                                        { (yyval.chr) = TRIGGER_FIRES_ON_ORIGIN; }
#line 38027 "gram.c"
    break;

  case 839: /* enable_trigger: ENABLE_P REPLICA  */
#line 6294 "gram.y"
                                                                { (yyval.chr) = TRIGGER_FIRES_ON_REPLICA; }
#line 38033 "gram.c"
    break;

  case 840: /* enable_trigger: ENABLE_P ALWAYS  */
#line 6295 "gram.y"
                                                                { (yyval.chr) = TRIGGER_FIRES_ALWAYS; }
#line 38039 "gram.c"
    break;

  case 841: /* enable_trigger: DISABLE_P  */
#line 6296 "gram.y"
                                                                        { (yyval.chr) = TRIGGER_DISABLED; }
#line 38045 "gram.c"
    break;

  case 842: /* CreateAssertionStmt: CREATE ASSERTION any_name CHECK '(' a_expr ')' ConstraintAttributeSpec  */
#line 6308 "gram.y"
                                {
                    ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("CREATE ASSERTION is not yet implemented")));

                    (yyval.node) = NULL;
                }
#line 38057 "gram.c"
    break;

  case 843: /* DefineStmt: CREATE opt_or_replace AGGREGATE func_name aggr_args definition  */
#line 6327 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_AGGREGATE;
                    n->oldstyle = false;
                    n->replace = (yyvsp[-4].boolean);
                    n->defnames = (yyvsp[-2].list);
                    n->args = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38073 "gram.c"
    break;

  case 844: /* DefineStmt: CREATE opt_or_replace AGGREGATE func_name old_aggr_definition  */
#line 6339 "gram.y"
                                {
                    /* old-style (pre-8.2) syntax for CREATE AGGREGATE */
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_AGGREGATE;
                    n->oldstyle = true;
                    n->replace = (yyvsp[-3].boolean);
                    n->defnames = (yyvsp[-1].list);
                    n->args = NIL;
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38090 "gram.c"
    break;

  case 845: /* DefineStmt: CREATE OPERATOR any_operator definition  */
#line 6352 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_OPERATOR;
                    n->oldstyle = false;
                    n->defnames = (yyvsp[-1].list);
                    n->args = NIL;
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38105 "gram.c"
    break;

  case 846: /* DefineStmt: CREATE TYPE_P any_name definition  */
#line 6363 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TYPE;
                    n->oldstyle = false;
                    n->defnames = (yyvsp[-1].list);
                    n->args = NIL;
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38120 "gram.c"
    break;

  case 847: /* DefineStmt: CREATE TYPE_P any_name  */
#line 6374 "gram.y"
                                {
                    /* Shell type (identified by lack of definition) */
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TYPE;
                    n->oldstyle = false;
                    n->defnames = (yyvsp[0].list);
                    n->args = NIL;
                    n->definition = NIL;
                    (yyval.node) = (Node *) n;
                }
#line 38136 "gram.c"
    break;

  case 848: /* DefineStmt: CREATE TYPE_P any_name AS '(' OptTableFuncElementList ')'  */
#line 6386 "gram.y"
                                {
                    CompositeTypeStmt *n = makeNode(CompositeTypeStmt);

                    /* can't use qualified_name, sigh */
                    n->typevar = makeRangeVarFromAnyName((yyvsp[-4].list), (yylsp[-4]), yyscanner);
                    n->coldeflist = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38149 "gram.c"
    break;

  case 849: /* DefineStmt: CREATE TYPE_P any_name AS ENUM_P '(' opt_enum_val_list ')'  */
#line 6395 "gram.y"
                                {
                    CreateEnumStmt *n = makeNode(CreateEnumStmt);

                    n->typeName = (yyvsp[-5].list);
                    n->vals = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38161 "gram.c"
    break;

  case 850: /* DefineStmt: CREATE TYPE_P any_name AS RANGE definition  */
#line 6403 "gram.y"
                                {
                    CreateRangeStmt *n = makeNode(CreateRangeStmt);

                    n->typeName = (yyvsp[-3].list);
                    n->params = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38173 "gram.c"
    break;

  case 851: /* DefineStmt: CREATE TEXT_P SEARCH PARSER any_name definition  */
#line 6411 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TSPARSER;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38187 "gram.c"
    break;

  case 852: /* DefineStmt: CREATE TEXT_P SEARCH DICTIONARY any_name definition  */
#line 6421 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TSDICTIONARY;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38201 "gram.c"
    break;

  case 853: /* DefineStmt: CREATE TEXT_P SEARCH TEMPLATE any_name definition  */
#line 6431 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TSTEMPLATE;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38215 "gram.c"
    break;

  case 854: /* DefineStmt: CREATE TEXT_P SEARCH CONFIGURATION any_name definition  */
#line 6441 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_TSCONFIGURATION;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38229 "gram.c"
    break;

  case 855: /* DefineStmt: CREATE COLLATION any_name definition  */
#line 6451 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_COLLATION;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38243 "gram.c"
    break;

  case 856: /* DefineStmt: CREATE COLLATION IF_P NOT EXISTS any_name definition  */
#line 6461 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_COLLATION;
                    n->args = NIL;
                    n->defnames = (yyvsp[-1].list);
                    n->definition = (yyvsp[0].list);
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 38258 "gram.c"
    break;

  case 857: /* DefineStmt: CREATE COLLATION any_name FROM any_name  */
#line 6472 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_COLLATION;
                    n->args = NIL;
                    n->defnames = (yyvsp[-2].list);
                    n->definition = list_make1(makeDefElem("from", (Node *) (yyvsp[0].list), (yylsp[0])));
                    (yyval.node) = (Node *) n;
                }
#line 38272 "gram.c"
    break;

  case 858: /* DefineStmt: CREATE COLLATION IF_P NOT EXISTS any_name FROM any_name  */
#line 6482 "gram.y"
                                {
                    DefineStmt *n = makeNode(DefineStmt);

                    n->kind = OBJECT_COLLATION;
                    n->args = NIL;
                    n->defnames = (yyvsp[-2].list);
                    n->definition = list_make1(makeDefElem("from", (Node *) (yyvsp[0].list), (yylsp[0])));
                    n->if_not_exists = true;
                    (yyval.node) = (Node *) n;
                }
#line 38287 "gram.c"
    break;

  case 859: /* definition: '(' def_list ')'  */
#line 6494 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 38293 "gram.c"
    break;

  case 860: /* def_list: def_elem  */
#line 6497 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 38299 "gram.c"
    break;

  case 861: /* def_list: def_list ',' def_elem  */
#line 6498 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 38305 "gram.c"
    break;

  case 862: /* def_elem: ColLabel '=' def_arg  */
#line 6502 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 38313 "gram.c"
    break;

  case 863: /* def_elem: ColLabel  */
#line 6506 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[0].str), NULL, (yylsp[0]));
                }
#line 38321 "gram.c"
    break;

  case 864: /* def_arg: func_type  */
#line 6512 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].typnam); }
#line 38327 "gram.c"
    break;

  case 865: /* def_arg: reserved_keyword  */
#line 6513 "gram.y"
                                                                        { (yyval.node) = (Node *) makeString(pstrdup((yyvsp[0].keyword))); }
#line 38333 "gram.c"
    break;

  case 866: /* def_arg: qual_all_Op  */
#line 6514 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].list); }
#line 38339 "gram.c"
    break;

  case 867: /* def_arg: NumericOnly  */
#line 6515 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].node); }
#line 38345 "gram.c"
    break;

  case 868: /* def_arg: Sconst  */
#line 6516 "gram.y"
                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 38351 "gram.c"
    break;

  case 869: /* def_arg: NONE  */
#line 6517 "gram.y"
                                                                                { (yyval.node) = (Node *) makeString(pstrdup((yyvsp[0].keyword))); }
#line 38357 "gram.c"
    break;

  case 870: /* old_aggr_definition: '(' old_aggr_list ')'  */
#line 6520 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 38363 "gram.c"
    break;

  case 871: /* old_aggr_list: old_aggr_elem  */
#line 6523 "gram.y"
                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 38369 "gram.c"
    break;

  case 872: /* old_aggr_list: old_aggr_list ',' old_aggr_elem  */
#line 6524 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 38375 "gram.c"
    break;

  case 873: /* old_aggr_elem: IDENT '=' def_arg  */
#line 6533 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), (Node *) (yyvsp[0].node), (yylsp[-2]));
                }
#line 38383 "gram.c"
    break;

  case 874: /* opt_enum_val_list: enum_val_list  */
#line 6539 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 38389 "gram.c"
    break;

  case 875: /* opt_enum_val_list: %empty  */
#line 6540 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 38395 "gram.c"
    break;

  case 876: /* enum_val_list: Sconst  */
#line 6544 "gram.y"
                                { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 38401 "gram.c"
    break;

  case 877: /* enum_val_list: enum_val_list ',' Sconst  */
#line 6546 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-2].list), makeString((yyvsp[0].str))); }
#line 38407 "gram.c"
    break;

  case 878: /* AlterEnumStmt: ALTER TYPE_P any_name ADD_P VALUE_P opt_if_not_exists Sconst  */
#line 6557 "gram.y"
                        {
                AlterEnumStmt *n = makeNode(AlterEnumStmt);

                n->typeName = (yyvsp[-4].list);
                n->oldVal = NULL;
                n->newVal = (yyvsp[0].str);
                n->newValNeighbor = NULL;
                n->newValIsAfter = true;
                n->skipIfNewValExists = (yyvsp[-1].boolean);
                (yyval.node) = (Node *) n;
            }
#line 38423 "gram.c"
    break;

  case 879: /* AlterEnumStmt: ALTER TYPE_P any_name ADD_P VALUE_P opt_if_not_exists Sconst BEFORE Sconst  */
#line 6569 "gram.y"
                        {
                AlterEnumStmt *n = makeNode(AlterEnumStmt);

                n->typeName = (yyvsp[-6].list);
                n->oldVal = NULL;
                n->newVal = (yyvsp[-2].str);
                n->newValNeighbor = (yyvsp[0].str);
                n->newValIsAfter = false;
                n->skipIfNewValExists = (yyvsp[-3].boolean);
                (yyval.node) = (Node *) n;
            }
#line 38439 "gram.c"
    break;

  case 880: /* AlterEnumStmt: ALTER TYPE_P any_name ADD_P VALUE_P opt_if_not_exists Sconst AFTER Sconst  */
#line 6581 "gram.y"
                        {
                AlterEnumStmt *n = makeNode(AlterEnumStmt);

                n->typeName = (yyvsp[-6].list);
                n->oldVal = NULL;
                n->newVal = (yyvsp[-2].str);
                n->newValNeighbor = (yyvsp[0].str);
                n->newValIsAfter = true;
                n->skipIfNewValExists = (yyvsp[-3].boolean);
                (yyval.node) = (Node *) n;
            }
#line 38455 "gram.c"
    break;

  case 881: /* AlterEnumStmt: ALTER TYPE_P any_name RENAME VALUE_P Sconst TO Sconst  */
#line 6593 "gram.y"
                        {
                AlterEnumStmt *n = makeNode(AlterEnumStmt);

                n->typeName = (yyvsp[-5].list);
                n->oldVal = (yyvsp[-2].str);
                n->newVal = (yyvsp[0].str);
                n->newValNeighbor = NULL;
                n->newValIsAfter = false;
                n->skipIfNewValExists = false;
                (yyval.node) = (Node *) n;
            }
#line 38471 "gram.c"
    break;

  case 882: /* AlterEnumStmt: ALTER TYPE_P any_name DROP VALUE_P Sconst  */
#line 6605 "gram.y"
                        {
                /*
                 * The following problems must be solved before this can be
                 * implemented:
                 *
                 * - There must be no instance of the target value in
                 *   any table.
                 *
                 * - The value must not appear in any catalog metadata,
                 *   such as stored view expressions or column defaults.
                 *
                 * - The value must not appear in any non-leaf page of a
                 *   btree (and similar issues with other index types).
                 *   This is problematic because a value could persist
                 *   there long after it's gone from user-visible data.
                 *
                 * - Concurrent sessions must not be able to insert the
                 *   value while the preceding conditions are being checked.
                 *
                 * - Possibly more...
                 */
                ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                         errmsg("dropping an enum value is not implemented"),
                         parser_errposition((yylsp[-2]))));
            }
#line 38502 "gram.c"
    break;

  case 883: /* opt_if_not_exists: IF_P NOT EXISTS  */
#line 6633 "gram.y"
                                                { (yyval.boolean) = true; }
#line 38508 "gram.c"
    break;

  case 884: /* opt_if_not_exists: %empty  */
#line 6634 "gram.y"
                                                       { (yyval.boolean) = false; }
#line 38514 "gram.c"
    break;

  case 885: /* CreateOpClassStmt: CREATE OPERATOR CLASS any_name opt_default FOR TYPE_P Typename USING name opt_opfamily AS opclass_item_list  */
#line 6652 "gram.y"
                                {
                    CreateOpClassStmt *n = makeNode(CreateOpClassStmt);

                    n->opclassname = (yyvsp[-9].list);
                    n->isDefault = (yyvsp[-8].boolean);
                    n->datatype = (yyvsp[-5].typnam);
                    n->amname = (yyvsp[-3].str);
                    n->opfamilyname = (yyvsp[-2].list);
                    n->items = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38530 "gram.c"
    break;

  case 886: /* opclass_item_list: opclass_item  */
#line 6666 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 38536 "gram.c"
    break;

  case 887: /* opclass_item_list: opclass_item_list ',' opclass_item  */
#line 6667 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 38542 "gram.c"
    break;

  case 888: /* opclass_item: OPERATOR Iconst any_operator opclass_purpose opt_recheck  */
#line 6672 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);
                    ObjectWithArgs *owa = makeNode(ObjectWithArgs);

                    owa->objname = (yyvsp[-2].list);
                    owa->objargs = NIL;
                    n->itemtype = OPCLASS_ITEM_OPERATOR;
                    n->name = owa;
                    n->number = (yyvsp[-3].ival);
                    n->order_family = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38559 "gram.c"
    break;

  case 889: /* opclass_item: OPERATOR Iconst operator_with_argtypes opclass_purpose opt_recheck  */
#line 6686 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_OPERATOR;
                    n->name = (yyvsp[-2].objwithargs);
                    n->number = (yyvsp[-3].ival);
                    n->order_family = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38573 "gram.c"
    break;

  case 890: /* opclass_item: FUNCTION Iconst function_with_argtypes  */
#line 6696 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_FUNCTION;
                    n->name = (yyvsp[0].objwithargs);
                    n->number = (yyvsp[-1].ival);
                    (yyval.node) = (Node *) n;
                }
#line 38586 "gram.c"
    break;

  case 891: /* opclass_item: FUNCTION Iconst '(' type_list ')' function_with_argtypes  */
#line 6705 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_FUNCTION;
                    n->name = (yyvsp[0].objwithargs);
                    n->number = (yyvsp[-4].ival);
                    n->class_args = (yyvsp[-2].list);
                    (yyval.node) = (Node *) n;
                }
#line 38600 "gram.c"
    break;

  case 892: /* opclass_item: STORAGE Typename  */
#line 6715 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_STORAGETYPE;
                    n->storedtype = (yyvsp[0].typnam);
                    (yyval.node) = (Node *) n;
                }
#line 38612 "gram.c"
    break;

  case 893: /* opt_default: DEFAULT  */
#line 6724 "gram.y"
                                                                { (yyval.boolean) = true; }
#line 38618 "gram.c"
    break;

  case 894: /* opt_default: %empty  */
#line 6725 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 38624 "gram.c"
    break;

  case 895: /* opt_opfamily: FAMILY any_name  */
#line 6728 "gram.y"
                                                        { (yyval.list) = (yyvsp[0].list); }
#line 38630 "gram.c"
    break;

  case 896: /* opt_opfamily: %empty  */
#line 6729 "gram.y"
                                                                                { (yyval.list) = NIL; }
#line 38636 "gram.c"
    break;

  case 897: /* opclass_purpose: FOR SEARCH  */
#line 6732 "gram.y"
                                                                { (yyval.list) = NIL; }
#line 38642 "gram.c"
    break;

  case 898: /* opclass_purpose: FOR ORDER BY any_name  */
#line 6733 "gram.y"
                                                                { (yyval.list) = (yyvsp[0].list); }
#line 38648 "gram.c"
    break;

  case 899: /* opclass_purpose: %empty  */
#line 6734 "gram.y"
                                                                                { (yyval.list) = NIL; }
#line 38654 "gram.c"
    break;

  case 900: /* opt_recheck: RECHECK  */
#line 6738 "gram.y"
                                {
                    /*
                     * RECHECK no longer does anything in opclass definitions,
                     * but we still accept it to ease porting of old database
                     * dumps.
                     */
                    ereport(NOTICE,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("RECHECK is no longer required"),
                             errhint("Update your data type."),
                             parser_errposition((yylsp[0]))));
                    (yyval.boolean) = true;
                }
#line 38672 "gram.c"
    break;

  case 901: /* opt_recheck: %empty  */
#line 6751 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 38678 "gram.c"
    break;

  case 902: /* CreateOpFamilyStmt: CREATE OPERATOR FAMILY any_name USING name  */
#line 6757 "gram.y"
                                {
                    CreateOpFamilyStmt *n = makeNode(CreateOpFamilyStmt);

                    n->opfamilyname = (yyvsp[-2].list);
                    n->amname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 38690 "gram.c"
    break;

  case 903: /* AlterOpFamilyStmt: ALTER OPERATOR FAMILY any_name USING name ADD_P opclass_item_list  */
#line 6768 "gram.y"
                                {
                    AlterOpFamilyStmt *n = makeNode(AlterOpFamilyStmt);

                    n->opfamilyname = (yyvsp[-4].list);
                    n->amname = (yyvsp[-2].str);
                    n->isDrop = false;
                    n->items = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38704 "gram.c"
    break;

  case 904: /* AlterOpFamilyStmt: ALTER OPERATOR FAMILY any_name USING name DROP opclass_drop_list  */
#line 6778 "gram.y"
                                {
                    AlterOpFamilyStmt *n = makeNode(AlterOpFamilyStmt);

                    n->opfamilyname = (yyvsp[-4].list);
                    n->amname = (yyvsp[-2].str);
                    n->isDrop = true;
                    n->items = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 38718 "gram.c"
    break;

  case 905: /* opclass_drop_list: opclass_drop  */
#line 6790 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 38724 "gram.c"
    break;

  case 906: /* opclass_drop_list: opclass_drop_list ',' opclass_drop  */
#line 6791 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 38730 "gram.c"
    break;

  case 907: /* opclass_drop: OPERATOR Iconst '(' type_list ')'  */
#line 6796 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_OPERATOR;
                    n->number = (yyvsp[-3].ival);
                    n->class_args = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38743 "gram.c"
    break;

  case 908: /* opclass_drop: FUNCTION Iconst '(' type_list ')'  */
#line 6805 "gram.y"
                                {
                    CreateOpClassItem *n = makeNode(CreateOpClassItem);

                    n->itemtype = OPCLASS_ITEM_FUNCTION;
                    n->number = (yyvsp[-3].ival);
                    n->class_args = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 38756 "gram.c"
    break;

  case 909: /* DropOpClassStmt: DROP OPERATOR CLASS any_name USING name opt_drop_behavior  */
#line 6818 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->objects = list_make1(lcons(makeString((yyvsp[-1].str)), (yyvsp[-3].list)));
                    n->removeType = OBJECT_OPCLASS;
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38771 "gram.c"
    break;

  case 910: /* DropOpClassStmt: DROP OPERATOR CLASS IF_P EXISTS any_name USING name opt_drop_behavior  */
#line 6829 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->objects = list_make1(lcons(makeString((yyvsp[-1].str)), (yyvsp[-3].list)));
                    n->removeType = OBJECT_OPCLASS;
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38786 "gram.c"
    break;

  case 911: /* DropOpFamilyStmt: DROP OPERATOR FAMILY any_name USING name opt_drop_behavior  */
#line 6843 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->objects = list_make1(lcons(makeString((yyvsp[-1].str)), (yyvsp[-3].list)));
                    n->removeType = OBJECT_OPFAMILY;
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38801 "gram.c"
    break;

  case 912: /* DropOpFamilyStmt: DROP OPERATOR FAMILY IF_P EXISTS any_name USING name opt_drop_behavior  */
#line 6854 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->objects = list_make1(lcons(makeString((yyvsp[-1].str)), (yyvsp[-3].list)));
                    n->removeType = OBJECT_OPFAMILY;
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38816 "gram.c"
    break;

  case 913: /* DropOwnedStmt: DROP OWNED BY role_list opt_drop_behavior  */
#line 6877 "gram.y"
                                {
                    DropOwnedStmt *n = makeNode(DropOwnedStmt);

                    n->roles = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 38828 "gram.c"
    break;

  case 914: /* ReassignOwnedStmt: REASSIGN OWNED BY role_list TO RoleSpec  */
#line 6888 "gram.y"
                                {
                    ReassignOwnedStmt *n = makeNode(ReassignOwnedStmt);

                    n->roles = (yyvsp[-2].list);
                    n->newrole = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 38840 "gram.c"
    break;

  case 915: /* DropStmt: DROP object_type_any_name IF_P EXISTS any_name_list opt_drop_behavior  */
#line 6907 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-4].objtype);
                    n->missing_ok = true;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38855 "gram.c"
    break;

  case 916: /* DropStmt: DROP object_type_any_name any_name_list opt_drop_behavior  */
#line 6918 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-2].objtype);
                    n->missing_ok = false;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38870 "gram.c"
    break;

  case 917: /* DropStmt: DROP drop_type_name IF_P EXISTS name_list opt_drop_behavior  */
#line 6929 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-4].objtype);
                    n->missing_ok = true;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38885 "gram.c"
    break;

  case 918: /* DropStmt: DROP drop_type_name name_list opt_drop_behavior  */
#line 6940 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-2].objtype);
                    n->missing_ok = false;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38900 "gram.c"
    break;

  case 919: /* DropStmt: DROP object_type_name_on_any_name name ON any_name opt_drop_behavior  */
#line 6951 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-4].objtype);
                    n->objects = list_make1(lappend((yyvsp[-1].list), makeString((yyvsp[-3].str))));
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38915 "gram.c"
    break;

  case 920: /* DropStmt: DROP object_type_name_on_any_name IF_P EXISTS name ON any_name opt_drop_behavior  */
#line 6962 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = (yyvsp[-6].objtype);
                    n->objects = list_make1(lappend((yyvsp[-1].list), makeString((yyvsp[-3].str))));
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38930 "gram.c"
    break;

  case 921: /* DropStmt: DROP TYPE_P type_name_list opt_drop_behavior  */
#line 6973 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_TYPE;
                    n->missing_ok = false;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38945 "gram.c"
    break;

  case 922: /* DropStmt: DROP TYPE_P IF_P EXISTS type_name_list opt_drop_behavior  */
#line 6984 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_TYPE;
                    n->missing_ok = true;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38960 "gram.c"
    break;

  case 923: /* DropStmt: DROP DOMAIN_P type_name_list opt_drop_behavior  */
#line 6995 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_DOMAIN;
                    n->missing_ok = false;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38975 "gram.c"
    break;

  case 924: /* DropStmt: DROP DOMAIN_P IF_P EXISTS type_name_list opt_drop_behavior  */
#line 7006 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_DOMAIN;
                    n->missing_ok = true;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 38990 "gram.c"
    break;

  case 925: /* DropStmt: DROP INDEX CONCURRENTLY any_name_list opt_drop_behavior  */
#line 7017 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_INDEX;
                    n->missing_ok = false;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = true;
                    (yyval.node) = (Node *) n;
                }
#line 39005 "gram.c"
    break;

  case 926: /* DropStmt: DROP INDEX CONCURRENTLY IF_P EXISTS any_name_list opt_drop_behavior  */
#line 7028 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_INDEX;
                    n->missing_ok = true;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->concurrent = true;
                    (yyval.node) = (Node *) n;
                }
#line 39020 "gram.c"
    break;

  case 927: /* object_type_any_name: TABLE  */
#line 7042 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_TABLE; }
#line 39026 "gram.c"
    break;

  case 928: /* object_type_any_name: SEQUENCE  */
#line 7043 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_SEQUENCE; }
#line 39032 "gram.c"
    break;

  case 929: /* object_type_any_name: VIEW  */
#line 7044 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_VIEW; }
#line 39038 "gram.c"
    break;

  case 930: /* object_type_any_name: MATERIALIZED VIEW  */
#line 7045 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_MATVIEW; }
#line 39044 "gram.c"
    break;

  case 931: /* object_type_any_name: INDEX  */
#line 7046 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_INDEX; }
#line 39050 "gram.c"
    break;

  case 932: /* object_type_any_name: FOREIGN TABLE  */
#line 7047 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_FOREIGN_TABLE; }
#line 39056 "gram.c"
    break;

  case 933: /* object_type_any_name: COLLATION  */
#line 7048 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_COLLATION; }
#line 39062 "gram.c"
    break;

  case 934: /* object_type_any_name: CONVERSION_P  */
#line 7049 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_CONVERSION; }
#line 39068 "gram.c"
    break;

  case 935: /* object_type_any_name: STATISTICS  */
#line 7050 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_STATISTIC_EXT; }
#line 39074 "gram.c"
    break;

  case 936: /* object_type_any_name: TEXT_P SEARCH PARSER  */
#line 7051 "gram.y"
                                                                                { (yyval.objtype) = OBJECT_TSPARSER; }
#line 39080 "gram.c"
    break;

  case 937: /* object_type_any_name: TEXT_P SEARCH DICTIONARY  */
#line 7052 "gram.y"
                                                                                { (yyval.objtype) = OBJECT_TSDICTIONARY; }
#line 39086 "gram.c"
    break;

  case 938: /* object_type_any_name: TEXT_P SEARCH TEMPLATE  */
#line 7053 "gram.y"
                                                                                { (yyval.objtype) = OBJECT_TSTEMPLATE; }
#line 39092 "gram.c"
    break;

  case 939: /* object_type_any_name: TEXT_P SEARCH CONFIGURATION  */
#line 7054 "gram.y"
                                                                        { (yyval.objtype) = OBJECT_TSCONFIGURATION; }
#line 39098 "gram.c"
    break;

  case 940: /* object_type_name: drop_type_name  */
#line 7064 "gram.y"
                                                                                        { (yyval.objtype) = (yyvsp[0].objtype); }
#line 39104 "gram.c"
    break;

  case 941: /* object_type_name: DATABASE  */
#line 7065 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_DATABASE; }
#line 39110 "gram.c"
    break;

  case 942: /* object_type_name: ROLE  */
#line 7066 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_ROLE; }
#line 39116 "gram.c"
    break;

  case 943: /* object_type_name: SUBSCRIPTION  */
#line 7067 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_SUBSCRIPTION; }
#line 39122 "gram.c"
    break;

  case 944: /* object_type_name: TABLESPACE  */
#line 7068 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_TABLESPACE; }
#line 39128 "gram.c"
    break;

  case 945: /* drop_type_name: ACCESS METHOD  */
#line 7072 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_ACCESS_METHOD; }
#line 39134 "gram.c"
    break;

  case 946: /* drop_type_name: EVENT TRIGGER  */
#line 7073 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_EVENT_TRIGGER; }
#line 39140 "gram.c"
    break;

  case 947: /* drop_type_name: EXTENSION  */
#line 7074 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_EXTENSION; }
#line 39146 "gram.c"
    break;

  case 948: /* drop_type_name: FOREIGN DATA_P WRAPPER  */
#line 7075 "gram.y"
                                                                                { (yyval.objtype) = OBJECT_FDW; }
#line 39152 "gram.c"
    break;

  case 949: /* drop_type_name: opt_procedural LANGUAGE  */
#line 7076 "gram.y"
                                                                                { (yyval.objtype) = OBJECT_LANGUAGE; }
#line 39158 "gram.c"
    break;

  case 950: /* drop_type_name: PUBLICATION  */
#line 7077 "gram.y"
                                                                                        { (yyval.objtype) = OBJECT_PUBLICATION; }
#line 39164 "gram.c"
    break;

  case 951: /* drop_type_name: SCHEMA  */
#line 7078 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_SCHEMA; }
#line 39170 "gram.c"
    break;

  case 952: /* drop_type_name: SERVER  */
#line 7079 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_FOREIGN_SERVER; }
#line 39176 "gram.c"
    break;

  case 953: /* object_type_name_on_any_name: POLICY  */
#line 7084 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_POLICY; }
#line 39182 "gram.c"
    break;

  case 954: /* object_type_name_on_any_name: RULE  */
#line 7085 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_RULE; }
#line 39188 "gram.c"
    break;

  case 955: /* object_type_name_on_any_name: TRIGGER  */
#line 7086 "gram.y"
                                                                                                { (yyval.objtype) = OBJECT_TRIGGER; }
#line 39194 "gram.c"
    break;

  case 956: /* any_name_list: any_name  */
#line 7090 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].list)); }
#line 39200 "gram.c"
    break;

  case 957: /* any_name_list: any_name_list ',' any_name  */
#line 7091 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].list)); }
#line 39206 "gram.c"
    break;

  case 958: /* any_name: ColId  */
#line 7094 "gram.y"
                                                                { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 39212 "gram.c"
    break;

  case 959: /* any_name: ColId attrs  */
#line 7095 "gram.y"
                                                                { (yyval.list) = lcons(makeString((yyvsp[-1].str)), (yyvsp[0].list)); }
#line 39218 "gram.c"
    break;

  case 960: /* attrs: '.' attr_name  */
#line 7099 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 39224 "gram.c"
    break;

  case 961: /* attrs: attrs '.' attr_name  */
#line 7101 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), makeString((yyvsp[0].str))); }
#line 39230 "gram.c"
    break;

  case 962: /* type_name_list: Typename  */
#line 7105 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].typnam)); }
#line 39236 "gram.c"
    break;

  case 963: /* type_name_list: type_name_list ',' Typename  */
#line 7106 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].typnam)); }
#line 39242 "gram.c"
    break;

  case 964: /* TruncateStmt: TRUNCATE opt_table relation_expr_list opt_restart_seqs opt_drop_behavior  */
#line 7118 "gram.y"
                                {
                    TruncateStmt *n = makeNode(TruncateStmt);

                    n->relations = (yyvsp[-2].list);
                    n->restart_seqs = (yyvsp[-1].boolean);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 39255 "gram.c"
    break;

  case 965: /* opt_restart_seqs: CONTINUE_P IDENTITY_P  */
#line 7129 "gram.y"
                                                        { (yyval.boolean) = false; }
#line 39261 "gram.c"
    break;

  case 966: /* opt_restart_seqs: RESTART IDENTITY_P  */
#line 7130 "gram.y"
                                                        { (yyval.boolean) = true; }
#line 39267 "gram.c"
    break;

  case 967: /* opt_restart_seqs: %empty  */
#line 7131 "gram.y"
                                                                { (yyval.boolean) = false; }
#line 39273 "gram.c"
    break;

  case 968: /* CommentStmt: COMMENT ON object_type_any_name any_name IS comment_text  */
#line 7142 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = (yyvsp[-3].objtype);
                    n->object = (Node *) (yyvsp[-2].list);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39286 "gram.c"
    break;

  case 969: /* CommentStmt: COMMENT ON COLUMN any_name IS comment_text  */
#line 7151 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_COLUMN;
                    n->object = (Node *) (yyvsp[-2].list);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39299 "gram.c"
    break;

  case 970: /* CommentStmt: COMMENT ON object_type_name name IS comment_text  */
#line 7160 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = (yyvsp[-3].objtype);
                    n->object = (Node *) makeString((yyvsp[-2].str));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39312 "gram.c"
    break;

  case 971: /* CommentStmt: COMMENT ON TYPE_P Typename IS comment_text  */
#line 7169 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[-2].typnam);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39325 "gram.c"
    break;

  case 972: /* CommentStmt: COMMENT ON DOMAIN_P Typename IS comment_text  */
#line 7178 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[-2].typnam);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39338 "gram.c"
    break;

  case 973: /* CommentStmt: COMMENT ON AGGREGATE aggregate_with_argtypes IS comment_text  */
#line 7187 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39351 "gram.c"
    break;

  case 974: /* CommentStmt: COMMENT ON FUNCTION function_with_argtypes IS comment_text  */
#line 7196 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39364 "gram.c"
    break;

  case 975: /* CommentStmt: COMMENT ON OPERATOR operator_with_argtypes IS comment_text  */
#line 7205 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_OPERATOR;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39377 "gram.c"
    break;

  case 976: /* CommentStmt: COMMENT ON CONSTRAINT name ON any_name IS comment_text  */
#line 7214 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_TABCONSTRAINT;
                    n->object = (Node *) lappend((yyvsp[-2].list), makeString((yyvsp[-4].str)));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39390 "gram.c"
    break;

  case 977: /* CommentStmt: COMMENT ON CONSTRAINT name ON DOMAIN_P any_name IS comment_text  */
#line 7223 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_DOMCONSTRAINT;
                    /*
                     * should use Typename not any_name in the production, but
                     * there's a shift/reduce conflict if we do that, so fix it
                     * up here.
                     */
                    n->object = (Node *) list_make2(makeTypeNameFromNameList((yyvsp[-2].list)), makeString((yyvsp[-5].str)));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39408 "gram.c"
    break;

  case 978: /* CommentStmt: COMMENT ON object_type_name_on_any_name name ON any_name IS comment_text  */
#line 7237 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = (yyvsp[-5].objtype);
                    n->object = (Node *) lappend((yyvsp[-2].list), makeString((yyvsp[-4].str)));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39421 "gram.c"
    break;

  case 979: /* CommentStmt: COMMENT ON PROCEDURE function_with_argtypes IS comment_text  */
#line 7246 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39434 "gram.c"
    break;

  case 980: /* CommentStmt: COMMENT ON ROUTINE function_with_argtypes IS comment_text  */
#line 7255 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39447 "gram.c"
    break;

  case 981: /* CommentStmt: COMMENT ON TRANSFORM FOR Typename LANGUAGE name IS comment_text  */
#line 7264 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_TRANSFORM;
                    n->object = (Node *) list_make2((yyvsp[-4].typnam), makeString((yyvsp[-2].str)));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39460 "gram.c"
    break;

  case 982: /* CommentStmt: COMMENT ON OPERATOR CLASS any_name USING name IS comment_text  */
#line 7273 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_OPCLASS;
                    n->object = (Node *) lcons(makeString((yyvsp[-2].str)), (yyvsp[-4].list));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39473 "gram.c"
    break;

  case 983: /* CommentStmt: COMMENT ON OPERATOR FAMILY any_name USING name IS comment_text  */
#line 7282 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_OPFAMILY;
                    n->object = (Node *) lcons(makeString((yyvsp[-2].str)), (yyvsp[-4].list));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39486 "gram.c"
    break;

  case 984: /* CommentStmt: COMMENT ON LARGE_P OBJECT_P NumericOnly IS comment_text  */
#line 7291 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_LARGEOBJECT;
                    n->object = (Node *) (yyvsp[-2].node);
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39499 "gram.c"
    break;

  case 985: /* CommentStmt: COMMENT ON CAST '(' Typename AS Typename ')' IS comment_text  */
#line 7300 "gram.y"
                                {
                    CommentStmt *n = makeNode(CommentStmt);

                    n->objtype = OBJECT_CAST;
                    n->object = (Node *) list_make2((yyvsp[-5].typnam), (yyvsp[-3].typnam));
                    n->comment = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39512 "gram.c"
    break;

  case 986: /* comment_text: Sconst  */
#line 7311 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 39518 "gram.c"
    break;

  case 987: /* comment_text: NULL_P  */
#line 7312 "gram.y"
                                                                                        { (yyval.str) = NULL; }
#line 39524 "gram.c"
    break;

  case 988: /* SecLabelStmt: SECURITY LABEL opt_provider ON object_type_any_name any_name IS security_label  */
#line 7328 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = (yyvsp[-3].objtype);
                    n->object = (Node *) (yyvsp[-2].list);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39538 "gram.c"
    break;

  case 989: /* SecLabelStmt: SECURITY LABEL opt_provider ON COLUMN any_name IS security_label  */
#line 7339 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_COLUMN;
                    n->object = (Node *) (yyvsp[-2].list);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39552 "gram.c"
    break;

  case 990: /* SecLabelStmt: SECURITY LABEL opt_provider ON object_type_name name IS security_label  */
#line 7350 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = (yyvsp[-3].objtype);
                    n->object = (Node *) makeString((yyvsp[-2].str));
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39566 "gram.c"
    break;

  case 991: /* SecLabelStmt: SECURITY LABEL opt_provider ON TYPE_P Typename IS security_label  */
#line 7361 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[-2].typnam);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39580 "gram.c"
    break;

  case 992: /* SecLabelStmt: SECURITY LABEL opt_provider ON DOMAIN_P Typename IS security_label  */
#line 7372 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[-2].typnam);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39594 "gram.c"
    break;

  case 993: /* SecLabelStmt: SECURITY LABEL opt_provider ON AGGREGATE aggregate_with_argtypes IS security_label  */
#line 7383 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39608 "gram.c"
    break;

  case 994: /* SecLabelStmt: SECURITY LABEL opt_provider ON FUNCTION function_with_argtypes IS security_label  */
#line 7394 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39622 "gram.c"
    break;

  case 995: /* SecLabelStmt: SECURITY LABEL opt_provider ON LARGE_P OBJECT_P NumericOnly IS security_label  */
#line 7405 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-6].str);
                    n->objtype = OBJECT_LARGEOBJECT;
                    n->object = (Node *) (yyvsp[-2].node);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39636 "gram.c"
    break;

  case 996: /* SecLabelStmt: SECURITY LABEL opt_provider ON PROCEDURE function_with_argtypes IS security_label  */
#line 7416 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39650 "gram.c"
    break;

  case 997: /* SecLabelStmt: SECURITY LABEL opt_provider ON ROUTINE function_with_argtypes IS security_label  */
#line 7427 "gram.y"
                                {
                    SecLabelStmt *n = makeNode(SecLabelStmt);

                    n->provider = (yyvsp[-5].str);
                    n->objtype = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-2].objwithargs);
                    n->label = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 39664 "gram.c"
    break;

  case 998: /* opt_provider: FOR NonReservedWord_or_Sconst  */
#line 7438 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 39670 "gram.c"
    break;

  case 999: /* opt_provider: %empty  */
#line 7439 "gram.y"
                                                                                { (yyval.str) = NULL; }
#line 39676 "gram.c"
    break;

  case 1000: /* security_label: Sconst  */
#line 7442 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 39682 "gram.c"
    break;

  case 1001: /* security_label: NULL_P  */
#line 7443 "gram.y"
                                                                { (yyval.str) = NULL; }
#line 39688 "gram.c"
    break;

  case 1002: /* FetchStmt: FETCH fetch_args  */
#line 7454 "gram.y"
                                {
                    FetchStmt *n = (FetchStmt *) (yyvsp[0].node);

                    n->ismove = false;
                    (yyval.node) = (Node *) n;
                }
#line 39699 "gram.c"
    break;

  case 1003: /* FetchStmt: MOVE fetch_args  */
#line 7461 "gram.y"
                                {
                    FetchStmt *n = (FetchStmt *) (yyvsp[0].node);

                    n->ismove = true;
                    (yyval.node) = (Node *) n;
                }
#line 39710 "gram.c"
    break;

  case 1004: /* fetch_args: cursor_name  */
#line 7470 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39723 "gram.c"
    break;

  case 1005: /* fetch_args: from_in cursor_name  */
#line 7479 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39736 "gram.c"
    break;

  case 1006: /* fetch_args: NEXT opt_from_in cursor_name  */
#line 7488 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39749 "gram.c"
    break;

  case 1007: /* fetch_args: PRIOR opt_from_in cursor_name  */
#line 7497 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_BACKWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39762 "gram.c"
    break;

  case 1008: /* fetch_args: FIRST_P opt_from_in cursor_name  */
#line 7506 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_ABSOLUTE;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39775 "gram.c"
    break;

  case 1009: /* fetch_args: LAST_P opt_from_in cursor_name  */
#line 7515 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_ABSOLUTE;
                    n->howMany = -1;
                    (yyval.node) = (Node *) n;
                }
#line 39788 "gram.c"
    break;

  case 1010: /* fetch_args: ABSOLUTE_P SignedIconst opt_from_in cursor_name  */
#line 7524 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_ABSOLUTE;
                    n->howMany = (yyvsp[-2].ival);
                    (yyval.node) = (Node *) n;
                }
#line 39801 "gram.c"
    break;

  case 1011: /* fetch_args: RELATIVE_P SignedIconst opt_from_in cursor_name  */
#line 7533 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_RELATIVE;
                    n->howMany = (yyvsp[-2].ival);
                    (yyval.node) = (Node *) n;
                }
#line 39814 "gram.c"
    break;

  case 1012: /* fetch_args: SignedIconst opt_from_in cursor_name  */
#line 7542 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = (yyvsp[-2].ival);
                    (yyval.node) = (Node *) n;
                }
#line 39827 "gram.c"
    break;

  case 1013: /* fetch_args: ALL opt_from_in cursor_name  */
#line 7551 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = FETCH_ALL;
                    (yyval.node) = (Node *) n;
                }
#line 39840 "gram.c"
    break;

  case 1014: /* fetch_args: FORWARD opt_from_in cursor_name  */
#line 7560 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39853 "gram.c"
    break;

  case 1015: /* fetch_args: FORWARD SignedIconst opt_from_in cursor_name  */
#line 7569 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = (yyvsp[-2].ival);
                    (yyval.node) = (Node *) n;
                }
#line 39866 "gram.c"
    break;

  case 1016: /* fetch_args: FORWARD ALL opt_from_in cursor_name  */
#line 7578 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_FORWARD;
                    n->howMany = FETCH_ALL;
                    (yyval.node) = (Node *) n;
                }
#line 39879 "gram.c"
    break;

  case 1017: /* fetch_args: BACKWARD opt_from_in cursor_name  */
#line 7587 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_BACKWARD;
                    n->howMany = 1;
                    (yyval.node) = (Node *) n;
                }
#line 39892 "gram.c"
    break;

  case 1018: /* fetch_args: BACKWARD SignedIconst opt_from_in cursor_name  */
#line 7596 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_BACKWARD;
                    n->howMany = (yyvsp[-2].ival);
                    (yyval.node) = (Node *) n;
                }
#line 39905 "gram.c"
    break;

  case 1019: /* fetch_args: BACKWARD ALL opt_from_in cursor_name  */
#line 7605 "gram.y"
                                {
                    FetchStmt *n = makeNode(FetchStmt);

                    n->portalname = (yyvsp[0].str);
                    n->direction = FETCH_BACKWARD;
                    n->howMany = FETCH_ALL;
                    (yyval.node) = (Node *) n;
                }
#line 39918 "gram.c"
    break;

  case 1024: /* GrantStmt: GRANT privileges ON privilege_target TO grantee_list opt_grant_grant_option opt_granted_by  */
#line 7632 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = true;
                    n->privileges = (yyvsp[-6].list);
                    n->targtype = ((yyvsp[-4].privtarget))->targtype;
                    n->objtype = ((yyvsp[-4].privtarget))->objtype;
                    n->objects = ((yyvsp[-4].privtarget))->objs;
                    n->grantees = (yyvsp[-2].list);
                    n->grant_option = (yyvsp[-1].boolean);
                    n->grantor = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 39936 "gram.c"
    break;

  case 1025: /* RevokeStmt: REVOKE privileges ON privilege_target FROM grantee_list opt_granted_by opt_drop_behavior  */
#line 7650 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = false;
                    n->grant_option = false;
                    n->privileges = (yyvsp[-6].list);
                    n->targtype = ((yyvsp[-4].privtarget))->targtype;
                    n->objtype = ((yyvsp[-4].privtarget))->objtype;
                    n->objects = ((yyvsp[-4].privtarget))->objs;
                    n->grantees = (yyvsp[-2].list);
                    n->grantor = (yyvsp[-1].rolespec);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 39955 "gram.c"
    break;

  case 1026: /* RevokeStmt: REVOKE GRANT OPTION FOR privileges ON privilege_target FROM grantee_list opt_granted_by opt_drop_behavior  */
#line 7666 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = false;
                    n->grant_option = true;
                    n->privileges = (yyvsp[-6].list);
                    n->targtype = ((yyvsp[-4].privtarget))->targtype;
                    n->objtype = ((yyvsp[-4].privtarget))->objtype;
                    n->objects = ((yyvsp[-4].privtarget))->objs;
                    n->grantees = (yyvsp[-2].list);
                    n->grantor = (yyvsp[-1].rolespec);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 39974 "gram.c"
    break;

  case 1027: /* privileges: privilege_list  */
#line 7693 "gram.y"
                                { (yyval.list) = (yyvsp[0].list); }
#line 39980 "gram.c"
    break;

  case 1028: /* privileges: ALL  */
#line 7695 "gram.y"
                                { (yyval.list) = NIL; }
#line 39986 "gram.c"
    break;

  case 1029: /* privileges: ALL PRIVILEGES  */
#line 7697 "gram.y"
                                { (yyval.list) = NIL; }
#line 39992 "gram.c"
    break;

  case 1030: /* privileges: ALL '(' columnList ')'  */
#line 7699 "gram.y"
                                {
                    AccessPriv *n = makeNode(AccessPriv);

                    n->priv_name = NULL;
                    n->cols = (yyvsp[-1].list);
                    (yyval.list) = list_make1(n);
                }
#line 40004 "gram.c"
    break;

  case 1031: /* privileges: ALL PRIVILEGES '(' columnList ')'  */
#line 7707 "gram.y"
                                {
                    AccessPriv *n = makeNode(AccessPriv);

                    n->priv_name = NULL;
                    n->cols = (yyvsp[-1].list);
                    (yyval.list) = list_make1(n);
                }
#line 40016 "gram.c"
    break;

  case 1032: /* privilege_list: privilege  */
#line 7716 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].accesspriv)); }
#line 40022 "gram.c"
    break;

  case 1033: /* privilege_list: privilege_list ',' privilege  */
#line 7717 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].accesspriv)); }
#line 40028 "gram.c"
    break;

  case 1034: /* privilege: SELECT opt_column_list  */
#line 7721 "gram.y"
                        {
                AccessPriv *n = makeNode(AccessPriv);

                n->priv_name = pstrdup((yyvsp[-1].keyword));
                n->cols = (yyvsp[0].list);
                (yyval.accesspriv) = n;
            }
#line 40040 "gram.c"
    break;

  case 1035: /* privilege: REFERENCES opt_column_list  */
#line 7729 "gram.y"
                        {
                AccessPriv *n = makeNode(AccessPriv);

                n->priv_name = pstrdup((yyvsp[-1].keyword));
                n->cols = (yyvsp[0].list);
                (yyval.accesspriv) = n;
            }
#line 40052 "gram.c"
    break;

  case 1036: /* privilege: CREATE opt_column_list  */
#line 7737 "gram.y"
                        {
                AccessPriv *n = makeNode(AccessPriv);

                n->priv_name = pstrdup((yyvsp[-1].keyword));
                n->cols = (yyvsp[0].list);
                (yyval.accesspriv) = n;
            }
#line 40064 "gram.c"
    break;

  case 1037: /* privilege: ALTER SYSTEM_P  */
#line 7745 "gram.y"
                        {
                AccessPriv *n = makeNode(AccessPriv);
                n->priv_name = pstrdup("alter system");
                n->cols = NIL;
                (yyval.accesspriv) = n;
            }
#line 40075 "gram.c"
    break;

  case 1038: /* privilege: ColId opt_column_list  */
#line 7752 "gram.y"
                        {
                AccessPriv *n = makeNode(AccessPriv);

                n->priv_name = (yyvsp[-1].str);
                n->cols = (yyvsp[0].list);
                (yyval.accesspriv) = n;
            }
#line 40087 "gram.c"
    break;

  case 1039: /* parameter_name_list: parameter_name  */
#line 7763 "gram.y"
                        {
                (yyval.list) = list_make1(makeString((yyvsp[0].str)));
            }
#line 40095 "gram.c"
    break;

  case 1040: /* parameter_name_list: parameter_name_list ',' parameter_name  */
#line 7767 "gram.y"
                        {
                (yyval.list) = lappend((yyvsp[-2].list), makeString((yyvsp[0].str)));
            }
#line 40103 "gram.c"
    break;

  case 1041: /* parameter_name: ColId  */
#line 7774 "gram.y"
                        {
                (yyval.str) = (yyvsp[0].str);
            }
#line 40111 "gram.c"
    break;

  case 1042: /* parameter_name: parameter_name '.' ColId  */
#line 7778 "gram.y"
                        {
                (yyval.str) = psprintf("%s.%s", (yyvsp[-2].str), (yyvsp[0].str));
            }
#line 40119 "gram.c"
    break;

  case 1043: /* privilege_target: qualified_name_list  */
#line 7789 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_TABLE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40132 "gram.c"
    break;

  case 1044: /* privilege_target: TABLE qualified_name_list  */
#line 7798 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_TABLE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40145 "gram.c"
    break;

  case 1045: /* privilege_target: SEQUENCE qualified_name_list  */
#line 7807 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_SEQUENCE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40158 "gram.c"
    break;

  case 1046: /* privilege_target: FOREIGN DATA_P WRAPPER name_list  */
#line 7816 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_FDW;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40171 "gram.c"
    break;

  case 1047: /* privilege_target: FOREIGN SERVER name_list  */
#line 7825 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_FOREIGN_SERVER;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40184 "gram.c"
    break;

  case 1048: /* privilege_target: FUNCTION function_with_argtypes_list  */
#line 7834 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_FUNCTION;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40197 "gram.c"
    break;

  case 1049: /* privilege_target: PROCEDURE function_with_argtypes_list  */
#line 7843 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_PROCEDURE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40210 "gram.c"
    break;

  case 1050: /* privilege_target: ROUTINE function_with_argtypes_list  */
#line 7852 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_ROUTINE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40223 "gram.c"
    break;

  case 1051: /* privilege_target: DATABASE name_list  */
#line 7861 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_DATABASE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40236 "gram.c"
    break;

  case 1052: /* privilege_target: DOMAIN_P any_name_list  */
#line 7870 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_DOMAIN;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40249 "gram.c"
    break;

  case 1053: /* privilege_target: LANGUAGE name_list  */
#line 7879 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_LANGUAGE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40262 "gram.c"
    break;

  case 1054: /* privilege_target: LARGE_P OBJECT_P NumericOnly_list  */
#line 7888 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_LARGEOBJECT;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40275 "gram.c"
    break;

  case 1055: /* privilege_target: PARAMETER parameter_name_list  */
#line 7897 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));
                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_PARAMETER_ACL;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40287 "gram.c"
    break;

  case 1056: /* privilege_target: SCHEMA name_list  */
#line 7905 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_SCHEMA;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40300 "gram.c"
    break;

  case 1057: /* privilege_target: TABLESPACE name_list  */
#line 7914 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_TABLESPACE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40313 "gram.c"
    break;

  case 1058: /* privilege_target: TYPE_P any_name_list  */
#line 7923 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_OBJECT;
                    n->objtype = OBJECT_TYPE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40326 "gram.c"
    break;

  case 1059: /* privilege_target: ALL TABLES IN_P SCHEMA name_list  */
#line 7932 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_ALL_IN_SCHEMA;
                    n->objtype = OBJECT_TABLE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40339 "gram.c"
    break;

  case 1060: /* privilege_target: ALL SEQUENCES IN_P SCHEMA name_list  */
#line 7941 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_ALL_IN_SCHEMA;
                    n->objtype = OBJECT_SEQUENCE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40352 "gram.c"
    break;

  case 1061: /* privilege_target: ALL FUNCTIONS IN_P SCHEMA name_list  */
#line 7950 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_ALL_IN_SCHEMA;
                    n->objtype = OBJECT_FUNCTION;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40365 "gram.c"
    break;

  case 1062: /* privilege_target: ALL PROCEDURES IN_P SCHEMA name_list  */
#line 7959 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_ALL_IN_SCHEMA;
                    n->objtype = OBJECT_PROCEDURE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40378 "gram.c"
    break;

  case 1063: /* privilege_target: ALL ROUTINES IN_P SCHEMA name_list  */
#line 7968 "gram.y"
                                {
                    PrivTarget *n = (PrivTarget *) palloc(sizeof(PrivTarget));

                    n->targtype = ACL_TARGET_ALL_IN_SCHEMA;
                    n->objtype = OBJECT_ROUTINE;
                    n->objs = (yyvsp[0].list);
                    (yyval.privtarget) = n;
                }
#line 40391 "gram.c"
    break;

  case 1064: /* grantee_list: grantee  */
#line 7980 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].rolespec)); }
#line 40397 "gram.c"
    break;

  case 1065: /* grantee_list: grantee_list ',' grantee  */
#line 7981 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].rolespec)); }
#line 40403 "gram.c"
    break;

  case 1066: /* grantee: RoleSpec  */
#line 7985 "gram.y"
                                                                                                { (yyval.rolespec) = (yyvsp[0].rolespec); }
#line 40409 "gram.c"
    break;

  case 1067: /* grantee: GROUP_P RoleSpec  */
#line 7986 "gram.y"
                                                                                        { (yyval.rolespec) = (yyvsp[0].rolespec); }
#line 40415 "gram.c"
    break;

  case 1068: /* opt_grant_grant_option: WITH GRANT OPTION  */
#line 7991 "gram.y"
                                          { (yyval.boolean) = true; }
#line 40421 "gram.c"
    break;

  case 1069: /* opt_grant_grant_option: %empty  */
#line 7992 "gram.y"
                                    { (yyval.boolean) = false; }
#line 40427 "gram.c"
    break;

  case 1070: /* GrantRoleStmt: GRANT privilege_list TO role_list opt_granted_by  */
#line 8003 "gram.y"
                                {
                    GrantRoleStmt *n = makeNode(GrantRoleStmt);

                    n->is_grant = true;
                    n->granted_roles = (yyvsp[-3].list);
                    n->grantee_roles = (yyvsp[-1].list);
                    n->opt = NIL;
                    n->grantor = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 40442 "gram.c"
    break;

  case 1071: /* GrantRoleStmt: GRANT privilege_list TO role_list WITH grant_role_opt_list opt_granted_by  */
#line 8014 "gram.y"
                                {
                    GrantRoleStmt *n = makeNode(GrantRoleStmt);

                    n->is_grant = true;
                    n->granted_roles = (yyvsp[-5].list);
                    n->grantee_roles = (yyvsp[-3].list);
                    n->opt = (yyvsp[-1].list);
                    n->grantor = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 40457 "gram.c"
    break;

  case 1072: /* RevokeRoleStmt: REVOKE privilege_list FROM role_list opt_granted_by opt_drop_behavior  */
#line 8028 "gram.y"
                                {
                    GrantRoleStmt *n = makeNode(GrantRoleStmt);

                    n->is_grant = false;
                    n->opt = NIL;
                    n->granted_roles = (yyvsp[-4].list);
                    n->grantee_roles = (yyvsp[-2].list);
                    n->grantor = (yyvsp[-1].rolespec);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 40473 "gram.c"
    break;

  case 1073: /* RevokeRoleStmt: REVOKE ColId OPTION FOR privilege_list FROM role_list opt_granted_by opt_drop_behavior  */
#line 8040 "gram.y"
                                {
                    GrantRoleStmt *n = makeNode(GrantRoleStmt);
                    DefElem *opt;

                    opt = makeDefElem(pstrdup((yyvsp[-7].str)),
                                      (Node *) makeBoolean(false), (yylsp[-7]));
                    n->is_grant = false;
                    n->opt = list_make1(opt);
                    n->granted_roles = (yyvsp[-4].list);
                    n->grantee_roles = (yyvsp[-2].list);
                    n->grantor = (yyvsp[-1].rolespec);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 40492 "gram.c"
    break;

  case 1074: /* grant_role_opt_list: grant_role_opt_list ',' grant_role_opt  */
#line 8057 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 40498 "gram.c"
    break;

  case 1075: /* grant_role_opt_list: grant_role_opt  */
#line 8058 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 40504 "gram.c"
    break;

  case 1076: /* grant_role_opt: ColLabel grant_role_opt_value  */
#line 8063 "gram.y"
                        {
                (yyval.defelt) = makeDefElem(pstrdup((yyvsp[-1].str)), (yyvsp[0].node), (yylsp[-1]));
            }
#line 40512 "gram.c"
    break;

  case 1077: /* grant_role_opt_value: OPTION  */
#line 8069 "gram.y"
                                        { (yyval.node) = (Node *) makeBoolean(true); }
#line 40518 "gram.c"
    break;

  case 1078: /* grant_role_opt_value: TRUE_P  */
#line 8070 "gram.y"
                                        { (yyval.node) = (Node *) makeBoolean(true); }
#line 40524 "gram.c"
    break;

  case 1079: /* grant_role_opt_value: FALSE_P  */
#line 8071 "gram.y"
                                        { (yyval.node) = (Node *) makeBoolean(false); }
#line 40530 "gram.c"
    break;

  case 1080: /* opt_granted_by: GRANTED BY RoleSpec  */
#line 8074 "gram.y"
                                                                                { (yyval.rolespec) = (yyvsp[0].rolespec); }
#line 40536 "gram.c"
    break;

  case 1081: /* opt_granted_by: %empty  */
#line 8075 "gram.y"
                                                                                                        { (yyval.rolespec) = NULL; }
#line 40542 "gram.c"
    break;

  case 1082: /* AlterDefaultPrivilegesStmt: ALTER DEFAULT PRIVILEGES DefACLOptionList DefACLAction  */
#line 8086 "gram.y"
                                {
                    AlterDefaultPrivilegesStmt *n = makeNode(AlterDefaultPrivilegesStmt);

                    n->options = (yyvsp[-1].list);
                    n->action = (GrantStmt *) (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 40554 "gram.c"
    break;

  case 1083: /* DefACLOptionList: DefACLOptionList DefACLOption  */
#line 8096 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 40560 "gram.c"
    break;

  case 1084: /* DefACLOptionList: %empty  */
#line 8097 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 40566 "gram.c"
    break;

  case 1085: /* DefACLOption: IN_P SCHEMA name_list  */
#line 8102 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("schemas", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 40574 "gram.c"
    break;

  case 1086: /* DefACLOption: FOR ROLE role_list  */
#line 8106 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("roles", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 40582 "gram.c"
    break;

  case 1087: /* DefACLOption: FOR USER role_list  */
#line 8110 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("roles", (Node *) (yyvsp[0].list), (yylsp[-2]));
                }
#line 40590 "gram.c"
    break;

  case 1088: /* DefACLAction: GRANT privileges ON defacl_privilege_target TO grantee_list opt_grant_grant_option  */
#line 8122 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = true;
                    n->privileges = (yyvsp[-5].list);
                    n->targtype = ACL_TARGET_DEFAULTS;
                    n->objtype = (yyvsp[-3].ival);
                    n->objects = NIL;
                    n->grantees = (yyvsp[-1].list);
                    n->grant_option = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 40607 "gram.c"
    break;

  case 1089: /* DefACLAction: REVOKE privileges ON defacl_privilege_target FROM grantee_list opt_drop_behavior  */
#line 8136 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = false;
                    n->grant_option = false;
                    n->privileges = (yyvsp[-5].list);
                    n->targtype = ACL_TARGET_DEFAULTS;
                    n->objtype = (yyvsp[-3].ival);
                    n->objects = NIL;
                    n->grantees = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 40625 "gram.c"
    break;

  case 1090: /* DefACLAction: REVOKE GRANT OPTION FOR privileges ON defacl_privilege_target FROM grantee_list opt_drop_behavior  */
#line 8151 "gram.y"
                                {
                    GrantStmt *n = makeNode(GrantStmt);

                    n->is_grant = false;
                    n->grant_option = true;
                    n->privileges = (yyvsp[-5].list);
                    n->targtype = ACL_TARGET_DEFAULTS;
                    n->objtype = (yyvsp[-3].ival);
                    n->objects = NIL;
                    n->grantees = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 40643 "gram.c"
    break;

  case 1091: /* defacl_privilege_target: TABLES  */
#line 8167 "gram.y"
                                                { (yyval.ival) = OBJECT_TABLE; }
#line 40649 "gram.c"
    break;

  case 1092: /* defacl_privilege_target: FUNCTIONS  */
#line 8168 "gram.y"
                                                { (yyval.ival) = OBJECT_FUNCTION; }
#line 40655 "gram.c"
    break;

  case 1093: /* defacl_privilege_target: ROUTINES  */
#line 8169 "gram.y"
                                                { (yyval.ival) = OBJECT_FUNCTION; }
#line 40661 "gram.c"
    break;

  case 1094: /* defacl_privilege_target: SEQUENCES  */
#line 8170 "gram.y"
                                                { (yyval.ival) = OBJECT_SEQUENCE; }
#line 40667 "gram.c"
    break;

  case 1095: /* defacl_privilege_target: TYPES_P  */
#line 8171 "gram.y"
                                                { (yyval.ival) = OBJECT_TYPE; }
#line 40673 "gram.c"
    break;

  case 1096: /* defacl_privilege_target: SCHEMAS  */
#line 8172 "gram.y"
                                                { (yyval.ival) = OBJECT_SCHEMA; }
#line 40679 "gram.c"
    break;

  case 1097: /* IndexStmt: CREATE opt_unique INDEX opt_concurrently opt_single_name ON relation_expr access_method_clause '(' index_params ')' opt_include opt_unique_null_treatment opt_reloptions OptTableSpace where_clause  */
#line 8187 "gram.y"
                                {
                    IndexStmt *n = makeNode(IndexStmt);

                    n->unique = (yyvsp[-14].boolean);
                    n->concurrent = (yyvsp[-12].boolean);
                    n->idxname = (yyvsp[-11].str);
                    n->relation = (yyvsp[-9].range);
                    n->accessMethod = (yyvsp[-8].str);
                    n->indexParams = (yyvsp[-6].list);
                    n->indexIncludingParams = (yyvsp[-4].list);
                    n->nulls_not_distinct = !(yyvsp[-3].boolean);
                    n->options = (yyvsp[-2].list);
                    n->tableSpace = (yyvsp[-1].str);
                    n->whereClause = (yyvsp[0].node);
                    n->excludeOpNames = NIL;
                    n->idxcomment = NULL;
                    n->indexOid = InvalidOid;
                    n->oldNumber = InvalidRelFileNumber;
                    n->oldCreateSubid = InvalidSubTransactionId;
                    n->oldFirstRelfilelocatorSubid = InvalidSubTransactionId;
                    n->primary = false;
                    n->isconstraint = false;
                    n->deferrable = false;
                    n->initdeferred = false;
                    n->transformed = false;
                    n->if_not_exists = false;
                    n->reset_default_tblspc = false;
                    (yyval.node) = (Node *) n;
                }
#line 40713 "gram.c"
    break;

  case 1098: /* IndexStmt: CREATE opt_unique INDEX opt_concurrently IF_P NOT EXISTS name ON relation_expr access_method_clause '(' index_params ')' opt_include opt_unique_null_treatment opt_reloptions OptTableSpace where_clause  */
#line 8219 "gram.y"
                                {
                    IndexStmt *n = makeNode(IndexStmt);

                    n->unique = (yyvsp[-17].boolean);
                    n->concurrent = (yyvsp[-15].boolean);
                    n->idxname = (yyvsp[-11].str);
                    n->relation = (yyvsp[-9].range);
                    n->accessMethod = (yyvsp[-8].str);
                    n->indexParams = (yyvsp[-6].list);
                    n->indexIncludingParams = (yyvsp[-4].list);
                    n->nulls_not_distinct = !(yyvsp[-3].boolean);
                    n->options = (yyvsp[-2].list);
                    n->tableSpace = (yyvsp[-1].str);
                    n->whereClause = (yyvsp[0].node);
                    n->excludeOpNames = NIL;
                    n->idxcomment = NULL;
                    n->indexOid = InvalidOid;
                    n->oldNumber = InvalidRelFileNumber;
                    n->oldCreateSubid = InvalidSubTransactionId;
                    n->oldFirstRelfilelocatorSubid = InvalidSubTransactionId;
                    n->primary = false;
                    n->isconstraint = false;
                    n->deferrable = false;
                    n->initdeferred = false;
                    n->transformed = false;
                    n->if_not_exists = true;
                    n->reset_default_tblspc = false;
                    (yyval.node) = (Node *) n;
                }
#line 40747 "gram.c"
    break;

  case 1099: /* opt_unique: UNIQUE  */
#line 8251 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 40753 "gram.c"
    break;

  case 1100: /* opt_unique: %empty  */
#line 8252 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 40759 "gram.c"
    break;

  case 1101: /* access_method_clause: USING name  */
#line 8256 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 40765 "gram.c"
    break;

  case 1102: /* access_method_clause: %empty  */
#line 8257 "gram.y"
                                                                                                { (yyval.str) = DEFAULT_INDEX_TYPE; }
#line 40771 "gram.c"
    break;

  case 1103: /* index_params: index_elem  */
#line 8260 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].ielem)); }
#line 40777 "gram.c"
    break;

  case 1104: /* index_params: index_params ',' index_elem  */
#line 8261 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].ielem)); }
#line 40783 "gram.c"
    break;

  case 1105: /* index_elem_options: opt_collate opt_qualified_name opt_asc_desc opt_nulls_order  */
#line 8267 "gram.y"
                {
            (yyval.ielem) = makeNode(IndexElem);
            (yyval.ielem)->name = NULL;
            (yyval.ielem)->expr = NULL;
            (yyval.ielem)->indexcolname = NULL;
            (yyval.ielem)->collation = (yyvsp[-3].list);
            (yyval.ielem)->opclass = (yyvsp[-2].list);
            (yyval.ielem)->opclassopts = NIL;
            (yyval.ielem)->ordering = (yyvsp[-1].ival);
            (yyval.ielem)->nulls_ordering = (yyvsp[0].ival);
        }
#line 40799 "gram.c"
    break;

  case 1106: /* index_elem_options: opt_collate any_name reloptions opt_asc_desc opt_nulls_order  */
#line 8279 "gram.y"
                {
            (yyval.ielem) = makeNode(IndexElem);
            (yyval.ielem)->name = NULL;
            (yyval.ielem)->expr = NULL;
            (yyval.ielem)->indexcolname = NULL;
            (yyval.ielem)->collation = (yyvsp[-4].list);
            (yyval.ielem)->opclass = (yyvsp[-3].list);
            (yyval.ielem)->opclassopts = (yyvsp[-2].list);
            (yyval.ielem)->ordering = (yyvsp[-1].ival);
            (yyval.ielem)->nulls_ordering = (yyvsp[0].ival);
        }
#line 40815 "gram.c"
    break;

  case 1107: /* index_elem: ColId index_elem_options  */
#line 8298 "gram.y"
                                {
                    (yyval.ielem) = (yyvsp[0].ielem);
                    (yyval.ielem)->name = (yyvsp[-1].str);
                }
#line 40824 "gram.c"
    break;

  case 1108: /* index_elem: func_expr_windowless index_elem_options  */
#line 8303 "gram.y"
                                {
                    (yyval.ielem) = (yyvsp[0].ielem);
                    (yyval.ielem)->expr = (yyvsp[-1].node);
                }
#line 40833 "gram.c"
    break;

  case 1109: /* index_elem: '(' a_expr ')' index_elem_options  */
#line 8308 "gram.y"
                                {
                    (yyval.ielem) = (yyvsp[0].ielem);
                    (yyval.ielem)->expr = (yyvsp[-2].node);
                }
#line 40842 "gram.c"
    break;

  case 1110: /* opt_include: INCLUDE '(' index_including_params ')'  */
#line 8314 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 40848 "gram.c"
    break;

  case 1111: /* opt_include: %empty  */
#line 8315 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 40854 "gram.c"
    break;

  case 1112: /* index_including_params: index_elem  */
#line 8318 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].ielem)); }
#line 40860 "gram.c"
    break;

  case 1113: /* index_including_params: index_including_params ',' index_elem  */
#line 8319 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].ielem)); }
#line 40866 "gram.c"
    break;

  case 1114: /* opt_collate: COLLATE any_name  */
#line 8322 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 40872 "gram.c"
    break;

  case 1115: /* opt_collate: %empty  */
#line 8323 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 40878 "gram.c"
    break;

  case 1116: /* opt_asc_desc: ASC  */
#line 8327 "gram.y"
                                                                        { (yyval.ival) = SORTBY_ASC; }
#line 40884 "gram.c"
    break;

  case 1117: /* opt_asc_desc: DESC  */
#line 8328 "gram.y"
                                                                                { (yyval.ival) = SORTBY_DESC; }
#line 40890 "gram.c"
    break;

  case 1118: /* opt_asc_desc: %empty  */
#line 8329 "gram.y"
                                                                                { (yyval.ival) = SORTBY_DEFAULT; }
#line 40896 "gram.c"
    break;

  case 1119: /* opt_nulls_order: NULLS_LA FIRST_P  */
#line 8332 "gram.y"
                                                        { (yyval.ival) = SORTBY_NULLS_FIRST; }
#line 40902 "gram.c"
    break;

  case 1120: /* opt_nulls_order: NULLS_LA LAST_P  */
#line 8333 "gram.y"
                                                                        { (yyval.ival) = SORTBY_NULLS_LAST; }
#line 40908 "gram.c"
    break;

  case 1121: /* opt_nulls_order: %empty  */
#line 8334 "gram.y"
                                                                                { (yyval.ival) = SORTBY_NULLS_DEFAULT; }
#line 40914 "gram.c"
    break;

  case 1122: /* CreateFunctionStmt: CREATE opt_or_replace FUNCTION func_name func_args_with_defaults RETURNS func_return opt_createfunc_opt_list opt_routine_body  */
#line 8352 "gram.y"
                                {
                    CreateFunctionStmt *n = makeNode(CreateFunctionStmt);

                    n->is_procedure = false;
                    n->replace = (yyvsp[-7].boolean);
                    n->funcname = (yyvsp[-5].list);
                    n->parameters = (yyvsp[-4].list);
                    n->returnType = (yyvsp[-2].typnam);
                    n->options = (yyvsp[-1].list);
                    n->sql_body = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 40931 "gram.c"
    break;

  case 1123: /* CreateFunctionStmt: CREATE opt_or_replace FUNCTION func_name func_args_with_defaults RETURNS TABLE '(' table_func_column_list ')' opt_createfunc_opt_list opt_routine_body  */
#line 8366 "gram.y"
                                {
                    CreateFunctionStmt *n = makeNode(CreateFunctionStmt);

                    n->is_procedure = false;
                    n->replace = (yyvsp[-10].boolean);
                    n->funcname = (yyvsp[-8].list);
                    n->parameters = mergeTableFuncParameters((yyvsp[-7].list), (yyvsp[-3].list));
                    n->returnType = TableFuncTypeName((yyvsp[-3].list));
                    n->returnType->location = (yylsp[-5]);
                    n->options = (yyvsp[-1].list);
                    n->sql_body = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 40949 "gram.c"
    break;

  case 1124: /* CreateFunctionStmt: CREATE opt_or_replace FUNCTION func_name func_args_with_defaults opt_createfunc_opt_list opt_routine_body  */
#line 8381 "gram.y"
                                {
                    CreateFunctionStmt *n = makeNode(CreateFunctionStmt);

                    n->is_procedure = false;
                    n->replace = (yyvsp[-5].boolean);
                    n->funcname = (yyvsp[-3].list);
                    n->parameters = (yyvsp[-2].list);
                    n->returnType = NULL;
                    n->options = (yyvsp[-1].list);
                    n->sql_body = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 40966 "gram.c"
    break;

  case 1125: /* CreateFunctionStmt: CREATE opt_or_replace PROCEDURE func_name func_args_with_defaults opt_createfunc_opt_list opt_routine_body  */
#line 8395 "gram.y"
                                {
                    CreateFunctionStmt *n = makeNode(CreateFunctionStmt);

                    n->is_procedure = true;
                    n->replace = (yyvsp[-5].boolean);
                    n->funcname = (yyvsp[-3].list);
                    n->parameters = (yyvsp[-2].list);
                    n->returnType = NULL;
                    n->options = (yyvsp[-1].list);
                    n->sql_body = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 40983 "gram.c"
    break;

  case 1126: /* opt_or_replace: OR REPLACE  */
#line 8410 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 40989 "gram.c"
    break;

  case 1127: /* opt_or_replace: %empty  */
#line 8411 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 40995 "gram.c"
    break;

  case 1128: /* func_args: '(' func_args_list ')'  */
#line 8414 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 41001 "gram.c"
    break;

  case 1129: /* func_args: '(' ')'  */
#line 8415 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 41007 "gram.c"
    break;

  case 1130: /* func_args_list: func_arg  */
#line 8419 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].fun_param)); }
#line 41013 "gram.c"
    break;

  case 1131: /* func_args_list: func_args_list ',' func_arg  */
#line 8420 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].fun_param)); }
#line 41019 "gram.c"
    break;

  case 1132: /* function_with_argtypes_list: function_with_argtypes  */
#line 8424 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].objwithargs)); }
#line 41025 "gram.c"
    break;

  case 1133: /* function_with_argtypes_list: function_with_argtypes_list ',' function_with_argtypes  */
#line 8426 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].objwithargs)); }
#line 41031 "gram.c"
    break;

  case 1134: /* function_with_argtypes: func_name func_args  */
#line 8431 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = (yyvsp[-1].list);
                    n->objargs = extractArgTypes((yyvsp[0].list));
                    n->objfuncargs = (yyvsp[0].list);
                    (yyval.objwithargs) = n;
                }
#line 41044 "gram.c"
    break;

  case 1135: /* function_with_argtypes: type_func_name_keyword  */
#line 8445 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = list_make1(makeString(pstrdup((yyvsp[0].keyword))));
                    n->args_unspecified = true;
                    (yyval.objwithargs) = n;
                }
#line 41056 "gram.c"
    break;

  case 1136: /* function_with_argtypes: ColId  */
#line 8453 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = list_make1(makeString((yyvsp[0].str)));
                    n->args_unspecified = true;
                    (yyval.objwithargs) = n;
                }
#line 41068 "gram.c"
    break;

  case 1137: /* function_with_argtypes: ColId indirection  */
#line 8461 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = check_func_name(lcons(makeString((yyvsp[-1].str)), (yyvsp[0].list)),
                                                  yyscanner);
                    n->args_unspecified = true;
                    (yyval.objwithargs) = n;
                }
#line 41081 "gram.c"
    break;

  case 1138: /* func_args_with_defaults: '(' func_args_with_defaults_list ')'  */
#line 8476 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 41087 "gram.c"
    break;

  case 1139: /* func_args_with_defaults: '(' ')'  */
#line 8477 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 41093 "gram.c"
    break;

  case 1140: /* func_args_with_defaults_list: func_arg_with_default  */
#line 8481 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].fun_param)); }
#line 41099 "gram.c"
    break;

  case 1141: /* func_args_with_defaults_list: func_args_with_defaults_list ',' func_arg_with_default  */
#line 8483 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].fun_param)); }
#line 41105 "gram.c"
    break;

  case 1142: /* func_arg: arg_class param_name func_type  */
#line 8498 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = (yyvsp[-1].str);
                    n->argType = (yyvsp[0].typnam);
                    n->mode = (yyvsp[-2].fun_param_mode);
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41119 "gram.c"
    break;

  case 1143: /* func_arg: param_name arg_class func_type  */
#line 8508 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = (yyvsp[-2].str);
                    n->argType = (yyvsp[0].typnam);
                    n->mode = (yyvsp[-1].fun_param_mode);
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41133 "gram.c"
    break;

  case 1144: /* func_arg: param_name func_type  */
#line 8518 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = (yyvsp[-1].str);
                    n->argType = (yyvsp[0].typnam);
                    n->mode = FUNC_PARAM_DEFAULT;
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41147 "gram.c"
    break;

  case 1145: /* func_arg: arg_class func_type  */
#line 8528 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = NULL;
                    n->argType = (yyvsp[0].typnam);
                    n->mode = (yyvsp[-1].fun_param_mode);
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41161 "gram.c"
    break;

  case 1146: /* func_arg: func_type  */
#line 8538 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = NULL;
                    n->argType = (yyvsp[0].typnam);
                    n->mode = FUNC_PARAM_DEFAULT;
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41175 "gram.c"
    break;

  case 1147: /* arg_class: IN_P  */
#line 8550 "gram.y"
                                                                                { (yyval.fun_param_mode) = FUNC_PARAM_IN; }
#line 41181 "gram.c"
    break;

  case 1148: /* arg_class: OUT_P  */
#line 8551 "gram.y"
                                                                                        { (yyval.fun_param_mode) = FUNC_PARAM_OUT; }
#line 41187 "gram.c"
    break;

  case 1149: /* arg_class: INOUT  */
#line 8552 "gram.y"
                                                                                        { (yyval.fun_param_mode) = FUNC_PARAM_INOUT; }
#line 41193 "gram.c"
    break;

  case 1150: /* arg_class: IN_P OUT_P  */
#line 8553 "gram.y"
                                                                                { (yyval.fun_param_mode) = FUNC_PARAM_INOUT; }
#line 41199 "gram.c"
    break;

  case 1151: /* arg_class: VARIADIC  */
#line 8554 "gram.y"
                                                                                        { (yyval.fun_param_mode) = FUNC_PARAM_VARIADIC; }
#line 41205 "gram.c"
    break;

  case 1153: /* func_return: func_type  */
#line 8565 "gram.y"
                                {
                    /* We can catch over-specified results here if we want to,
                     * but for now better to silently swallow typmod, etc.
                     * - thomas 2000-03-22
                     */
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 41217 "gram.c"
    break;

  case 1154: /* func_type: Typename  */
#line 8579 "gram.y"
                                                                                        { (yyval.typnam) = (yyvsp[0].typnam); }
#line 41223 "gram.c"
    break;

  case 1155: /* func_type: type_function_name attrs '%' TYPE_P  */
#line 8581 "gram.y"
                                {
                    (yyval.typnam) = makeTypeNameFromNameList(lcons(makeString((yyvsp[-3].str)), (yyvsp[-2].list)));
                    (yyval.typnam)->pct_type = true;
                    (yyval.typnam)->location = (yylsp[-3]);
                }
#line 41233 "gram.c"
    break;

  case 1156: /* func_type: SETOF type_function_name attrs '%' TYPE_P  */
#line 8587 "gram.y"
                                {
                    (yyval.typnam) = makeTypeNameFromNameList(lcons(makeString((yyvsp[-3].str)), (yyvsp[-2].list)));
                    (yyval.typnam)->pct_type = true;
                    (yyval.typnam)->setof = true;
                    (yyval.typnam)->location = (yylsp[-3]);
                }
#line 41244 "gram.c"
    break;

  case 1157: /* func_arg_with_default: func_arg  */
#line 8597 "gram.y"
                                {
                    (yyval.fun_param) = (yyvsp[0].fun_param);
                }
#line 41252 "gram.c"
    break;

  case 1158: /* func_arg_with_default: func_arg DEFAULT a_expr  */
#line 8601 "gram.y"
                                {
                    (yyval.fun_param) = (yyvsp[-2].fun_param);
                    (yyval.fun_param)->defexpr = (yyvsp[0].node);
                }
#line 41261 "gram.c"
    break;

  case 1159: /* func_arg_with_default: func_arg '=' a_expr  */
#line 8606 "gram.y"
                                {
                    (yyval.fun_param) = (yyvsp[-2].fun_param);
                    (yyval.fun_param)->defexpr = (yyvsp[0].node);
                }
#line 41270 "gram.c"
    break;

  case 1160: /* aggr_arg: func_arg  */
#line 8614 "gram.y"
                                {
                    if (!((yyvsp[0].fun_param)->mode == FUNC_PARAM_DEFAULT ||
                          (yyvsp[0].fun_param)->mode == FUNC_PARAM_IN ||
                          (yyvsp[0].fun_param)->mode == FUNC_PARAM_VARIADIC))
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("aggregates cannot have output arguments"),
                                 parser_errposition((yylsp[0]))));
                    (yyval.fun_param) = (yyvsp[0].fun_param);
                }
#line 41285 "gram.c"
    break;

  case 1161: /* aggr_args: '(' '*' ')'  */
#line 8656 "gram.y"
                                {
                    (yyval.list) = list_make2(NIL, makeInteger(-1));
                }
#line 41293 "gram.c"
    break;

  case 1162: /* aggr_args: '(' aggr_args_list ')'  */
#line 8660 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[-1].list), makeInteger(-1));
                }
#line 41301 "gram.c"
    break;

  case 1163: /* aggr_args: '(' ORDER BY aggr_args_list ')'  */
#line 8664 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[-1].list), makeInteger(0));
                }
#line 41309 "gram.c"
    break;

  case 1164: /* aggr_args: '(' aggr_args_list ORDER BY aggr_args_list ')'  */
#line 8668 "gram.y"
                                {
                    /* this is the only case requiring consistency checking */
                    (yyval.list) = makeOrderedSetArgs((yyvsp[-4].list), (yyvsp[-1].list), yyscanner);
                }
#line 41318 "gram.c"
    break;

  case 1165: /* aggr_args_list: aggr_arg  */
#line 8675 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].fun_param)); }
#line 41324 "gram.c"
    break;

  case 1166: /* aggr_args_list: aggr_args_list ',' aggr_arg  */
#line 8676 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].fun_param)); }
#line 41330 "gram.c"
    break;

  case 1167: /* aggregate_with_argtypes: func_name aggr_args  */
#line 8681 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = (yyvsp[-1].list);
                    n->objargs = extractAggrArgTypes((yyvsp[0].list));
                    n->objfuncargs = (List *) linitial((yyvsp[0].list));
                    (yyval.objwithargs) = n;
                }
#line 41343 "gram.c"
    break;

  case 1168: /* aggregate_with_argtypes_list: aggregate_with_argtypes  */
#line 8692 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].objwithargs)); }
#line 41349 "gram.c"
    break;

  case 1169: /* aggregate_with_argtypes_list: aggregate_with_argtypes_list ',' aggregate_with_argtypes  */
#line 8694 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].objwithargs)); }
#line 41355 "gram.c"
    break;

  case 1171: /* opt_createfunc_opt_list: %empty  */
#line 8699 "gram.y"
                                    { (yyval.list) = NIL; }
#line 41361 "gram.c"
    break;

  case 1172: /* createfunc_opt_list: createfunc_opt_item  */
#line 8704 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 41367 "gram.c"
    break;

  case 1173: /* createfunc_opt_list: createfunc_opt_list createfunc_opt_item  */
#line 8705 "gram.y"
                                                                  { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 41373 "gram.c"
    break;

  case 1174: /* common_func_opt_item: CALLED ON NULL_P INPUT_P  */
#line 8713 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("strict", (Node *) makeBoolean(false), (yylsp[-3]));
                }
#line 41381 "gram.c"
    break;

  case 1175: /* common_func_opt_item: RETURNS NULL_P ON NULL_P INPUT_P  */
#line 8717 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("strict", (Node *) makeBoolean(true), (yylsp[-4]));
                }
#line 41389 "gram.c"
    break;

  case 1176: /* common_func_opt_item: STRICT_P  */
#line 8721 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("strict", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 41397 "gram.c"
    break;

  case 1177: /* common_func_opt_item: IMMUTABLE  */
#line 8725 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("volatility", (Node *) makeString("immutable"), (yylsp[0]));
                }
#line 41405 "gram.c"
    break;

  case 1178: /* common_func_opt_item: STABLE  */
#line 8729 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("volatility", (Node *) makeString("stable"), (yylsp[0]));
                }
#line 41413 "gram.c"
    break;

  case 1179: /* common_func_opt_item: VOLATILE  */
#line 8733 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("volatility", (Node *) makeString("volatile"), (yylsp[0]));
                }
#line 41421 "gram.c"
    break;

  case 1180: /* common_func_opt_item: EXTERNAL SECURITY DEFINER  */
#line 8737 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("security", (Node *) makeBoolean(true), (yylsp[-2]));
                }
#line 41429 "gram.c"
    break;

  case 1181: /* common_func_opt_item: EXTERNAL SECURITY INVOKER  */
#line 8741 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("security", (Node *) makeBoolean(false), (yylsp[-2]));
                }
#line 41437 "gram.c"
    break;

  case 1182: /* common_func_opt_item: SECURITY DEFINER  */
#line 8745 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("security", (Node *) makeBoolean(true), (yylsp[-1]));
                }
#line 41445 "gram.c"
    break;

  case 1183: /* common_func_opt_item: SECURITY INVOKER  */
#line 8749 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("security", (Node *) makeBoolean(false), (yylsp[-1]));
                }
#line 41453 "gram.c"
    break;

  case 1184: /* common_func_opt_item: LEAKPROOF  */
#line 8753 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("leakproof", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 41461 "gram.c"
    break;

  case 1185: /* common_func_opt_item: NOT LEAKPROOF  */
#line 8757 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("leakproof", (Node *) makeBoolean(false), (yylsp[-1]));
                }
#line 41469 "gram.c"
    break;

  case 1186: /* common_func_opt_item: COST NumericOnly  */
#line 8761 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("cost", (Node *) (yyvsp[0].node), (yylsp[-1]));
                }
#line 41477 "gram.c"
    break;

  case 1187: /* common_func_opt_item: ROWS NumericOnly  */
#line 8765 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("rows", (Node *) (yyvsp[0].node), (yylsp[-1]));
                }
#line 41485 "gram.c"
    break;

  case 1188: /* common_func_opt_item: SUPPORT any_name  */
#line 8769 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("support", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 41493 "gram.c"
    break;

  case 1189: /* common_func_opt_item: FunctionSetResetClause  */
#line 8773 "gram.y"
                                {
                    /* we abuse the normal content of a DefElem here */
                    (yyval.defelt) = makeDefElem("set", (Node *) (yyvsp[0].vsetstmt), (yylsp[0]));
                }
#line 41502 "gram.c"
    break;

  case 1190: /* common_func_opt_item: PARALLEL ColId  */
#line 8778 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("parallel", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 41510 "gram.c"
    break;

  case 1191: /* createfunc_opt_item: AS func_as  */
#line 8785 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("as", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 41518 "gram.c"
    break;

  case 1192: /* createfunc_opt_item: LANGUAGE NonReservedWord_or_Sconst  */
#line 8789 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("language", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 41526 "gram.c"
    break;

  case 1193: /* createfunc_opt_item: TRANSFORM transform_type_list  */
#line 8793 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("transform", (Node *) (yyvsp[0].list), (yylsp[-1]));
                }
#line 41534 "gram.c"
    break;

  case 1194: /* createfunc_opt_item: WINDOW  */
#line 8797 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("window", (Node *) makeBoolean(true), (yylsp[0]));
                }
#line 41542 "gram.c"
    break;

  case 1195: /* createfunc_opt_item: common_func_opt_item  */
#line 8801 "gram.y"
                                {
                    (yyval.defelt) = (yyvsp[0].defelt);
                }
#line 41550 "gram.c"
    break;

  case 1196: /* func_as: Sconst  */
#line 8806 "gram.y"
                                                                { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 41556 "gram.c"
    break;

  case 1197: /* func_as: Sconst ',' Sconst  */
#line 8808 "gram.y"
                                {
                    (yyval.list) = list_make2(makeString((yyvsp[-2].str)), makeString((yyvsp[0].str)));
                }
#line 41564 "gram.c"
    break;

  case 1198: /* ReturnStmt: RETURN a_expr  */
#line 8814 "gram.y"
                                {
                    ReturnStmt *r = makeNode(ReturnStmt);

                    r->returnval = (Node *) (yyvsp[0].node);
                    (yyval.node) = (Node *) r;
                }
#line 41575 "gram.c"
    break;

  case 1199: /* opt_routine_body: ReturnStmt  */
#line 8824 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 41583 "gram.c"
    break;

  case 1200: /* opt_routine_body: BEGIN_P ATOMIC routine_body_stmt_list END_P  */
#line 8828 "gram.y"
                                {
                    /*
                     * A compound statement is stored as a single-item list
                     * containing the list of statements as its member.  That
                     * way, the parse analysis code can tell apart an empty
                     * body from no body at all.
                     */
                    (yyval.node) = (Node *) list_make1((yyvsp[-1].list));
                }
#line 41597 "gram.c"
    break;

  case 1201: /* opt_routine_body: %empty  */
#line 8838 "gram.y"
                                {
                    (yyval.node) = NULL;
                }
#line 41605 "gram.c"
    break;

  case 1202: /* routine_body_stmt_list: routine_body_stmt_list routine_body_stmt ';'  */
#line 8845 "gram.y"
                                {
                    /* As in stmtmulti, discard empty statements */
                    if ((yyvsp[-1].node) != NULL)
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[-1].node));
                    else
                        (yyval.list) = (yyvsp[-2].list);
                }
#line 41617 "gram.c"
    break;

  case 1203: /* routine_body_stmt_list: %empty  */
#line 8853 "gram.y"
                                {
                    (yyval.list) = NIL;
                }
#line 41625 "gram.c"
    break;

  case 1206: /* transform_type_list: FOR TYPE_P Typename  */
#line 8864 "gram.y"
                                            { (yyval.list) = list_make1((yyvsp[0].typnam)); }
#line 41631 "gram.c"
    break;

  case 1207: /* transform_type_list: transform_type_list ',' FOR TYPE_P Typename  */
#line 8865 "gram.y"
                                                                      { (yyval.list) = lappend((yyvsp[-4].list), (yyvsp[0].typnam)); }
#line 41637 "gram.c"
    break;

  case 1208: /* opt_definition: WITH definition  */
#line 8869 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 41643 "gram.c"
    break;

  case 1209: /* opt_definition: %empty  */
#line 8870 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 41649 "gram.c"
    break;

  case 1210: /* table_func_column: param_name func_type  */
#line 8874 "gram.y"
                                {
                    FunctionParameter *n = makeNode(FunctionParameter);

                    n->name = (yyvsp[-1].str);
                    n->argType = (yyvsp[0].typnam);
                    n->mode = FUNC_PARAM_TABLE;
                    n->defexpr = NULL;
                    (yyval.fun_param) = n;
                }
#line 41663 "gram.c"
    break;

  case 1211: /* table_func_column_list: table_func_column  */
#line 8887 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].fun_param));
                }
#line 41671 "gram.c"
    break;

  case 1212: /* table_func_column_list: table_func_column_list ',' table_func_column  */
#line 8891 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].fun_param));
                }
#line 41679 "gram.c"
    break;

  case 1213: /* AlterFunctionStmt: ALTER FUNCTION function_with_argtypes alterfunc_opt_list opt_restrict  */
#line 8906 "gram.y"
                                {
                    AlterFunctionStmt *n = makeNode(AlterFunctionStmt);

                    n->objtype = OBJECT_FUNCTION;
                    n->func = (yyvsp[-2].objwithargs);
                    n->actions = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 41692 "gram.c"
    break;

  case 1214: /* AlterFunctionStmt: ALTER PROCEDURE function_with_argtypes alterfunc_opt_list opt_restrict  */
#line 8915 "gram.y"
                                {
                    AlterFunctionStmt *n = makeNode(AlterFunctionStmt);

                    n->objtype = OBJECT_PROCEDURE;
                    n->func = (yyvsp[-2].objwithargs);
                    n->actions = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 41705 "gram.c"
    break;

  case 1215: /* AlterFunctionStmt: ALTER ROUTINE function_with_argtypes alterfunc_opt_list opt_restrict  */
#line 8924 "gram.y"
                                {
                    AlterFunctionStmt *n = makeNode(AlterFunctionStmt);

                    n->objtype = OBJECT_ROUTINE;
                    n->func = (yyvsp[-2].objwithargs);
                    n->actions = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 41718 "gram.c"
    break;

  case 1216: /* alterfunc_opt_list: common_func_opt_item  */
#line 8936 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 41724 "gram.c"
    break;

  case 1217: /* alterfunc_opt_list: alterfunc_opt_list common_func_opt_item  */
#line 8937 "gram.y"
                                                                  { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 41730 "gram.c"
    break;

  case 1220: /* RemoveFuncStmt: DROP FUNCTION function_with_argtypes_list opt_drop_behavior  */
#line 8961 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_FUNCTION;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41745 "gram.c"
    break;

  case 1221: /* RemoveFuncStmt: DROP FUNCTION IF_P EXISTS function_with_argtypes_list opt_drop_behavior  */
#line 8972 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_FUNCTION;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41760 "gram.c"
    break;

  case 1222: /* RemoveFuncStmt: DROP PROCEDURE function_with_argtypes_list opt_drop_behavior  */
#line 8983 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_PROCEDURE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41775 "gram.c"
    break;

  case 1223: /* RemoveFuncStmt: DROP PROCEDURE IF_P EXISTS function_with_argtypes_list opt_drop_behavior  */
#line 8994 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_PROCEDURE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41790 "gram.c"
    break;

  case 1224: /* RemoveFuncStmt: DROP ROUTINE function_with_argtypes_list opt_drop_behavior  */
#line 9005 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_ROUTINE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41805 "gram.c"
    break;

  case 1225: /* RemoveFuncStmt: DROP ROUTINE IF_P EXISTS function_with_argtypes_list opt_drop_behavior  */
#line 9016 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_ROUTINE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41820 "gram.c"
    break;

  case 1226: /* RemoveAggrStmt: DROP AGGREGATE aggregate_with_argtypes_list opt_drop_behavior  */
#line 9030 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_AGGREGATE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41835 "gram.c"
    break;

  case 1227: /* RemoveAggrStmt: DROP AGGREGATE IF_P EXISTS aggregate_with_argtypes_list opt_drop_behavior  */
#line 9041 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_AGGREGATE;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41850 "gram.c"
    break;

  case 1228: /* RemoveOperStmt: DROP OPERATOR operator_with_argtypes_list opt_drop_behavior  */
#line 9055 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_OPERATOR;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41865 "gram.c"
    break;

  case 1229: /* RemoveOperStmt: DROP OPERATOR IF_P EXISTS operator_with_argtypes_list opt_drop_behavior  */
#line 9066 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_OPERATOR;
                    n->objects = (yyvsp[-1].list);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 41880 "gram.c"
    break;

  case 1230: /* oper_argtypes: '(' Typename ')'  */
#line 9080 "gram.y"
                                {
                   ereport(ERROR,
                           (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("missing argument"),
                            errhint("Use NONE to denote the missing argument of a unary operator."),
                            parser_errposition((yylsp[0]))));
                }
#line 41892 "gram.c"
    break;

  case 1231: /* oper_argtypes: '(' Typename ',' Typename ')'  */
#line 9088 "gram.y"
                                        { (yyval.list) = list_make2((yyvsp[-3].typnam), (yyvsp[-1].typnam)); }
#line 41898 "gram.c"
    break;

  case 1232: /* oper_argtypes: '(' NONE ',' Typename ')'  */
#line 9090 "gram.y"
                                        { (yyval.list) = list_make2(NULL, (yyvsp[-1].typnam)); }
#line 41904 "gram.c"
    break;

  case 1233: /* oper_argtypes: '(' Typename ',' NONE ')'  */
#line 9092 "gram.y"
                                        { (yyval.list) = list_make2((yyvsp[-3].typnam), NULL); }
#line 41910 "gram.c"
    break;

  case 1234: /* any_operator: all_Op  */
#line 9097 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 41916 "gram.c"
    break;

  case 1235: /* any_operator: ColId '.' any_operator  */
#line 9099 "gram.y"
                                        { (yyval.list) = lcons(makeString((yyvsp[-2].str)), (yyvsp[0].list)); }
#line 41922 "gram.c"
    break;

  case 1236: /* operator_with_argtypes_list: operator_with_argtypes  */
#line 9103 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].objwithargs)); }
#line 41928 "gram.c"
    break;

  case 1237: /* operator_with_argtypes_list: operator_with_argtypes_list ',' operator_with_argtypes  */
#line 9105 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].objwithargs)); }
#line 41934 "gram.c"
    break;

  case 1238: /* operator_with_argtypes: any_operator oper_argtypes  */
#line 9110 "gram.y"
                                {
                    ObjectWithArgs *n = makeNode(ObjectWithArgs);

                    n->objname = (yyvsp[-1].list);
                    n->objargs = (yyvsp[0].list);
                    (yyval.objwithargs) = n;
                }
#line 41946 "gram.c"
    break;

  case 1239: /* DoStmt: DO dostmt_opt_list  */
#line 9129 "gram.y"
                                {
                    DoStmt *n = makeNode(DoStmt);

                    n->args = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 41957 "gram.c"
    break;

  case 1240: /* dostmt_opt_list: dostmt_opt_item  */
#line 9138 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 41963 "gram.c"
    break;

  case 1241: /* dostmt_opt_list: dostmt_opt_list dostmt_opt_item  */
#line 9139 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 41969 "gram.c"
    break;

  case 1242: /* dostmt_opt_item: Sconst  */
#line 9144 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("as", (Node *) makeString((yyvsp[0].str)), (yylsp[0]));
                }
#line 41977 "gram.c"
    break;

  case 1243: /* dostmt_opt_item: LANGUAGE NonReservedWord_or_Sconst  */
#line 9148 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("language", (Node *) makeString((yyvsp[0].str)), (yylsp[-1]));
                }
#line 41985 "gram.c"
    break;

  case 1244: /* CreateCastStmt: CREATE CAST '(' Typename AS Typename ')' WITH FUNCTION function_with_argtypes cast_context  */
#line 9161 "gram.y"
                                {
                    CreateCastStmt *n = makeNode(CreateCastStmt);

                    n->sourcetype = (yyvsp[-7].typnam);
                    n->targettype = (yyvsp[-5].typnam);
                    n->func = (yyvsp[-1].objwithargs);
                    n->context = (CoercionContext) (yyvsp[0].ival);
                    n->inout = false;
                    (yyval.node) = (Node *) n;
                }
#line 42000 "gram.c"
    break;

  case 1245: /* CreateCastStmt: CREATE CAST '(' Typename AS Typename ')' WITHOUT FUNCTION cast_context  */
#line 9173 "gram.y"
                                {
                    CreateCastStmt *n = makeNode(CreateCastStmt);

                    n->sourcetype = (yyvsp[-6].typnam);
                    n->targettype = (yyvsp[-4].typnam);
                    n->func = NULL;
                    n->context = (CoercionContext) (yyvsp[0].ival);
                    n->inout = false;
                    (yyval.node) = (Node *) n;
                }
#line 42015 "gram.c"
    break;

  case 1246: /* CreateCastStmt: CREATE CAST '(' Typename AS Typename ')' WITH INOUT cast_context  */
#line 9185 "gram.y"
                                {
                    CreateCastStmt *n = makeNode(CreateCastStmt);

                    n->sourcetype = (yyvsp[-6].typnam);
                    n->targettype = (yyvsp[-4].typnam);
                    n->func = NULL;
                    n->context = (CoercionContext) (yyvsp[0].ival);
                    n->inout = true;
                    (yyval.node) = (Node *) n;
                }
#line 42030 "gram.c"
    break;

  case 1247: /* cast_context: AS IMPLICIT_P  */
#line 9197 "gram.y"
                                                                { (yyval.ival) = COERCION_IMPLICIT; }
#line 42036 "gram.c"
    break;

  case 1248: /* cast_context: AS ASSIGNMENT  */
#line 9198 "gram.y"
                                                                                { (yyval.ival) = COERCION_ASSIGNMENT; }
#line 42042 "gram.c"
    break;

  case 1249: /* cast_context: %empty  */
#line 9199 "gram.y"
                                                                                        { (yyval.ival) = COERCION_EXPLICIT; }
#line 42048 "gram.c"
    break;

  case 1250: /* DropCastStmt: DROP CAST opt_if_exists '(' Typename AS Typename ')' opt_drop_behavior  */
#line 9204 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_CAST;
                    n->objects = list_make1(list_make2((yyvsp[-4].typnam), (yyvsp[-2].typnam)));
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = (yyvsp[-6].boolean);
                    n->concurrent = false;
                    (yyval.node) = (Node *) n;
                }
#line 42063 "gram.c"
    break;

  case 1251: /* opt_if_exists: IF_P EXISTS  */
#line 9216 "gram.y"
                                                                        { (yyval.boolean) = true; }
#line 42069 "gram.c"
    break;

  case 1252: /* opt_if_exists: %empty  */
#line 9217 "gram.y"
                                                                                        { (yyval.boolean) = false; }
#line 42075 "gram.c"
    break;

  case 1253: /* CreateTransformStmt: CREATE opt_or_replace TRANSFORM FOR Typename LANGUAGE name '(' transform_element_list ')'  */
#line 9228 "gram.y"
                                {
                    CreateTransformStmt *n = makeNode(CreateTransformStmt);

                    n->replace = (yyvsp[-8].boolean);
                    n->type_name = (yyvsp[-5].typnam);
                    n->lang = (yyvsp[-3].str);
                    n->fromsql = linitial((yyvsp[-1].list));
                    n->tosql = lsecond((yyvsp[-1].list));
                    (yyval.node) = (Node *) n;
                }
#line 42090 "gram.c"
    break;

  case 1254: /* transform_element_list: FROM SQL_P WITH FUNCTION function_with_argtypes ',' TO SQL_P WITH FUNCTION function_with_argtypes  */
#line 9241 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[-6].objwithargs), (yyvsp[0].objwithargs));
                }
#line 42098 "gram.c"
    break;

  case 1255: /* transform_element_list: TO SQL_P WITH FUNCTION function_with_argtypes ',' FROM SQL_P WITH FUNCTION function_with_argtypes  */
#line 9245 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[0].objwithargs), (yyvsp[-6].objwithargs));
                }
#line 42106 "gram.c"
    break;

  case 1256: /* transform_element_list: FROM SQL_P WITH FUNCTION function_with_argtypes  */
#line 9249 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[0].objwithargs), NULL);
                }
#line 42114 "gram.c"
    break;

  case 1257: /* transform_element_list: TO SQL_P WITH FUNCTION function_with_argtypes  */
#line 9253 "gram.y"
                                {
                    (yyval.list) = list_make2(NULL, (yyvsp[0].objwithargs));
                }
#line 42122 "gram.c"
    break;

  case 1258: /* DropTransformStmt: DROP TRANSFORM opt_if_exists FOR Typename LANGUAGE name opt_drop_behavior  */
#line 9260 "gram.y"
                                {
                    DropStmt *n = makeNode(DropStmt);

                    n->removeType = OBJECT_TRANSFORM;
                    n->objects = list_make1(list_make2((yyvsp[-3].typnam), makeString((yyvsp[-1].str))));
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = (yyvsp[-5].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 42136 "gram.c"
    break;

  case 1259: /* ReindexStmt: REINDEX opt_reindex_option_list reindex_target_relation opt_concurrently qualified_name  */
#line 9282 "gram.y"
                                {
                    ReindexStmt *n = makeNode(ReindexStmt);

                    n->kind = (yyvsp[-2].ival);
                    n->relation = (yyvsp[0].range);
                    n->name = NULL;
                    n->params = (yyvsp[-3].list);
                    if ((yyvsp[-1].boolean))
                        n->params = lappend(n->params,
                                            makeDefElem("concurrently", NULL, (yylsp[-1])));
                    (yyval.node) = (Node *) n;
                }
#line 42153 "gram.c"
    break;

  case 1260: /* ReindexStmt: REINDEX opt_reindex_option_list SCHEMA opt_concurrently name  */
#line 9295 "gram.y"
                                {
                    ReindexStmt *n = makeNode(ReindexStmt);

                    n->kind = REINDEX_OBJECT_SCHEMA;
                    n->relation = NULL;
                    n->name = (yyvsp[0].str);
                    n->params = (yyvsp[-3].list);
                    if ((yyvsp[-1].boolean))
                        n->params = lappend(n->params,
                                            makeDefElem("concurrently", NULL, (yylsp[-1])));
                    (yyval.node) = (Node *) n;
                }
#line 42170 "gram.c"
    break;

  case 1261: /* ReindexStmt: REINDEX opt_reindex_option_list reindex_target_all opt_concurrently opt_single_name  */
#line 9308 "gram.y"
                                {
                    ReindexStmt *n = makeNode(ReindexStmt);

                    n->kind = (yyvsp[-2].ival);
                    n->relation = NULL;
                    n->name = (yyvsp[0].str);
                    n->params = (yyvsp[-3].list);
                    if ((yyvsp[-1].boolean))
                        n->params = lappend(n->params,
                                            makeDefElem("concurrently", NULL, (yylsp[-1])));
                    (yyval.node) = (Node *) n;
                }
#line 42187 "gram.c"
    break;

  case 1262: /* reindex_target_relation: INDEX  */
#line 9322 "gram.y"
                                                                { (yyval.ival) = REINDEX_OBJECT_INDEX; }
#line 42193 "gram.c"
    break;

  case 1263: /* reindex_target_relation: TABLE  */
#line 9323 "gram.y"
                                                                { (yyval.ival) = REINDEX_OBJECT_TABLE; }
#line 42199 "gram.c"
    break;

  case 1264: /* reindex_target_all: SYSTEM_P  */
#line 9326 "gram.y"
                                                                { (yyval.ival) = REINDEX_OBJECT_SYSTEM; }
#line 42205 "gram.c"
    break;

  case 1265: /* reindex_target_all: DATABASE  */
#line 9327 "gram.y"
                                                                { (yyval.ival) = REINDEX_OBJECT_DATABASE; }
#line 42211 "gram.c"
    break;

  case 1266: /* opt_reindex_option_list: '(' utility_option_list ')'  */
#line 9330 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 42217 "gram.c"
    break;

  case 1267: /* opt_reindex_option_list: %empty  */
#line 9331 "gram.y"
                                                                                        { (yyval.list) = NULL; }
#line 42223 "gram.c"
    break;

  case 1268: /* AlterTblSpcStmt: ALTER TABLESPACE name SET reloptions  */
#line 9342 "gram.y"
                                {
                    AlterTableSpaceOptionsStmt *n =
                        makeNode(AlterTableSpaceOptionsStmt);

                    n->tablespacename = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    n->isReset = false;
                    (yyval.node) = (Node *) n;
                }
#line 42237 "gram.c"
    break;

  case 1269: /* AlterTblSpcStmt: ALTER TABLESPACE name RESET reloptions  */
#line 9352 "gram.y"
                                {
                    AlterTableSpaceOptionsStmt *n =
                        makeNode(AlterTableSpaceOptionsStmt);

                    n->tablespacename = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    n->isReset = true;
                    (yyval.node) = (Node *) n;
                }
#line 42251 "gram.c"
    break;

  case 1270: /* RenameStmt: ALTER AGGREGATE aggregate_with_argtypes RENAME TO name  */
#line 9370 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42265 "gram.c"
    break;

  case 1271: /* RenameStmt: ALTER COLLATION any_name RENAME TO name  */
#line 9380 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLLATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42279 "gram.c"
    break;

  case 1272: /* RenameStmt: ALTER CONVERSION_P any_name RENAME TO name  */
#line 9390 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_CONVERSION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42293 "gram.c"
    break;

  case 1273: /* RenameStmt: ALTER DATABASE name RENAME TO name  */
#line 9400 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_DATABASE;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42307 "gram.c"
    break;

  case 1274: /* RenameStmt: ALTER DOMAIN_P any_name RENAME TO name  */
#line 9410 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42321 "gram.c"
    break;

  case 1275: /* RenameStmt: ALTER DOMAIN_P any_name RENAME CONSTRAINT name TO name  */
#line 9420 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_DOMCONSTRAINT;
                    n->object = (Node *) (yyvsp[-5].list);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 42335 "gram.c"
    break;

  case 1276: /* RenameStmt: ALTER FOREIGN DATA_P WRAPPER name RENAME TO name  */
#line 9430 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_FDW;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42349 "gram.c"
    break;

  case 1277: /* RenameStmt: ALTER FUNCTION function_with_argtypes RENAME TO name  */
#line 9440 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42363 "gram.c"
    break;

  case 1278: /* RenameStmt: ALTER GROUP_P RoleId RENAME TO RoleId  */
#line 9450 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_ROLE;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42377 "gram.c"
    break;

  case 1279: /* RenameStmt: ALTER opt_procedural LANGUAGE name RENAME TO name  */
#line 9460 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_LANGUAGE;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42391 "gram.c"
    break;

  case 1280: /* RenameStmt: ALTER OPERATOR CLASS any_name USING name RENAME TO name  */
#line 9470 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_OPCLASS;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42405 "gram.c"
    break;

  case 1281: /* RenameStmt: ALTER OPERATOR FAMILY any_name USING name RENAME TO name  */
#line 9480 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_OPFAMILY;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42419 "gram.c"
    break;

  case 1282: /* RenameStmt: ALTER POLICY name ON qualified_name RENAME TO name  */
#line 9490 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_POLICY;
                    n->relation = (yyvsp[-3].range);
                    n->subname = (yyvsp[-5].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42434 "gram.c"
    break;

  case 1283: /* RenameStmt: ALTER POLICY IF_P EXISTS name ON qualified_name RENAME TO name  */
#line 9501 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_POLICY;
                    n->relation = (yyvsp[-3].range);
                    n->subname = (yyvsp[-5].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42449 "gram.c"
    break;

  case 1284: /* RenameStmt: ALTER PROCEDURE function_with_argtypes RENAME TO name  */
#line 9512 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42463 "gram.c"
    break;

  case 1285: /* RenameStmt: ALTER PUBLICATION name RENAME TO name  */
#line 9522 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_PUBLICATION;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42477 "gram.c"
    break;

  case 1286: /* RenameStmt: ALTER ROUTINE function_with_argtypes RENAME TO name  */
#line 9532 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42491 "gram.c"
    break;

  case 1287: /* RenameStmt: ALTER SCHEMA name RENAME TO name  */
#line 9542 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_SCHEMA;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42505 "gram.c"
    break;

  case 1288: /* RenameStmt: ALTER SERVER name RENAME TO name  */
#line 9552 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_FOREIGN_SERVER;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42519 "gram.c"
    break;

  case 1289: /* RenameStmt: ALTER SUBSCRIPTION name RENAME TO name  */
#line 9562 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_SUBSCRIPTION;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42533 "gram.c"
    break;

  case 1290: /* RenameStmt: ALTER TABLE relation_expr RENAME TO name  */
#line 9572 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42548 "gram.c"
    break;

  case 1291: /* RenameStmt: ALTER TABLE IF_P EXISTS relation_expr RENAME TO name  */
#line 9583 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42563 "gram.c"
    break;

  case 1292: /* RenameStmt: ALTER SEQUENCE qualified_name RENAME TO name  */
#line 9594 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_SEQUENCE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42578 "gram.c"
    break;

  case 1293: /* RenameStmt: ALTER SEQUENCE IF_P EXISTS qualified_name RENAME TO name  */
#line 9605 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_SEQUENCE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42593 "gram.c"
    break;

  case 1294: /* RenameStmt: ALTER VIEW qualified_name RENAME TO name  */
#line 9616 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_VIEW;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42608 "gram.c"
    break;

  case 1295: /* RenameStmt: ALTER VIEW IF_P EXISTS qualified_name RENAME TO name  */
#line 9627 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_VIEW;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42623 "gram.c"
    break;

  case 1296: /* RenameStmt: ALTER MATERIALIZED VIEW qualified_name RENAME TO name  */
#line 9638 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42638 "gram.c"
    break;

  case 1297: /* RenameStmt: ALTER MATERIALIZED VIEW IF_P EXISTS qualified_name RENAME TO name  */
#line 9649 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42653 "gram.c"
    break;

  case 1298: /* RenameStmt: ALTER INDEX qualified_name RENAME TO name  */
#line 9660 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_INDEX;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42668 "gram.c"
    break;

  case 1299: /* RenameStmt: ALTER INDEX IF_P EXISTS qualified_name RENAME TO name  */
#line 9671 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_INDEX;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42683 "gram.c"
    break;

  case 1300: /* RenameStmt: ALTER FOREIGN TABLE relation_expr RENAME TO name  */
#line 9682 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42698 "gram.c"
    break;

  case 1301: /* RenameStmt: ALTER FOREIGN TABLE IF_P EXISTS relation_expr RENAME TO name  */
#line 9693 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = NULL;
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42713 "gram.c"
    break;

  case 1302: /* RenameStmt: ALTER TABLE relation_expr RENAME opt_column name TO name  */
#line 9704 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_TABLE;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42729 "gram.c"
    break;

  case 1303: /* RenameStmt: ALTER TABLE IF_P EXISTS relation_expr RENAME opt_column name TO name  */
#line 9716 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_TABLE;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42745 "gram.c"
    break;

  case 1304: /* RenameStmt: ALTER VIEW qualified_name RENAME opt_column name TO name  */
#line 9728 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_VIEW;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42761 "gram.c"
    break;

  case 1305: /* RenameStmt: ALTER VIEW IF_P EXISTS qualified_name RENAME opt_column name TO name  */
#line 9740 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_VIEW;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42777 "gram.c"
    break;

  case 1306: /* RenameStmt: ALTER MATERIALIZED VIEW qualified_name RENAME opt_column name TO name  */
#line 9752 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42793 "gram.c"
    break;

  case 1307: /* RenameStmt: ALTER MATERIALIZED VIEW IF_P EXISTS qualified_name RENAME opt_column name TO name  */
#line 9764 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42809 "gram.c"
    break;

  case 1308: /* RenameStmt: ALTER TABLE relation_expr RENAME CONSTRAINT name TO name  */
#line 9776 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TABCONSTRAINT;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42824 "gram.c"
    break;

  case 1309: /* RenameStmt: ALTER TABLE IF_P EXISTS relation_expr RENAME CONSTRAINT name TO name  */
#line 9787 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TABCONSTRAINT;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42839 "gram.c"
    break;

  case 1310: /* RenameStmt: ALTER FOREIGN TABLE relation_expr RENAME opt_column name TO name  */
#line 9798 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42855 "gram.c"
    break;

  case 1311: /* RenameStmt: ALTER FOREIGN TABLE IF_P EXISTS relation_expr RENAME opt_column name TO name  */
#line 9810 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_COLUMN;
                    n->relationType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-5].range);
                    n->subname = (yyvsp[-2].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 42871 "gram.c"
    break;

  case 1312: /* RenameStmt: ALTER RULE name ON qualified_name RENAME TO name  */
#line 9822 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_RULE;
                    n->relation = (yyvsp[-3].range);
                    n->subname = (yyvsp[-5].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42886 "gram.c"
    break;

  case 1313: /* RenameStmt: ALTER TRIGGER name ON qualified_name RENAME TO name  */
#line 9833 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TRIGGER;
                    n->relation = (yyvsp[-3].range);
                    n->subname = (yyvsp[-5].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42901 "gram.c"
    break;

  case 1314: /* RenameStmt: ALTER EVENT TRIGGER name RENAME TO name  */
#line 9844 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_EVENT_TRIGGER;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 42914 "gram.c"
    break;

  case 1315: /* RenameStmt: ALTER ROLE RoleId RENAME TO RoleId  */
#line 9853 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_ROLE;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42928 "gram.c"
    break;

  case 1316: /* RenameStmt: ALTER USER RoleId RENAME TO RoleId  */
#line 9863 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_ROLE;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42942 "gram.c"
    break;

  case 1317: /* RenameStmt: ALTER TABLESPACE name RENAME TO name  */
#line 9873 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TABLESPACE;
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42956 "gram.c"
    break;

  case 1318: /* RenameStmt: ALTER STATISTICS any_name RENAME TO name  */
#line 9883 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_STATISTIC_EXT;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42970 "gram.c"
    break;

  case 1319: /* RenameStmt: ALTER TEXT_P SEARCH PARSER any_name RENAME TO name  */
#line 9893 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TSPARSER;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42984 "gram.c"
    break;

  case 1320: /* RenameStmt: ALTER TEXT_P SEARCH DICTIONARY any_name RENAME TO name  */
#line 9903 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TSDICTIONARY;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 42998 "gram.c"
    break;

  case 1321: /* RenameStmt: ALTER TEXT_P SEARCH TEMPLATE any_name RENAME TO name  */
#line 9913 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TSTEMPLATE;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43012 "gram.c"
    break;

  case 1322: /* RenameStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name RENAME TO name  */
#line 9923 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TSCONFIGURATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43026 "gram.c"
    break;

  case 1323: /* RenameStmt: ALTER TYPE_P any_name RENAME TO name  */
#line 9933 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newname = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43040 "gram.c"
    break;

  case 1324: /* RenameStmt: ALTER TYPE_P any_name RENAME ATTRIBUTE name TO name opt_drop_behavior  */
#line 9943 "gram.y"
                                {
                    RenameStmt *n = makeNode(RenameStmt);

                    n->renameType = OBJECT_ATTRIBUTE;
                    n->relationType = OBJECT_TYPE;
                    n->relation = makeRangeVarFromAnyName((yyvsp[-6].list), (yylsp[-6]), yyscanner);
                    n->subname = (yyvsp[-3].str);
                    n->newname = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43057 "gram.c"
    break;

  case 1327: /* opt_set_data: SET DATA_P  */
#line 9961 "gram.y"
                                                                                { (yyval.ival) = 1; }
#line 43063 "gram.c"
    break;

  case 1328: /* opt_set_data: %empty  */
#line 9962 "gram.y"
                                                                                                { (yyval.ival) = 0; }
#line 43069 "gram.c"
    break;

  case 1329: /* AlterObjectDependsStmt: ALTER FUNCTION function_with_argtypes opt_no DEPENDS ON EXTENSION name  */
#line 9973 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-5].objwithargs);
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43083 "gram.c"
    break;

  case 1330: /* AlterObjectDependsStmt: ALTER PROCEDURE function_with_argtypes opt_no DEPENDS ON EXTENSION name  */
#line 9983 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-5].objwithargs);
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43097 "gram.c"
    break;

  case 1331: /* AlterObjectDependsStmt: ALTER ROUTINE function_with_argtypes opt_no DEPENDS ON EXTENSION name  */
#line 9993 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-5].objwithargs);
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43111 "gram.c"
    break;

  case 1332: /* AlterObjectDependsStmt: ALTER TRIGGER name ON qualified_name opt_no DEPENDS ON EXTENSION name  */
#line 10003 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_TRIGGER;
                    n->relation = (yyvsp[-5].range);
                    n->object = (Node *) list_make1(makeString((yyvsp[-7].str)));
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43126 "gram.c"
    break;

  case 1333: /* AlterObjectDependsStmt: ALTER MATERIALIZED VIEW qualified_name opt_no DEPENDS ON EXTENSION name  */
#line 10014 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-5].range);
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43140 "gram.c"
    break;

  case 1334: /* AlterObjectDependsStmt: ALTER INDEX qualified_name opt_no DEPENDS ON EXTENSION name  */
#line 10024 "gram.y"
                                {
                    AlterObjectDependsStmt *n = makeNode(AlterObjectDependsStmt);

                    n->objectType = OBJECT_INDEX;
                    n->relation = (yyvsp[-5].range);
                    n->extname = makeString((yyvsp[0].str));
                    n->remove = (yyvsp[-4].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 43154 "gram.c"
    break;

  case 1335: /* opt_no: NO  */
#line 10035 "gram.y"
                                                { (yyval.boolean) = true; }
#line 43160 "gram.c"
    break;

  case 1336: /* opt_no: %empty  */
#line 10036 "gram.y"
                                        { (yyval.boolean) = false;  }
#line 43166 "gram.c"
    break;

  case 1337: /* AlterObjectSchemaStmt: ALTER AGGREGATE aggregate_with_argtypes SET SCHEMA name  */
#line 10047 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43180 "gram.c"
    break;

  case 1338: /* AlterObjectSchemaStmt: ALTER COLLATION any_name SET SCHEMA name  */
#line 10057 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_COLLATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43194 "gram.c"
    break;

  case 1339: /* AlterObjectSchemaStmt: ALTER CONVERSION_P any_name SET SCHEMA name  */
#line 10067 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_CONVERSION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43208 "gram.c"
    break;

  case 1340: /* AlterObjectSchemaStmt: ALTER DOMAIN_P any_name SET SCHEMA name  */
#line 10077 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43222 "gram.c"
    break;

  case 1341: /* AlterObjectSchemaStmt: ALTER EXTENSION name SET SCHEMA name  */
#line 10087 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_EXTENSION;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43236 "gram.c"
    break;

  case 1342: /* AlterObjectSchemaStmt: ALTER FUNCTION function_with_argtypes SET SCHEMA name  */
#line 10097 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43250 "gram.c"
    break;

  case 1343: /* AlterObjectSchemaStmt: ALTER OPERATOR operator_with_argtypes SET SCHEMA name  */
#line 10107 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_OPERATOR;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43264 "gram.c"
    break;

  case 1344: /* AlterObjectSchemaStmt: ALTER OPERATOR CLASS any_name USING name SET SCHEMA name  */
#line 10117 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_OPCLASS;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43278 "gram.c"
    break;

  case 1345: /* AlterObjectSchemaStmt: ALTER OPERATOR FAMILY any_name USING name SET SCHEMA name  */
#line 10127 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_OPFAMILY;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43292 "gram.c"
    break;

  case 1346: /* AlterObjectSchemaStmt: ALTER PROCEDURE function_with_argtypes SET SCHEMA name  */
#line 10137 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43306 "gram.c"
    break;

  case 1347: /* AlterObjectSchemaStmt: ALTER ROUTINE function_with_argtypes SET SCHEMA name  */
#line 10147 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43320 "gram.c"
    break;

  case 1348: /* AlterObjectSchemaStmt: ALTER TABLE relation_expr SET SCHEMA name  */
#line 10157 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43334 "gram.c"
    break;

  case 1349: /* AlterObjectSchemaStmt: ALTER TABLE IF_P EXISTS relation_expr SET SCHEMA name  */
#line 10167 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 43348 "gram.c"
    break;

  case 1350: /* AlterObjectSchemaStmt: ALTER STATISTICS any_name SET SCHEMA name  */
#line 10177 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_STATISTIC_EXT;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43362 "gram.c"
    break;

  case 1351: /* AlterObjectSchemaStmt: ALTER TEXT_P SEARCH PARSER any_name SET SCHEMA name  */
#line 10187 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TSPARSER;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43376 "gram.c"
    break;

  case 1352: /* AlterObjectSchemaStmt: ALTER TEXT_P SEARCH DICTIONARY any_name SET SCHEMA name  */
#line 10197 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TSDICTIONARY;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43390 "gram.c"
    break;

  case 1353: /* AlterObjectSchemaStmt: ALTER TEXT_P SEARCH TEMPLATE any_name SET SCHEMA name  */
#line 10207 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TSTEMPLATE;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43404 "gram.c"
    break;

  case 1354: /* AlterObjectSchemaStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name SET SCHEMA name  */
#line 10217 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TSCONFIGURATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43418 "gram.c"
    break;

  case 1355: /* AlterObjectSchemaStmt: ALTER SEQUENCE qualified_name SET SCHEMA name  */
#line 10227 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_SEQUENCE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43432 "gram.c"
    break;

  case 1356: /* AlterObjectSchemaStmt: ALTER SEQUENCE IF_P EXISTS qualified_name SET SCHEMA name  */
#line 10237 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_SEQUENCE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 43446 "gram.c"
    break;

  case 1357: /* AlterObjectSchemaStmt: ALTER VIEW qualified_name SET SCHEMA name  */
#line 10247 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_VIEW;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43460 "gram.c"
    break;

  case 1358: /* AlterObjectSchemaStmt: ALTER VIEW IF_P EXISTS qualified_name SET SCHEMA name  */
#line 10257 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_VIEW;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 43474 "gram.c"
    break;

  case 1359: /* AlterObjectSchemaStmt: ALTER MATERIALIZED VIEW qualified_name SET SCHEMA name  */
#line 10267 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43488 "gram.c"
    break;

  case 1360: /* AlterObjectSchemaStmt: ALTER MATERIALIZED VIEW IF_P EXISTS qualified_name SET SCHEMA name  */
#line 10277 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_MATVIEW;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 43502 "gram.c"
    break;

  case 1361: /* AlterObjectSchemaStmt: ALTER FOREIGN TABLE relation_expr SET SCHEMA name  */
#line 10287 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43516 "gram.c"
    break;

  case 1362: /* AlterObjectSchemaStmt: ALTER FOREIGN TABLE IF_P EXISTS relation_expr SET SCHEMA name  */
#line 10297 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_FOREIGN_TABLE;
                    n->relation = (yyvsp[-3].range);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 43530 "gram.c"
    break;

  case 1363: /* AlterObjectSchemaStmt: ALTER TYPE_P any_name SET SCHEMA name  */
#line 10307 "gram.y"
                                {
                    AlterObjectSchemaStmt *n = makeNode(AlterObjectSchemaStmt);

                    n->objectType = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newschema = (yyvsp[0].str);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 43544 "gram.c"
    break;

  case 1364: /* AlterOperatorStmt: ALTER OPERATOR operator_with_argtypes SET '(' operator_def_list ')'  */
#line 10326 "gram.y"
                                {
                    AlterOperatorStmt *n = makeNode(AlterOperatorStmt);

                    n->opername = (yyvsp[-4].objwithargs);
                    n->options = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 43556 "gram.c"
    break;

  case 1365: /* operator_def_list: operator_def_elem  */
#line 10335 "gram.y"
                                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 43562 "gram.c"
    break;

  case 1366: /* operator_def_list: operator_def_list ',' operator_def_elem  */
#line 10336 "gram.y"
                                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 43568 "gram.c"
    break;

  case 1367: /* operator_def_elem: ColLabel '=' NONE  */
#line 10340 "gram.y"
                                                { (yyval.defelt) = makeDefElem((yyvsp[-2].str), NULL, (yylsp[-2])); }
#line 43574 "gram.c"
    break;

  case 1368: /* operator_def_elem: ColLabel '=' operator_def_arg  */
#line 10342 "gram.y"
                                                { (yyval.defelt) = makeDefElem((yyvsp[-2].str), (Node *) (yyvsp[0].node), (yylsp[-2])); }
#line 43580 "gram.c"
    break;

  case 1369: /* operator_def_elem: ColLabel  */
#line 10344 "gram.y"
                                                { (yyval.defelt) = makeDefElem((yyvsp[0].str), NULL, (yylsp[0])); }
#line 43586 "gram.c"
    break;

  case 1370: /* operator_def_arg: func_type  */
#line 10349 "gram.y"
                                                                                { (yyval.node) = (Node *) (yyvsp[0].typnam); }
#line 43592 "gram.c"
    break;

  case 1371: /* operator_def_arg: reserved_keyword  */
#line 10350 "gram.y"
                                                                        { (yyval.node) = (Node *) makeString(pstrdup((yyvsp[0].keyword))); }
#line 43598 "gram.c"
    break;

  case 1372: /* operator_def_arg: qual_all_Op  */
#line 10351 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].list); }
#line 43604 "gram.c"
    break;

  case 1373: /* operator_def_arg: NumericOnly  */
#line 10352 "gram.y"
                                                                        { (yyval.node) = (Node *) (yyvsp[0].node); }
#line 43610 "gram.c"
    break;

  case 1374: /* operator_def_arg: Sconst  */
#line 10353 "gram.y"
                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 43616 "gram.c"
    break;

  case 1375: /* AlterTypeStmt: ALTER TYPE_P any_name SET '(' operator_def_list ')'  */
#line 10366 "gram.y"
                                {
                    AlterTypeStmt *n = makeNode(AlterTypeStmt);

                    n->typeName = (yyvsp[-4].list);
                    n->options = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 43628 "gram.c"
    break;

  case 1376: /* AlterOwnerStmt: ALTER AGGREGATE aggregate_with_argtypes OWNER TO RoleSpec  */
#line 10382 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_AGGREGATE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43641 "gram.c"
    break;

  case 1377: /* AlterOwnerStmt: ALTER COLLATION any_name OWNER TO RoleSpec  */
#line 10391 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_COLLATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43654 "gram.c"
    break;

  case 1378: /* AlterOwnerStmt: ALTER CONVERSION_P any_name OWNER TO RoleSpec  */
#line 10400 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_CONVERSION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43667 "gram.c"
    break;

  case 1379: /* AlterOwnerStmt: ALTER DATABASE name OWNER TO RoleSpec  */
#line 10409 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_DATABASE;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43680 "gram.c"
    break;

  case 1380: /* AlterOwnerStmt: ALTER DOMAIN_P any_name OWNER TO RoleSpec  */
#line 10418 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_DOMAIN;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43693 "gram.c"
    break;

  case 1381: /* AlterOwnerStmt: ALTER FUNCTION function_with_argtypes OWNER TO RoleSpec  */
#line 10427 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_FUNCTION;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43706 "gram.c"
    break;

  case 1382: /* AlterOwnerStmt: ALTER opt_procedural LANGUAGE name OWNER TO RoleSpec  */
#line 10436 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_LANGUAGE;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43719 "gram.c"
    break;

  case 1383: /* AlterOwnerStmt: ALTER LARGE_P OBJECT_P NumericOnly OWNER TO RoleSpec  */
#line 10445 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_LARGEOBJECT;
                    n->object = (Node *) (yyvsp[-3].node);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43732 "gram.c"
    break;

  case 1384: /* AlterOwnerStmt: ALTER OPERATOR operator_with_argtypes OWNER TO RoleSpec  */
#line 10454 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_OPERATOR;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43745 "gram.c"
    break;

  case 1385: /* AlterOwnerStmt: ALTER OPERATOR CLASS any_name USING name OWNER TO RoleSpec  */
#line 10463 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_OPCLASS;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43758 "gram.c"
    break;

  case 1386: /* AlterOwnerStmt: ALTER OPERATOR FAMILY any_name USING name OWNER TO RoleSpec  */
#line 10472 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_OPFAMILY;
                    n->object = (Node *) lcons(makeString((yyvsp[-3].str)), (yyvsp[-5].list));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43771 "gram.c"
    break;

  case 1387: /* AlterOwnerStmt: ALTER PROCEDURE function_with_argtypes OWNER TO RoleSpec  */
#line 10481 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_PROCEDURE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43784 "gram.c"
    break;

  case 1388: /* AlterOwnerStmt: ALTER ROUTINE function_with_argtypes OWNER TO RoleSpec  */
#line 10490 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_ROUTINE;
                    n->object = (Node *) (yyvsp[-3].objwithargs);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43797 "gram.c"
    break;

  case 1389: /* AlterOwnerStmt: ALTER SCHEMA name OWNER TO RoleSpec  */
#line 10499 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_SCHEMA;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43810 "gram.c"
    break;

  case 1390: /* AlterOwnerStmt: ALTER TYPE_P any_name OWNER TO RoleSpec  */
#line 10508 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_TYPE;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43823 "gram.c"
    break;

  case 1391: /* AlterOwnerStmt: ALTER TABLESPACE name OWNER TO RoleSpec  */
#line 10517 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_TABLESPACE;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43836 "gram.c"
    break;

  case 1392: /* AlterOwnerStmt: ALTER STATISTICS any_name OWNER TO RoleSpec  */
#line 10526 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_STATISTIC_EXT;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43849 "gram.c"
    break;

  case 1393: /* AlterOwnerStmt: ALTER TEXT_P SEARCH DICTIONARY any_name OWNER TO RoleSpec  */
#line 10535 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_TSDICTIONARY;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43862 "gram.c"
    break;

  case 1394: /* AlterOwnerStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name OWNER TO RoleSpec  */
#line 10544 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_TSCONFIGURATION;
                    n->object = (Node *) (yyvsp[-3].list);
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43875 "gram.c"
    break;

  case 1395: /* AlterOwnerStmt: ALTER FOREIGN DATA_P WRAPPER name OWNER TO RoleSpec  */
#line 10553 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_FDW;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43888 "gram.c"
    break;

  case 1396: /* AlterOwnerStmt: ALTER SERVER name OWNER TO RoleSpec  */
#line 10562 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_FOREIGN_SERVER;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43901 "gram.c"
    break;

  case 1397: /* AlterOwnerStmt: ALTER EVENT TRIGGER name OWNER TO RoleSpec  */
#line 10571 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_EVENT_TRIGGER;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43914 "gram.c"
    break;

  case 1398: /* AlterOwnerStmt: ALTER PUBLICATION name OWNER TO RoleSpec  */
#line 10580 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_PUBLICATION;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43927 "gram.c"
    break;

  case 1399: /* AlterOwnerStmt: ALTER SUBSCRIPTION name OWNER TO RoleSpec  */
#line 10589 "gram.y"
                                {
                    AlterOwnerStmt *n = makeNode(AlterOwnerStmt);

                    n->objectType = OBJECT_SUBSCRIPTION;
                    n->object = (Node *) makeString((yyvsp[-3].str));
                    n->newowner = (yyvsp[0].rolespec);
                    (yyval.node) = (Node *) n;
                }
#line 43940 "gram.c"
    break;

  case 1400: /* CreatePublicationStmt: CREATE PUBLICATION name opt_definition  */
#line 10617 "gram.y"
                                {
                    CreatePublicationStmt *n = makeNode(CreatePublicationStmt);

                    n->pubname = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 43952 "gram.c"
    break;

  case 1401: /* CreatePublicationStmt: CREATE PUBLICATION name FOR ALL TABLES opt_definition  */
#line 10625 "gram.y"
                                {
                    CreatePublicationStmt *n = makeNode(CreatePublicationStmt);

                    n->pubname = (yyvsp[-4].str);
                    n->options = (yyvsp[0].list);
                    n->for_all_tables = true;
                    (yyval.node) = (Node *) n;
                }
#line 43965 "gram.c"
    break;

  case 1402: /* CreatePublicationStmt: CREATE PUBLICATION name FOR pub_obj_list opt_definition  */
#line 10634 "gram.y"
                                {
                    CreatePublicationStmt *n = makeNode(CreatePublicationStmt);

                    n->pubname = (yyvsp[-3].str);
                    n->options = (yyvsp[0].list);
                    n->pubobjects = (List *) (yyvsp[-1].list);
                    preprocess_pubobj_list(n->pubobjects, yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 43979 "gram.c"
    break;

  case 1403: /* PublicationObjSpec: TABLE relation_expr opt_column_list OptWhereClause  */
#line 10660 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_TABLE;
                    (yyval.publicationobjectspec)->pubtable = makeNode(PublicationTable);
                    (yyval.publicationobjectspec)->pubtable->relation = (yyvsp[-2].range);
                    (yyval.publicationobjectspec)->pubtable->columns = (yyvsp[-1].list);
                    (yyval.publicationobjectspec)->pubtable->whereClause = (yyvsp[0].node);
                }
#line 43992 "gram.c"
    break;

  case 1404: /* PublicationObjSpec: TABLES IN_P SCHEMA ColId  */
#line 10669 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_TABLES_IN_SCHEMA;
                    (yyval.publicationobjectspec)->name = (yyvsp[0].str);
                    (yyval.publicationobjectspec)->location = (yylsp[0]);
                }
#line 44003 "gram.c"
    break;

  case 1405: /* PublicationObjSpec: TABLES IN_P SCHEMA CURRENT_SCHEMA  */
#line 10676 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_TABLES_IN_CUR_SCHEMA;
                    (yyval.publicationobjectspec)->location = (yylsp[0]);
                }
#line 44013 "gram.c"
    break;

  case 1406: /* PublicationObjSpec: ColId opt_column_list OptWhereClause  */
#line 10682 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_CONTINUATION;
                    /*
                     * If either a row filter or column list is specified, create
                     * a PublicationTable object.
                     */
                    if ((yyvsp[-1].list) || (yyvsp[0].node))
                    {
                        /*
                         * The OptWhereClause must be stored here but it is
                         * valid only for tables. For non-table objects, an
                         * error will be thrown later via
                         * preprocess_pubobj_list().
                         */
                        (yyval.publicationobjectspec)->pubtable = makeNode(PublicationTable);
                        (yyval.publicationobjectspec)->pubtable->relation = makeRangeVar(NULL, (yyvsp[-2].str), (yylsp[-2]));
                        (yyval.publicationobjectspec)->pubtable->columns = (yyvsp[-1].list);
                        (yyval.publicationobjectspec)->pubtable->whereClause = (yyvsp[0].node);
                    }
                    else
                    {
                        (yyval.publicationobjectspec)->name = (yyvsp[-2].str);
                    }
                    (yyval.publicationobjectspec)->location = (yylsp[-2]);
                }
#line 44044 "gram.c"
    break;

  case 1407: /* PublicationObjSpec: ColId indirection opt_column_list OptWhereClause  */
#line 10709 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_CONTINUATION;
                    (yyval.publicationobjectspec)->pubtable = makeNode(PublicationTable);
                    (yyval.publicationobjectspec)->pubtable->relation = makeRangeVarFromQualifiedName((yyvsp[-3].str), (yyvsp[-2].list), (yylsp[-3]), yyscanner);
                    (yyval.publicationobjectspec)->pubtable->columns = (yyvsp[-1].list);
                    (yyval.publicationobjectspec)->pubtable->whereClause = (yyvsp[0].node);
                    (yyval.publicationobjectspec)->location = (yylsp[-3]);
                }
#line 44058 "gram.c"
    break;

  case 1408: /* PublicationObjSpec: extended_relation_expr opt_column_list OptWhereClause  */
#line 10720 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_CONTINUATION;
                    (yyval.publicationobjectspec)->pubtable = makeNode(PublicationTable);
                    (yyval.publicationobjectspec)->pubtable->relation = (yyvsp[-2].range);
                    (yyval.publicationobjectspec)->pubtable->columns = (yyvsp[-1].list);
                    (yyval.publicationobjectspec)->pubtable->whereClause = (yyvsp[0].node);
                }
#line 44071 "gram.c"
    break;

  case 1409: /* PublicationObjSpec: CURRENT_SCHEMA  */
#line 10729 "gram.y"
                                {
                    (yyval.publicationobjectspec) = makeNode(PublicationObjSpec);
                    (yyval.publicationobjectspec)->pubobjtype = PUBLICATIONOBJ_CONTINUATION;
                    (yyval.publicationobjectspec)->location = (yylsp[0]);
                }
#line 44081 "gram.c"
    break;

  case 1410: /* pub_obj_list: PublicationObjSpec  */
#line 10737 "gram.y"
                                        { (yyval.list) = list_make1((yyvsp[0].publicationobjectspec)); }
#line 44087 "gram.c"
    break;

  case 1411: /* pub_obj_list: pub_obj_list ',' PublicationObjSpec  */
#line 10739 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].publicationobjectspec)); }
#line 44093 "gram.c"
    break;

  case 1412: /* AlterPublicationStmt: ALTER PUBLICATION name SET definition  */
#line 10761 "gram.y"
                                {
                    AlterPublicationStmt *n = makeNode(AlterPublicationStmt);

                    n->pubname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44105 "gram.c"
    break;

  case 1413: /* AlterPublicationStmt: ALTER PUBLICATION name ADD_P pub_obj_list  */
#line 10769 "gram.y"
                                {
                    AlterPublicationStmt *n = makeNode(AlterPublicationStmt);

                    n->pubname = (yyvsp[-2].str);
                    n->pubobjects = (yyvsp[0].list);
                    preprocess_pubobj_list(n->pubobjects, yyscanner);
                    n->action = AP_AddObjects;
                    (yyval.node) = (Node *) n;
                }
#line 44119 "gram.c"
    break;

  case 1414: /* AlterPublicationStmt: ALTER PUBLICATION name SET pub_obj_list  */
#line 10779 "gram.y"
                                {
                    AlterPublicationStmt *n = makeNode(AlterPublicationStmt);

                    n->pubname = (yyvsp[-2].str);
                    n->pubobjects = (yyvsp[0].list);
                    preprocess_pubobj_list(n->pubobjects, yyscanner);
                    n->action = AP_SetObjects;
                    (yyval.node) = (Node *) n;
                }
#line 44133 "gram.c"
    break;

  case 1415: /* AlterPublicationStmt: ALTER PUBLICATION name DROP pub_obj_list  */
#line 10789 "gram.y"
                                {
                    AlterPublicationStmt *n = makeNode(AlterPublicationStmt);

                    n->pubname = (yyvsp[-2].str);
                    n->pubobjects = (yyvsp[0].list);
                    preprocess_pubobj_list(n->pubobjects, yyscanner);
                    n->action = AP_DropObjects;
                    (yyval.node) = (Node *) n;
                }
#line 44147 "gram.c"
    break;

  case 1416: /* CreateSubscriptionStmt: CREATE SUBSCRIPTION name CONNECTION Sconst PUBLICATION name_list opt_definition  */
#line 10808 "gram.y"
                                {
                    CreateSubscriptionStmt *n =
                        makeNode(CreateSubscriptionStmt);
                    n->subname = (yyvsp[-5].str);
                    n->conninfo = (yyvsp[-3].str);
                    n->publication = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44161 "gram.c"
    break;

  case 1417: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name SET definition  */
#line 10827 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_OPTIONS;
                    n->subname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44175 "gram.c"
    break;

  case 1418: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name CONNECTION Sconst  */
#line 10837 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_CONNECTION;
                    n->subname = (yyvsp[-2].str);
                    n->conninfo = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 44189 "gram.c"
    break;

  case 1419: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name REFRESH PUBLICATION opt_definition  */
#line 10847 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_REFRESH;
                    n->subname = (yyvsp[-3].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44203 "gram.c"
    break;

  case 1420: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name ADD_P PUBLICATION name_list opt_definition  */
#line 10857 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_ADD_PUBLICATION;
                    n->subname = (yyvsp[-4].str);
                    n->publication = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44218 "gram.c"
    break;

  case 1421: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name DROP PUBLICATION name_list opt_definition  */
#line 10868 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_DROP_PUBLICATION;
                    n->subname = (yyvsp[-4].str);
                    n->publication = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44233 "gram.c"
    break;

  case 1422: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name SET PUBLICATION name_list opt_definition  */
#line 10879 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_SET_PUBLICATION;
                    n->subname = (yyvsp[-4].str);
                    n->publication = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44248 "gram.c"
    break;

  case 1423: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name ENABLE_P  */
#line 10890 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_ENABLED;
                    n->subname = (yyvsp[-1].str);
                    n->options = list_make1(makeDefElem("enabled",
                                            (Node *) makeBoolean(true), (yylsp[-3])));
                    (yyval.node) = (Node *) n;
                }
#line 44263 "gram.c"
    break;

  case 1424: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name DISABLE_P  */
#line 10901 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_ENABLED;
                    n->subname = (yyvsp[-1].str);
                    n->options = list_make1(makeDefElem("enabled",
                                            (Node *) makeBoolean(false), (yylsp[-3])));
                    (yyval.node) = (Node *) n;
                }
#line 44278 "gram.c"
    break;

  case 1425: /* AlterSubscriptionStmt: ALTER SUBSCRIPTION name SKIP definition  */
#line 10912 "gram.y"
                                {
                    AlterSubscriptionStmt *n =
                        makeNode(AlterSubscriptionStmt);

                    n->kind = ALTER_SUBSCRIPTION_SKIP;
                    n->subname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44292 "gram.c"
    break;

  case 1426: /* DropSubscriptionStmt: DROP SUBSCRIPTION name opt_drop_behavior  */
#line 10930 "gram.y"
                                {
                    DropSubscriptionStmt *n = makeNode(DropSubscriptionStmt);

                    n->subname = (yyvsp[-1].str);
                    n->missing_ok = false;
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 44305 "gram.c"
    break;

  case 1427: /* DropSubscriptionStmt: DROP SUBSCRIPTION IF_P EXISTS name opt_drop_behavior  */
#line 10939 "gram.y"
                                {
                    DropSubscriptionStmt *n = makeNode(DropSubscriptionStmt);

                    n->subname = (yyvsp[-1].str);
                    n->missing_ok = true;
                    n->behavior = (yyvsp[0].dbehavior);
                    (yyval.node) = (Node *) n;
                }
#line 44318 "gram.c"
    break;

  case 1428: /* RuleStmt: CREATE opt_or_replace RULE name AS ON event TO qualified_name where_clause DO opt_instead RuleActionList  */
#line 10958 "gram.y"
                                {
                    RuleStmt   *n = makeNode(RuleStmt);

                    n->replace = (yyvsp[-11].boolean);
                    n->relation = (yyvsp[-4].range);
                    n->rulename = (yyvsp[-9].str);
                    n->whereClause = (yyvsp[-3].node);
                    n->event = (yyvsp[-6].ival);
                    n->instead = (yyvsp[-1].boolean);
                    n->actions = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44335 "gram.c"
    break;

  case 1429: /* RuleActionList: NOTHING  */
#line 10973 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 44341 "gram.c"
    break;

  case 1430: /* RuleActionList: RuleActionStmt  */
#line 10974 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 44347 "gram.c"
    break;

  case 1431: /* RuleActionList: '(' RuleActionMulti ')'  */
#line 10975 "gram.y"
                                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 44353 "gram.c"
    break;

  case 1432: /* RuleActionMulti: RuleActionMulti ';' RuleActionStmtOrEmpty  */
#line 10981 "gram.y"
                                { if ((yyvsp[0].node) != NULL)
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                  else
                    (yyval.list) = (yyvsp[-2].list);
                }
#line 44363 "gram.c"
    break;

  case 1433: /* RuleActionMulti: RuleActionStmtOrEmpty  */
#line 10987 "gram.y"
                                { if ((yyvsp[0].node) != NULL)
                    (yyval.list) = list_make1((yyvsp[0].node));
                  else
                    (yyval.list) = NIL;
                }
#line 44373 "gram.c"
    break;

  case 1439: /* RuleActionStmtOrEmpty: RuleActionStmt  */
#line 11003 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 44379 "gram.c"
    break;

  case 1440: /* RuleActionStmtOrEmpty: %empty  */
#line 11004 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 44385 "gram.c"
    break;

  case 1441: /* event: SELECT  */
#line 11007 "gram.y"
                                                                                        { (yyval.ival) = CMD_SELECT; }
#line 44391 "gram.c"
    break;

  case 1442: /* event: UPDATE  */
#line 11008 "gram.y"
                                                                                                { (yyval.ival) = CMD_UPDATE; }
#line 44397 "gram.c"
    break;

  case 1443: /* event: DELETE_P  */
#line 11009 "gram.y"
                                                                                                { (yyval.ival) = CMD_DELETE; }
#line 44403 "gram.c"
    break;

  case 1444: /* event: INSERT  */
#line 11010 "gram.y"
                                                                                                { (yyval.ival) = CMD_INSERT; }
#line 44409 "gram.c"
    break;

  case 1445: /* opt_instead: INSTEAD  */
#line 11014 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 44415 "gram.c"
    break;

  case 1446: /* opt_instead: ALSO  */
#line 11015 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 44421 "gram.c"
    break;

  case 1447: /* opt_instead: %empty  */
#line 11016 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 44427 "gram.c"
    break;

  case 1448: /* NotifyStmt: NOTIFY ColId notify_payload  */
#line 11029 "gram.y"
                                {
                    NotifyStmt *n = makeNode(NotifyStmt);

                    n->conditionname = (yyvsp[-1].str);
                    n->payload = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 44439 "gram.c"
    break;

  case 1449: /* notify_payload: ',' Sconst  */
#line 11039 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 44445 "gram.c"
    break;

  case 1450: /* notify_payload: %empty  */
#line 11040 "gram.y"
                                                                                        { (yyval.str) = NULL; }
#line 44451 "gram.c"
    break;

  case 1451: /* ListenStmt: LISTEN ColId  */
#line 11044 "gram.y"
                                {
                    ListenStmt *n = makeNode(ListenStmt);

                    n->conditionname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 44462 "gram.c"
    break;

  case 1452: /* UnlistenStmt: UNLISTEN ColId  */
#line 11054 "gram.y"
                                {
                    UnlistenStmt *n = makeNode(UnlistenStmt);

                    n->conditionname = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 44473 "gram.c"
    break;

  case 1453: /* UnlistenStmt: UNLISTEN '*'  */
#line 11061 "gram.y"
                                {
                    UnlistenStmt *n = makeNode(UnlistenStmt);

                    n->conditionname = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 44484 "gram.c"
    break;

  case 1454: /* TransactionStmt: ABORT_P opt_transaction opt_transaction_chain  */
#line 11081 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_ROLLBACK;
                    n->options = NIL;
                    n->chain = (yyvsp[0].boolean);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44498 "gram.c"
    break;

  case 1455: /* TransactionStmt: START TRANSACTION transaction_mode_list_or_empty  */
#line 11091 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_START;
                    n->options = (yyvsp[0].list);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44511 "gram.c"
    break;

  case 1456: /* TransactionStmt: COMMIT opt_transaction opt_transaction_chain  */
#line 11100 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_COMMIT;
                    n->options = NIL;
                    n->chain = (yyvsp[0].boolean);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44525 "gram.c"
    break;

  case 1457: /* TransactionStmt: ROLLBACK opt_transaction opt_transaction_chain  */
#line 11110 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_ROLLBACK;
                    n->options = NIL;
                    n->chain = (yyvsp[0].boolean);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44539 "gram.c"
    break;

  case 1458: /* TransactionStmt: SAVEPOINT ColId  */
#line 11120 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_SAVEPOINT;
                    n->savepoint_name = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44552 "gram.c"
    break;

  case 1459: /* TransactionStmt: RELEASE SAVEPOINT ColId  */
#line 11129 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_RELEASE;
                    n->savepoint_name = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44565 "gram.c"
    break;

  case 1460: /* TransactionStmt: RELEASE ColId  */
#line 11138 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_RELEASE;
                    n->savepoint_name = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44578 "gram.c"
    break;

  case 1461: /* TransactionStmt: ROLLBACK opt_transaction TO SAVEPOINT ColId  */
#line 11147 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_ROLLBACK_TO;
                    n->savepoint_name = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44591 "gram.c"
    break;

  case 1462: /* TransactionStmt: ROLLBACK opt_transaction TO ColId  */
#line 11156 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_ROLLBACK_TO;
                    n->savepoint_name = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44604 "gram.c"
    break;

  case 1463: /* TransactionStmt: PREPARE TRANSACTION Sconst  */
#line 11165 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_PREPARE;
                    n->gid = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44617 "gram.c"
    break;

  case 1464: /* TransactionStmt: COMMIT PREPARED Sconst  */
#line 11174 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_COMMIT_PREPARED;
                    n->gid = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44630 "gram.c"
    break;

  case 1465: /* TransactionStmt: ROLLBACK PREPARED Sconst  */
#line 11183 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_ROLLBACK_PREPARED;
                    n->gid = (yyvsp[0].str);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 44643 "gram.c"
    break;

  case 1466: /* TransactionStmtLegacy: BEGIN_P opt_transaction transaction_mode_list_or_empty  */
#line 11195 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_BEGIN;
                    n->options = (yyvsp[0].list);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44656 "gram.c"
    break;

  case 1467: /* TransactionStmtLegacy: END_P opt_transaction opt_transaction_chain  */
#line 11204 "gram.y"
                                {
                    TransactionStmt *n = makeNode(TransactionStmt);

                    n->kind = TRANS_STMT_COMMIT;
                    n->options = NIL;
                    n->chain = (yyvsp[0].boolean);
                    n->location = -1;
                    (yyval.node) = (Node *) n;
                }
#line 44670 "gram.c"
    break;

  case 1471: /* transaction_mode_item: ISOLATION LEVEL iso_level  */
#line 11222 "gram.y"
                                        { (yyval.defelt) = makeDefElem("transaction_isolation",
                                       makeStringConst((yyvsp[0].str), (yylsp[0])), (yylsp[-2])); }
#line 44677 "gram.c"
    break;

  case 1472: /* transaction_mode_item: READ ONLY  */
#line 11225 "gram.y"
                                        { (yyval.defelt) = makeDefElem("transaction_read_only",
                                       makeIntConst(true, (yylsp[-1])), (yylsp[-1])); }
#line 44684 "gram.c"
    break;

  case 1473: /* transaction_mode_item: READ WRITE  */
#line 11228 "gram.y"
                                        { (yyval.defelt) = makeDefElem("transaction_read_only",
                                       makeIntConst(false, (yylsp[-1])), (yylsp[-1])); }
#line 44691 "gram.c"
    break;

  case 1474: /* transaction_mode_item: DEFERRABLE  */
#line 11231 "gram.y"
                                        { (yyval.defelt) = makeDefElem("transaction_deferrable",
                                       makeIntConst(true, (yylsp[0])), (yylsp[0])); }
#line 44698 "gram.c"
    break;

  case 1475: /* transaction_mode_item: NOT DEFERRABLE  */
#line 11234 "gram.y"
                                        { (yyval.defelt) = makeDefElem("transaction_deferrable",
                                       makeIntConst(false, (yylsp[-1])), (yylsp[-1])); }
#line 44705 "gram.c"
    break;

  case 1476: /* transaction_mode_list: transaction_mode_item  */
#line 11241 "gram.y"
                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 44711 "gram.c"
    break;

  case 1477: /* transaction_mode_list: transaction_mode_list ',' transaction_mode_item  */
#line 11243 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt)); }
#line 44717 "gram.c"
    break;

  case 1478: /* transaction_mode_list: transaction_mode_list transaction_mode_item  */
#line 11245 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 44723 "gram.c"
    break;

  case 1480: /* transaction_mode_list_or_empty: %empty  */
#line 11251 "gram.y"
                                        { (yyval.list) = NIL; }
#line 44729 "gram.c"
    break;

  case 1481: /* opt_transaction_chain: AND CHAIN  */
#line 11255 "gram.y"
                                                { (yyval.boolean) = true; }
#line 44735 "gram.c"
    break;

  case 1482: /* opt_transaction_chain: AND NO CHAIN  */
#line 11256 "gram.y"
                                        { (yyval.boolean) = false; }
#line 44741 "gram.c"
    break;

  case 1483: /* opt_transaction_chain: %empty  */
#line 11257 "gram.y"
                                        { (yyval.boolean) = false; }
#line 44747 "gram.c"
    break;

  case 1484: /* ViewStmt: CREATE OptTemp VIEW qualified_name opt_column_list opt_reloptions AS SelectStmt opt_check_option  */
#line 11271 "gram.y"
                                {
                    ViewStmt   *n = makeNode(ViewStmt);

                    n->view = (yyvsp[-5].range);
                    n->view->relpersistence = (yyvsp[-7].ival);
                    n->aliases = (yyvsp[-4].list);
                    n->query = (yyvsp[-1].node);
                    n->replace = false;
                    n->options = (yyvsp[-3].list);
                    n->withCheckOption = (yyvsp[0].ival);
                    (yyval.node) = (Node *) n;
                }
#line 44764 "gram.c"
    break;

  case 1485: /* ViewStmt: CREATE OR REPLACE OptTemp VIEW qualified_name opt_column_list opt_reloptions AS SelectStmt opt_check_option  */
#line 11285 "gram.y"
                                {
                    ViewStmt   *n = makeNode(ViewStmt);

                    n->view = (yyvsp[-5].range);
                    n->view->relpersistence = (yyvsp[-7].ival);
                    n->aliases = (yyvsp[-4].list);
                    n->query = (yyvsp[-1].node);
                    n->replace = true;
                    n->options = (yyvsp[-3].list);
                    n->withCheckOption = (yyvsp[0].ival);
                    (yyval.node) = (Node *) n;
                }
#line 44781 "gram.c"
    break;

  case 1486: /* ViewStmt: CREATE OptTemp RECURSIVE VIEW qualified_name '(' columnList ')' opt_reloptions AS SelectStmt opt_check_option  */
#line 11299 "gram.y"
                                {
                    ViewStmt   *n = makeNode(ViewStmt);

                    n->view = (yyvsp[-7].range);
                    n->view->relpersistence = (yyvsp[-10].ival);
                    n->aliases = (yyvsp[-5].list);
                    n->query = makeRecursiveViewSelect(n->view->relname, n->aliases, (yyvsp[-1].node));
                    n->replace = false;
                    n->options = (yyvsp[-3].list);
                    n->withCheckOption = (yyvsp[0].ival);
                    if (n->withCheckOption != NO_CHECK_OPTION)
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("WITH CHECK OPTION not supported on recursive views"),
                                 parser_errposition((yylsp[0]))));
                    (yyval.node) = (Node *) n;
                }
#line 44803 "gram.c"
    break;

  case 1487: /* ViewStmt: CREATE OR REPLACE OptTemp RECURSIVE VIEW qualified_name '(' columnList ')' opt_reloptions AS SelectStmt opt_check_option  */
#line 11318 "gram.y"
                                {
                    ViewStmt   *n = makeNode(ViewStmt);

                    n->view = (yyvsp[-7].range);
                    n->view->relpersistence = (yyvsp[-10].ival);
                    n->aliases = (yyvsp[-5].list);
                    n->query = makeRecursiveViewSelect(n->view->relname, n->aliases, (yyvsp[-1].node));
                    n->replace = true;
                    n->options = (yyvsp[-3].list);
                    n->withCheckOption = (yyvsp[0].ival);
                    if (n->withCheckOption != NO_CHECK_OPTION)
                        ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("WITH CHECK OPTION not supported on recursive views"),
                                 parser_errposition((yylsp[0]))));
                    (yyval.node) = (Node *) n;
                }
#line 44825 "gram.c"
    break;

  case 1488: /* opt_check_option: WITH CHECK OPTION  */
#line 11338 "gram.y"
                                                                { (yyval.ival) = CASCADED_CHECK_OPTION; }
#line 44831 "gram.c"
    break;

  case 1489: /* opt_check_option: WITH CASCADED CHECK OPTION  */
#line 11339 "gram.y"
                                                { (yyval.ival) = CASCADED_CHECK_OPTION; }
#line 44837 "gram.c"
    break;

  case 1490: /* opt_check_option: WITH LOCAL CHECK OPTION  */
#line 11340 "gram.y"
                                                        { (yyval.ival) = LOCAL_CHECK_OPTION; }
#line 44843 "gram.c"
    break;

  case 1491: /* opt_check_option: %empty  */
#line 11341 "gram.y"
                                                                { (yyval.ival) = NO_CHECK_OPTION; }
#line 44849 "gram.c"
    break;

  case 1492: /* LoadStmt: LOAD file_name  */
#line 11352 "gram.y"
                                {
                    LoadStmt   *n = makeNode(LoadStmt);

                    n->filename = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 44860 "gram.c"
    break;

  case 1493: /* CreatedbStmt: CREATE DATABASE name opt_with createdb_opt_list  */
#line 11369 "gram.y"
                                {
                    CreatedbStmt *n = makeNode(CreatedbStmt);

                    n->dbname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 44872 "gram.c"
    break;

  case 1494: /* createdb_opt_list: createdb_opt_items  */
#line 11379 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 44878 "gram.c"
    break;

  case 1495: /* createdb_opt_list: %empty  */
#line 11380 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 44884 "gram.c"
    break;

  case 1496: /* createdb_opt_items: createdb_opt_item  */
#line 11384 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 44890 "gram.c"
    break;

  case 1497: /* createdb_opt_items: createdb_opt_items createdb_opt_item  */
#line 11385 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 44896 "gram.c"
    break;

  case 1498: /* createdb_opt_item: createdb_opt_name opt_equal NumericOnly  */
#line 11390 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), (yyvsp[0].node), (yylsp[-2]));
                }
#line 44904 "gram.c"
    break;

  case 1499: /* createdb_opt_item: createdb_opt_name opt_equal opt_boolean_or_string  */
#line 11394 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), (Node *) makeString((yyvsp[0].str)), (yylsp[-2]));
                }
#line 44912 "gram.c"
    break;

  case 1500: /* createdb_opt_item: createdb_opt_name opt_equal DEFAULT  */
#line 11398 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-2].str), NULL, (yylsp[-2]));
                }
#line 44920 "gram.c"
    break;

  case 1501: /* createdb_opt_name: IDENT  */
#line 11415 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 44926 "gram.c"
    break;

  case 1502: /* createdb_opt_name: CONNECTION LIMIT  */
#line 11416 "gram.y"
                                                                        { (yyval.str) = pstrdup("connection_limit"); }
#line 44932 "gram.c"
    break;

  case 1503: /* createdb_opt_name: ENCODING  */
#line 11417 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 44938 "gram.c"
    break;

  case 1504: /* createdb_opt_name: LOCATION  */
#line 11418 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 44944 "gram.c"
    break;

  case 1505: /* createdb_opt_name: OWNER  */
#line 11419 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 44950 "gram.c"
    break;

  case 1506: /* createdb_opt_name: TABLESPACE  */
#line 11420 "gram.y"
                                                                        { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 44956 "gram.c"
    break;

  case 1507: /* createdb_opt_name: TEMPLATE  */
#line 11421 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 44962 "gram.c"
    break;

  case 1510: /* AlterDatabaseStmt: ALTER DATABASE name WITH createdb_opt_list  */
#line 11441 "gram.y"
                                 {
                    AlterDatabaseStmt *n = makeNode(AlterDatabaseStmt);

                    n->dbname = (yyvsp[-2].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                 }
#line 44974 "gram.c"
    break;

  case 1511: /* AlterDatabaseStmt: ALTER DATABASE name createdb_opt_list  */
#line 11449 "gram.y"
                                 {
                    AlterDatabaseStmt *n = makeNode(AlterDatabaseStmt);

                    n->dbname = (yyvsp[-1].str);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                 }
#line 44986 "gram.c"
    break;

  case 1512: /* AlterDatabaseStmt: ALTER DATABASE name SET TABLESPACE name  */
#line 11457 "gram.y"
                                 {
                    AlterDatabaseStmt *n = makeNode(AlterDatabaseStmt);

                    n->dbname = (yyvsp[-3].str);
                    n->options = list_make1(makeDefElem("tablespace",
                                                        (Node *) makeString((yyvsp[0].str)), (yylsp[0])));
                    (yyval.node) = (Node *) n;
                 }
#line 44999 "gram.c"
    break;

  case 1513: /* AlterDatabaseStmt: ALTER DATABASE name REFRESH COLLATION VERSION_P  */
#line 11466 "gram.y"
                                 {
                    AlterDatabaseRefreshCollStmt *n = makeNode(AlterDatabaseRefreshCollStmt);

                    n->dbname = (yyvsp[-3].str);
                    (yyval.node) = (Node *) n;
                 }
#line 45010 "gram.c"
    break;

  case 1514: /* AlterDatabaseSetStmt: ALTER DATABASE name SetResetClause  */
#line 11476 "gram.y"
                                {
                    AlterDatabaseSetStmt *n = makeNode(AlterDatabaseSetStmt);

                    n->dbname = (yyvsp[-1].str);
                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 45022 "gram.c"
    break;

  case 1515: /* DropdbStmt: DROP DATABASE name  */
#line 11494 "gram.y"
                                {
                    DropdbStmt *n = makeNode(DropdbStmt);

                    n->dbname = (yyvsp[0].str);
                    n->missing_ok = false;
                    n->options = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 45035 "gram.c"
    break;

  case 1516: /* DropdbStmt: DROP DATABASE IF_P EXISTS name  */
#line 11503 "gram.y"
                                {
                    DropdbStmt *n = makeNode(DropdbStmt);

                    n->dbname = (yyvsp[0].str);
                    n->missing_ok = true;
                    n->options = NULL;
                    (yyval.node) = (Node *) n;
                }
#line 45048 "gram.c"
    break;

  case 1517: /* DropdbStmt: DROP DATABASE name opt_with '(' drop_option_list ')'  */
#line 11512 "gram.y"
                                {
                    DropdbStmt *n = makeNode(DropdbStmt);

                    n->dbname = (yyvsp[-4].str);
                    n->missing_ok = false;
                    n->options = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 45061 "gram.c"
    break;

  case 1518: /* DropdbStmt: DROP DATABASE IF_P EXISTS name opt_with '(' drop_option_list ')'  */
#line 11521 "gram.y"
                                {
                    DropdbStmt *n = makeNode(DropdbStmt);

                    n->dbname = (yyvsp[-4].str);
                    n->missing_ok = true;
                    n->options = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 45074 "gram.c"
    break;

  case 1519: /* drop_option_list: drop_option  */
#line 11533 "gram.y"
                                {
                    (yyval.list) = list_make1((Node *) (yyvsp[0].defelt));
                }
#line 45082 "gram.c"
    break;

  case 1520: /* drop_option_list: drop_option_list ',' drop_option  */
#line 11537 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (Node *) (yyvsp[0].defelt));
                }
#line 45090 "gram.c"
    break;

  case 1521: /* drop_option: FORCE  */
#line 11548 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem("force", NULL, (yylsp[0]));
                }
#line 45098 "gram.c"
    break;

  case 1522: /* AlterCollationStmt: ALTER COLLATION any_name REFRESH VERSION_P  */
#line 11560 "gram.y"
                                {
                    AlterCollationStmt *n = makeNode(AlterCollationStmt);

                    n->collname = (yyvsp[-2].list);
                    (yyval.node) = (Node *) n;
                }
#line 45109 "gram.c"
    break;

  case 1523: /* AlterSystemStmt: ALTER SYSTEM_P SET generic_set  */
#line 11578 "gram.y"
                                {
                    AlterSystemStmt *n = makeNode(AlterSystemStmt);

                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 45120 "gram.c"
    break;

  case 1524: /* AlterSystemStmt: ALTER SYSTEM_P RESET generic_reset  */
#line 11585 "gram.y"
                                {
                    AlterSystemStmt *n = makeNode(AlterSystemStmt);

                    n->setstmt = (yyvsp[0].vsetstmt);
                    (yyval.node) = (Node *) n;
                }
#line 45131 "gram.c"
    break;

  case 1525: /* CreateDomainStmt: CREATE DOMAIN_P any_name opt_as Typename ColQualList  */
#line 11602 "gram.y"
                                {
                    CreateDomainStmt *n = makeNode(CreateDomainStmt);

                    n->domainname = (yyvsp[-3].list);
                    n->typeName = (yyvsp[-1].typnam);
                    SplitColQualList((yyvsp[0].list), &n->constraints, &n->collClause,
                                     yyscanner);
                    (yyval.node) = (Node *) n;
                }
#line 45145 "gram.c"
    break;

  case 1526: /* AlterDomainStmt: ALTER DOMAIN_P any_name alter_column_default  */
#line 11616 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'T';
                    n->typeName = (yyvsp[-1].list);
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 45158 "gram.c"
    break;

  case 1527: /* AlterDomainStmt: ALTER DOMAIN_P any_name DROP NOT NULL_P  */
#line 11626 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'N';
                    n->typeName = (yyvsp[-3].list);
                    (yyval.node) = (Node *) n;
                }
#line 45170 "gram.c"
    break;

  case 1528: /* AlterDomainStmt: ALTER DOMAIN_P any_name SET NOT NULL_P  */
#line 11635 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'O';
                    n->typeName = (yyvsp[-3].list);
                    (yyval.node) = (Node *) n;
                }
#line 45182 "gram.c"
    break;

  case 1529: /* AlterDomainStmt: ALTER DOMAIN_P any_name ADD_P DomainConstraint  */
#line 11644 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'C';
                    n->typeName = (yyvsp[-2].list);
                    n->def = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 45195 "gram.c"
    break;

  case 1530: /* AlterDomainStmt: ALTER DOMAIN_P any_name DROP CONSTRAINT name opt_drop_behavior  */
#line 11654 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'X';
                    n->typeName = (yyvsp[-4].list);
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 45210 "gram.c"
    break;

  case 1531: /* AlterDomainStmt: ALTER DOMAIN_P any_name DROP CONSTRAINT IF_P EXISTS name opt_drop_behavior  */
#line 11666 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'X';
                    n->typeName = (yyvsp[-6].list);
                    n->name = (yyvsp[-1].str);
                    n->behavior = (yyvsp[0].dbehavior);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 45225 "gram.c"
    break;

  case 1532: /* AlterDomainStmt: ALTER DOMAIN_P any_name VALIDATE CONSTRAINT name  */
#line 11678 "gram.y"
                                {
                    AlterDomainStmt *n = makeNode(AlterDomainStmt);

                    n->subtype = 'V';
                    n->typeName = (yyvsp[-3].list);
                    n->name = (yyvsp[0].str);
                    (yyval.node) = (Node *) n;
                }
#line 45238 "gram.c"
    break;

  case 1535: /* AlterTSDictionaryStmt: ALTER TEXT_P SEARCH DICTIONARY any_name definition  */
#line 11701 "gram.y"
                                {
                    AlterTSDictionaryStmt *n = makeNode(AlterTSDictionaryStmt);

                    n->dictname = (yyvsp[-1].list);
                    n->options = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 45250 "gram.c"
    break;

  case 1536: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name ADD_P MAPPING FOR name_list any_with any_name_list  */
#line 11712 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_ADD_MAPPING;
                    n->cfgname = (yyvsp[-6].list);
                    n->tokentype = (yyvsp[-2].list);
                    n->dicts = (yyvsp[0].list);
                    n->override = false;
                    n->replace = false;
                    (yyval.node) = (Node *) n;
                }
#line 45266 "gram.c"
    break;

  case 1537: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name ALTER MAPPING FOR name_list any_with any_name_list  */
#line 11724 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_ALTER_MAPPING_FOR_TOKEN;
                    n->cfgname = (yyvsp[-6].list);
                    n->tokentype = (yyvsp[-2].list);
                    n->dicts = (yyvsp[0].list);
                    n->override = true;
                    n->replace = false;
                    (yyval.node) = (Node *) n;
                }
#line 45282 "gram.c"
    break;

  case 1538: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name ALTER MAPPING REPLACE any_name any_with any_name  */
#line 11736 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_REPLACE_DICT;
                    n->cfgname = (yyvsp[-6].list);
                    n->tokentype = NIL;
                    n->dicts = list_make2((yyvsp[-2].list),(yyvsp[0].list));
                    n->override = false;
                    n->replace = true;
                    (yyval.node) = (Node *) n;
                }
#line 45298 "gram.c"
    break;

  case 1539: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name ALTER MAPPING FOR name_list REPLACE any_name any_with any_name  */
#line 11748 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_REPLACE_DICT_FOR_TOKEN;
                    n->cfgname = (yyvsp[-8].list);
                    n->tokentype = (yyvsp[-4].list);
                    n->dicts = list_make2((yyvsp[-2].list),(yyvsp[0].list));
                    n->override = false;
                    n->replace = true;
                    (yyval.node) = (Node *) n;
                }
#line 45314 "gram.c"
    break;

  case 1540: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name DROP MAPPING FOR name_list  */
#line 11760 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_DROP_MAPPING;
                    n->cfgname = (yyvsp[-4].list);
                    n->tokentype = (yyvsp[0].list);
                    n->missing_ok = false;
                    (yyval.node) = (Node *) n;
                }
#line 45328 "gram.c"
    break;

  case 1541: /* AlterTSConfigurationStmt: ALTER TEXT_P SEARCH CONFIGURATION any_name DROP MAPPING IF_P EXISTS FOR name_list  */
#line 11770 "gram.y"
                                {
                    AlterTSConfigurationStmt *n = makeNode(AlterTSConfigurationStmt);

                    n->kind = ALTER_TSCONFIG_DROP_MAPPING;
                    n->cfgname = (yyvsp[-6].list);
                    n->tokentype = (yyvsp[0].list);
                    n->missing_ok = true;
                    (yyval.node) = (Node *) n;
                }
#line 45342 "gram.c"
    break;

  case 1544: /* CreateConversionStmt: CREATE opt_default CONVERSION_P any_name FOR Sconst TO Sconst FROM any_name  */
#line 11799 "gram.y"
                        {
                CreateConversionStmt *n = makeNode(CreateConversionStmt);

                n->conversion_name = (yyvsp[-6].list);
                n->for_encoding_name = (yyvsp[-4].str);
                n->to_encoding_name = (yyvsp[-2].str);
                n->func_name = (yyvsp[0].list);
                n->def = (yyvsp[-8].boolean);
                (yyval.node) = (Node *) n;
            }
#line 45357 "gram.c"
    break;

  case 1545: /* ClusterStmt: CLUSTER '(' utility_option_list ')' qualified_name cluster_index_specification  */
#line 11822 "gram.y"
                                {
                    ClusterStmt *n = makeNode(ClusterStmt);

                    n->relation = (yyvsp[-1].range);
                    n->indexname = (yyvsp[0].str);
                    n->params = (yyvsp[-3].list);
                    (yyval.node) = (Node *) n;
                }
#line 45370 "gram.c"
    break;

  case 1546: /* ClusterStmt: CLUSTER '(' utility_option_list ')'  */
#line 11831 "gram.y"
                                {
                    ClusterStmt *n = makeNode(ClusterStmt);

                    n->relation = NULL;
                    n->indexname = NULL;
                    n->params = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 45383 "gram.c"
    break;

  case 1547: /* ClusterStmt: CLUSTER opt_verbose qualified_name cluster_index_specification  */
#line 11841 "gram.y"
                                {
                    ClusterStmt *n = makeNode(ClusterStmt);

                    n->relation = (yyvsp[-1].range);
                    n->indexname = (yyvsp[0].str);
                    n->params = NIL;
                    if ((yyvsp[-2].boolean))
                        n->params = lappend(n->params, makeDefElem("verbose", NULL, (yylsp[-2])));
                    (yyval.node) = (Node *) n;
                }
#line 45398 "gram.c"
    break;

  case 1548: /* ClusterStmt: CLUSTER opt_verbose  */
#line 11853 "gram.y"
                                {
                    ClusterStmt *n = makeNode(ClusterStmt);

                    n->relation = NULL;
                    n->indexname = NULL;
                    n->params = NIL;
                    if ((yyvsp[0].boolean))
                        n->params = lappend(n->params, makeDefElem("verbose", NULL, (yylsp[0])));
                    (yyval.node) = (Node *) n;
                }
#line 45413 "gram.c"
    break;

  case 1549: /* ClusterStmt: CLUSTER opt_verbose name ON qualified_name  */
#line 11865 "gram.y"
                                {
                    ClusterStmt *n = makeNode(ClusterStmt);

                    n->relation = (yyvsp[0].range);
                    n->indexname = (yyvsp[-2].str);
                    n->params = NIL;
                    if ((yyvsp[-3].boolean))
                        n->params = lappend(n->params, makeDefElem("verbose", NULL, (yylsp[-3])));
                    (yyval.node) = (Node *) n;
                }
#line 45428 "gram.c"
    break;

  case 1550: /* cluster_index_specification: USING name  */
#line 11878 "gram.y"
                                                                { (yyval.str) = (yyvsp[0].str); }
#line 45434 "gram.c"
    break;

  case 1551: /* cluster_index_specification: %empty  */
#line 11879 "gram.y"
                                                                { (yyval.str) = NULL; }
#line 45440 "gram.c"
    break;

  case 1552: /* VacuumStmt: VACUUM opt_full opt_freeze opt_verbose opt_analyze opt_vacuum_relation_list  */
#line 11892 "gram.y"
                                {
                    VacuumStmt *n = makeNode(VacuumStmt);

                    n->options = NIL;
                    if ((yyvsp[-4].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("full", NULL, (yylsp[-4])));
                    if ((yyvsp[-3].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("freeze", NULL, (yylsp[-3])));
                    if ((yyvsp[-2].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("verbose", NULL, (yylsp[-2])));
                    if ((yyvsp[-1].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("analyze", NULL, (yylsp[-1])));
                    n->rels = (yyvsp[0].list);
                    n->is_vacuumcmd = true;
                    (yyval.node) = (Node *) n;
                }
#line 45465 "gram.c"
    break;

  case 1553: /* VacuumStmt: VACUUM '(' utility_option_list ')' opt_vacuum_relation_list  */
#line 11913 "gram.y"
                                {
                    VacuumStmt *n = makeNode(VacuumStmt);

                    n->options = (yyvsp[-2].list);
                    n->rels = (yyvsp[0].list);
                    n->is_vacuumcmd = true;
                    (yyval.node) = (Node *) n;
                }
#line 45478 "gram.c"
    break;

  case 1554: /* AnalyzeStmt: analyze_keyword opt_verbose opt_vacuum_relation_list  */
#line 11924 "gram.y"
                                {
                    VacuumStmt *n = makeNode(VacuumStmt);

                    n->options = NIL;
                    if ((yyvsp[-1].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("verbose", NULL, (yylsp[-1])));
                    n->rels = (yyvsp[0].list);
                    n->is_vacuumcmd = false;
                    (yyval.node) = (Node *) n;
                }
#line 45494 "gram.c"
    break;

  case 1555: /* AnalyzeStmt: analyze_keyword '(' utility_option_list ')' opt_vacuum_relation_list  */
#line 11936 "gram.y"
                                {
                    VacuumStmt *n = makeNode(VacuumStmt);

                    n->options = (yyvsp[-2].list);
                    n->rels = (yyvsp[0].list);
                    n->is_vacuumcmd = false;
                    (yyval.node) = (Node *) n;
                }
#line 45507 "gram.c"
    break;

  case 1556: /* utility_option_list: utility_option_elem  */
#line 11948 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].defelt));
                }
#line 45515 "gram.c"
    break;

  case 1557: /* utility_option_list: utility_option_list ',' utility_option_elem  */
#line 11952 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].defelt));
                }
#line 45523 "gram.c"
    break;

  case 1560: /* utility_option_elem: utility_option_name utility_option_arg  */
#line 11964 "gram.y"
                                {
                    (yyval.defelt) = makeDefElem((yyvsp[-1].str), (yyvsp[0].node), (yylsp[-1]));
                }
#line 45531 "gram.c"
    break;

  case 1561: /* utility_option_name: NonReservedWord  */
#line 11970 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 45537 "gram.c"
    break;

  case 1562: /* utility_option_name: analyze_keyword  */
#line 11971 "gram.y"
                                                                                        { (yyval.str) = "analyze"; }
#line 45543 "gram.c"
    break;

  case 1563: /* utility_option_name: FORMAT_LA  */
#line 11972 "gram.y"
                                                                                                { (yyval.str) = "format"; }
#line 45549 "gram.c"
    break;

  case 1564: /* utility_option_arg: opt_boolean_or_string  */
#line 11976 "gram.y"
                                                                                { (yyval.node) = (Node *) makeString((yyvsp[0].str)); }
#line 45555 "gram.c"
    break;

  case 1565: /* utility_option_arg: NumericOnly  */
#line 11977 "gram.y"
                                                                                        { (yyval.node) = (Node *) (yyvsp[0].node); }
#line 45561 "gram.c"
    break;

  case 1566: /* utility_option_arg: %empty  */
#line 11978 "gram.y"
                                                                                        { (yyval.node) = NULL; }
#line 45567 "gram.c"
    break;

  case 1567: /* opt_analyze: analyze_keyword  */
#line 11982 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 45573 "gram.c"
    break;

  case 1568: /* opt_analyze: %empty  */
#line 11983 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 45579 "gram.c"
    break;

  case 1569: /* opt_verbose: VERBOSE  */
#line 11987 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 45585 "gram.c"
    break;

  case 1570: /* opt_verbose: %empty  */
#line 11988 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 45591 "gram.c"
    break;

  case 1571: /* opt_full: FULL  */
#line 11991 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 45597 "gram.c"
    break;

  case 1572: /* opt_full: %empty  */
#line 11992 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 45603 "gram.c"
    break;

  case 1573: /* opt_freeze: FREEZE  */
#line 11995 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 45609 "gram.c"
    break;

  case 1574: /* opt_freeze: %empty  */
#line 11996 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 45615 "gram.c"
    break;

  case 1575: /* opt_name_list: '(' name_list ')'  */
#line 12000 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 45621 "gram.c"
    break;

  case 1576: /* opt_name_list: %empty  */
#line 12001 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 45627 "gram.c"
    break;

  case 1577: /* vacuum_relation: qualified_name opt_name_list  */
#line 12006 "gram.y"
                                {
                    (yyval.node) = (Node *) makeVacuumRelation((yyvsp[-1].range), InvalidOid, (yyvsp[0].list));
                }
#line 45635 "gram.c"
    break;

  case 1578: /* vacuum_relation_list: vacuum_relation  */
#line 12013 "gram.y"
                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 45641 "gram.c"
    break;

  case 1579: /* vacuum_relation_list: vacuum_relation_list ',' vacuum_relation  */
#line 12015 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 45647 "gram.c"
    break;

  case 1580: /* opt_vacuum_relation_list: vacuum_relation_list  */
#line 12019 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 45653 "gram.c"
    break;

  case 1581: /* opt_vacuum_relation_list: %empty  */
#line 12020 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 45659 "gram.c"
    break;

  case 1582: /* ExplainStmt: EXPLAIN ExplainableStmt  */
#line 12034 "gram.y"
                                {
                    ExplainStmt *n = makeNode(ExplainStmt);

                    n->query = (yyvsp[0].node);
                    n->options = NIL;
                    (yyval.node) = (Node *) n;
                }
#line 45671 "gram.c"
    break;

  case 1583: /* ExplainStmt: EXPLAIN analyze_keyword opt_verbose ExplainableStmt  */
#line 12042 "gram.y"
                                {
                    ExplainStmt *n = makeNode(ExplainStmt);

                    n->query = (yyvsp[0].node);
                    n->options = list_make1(makeDefElem("analyze", NULL, (yylsp[-2])));
                    if ((yyvsp[-1].boolean))
                        n->options = lappend(n->options,
                                             makeDefElem("verbose", NULL, (yylsp[-1])));
                    (yyval.node) = (Node *) n;
                }
#line 45686 "gram.c"
    break;

  case 1584: /* ExplainStmt: EXPLAIN VERBOSE ExplainableStmt  */
#line 12053 "gram.y"
                                {
                    ExplainStmt *n = makeNode(ExplainStmt);

                    n->query = (yyvsp[0].node);
                    n->options = list_make1(makeDefElem("verbose", NULL, (yylsp[-1])));
                    (yyval.node) = (Node *) n;
                }
#line 45698 "gram.c"
    break;

  case 1585: /* ExplainStmt: EXPLAIN '(' utility_option_list ')' ExplainableStmt  */
#line 12061 "gram.y"
                                {
                    ExplainStmt *n = makeNode(ExplainStmt);

                    n->query = (yyvsp[0].node);
                    n->options = (yyvsp[-2].list);
                    (yyval.node) = (Node *) n;
                }
#line 45710 "gram.c"
    break;

  case 1596: /* PrepareStmt: PREPARE name prep_type_clause AS PreparableStmt  */
#line 12091 "gram.y"
                                {
                    PrepareStmt *n = makeNode(PrepareStmt);

                    n->name = (yyvsp[-3].str);
                    n->argtypes = (yyvsp[-2].list);
                    n->query = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 45723 "gram.c"
    break;

  case 1597: /* prep_type_clause: '(' type_list ')'  */
#line 12101 "gram.y"
                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 45729 "gram.c"
    break;

  case 1598: /* prep_type_clause: %empty  */
#line 12102 "gram.y"
                                                                        { (yyval.list) = NIL; }
#line 45735 "gram.c"
    break;

  case 1604: /* ExecuteStmt: EXECUTE name execute_param_clause  */
#line 12121 "gram.y"
                                {
                    ExecuteStmt *n = makeNode(ExecuteStmt);

                    n->name = (yyvsp[-1].str);
                    n->params = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 45747 "gram.c"
    break;

  case 1605: /* ExecuteStmt: CREATE OptTemp TABLE create_as_target AS EXECUTE name execute_param_clause opt_with_data  */
#line 12130 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);
                    ExecuteStmt *n = makeNode(ExecuteStmt);

                    n->name = (yyvsp[-2].str);
                    n->params = (yyvsp[-1].list);
                    ctas->query = (Node *) n;
                    ctas->into = (yyvsp[-5].into);
                    ctas->objtype = OBJECT_TABLE;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = false;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-5].into)->rel->relpersistence = (yyvsp[-7].ival);
                    (yyvsp[-5].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 45768 "gram.c"
    break;

  case 1606: /* ExecuteStmt: CREATE OptTemp TABLE IF_P NOT EXISTS create_as_target AS EXECUTE name execute_param_clause opt_with_data  */
#line 12148 "gram.y"
                                {
                    CreateTableAsStmt *ctas = makeNode(CreateTableAsStmt);
                    ExecuteStmt *n = makeNode(ExecuteStmt);

                    n->name = (yyvsp[-2].str);
                    n->params = (yyvsp[-1].list);
                    ctas->query = (Node *) n;
                    ctas->into = (yyvsp[-5].into);
                    ctas->objtype = OBJECT_TABLE;
                    ctas->is_select_into = false;
                    ctas->if_not_exists = true;
                    /* cram additional flags into the IntoClause */
                    (yyvsp[-5].into)->rel->relpersistence = (yyvsp[-10].ival);
                    (yyvsp[-5].into)->skipData = !((yyvsp[0].boolean));
                    (yyval.node) = (Node *) ctas;
                }
#line 45789 "gram.c"
    break;

  case 1607: /* execute_param_clause: '(' expr_list ')'  */
#line 12166 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 45795 "gram.c"
    break;

  case 1608: /* execute_param_clause: %empty  */
#line 12167 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 45801 "gram.c"
    break;

  case 1609: /* DeallocateStmt: DEALLOCATE name  */
#line 12178 "gram.y"
                                        {
                        DeallocateStmt *n = makeNode(DeallocateStmt);

                        n->name = (yyvsp[0].str);
                        n->isall = false;
                        n->location = (yylsp[0]);
                        (yyval.node) = (Node *) n;
                    }
#line 45814 "gram.c"
    break;

  case 1610: /* DeallocateStmt: DEALLOCATE PREPARE name  */
#line 12187 "gram.y"
                                        {
                        DeallocateStmt *n = makeNode(DeallocateStmt);

                        n->name = (yyvsp[0].str);
                        n->isall = false;
                        n->location = (yylsp[0]);
                        (yyval.node) = (Node *) n;
                    }
#line 45827 "gram.c"
    break;

  case 1611: /* DeallocateStmt: DEALLOCATE ALL  */
#line 12196 "gram.y"
                                        {
                        DeallocateStmt *n = makeNode(DeallocateStmt);

                        n->name = NULL;
                        n->isall = true;
                        n->location = -1;
                        (yyval.node) = (Node *) n;
                    }
#line 45840 "gram.c"
    break;

  case 1612: /* DeallocateStmt: DEALLOCATE PREPARE ALL  */
#line 12205 "gram.y"
                                        {
                        DeallocateStmt *n = makeNode(DeallocateStmt);

                        n->name = NULL;
                        n->isall = true;
                        n->location = -1;
                        (yyval.node) = (Node *) n;
                    }
#line 45853 "gram.c"
    break;

  case 1613: /* InsertStmt: opt_with_clause INSERT INTO insert_target insert_rest opt_on_conflict returning_clause  */
#line 12225 "gram.y"
                                {
                    (yyvsp[-2].istmt)->relation = (yyvsp[-3].range);
                    (yyvsp[-2].istmt)->onConflictClause = (yyvsp[-1].onconflict);
                    (yyvsp[-2].istmt)->returningList = (yyvsp[0].list);
                    (yyvsp[-2].istmt)->withClause = (yyvsp[-6].with);
                    (yyval.node) = (Node *) (yyvsp[-2].istmt);
                }
#line 45865 "gram.c"
    break;

  case 1614: /* insert_target: qualified_name  */
#line 12242 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                }
#line 45873 "gram.c"
    break;

  case 1615: /* insert_target: qualified_name AS ColId  */
#line 12246 "gram.y"
                                {
                    (yyvsp[-2].range)->alias = makeAlias((yyvsp[0].str), NIL);
                    (yyval.range) = (yyvsp[-2].range);
                }
#line 45882 "gram.c"
    break;

  case 1616: /* insert_rest: SelectStmt  */
#line 12254 "gram.y"
                                {
                    (yyval.istmt) = makeNode(InsertStmt);
                    (yyval.istmt)->cols = NIL;
                    (yyval.istmt)->selectStmt = (yyvsp[0].node);
                }
#line 45892 "gram.c"
    break;

  case 1617: /* insert_rest: OVERRIDING override_kind VALUE_P SelectStmt  */
#line 12260 "gram.y"
                                {
                    (yyval.istmt) = makeNode(InsertStmt);
                    (yyval.istmt)->cols = NIL;
                    (yyval.istmt)->override = (yyvsp[-2].ival);
                    (yyval.istmt)->selectStmt = (yyvsp[0].node);
                }
#line 45903 "gram.c"
    break;

  case 1618: /* insert_rest: '(' insert_column_list ')' SelectStmt  */
#line 12267 "gram.y"
                                {
                    (yyval.istmt) = makeNode(InsertStmt);
                    (yyval.istmt)->cols = (yyvsp[-2].list);
                    (yyval.istmt)->selectStmt = (yyvsp[0].node);
                }
#line 45913 "gram.c"
    break;

  case 1619: /* insert_rest: '(' insert_column_list ')' OVERRIDING override_kind VALUE_P SelectStmt  */
#line 12273 "gram.y"
                                {
                    (yyval.istmt) = makeNode(InsertStmt);
                    (yyval.istmt)->cols = (yyvsp[-5].list);
                    (yyval.istmt)->override = (yyvsp[-2].ival);
                    (yyval.istmt)->selectStmt = (yyvsp[0].node);
                }
#line 45924 "gram.c"
    break;

  case 1620: /* insert_rest: DEFAULT VALUES  */
#line 12280 "gram.y"
                                {
                    (yyval.istmt) = makeNode(InsertStmt);
                    (yyval.istmt)->cols = NIL;
                    (yyval.istmt)->selectStmt = NULL;
                }
#line 45934 "gram.c"
    break;

  case 1621: /* override_kind: USER  */
#line 12288 "gram.y"
                                        { (yyval.ival) = OVERRIDING_USER_VALUE; }
#line 45940 "gram.c"
    break;

  case 1622: /* override_kind: SYSTEM_P  */
#line 12289 "gram.y"
                                        { (yyval.ival) = OVERRIDING_SYSTEM_VALUE; }
#line 45946 "gram.c"
    break;

  case 1623: /* insert_column_list: insert_column_item  */
#line 12294 "gram.y"
                                        { (yyval.list) = list_make1((yyvsp[0].target)); }
#line 45952 "gram.c"
    break;

  case 1624: /* insert_column_list: insert_column_list ',' insert_column_item  */
#line 12296 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].target)); }
#line 45958 "gram.c"
    break;

  case 1625: /* insert_column_item: ColId opt_indirection  */
#line 12301 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[-1].str);
                    (yyval.target)->indirection = check_indirection((yyvsp[0].list), yyscanner);
                    (yyval.target)->val = NULL;
                    (yyval.target)->location = (yylsp[-1]);
                }
#line 45970 "gram.c"
    break;

  case 1626: /* opt_on_conflict: ON CONFLICT opt_conf_expr DO UPDATE SET set_clause_list where_clause  */
#line 12312 "gram.y"
                                {
                    (yyval.onconflict) = makeNode(OnConflictClause);
                    (yyval.onconflict)->action = ONCONFLICT_UPDATE;
                    (yyval.onconflict)->infer = (yyvsp[-5].infer);
                    (yyval.onconflict)->targetList = (yyvsp[-1].list);
                    (yyval.onconflict)->whereClause = (yyvsp[0].node);
                    (yyval.onconflict)->location = (yylsp[-7]);
                }
#line 45983 "gram.c"
    break;

  case 1627: /* opt_on_conflict: ON CONFLICT opt_conf_expr DO NOTHING  */
#line 12322 "gram.y"
                                {
                    (yyval.onconflict) = makeNode(OnConflictClause);
                    (yyval.onconflict)->action = ONCONFLICT_NOTHING;
                    (yyval.onconflict)->infer = (yyvsp[-2].infer);
                    (yyval.onconflict)->targetList = NIL;
                    (yyval.onconflict)->whereClause = NULL;
                    (yyval.onconflict)->location = (yylsp[-4]);
                }
#line 45996 "gram.c"
    break;

  case 1628: /* opt_on_conflict: %empty  */
#line 12331 "gram.y"
                                {
                    (yyval.onconflict) = NULL;
                }
#line 46004 "gram.c"
    break;

  case 1629: /* opt_conf_expr: '(' index_params ')' where_clause  */
#line 12338 "gram.y"
                                {
                    (yyval.infer) = makeNode(InferClause);
                    (yyval.infer)->indexElems = (yyvsp[-2].list);
                    (yyval.infer)->whereClause = (yyvsp[0].node);
                    (yyval.infer)->conname = NULL;
                    (yyval.infer)->location = (yylsp[-3]);
                }
#line 46016 "gram.c"
    break;

  case 1630: /* opt_conf_expr: ON CONSTRAINT name  */
#line 12347 "gram.y"
                                {
                    (yyval.infer) = makeNode(InferClause);
                    (yyval.infer)->indexElems = NIL;
                    (yyval.infer)->whereClause = NULL;
                    (yyval.infer)->conname = (yyvsp[0].str);
                    (yyval.infer)->location = (yylsp[-2]);
                }
#line 46028 "gram.c"
    break;

  case 1631: /* opt_conf_expr: %empty  */
#line 12355 "gram.y"
                                {
                    (yyval.infer) = NULL;
                }
#line 46036 "gram.c"
    break;

  case 1632: /* returning_clause: RETURNING target_list  */
#line 12361 "gram.y"
                                                        { (yyval.list) = (yyvsp[0].list); }
#line 46042 "gram.c"
    break;

  case 1633: /* returning_clause: %empty  */
#line 12362 "gram.y"
                                                                { (yyval.list) = NIL; }
#line 46048 "gram.c"
    break;

  case 1634: /* DeleteStmt: opt_with_clause DELETE_P FROM relation_expr_opt_alias using_clause where_or_current_clause returning_clause  */
#line 12375 "gram.y"
                                {
                    DeleteStmt *n = makeNode(DeleteStmt);

                    n->relation = (yyvsp[-3].range);
                    n->usingClause = (yyvsp[-2].list);
                    n->whereClause = (yyvsp[-1].node);
                    n->returningList = (yyvsp[0].list);
                    n->withClause = (yyvsp[-6].with);
                    (yyval.node) = (Node *) n;
                }
#line 46063 "gram.c"
    break;

  case 1635: /* using_clause: USING from_list  */
#line 12388 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 46069 "gram.c"
    break;

  case 1636: /* using_clause: %empty  */
#line 12389 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 46075 "gram.c"
    break;

  case 1637: /* LockStmt: LOCK_P opt_table relation_expr_list opt_lock opt_nowait  */
#line 12401 "gram.y"
                                {
                    LockStmt   *n = makeNode(LockStmt);

                    n->relations = (yyvsp[-2].list);
                    n->mode = (yyvsp[-1].ival);
                    n->nowait = (yyvsp[0].boolean);
                    (yyval.node) = (Node *) n;
                }
#line 46088 "gram.c"
    break;

  case 1638: /* opt_lock: IN_P lock_type MODE  */
#line 12411 "gram.y"
                                                                { (yyval.ival) = (yyvsp[-1].ival); }
#line 46094 "gram.c"
    break;

  case 1639: /* opt_lock: %empty  */
#line 12412 "gram.y"
                                                                                { (yyval.ival) = AccessExclusiveLock; }
#line 46100 "gram.c"
    break;

  case 1640: /* lock_type: ACCESS SHARE  */
#line 12415 "gram.y"
                                                                { (yyval.ival) = AccessShareLock; }
#line 46106 "gram.c"
    break;

  case 1641: /* lock_type: ROW SHARE  */
#line 12416 "gram.y"
                                                                                { (yyval.ival) = RowShareLock; }
#line 46112 "gram.c"
    break;

  case 1642: /* lock_type: ROW EXCLUSIVE  */
#line 12417 "gram.y"
                                                                        { (yyval.ival) = RowExclusiveLock; }
#line 46118 "gram.c"
    break;

  case 1643: /* lock_type: SHARE UPDATE EXCLUSIVE  */
#line 12418 "gram.y"
                                                                { (yyval.ival) = ShareUpdateExclusiveLock; }
#line 46124 "gram.c"
    break;

  case 1644: /* lock_type: SHARE  */
#line 12419 "gram.y"
                                                                                { (yyval.ival) = ShareLock; }
#line 46130 "gram.c"
    break;

  case 1645: /* lock_type: SHARE ROW EXCLUSIVE  */
#line 12420 "gram.y"
                                                                { (yyval.ival) = ShareRowExclusiveLock; }
#line 46136 "gram.c"
    break;

  case 1646: /* lock_type: EXCLUSIVE  */
#line 12421 "gram.y"
                                                                                { (yyval.ival) = ExclusiveLock; }
#line 46142 "gram.c"
    break;

  case 1647: /* lock_type: ACCESS EXCLUSIVE  */
#line 12422 "gram.y"
                                                                        { (yyval.ival) = AccessExclusiveLock; }
#line 46148 "gram.c"
    break;

  case 1648: /* opt_nowait: NOWAIT  */
#line 12425 "gram.y"
                                                                        { (yyval.boolean) = true; }
#line 46154 "gram.c"
    break;

  case 1649: /* opt_nowait: %empty  */
#line 12426 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 46160 "gram.c"
    break;

  case 1650: /* opt_nowait_or_skip: NOWAIT  */
#line 12430 "gram.y"
                                                                                { (yyval.ival) = LockWaitError; }
#line 46166 "gram.c"
    break;

  case 1651: /* opt_nowait_or_skip: SKIP LOCKED  */
#line 12431 "gram.y"
                                                                        { (yyval.ival) = LockWaitSkip; }
#line 46172 "gram.c"
    break;

  case 1652: /* opt_nowait_or_skip: %empty  */
#line 12432 "gram.y"
                                                                                { (yyval.ival) = LockWaitBlock; }
#line 46178 "gram.c"
    break;

  case 1653: /* UpdateStmt: opt_with_clause UPDATE relation_expr_opt_alias SET set_clause_list from_clause where_or_current_clause returning_clause  */
#line 12448 "gram.y"
                                {
                    UpdateStmt *n = makeNode(UpdateStmt);

                    n->relation = (yyvsp[-5].range);
                    n->targetList = (yyvsp[-3].list);
                    n->fromClause = (yyvsp[-2].list);
                    n->whereClause = (yyvsp[-1].node);
                    n->returningList = (yyvsp[0].list);
                    n->withClause = (yyvsp[-7].with);
                    (yyval.node) = (Node *) n;
                }
#line 46194 "gram.c"
    break;

  case 1654: /* set_clause_list: set_clause  */
#line 12462 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 46200 "gram.c"
    break;

  case 1655: /* set_clause_list: set_clause_list ',' set_clause  */
#line 12463 "gram.y"
                                                                { (yyval.list) = list_concat((yyvsp[-2].list),(yyvsp[0].list)); }
#line 46206 "gram.c"
    break;

  case 1656: /* set_clause: set_target '=' a_expr  */
#line 12468 "gram.y"
                                {
                    (yyvsp[-2].target)->val = (Node *) (yyvsp[0].node);
                    (yyval.list) = list_make1((yyvsp[-2].target));
                }
#line 46215 "gram.c"
    break;

  case 1657: /* set_clause: '(' set_target_list ')' '=' a_expr  */
#line 12473 "gram.y"
                                {
                    int         ncolumns = list_length((yyvsp[-3].list));
                    int         i = 1;
                    ListCell   *col_cell;

                    /* Create a MultiAssignRef source for each target */
                    foreach(col_cell, (yyvsp[-3].list))
                    {
                        ResTarget  *res_col = (ResTarget *) lfirst(col_cell);
                        MultiAssignRef *r = makeNode(MultiAssignRef);

                        r->source = (Node *) (yyvsp[0].node);
                        r->colno = i;
                        r->ncolumns = ncolumns;
                        res_col->val = (Node *) r;
                        i++;
                    }

                    (yyval.list) = (yyvsp[-3].list);
                }
#line 46240 "gram.c"
    break;

  case 1658: /* set_target: ColId opt_indirection  */
#line 12497 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[-1].str);
                    (yyval.target)->indirection = check_indirection((yyvsp[0].list), yyscanner);
                    (yyval.target)->val = NULL; /* upper production sets this */
                    (yyval.target)->location = (yylsp[-1]);
                }
#line 46252 "gram.c"
    break;

  case 1659: /* set_target_list: set_target  */
#line 12507 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].target)); }
#line 46258 "gram.c"
    break;

  case 1660: /* set_target_list: set_target_list ',' set_target  */
#line 12508 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list),(yyvsp[0].target)); }
#line 46264 "gram.c"
    break;

  case 1661: /* MergeStmt: opt_with_clause MERGE INTO relation_expr_opt_alias USING table_ref ON a_expr merge_when_list returning_clause  */
#line 12525 "gram.y"
                                {
                    MergeStmt  *m = makeNode(MergeStmt);

                    m->withClause = (yyvsp[-9].with);
                    m->relation = (yyvsp[-6].range);
                    m->sourceRelation = (yyvsp[-4].node);
                    m->joinCondition = (yyvsp[-2].node);
                    m->mergeWhenClauses = (yyvsp[-1].list);
                    m->returningList = (yyvsp[0].list);

                    (yyval.node) = (Node *) m;
                }
#line 46281 "gram.c"
    break;

  case 1662: /* merge_when_list: merge_when_clause  */
#line 12540 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 46287 "gram.c"
    break;

  case 1663: /* merge_when_list: merge_when_list merge_when_clause  */
#line 12541 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list),(yyvsp[0].node)); }
#line 46293 "gram.c"
    break;

  case 1664: /* merge_when_clause: merge_when_tgt_matched opt_merge_when_condition THEN merge_update  */
#line 12552 "gram.y"
                                {
                    (yyvsp[0].mergewhen)->matchKind = (yyvsp[-3].mergematch);
                    (yyvsp[0].mergewhen)->condition = (yyvsp[-2].node);

                    (yyval.node) = (Node *) (yyvsp[0].mergewhen);
                }
#line 46304 "gram.c"
    break;

  case 1665: /* merge_when_clause: merge_when_tgt_matched opt_merge_when_condition THEN merge_delete  */
#line 12559 "gram.y"
                                {
                    (yyvsp[0].mergewhen)->matchKind = (yyvsp[-3].mergematch);
                    (yyvsp[0].mergewhen)->condition = (yyvsp[-2].node);

                    (yyval.node) = (Node *) (yyvsp[0].mergewhen);
                }
#line 46315 "gram.c"
    break;

  case 1666: /* merge_when_clause: merge_when_tgt_not_matched opt_merge_when_condition THEN merge_insert  */
#line 12566 "gram.y"
                                {
                    (yyvsp[0].mergewhen)->matchKind = (yyvsp[-3].mergematch);
                    (yyvsp[0].mergewhen)->condition = (yyvsp[-2].node);

                    (yyval.node) = (Node *) (yyvsp[0].mergewhen);
                }
#line 46326 "gram.c"
    break;

  case 1667: /* merge_when_clause: merge_when_tgt_matched opt_merge_when_condition THEN DO NOTHING  */
#line 12573 "gram.y"
                                {
                    MergeWhenClause *m = makeNode(MergeWhenClause);

                    m->matchKind = (yyvsp[-4].mergematch);
                    m->commandType = CMD_NOTHING;
                    m->condition = (yyvsp[-3].node);

                    (yyval.node) = (Node *) m;
                }
#line 46340 "gram.c"
    break;

  case 1668: /* merge_when_clause: merge_when_tgt_not_matched opt_merge_when_condition THEN DO NOTHING  */
#line 12583 "gram.y"
                                {
                    MergeWhenClause *m = makeNode(MergeWhenClause);

                    m->matchKind = (yyvsp[-4].mergematch);
                    m->commandType = CMD_NOTHING;
                    m->condition = (yyvsp[-3].node);

                    (yyval.node) = (Node *) m;
                }
#line 46354 "gram.c"
    break;

  case 1669: /* merge_when_tgt_matched: WHEN MATCHED  */
#line 12595 "gram.y"
                                                                        { (yyval.mergematch) = MERGE_WHEN_MATCHED; }
#line 46360 "gram.c"
    break;

  case 1670: /* merge_when_tgt_matched: WHEN NOT MATCHED BY SOURCE  */
#line 12596 "gram.y"
                                                        { (yyval.mergematch) = MERGE_WHEN_NOT_MATCHED_BY_SOURCE; }
#line 46366 "gram.c"
    break;

  case 1671: /* merge_when_tgt_not_matched: WHEN NOT MATCHED  */
#line 12600 "gram.y"
                                                                        { (yyval.mergematch) = MERGE_WHEN_NOT_MATCHED_BY_TARGET; }
#line 46372 "gram.c"
    break;

  case 1672: /* merge_when_tgt_not_matched: WHEN NOT MATCHED BY TARGET  */
#line 12601 "gram.y"
                                                        { (yyval.mergematch) = MERGE_WHEN_NOT_MATCHED_BY_TARGET; }
#line 46378 "gram.c"
    break;

  case 1673: /* opt_merge_when_condition: AND a_expr  */
#line 12605 "gram.y"
                                                                { (yyval.node) = (yyvsp[0].node); }
#line 46384 "gram.c"
    break;

  case 1674: /* opt_merge_when_condition: %empty  */
#line 12606 "gram.y"
                                                                        { (yyval.node) = NULL; }
#line 46390 "gram.c"
    break;

  case 1675: /* merge_update: UPDATE SET set_clause_list  */
#line 12611 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_UPDATE;
                    n->override = OVERRIDING_NOT_SET;
                    n->targetList = (yyvsp[0].list);
                    n->values = NIL;

                    (yyval.mergewhen) = n;
                }
#line 46404 "gram.c"
    break;

  case 1676: /* merge_delete: DELETE_P  */
#line 12624 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_DELETE;
                    n->override = OVERRIDING_NOT_SET;
                    n->targetList = NIL;
                    n->values = NIL;

                    (yyval.mergewhen) = n;
                }
#line 46418 "gram.c"
    break;

  case 1677: /* merge_insert: INSERT merge_values_clause  */
#line 12637 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_INSERT;
                    n->override = OVERRIDING_NOT_SET;
                    n->targetList = NIL;
                    n->values = (yyvsp[0].list);
                    (yyval.mergewhen) = n;
                }
#line 46431 "gram.c"
    break;

  case 1678: /* merge_insert: INSERT OVERRIDING override_kind VALUE_P merge_values_clause  */
#line 12646 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_INSERT;
                    n->override = (yyvsp[-2].ival);
                    n->targetList = NIL;
                    n->values = (yyvsp[0].list);
                    (yyval.mergewhen) = n;
                }
#line 46444 "gram.c"
    break;

  case 1679: /* merge_insert: INSERT '(' insert_column_list ')' merge_values_clause  */
#line 12655 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_INSERT;
                    n->override = OVERRIDING_NOT_SET;
                    n->targetList = (yyvsp[-2].list);
                    n->values = (yyvsp[0].list);
                    (yyval.mergewhen) = n;
                }
#line 46457 "gram.c"
    break;

  case 1680: /* merge_insert: INSERT '(' insert_column_list ')' OVERRIDING override_kind VALUE_P merge_values_clause  */
#line 12664 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_INSERT;
                    n->override = (yyvsp[-2].ival);
                    n->targetList = (yyvsp[-5].list);
                    n->values = (yyvsp[0].list);
                    (yyval.mergewhen) = n;
                }
#line 46470 "gram.c"
    break;

  case 1681: /* merge_insert: INSERT DEFAULT VALUES  */
#line 12673 "gram.y"
                                {
                    MergeWhenClause *n = makeNode(MergeWhenClause);
                    n->commandType = CMD_INSERT;
                    n->override = OVERRIDING_NOT_SET;
                    n->targetList = NIL;
                    n->values = NIL;
                    (yyval.mergewhen) = n;
                }
#line 46483 "gram.c"
    break;

  case 1682: /* merge_values_clause: VALUES '(' expr_list ')'  */
#line 12685 "gram.y"
                                {
                    (yyval.list) = (yyvsp[-1].list);
                }
#line 46491 "gram.c"
    break;

  case 1683: /* DeclareCursorStmt: DECLARE cursor_name cursor_options CURSOR opt_hold FOR SelectStmt  */
#line 12697 "gram.y"
                                {
                    DeclareCursorStmt *n = makeNode(DeclareCursorStmt);

                    n->portalname = (yyvsp[-5].str);
                    /* currently we always set FAST_PLAN option */
                    n->options = (yyvsp[-4].ival) | (yyvsp[-2].ival) | CURSOR_OPT_FAST_PLAN;
                    n->query = (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 46505 "gram.c"
    break;

  case 1684: /* cursor_name: name  */
#line 12708 "gram.y"
                                                                { (yyval.str) = (yyvsp[0].str); }
#line 46511 "gram.c"
    break;

  case 1685: /* cursor_options: %empty  */
#line 12711 "gram.y"
                                                                { (yyval.ival) = 0; }
#line 46517 "gram.c"
    break;

  case 1686: /* cursor_options: cursor_options NO SCROLL  */
#line 12712 "gram.y"
                                                                { (yyval.ival) = (yyvsp[-2].ival) | CURSOR_OPT_NO_SCROLL; }
#line 46523 "gram.c"
    break;

  case 1687: /* cursor_options: cursor_options SCROLL  */
#line 12713 "gram.y"
                                                                { (yyval.ival) = (yyvsp[-1].ival) | CURSOR_OPT_SCROLL; }
#line 46529 "gram.c"
    break;

  case 1688: /* cursor_options: cursor_options BINARY  */
#line 12714 "gram.y"
                                                                { (yyval.ival) = (yyvsp[-1].ival) | CURSOR_OPT_BINARY; }
#line 46535 "gram.c"
    break;

  case 1689: /* cursor_options: cursor_options ASENSITIVE  */
#line 12715 "gram.y"
                                                                { (yyval.ival) = (yyvsp[-1].ival) | CURSOR_OPT_ASENSITIVE; }
#line 46541 "gram.c"
    break;

  case 1690: /* cursor_options: cursor_options INSENSITIVE  */
#line 12716 "gram.y"
                                                        { (yyval.ival) = (yyvsp[-1].ival) | CURSOR_OPT_INSENSITIVE; }
#line 46547 "gram.c"
    break;

  case 1691: /* opt_hold: %empty  */
#line 12719 "gram.y"
                                                                { (yyval.ival) = 0; }
#line 46553 "gram.c"
    break;

  case 1692: /* opt_hold: WITH HOLD  */
#line 12720 "gram.y"
                                                                                { (yyval.ival) = CURSOR_OPT_HOLD; }
#line 46559 "gram.c"
    break;

  case 1693: /* opt_hold: WITHOUT HOLD  */
#line 12721 "gram.y"
                                                                        { (yyval.ival) = 0; }
#line 46565 "gram.c"
    break;

  case 1696: /* select_with_parens: '(' select_no_parens ')'  */
#line 12774 "gram.y"
                                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 46571 "gram.c"
    break;

  case 1697: /* select_with_parens: '(' select_with_parens ')'  */
#line 12775 "gram.y"
                                                                        { (yyval.node) = (yyvsp[-1].node); }
#line 46577 "gram.c"
    break;

  case 1698: /* select_no_parens: simple_select  */
#line 12790 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 46583 "gram.c"
    break;

  case 1699: /* select_no_parens: select_clause sort_clause  */
#line 12792 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-1].node), (yyvsp[0].list), NIL,
                                        NULL, NULL,
                                        yyscanner);
                    (yyval.node) = (yyvsp[-1].node);
                }
#line 46594 "gram.c"
    break;

  case 1700: /* select_no_parens: select_clause opt_sort_clause for_locking_clause opt_select_limit  */
#line 12799 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-3].node), (yyvsp[-2].list), (yyvsp[-1].list),
                                        (yyvsp[0].selectlimit),
                                        NULL,
                                        yyscanner);
                    (yyval.node) = (yyvsp[-3].node);
                }
#line 46606 "gram.c"
    break;

  case 1701: /* select_no_parens: select_clause opt_sort_clause select_limit opt_for_locking_clause  */
#line 12807 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-3].node), (yyvsp[-2].list), (yyvsp[0].list),
                                        (yyvsp[-1].selectlimit),
                                        NULL,
                                        yyscanner);
                    (yyval.node) = (yyvsp[-3].node);
                }
#line 46618 "gram.c"
    break;

  case 1702: /* select_no_parens: with_clause select_clause  */
#line 12815 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[0].node), NULL, NIL,
                                        NULL,
                                        (yyvsp[-1].with),
                                        yyscanner);
                    (yyval.node) = (yyvsp[0].node);
                }
#line 46630 "gram.c"
    break;

  case 1703: /* select_no_parens: with_clause select_clause sort_clause  */
#line 12823 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-1].node), (yyvsp[0].list), NIL,
                                        NULL,
                                        (yyvsp[-2].with),
                                        yyscanner);
                    (yyval.node) = (yyvsp[-1].node);
                }
#line 46642 "gram.c"
    break;

  case 1704: /* select_no_parens: with_clause select_clause opt_sort_clause for_locking_clause opt_select_limit  */
#line 12831 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-3].node), (yyvsp[-2].list), (yyvsp[-1].list),
                                        (yyvsp[0].selectlimit),
                                        (yyvsp[-4].with),
                                        yyscanner);
                    (yyval.node) = (yyvsp[-3].node);
                }
#line 46654 "gram.c"
    break;

  case 1705: /* select_no_parens: with_clause select_clause opt_sort_clause select_limit opt_for_locking_clause  */
#line 12839 "gram.y"
                                {
                    insertSelectOptions((SelectStmt *) (yyvsp[-3].node), (yyvsp[-2].list), (yyvsp[0].list),
                                        (yyvsp[-1].selectlimit),
                                        (yyvsp[-4].with),
                                        yyscanner);
                    (yyval.node) = (yyvsp[-3].node);
                }
#line 46666 "gram.c"
    break;

  case 1706: /* select_clause: simple_select  */
#line 12849 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 46672 "gram.c"
    break;

  case 1707: /* select_clause: select_with_parens  */
#line 12850 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 46678 "gram.c"
    break;

  case 1708: /* simple_select: SELECT opt_all_clause opt_target_list into_clause from_clause where_clause group_clause having_clause window_clause  */
#line 12885 "gram.y"
                                {
                    SelectStmt *n = makeNode(SelectStmt);

                    n->targetList = (yyvsp[-6].list);
                    n->intoClause = (yyvsp[-5].into);
                    n->fromClause = (yyvsp[-4].list);
                    n->whereClause = (yyvsp[-3].node);
                    n->groupClause = ((yyvsp[-2].groupclause))->list;
                    n->groupDistinct = ((yyvsp[-2].groupclause))->distinct;
                    n->havingClause = (yyvsp[-1].node);
                    n->windowClause = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 46696 "gram.c"
    break;

  case 1709: /* simple_select: SELECT distinct_clause target_list into_clause from_clause where_clause group_clause having_clause window_clause  */
#line 12901 "gram.y"
                                {
                    SelectStmt *n = makeNode(SelectStmt);

                    n->distinctClause = (yyvsp[-7].list);
                    n->targetList = (yyvsp[-6].list);
                    n->intoClause = (yyvsp[-5].into);
                    n->fromClause = (yyvsp[-4].list);
                    n->whereClause = (yyvsp[-3].node);
                    n->groupClause = ((yyvsp[-2].groupclause))->list;
                    n->groupDistinct = ((yyvsp[-2].groupclause))->distinct;
                    n->havingClause = (yyvsp[-1].node);
                    n->windowClause = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 46715 "gram.c"
    break;

  case 1710: /* simple_select: values_clause  */
#line 12915 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 46721 "gram.c"
    break;

  case 1711: /* simple_select: TABLE relation_expr  */
#line 12917 "gram.y"
                                {
                    /* same as SELECT * FROM relation_expr */
                    ColumnRef  *cr = makeNode(ColumnRef);
                    ResTarget  *rt = makeNode(ResTarget);
                    SelectStmt *n = makeNode(SelectStmt);

                    cr->fields = list_make1(makeNode(A_Star));
                    cr->location = -1;

                    rt->name = NULL;
                    rt->indirection = NIL;
                    rt->val = (Node *) cr;
                    rt->location = -1;

                    n->targetList = list_make1(rt);
                    n->fromClause = list_make1((yyvsp[0].range));
                    (yyval.node) = (Node *) n;
                }
#line 46744 "gram.c"
    break;

  case 1712: /* simple_select: select_clause UNION set_quantifier select_clause  */
#line 12936 "gram.y"
                                {
                    (yyval.node) = makeSetOp(SETOP_UNION, (yyvsp[-1].setquantifier) == SET_QUANTIFIER_ALL, (yyvsp[-3].node), (yyvsp[0].node));
                }
#line 46752 "gram.c"
    break;

  case 1713: /* simple_select: select_clause INTERSECT set_quantifier select_clause  */
#line 12940 "gram.y"
                                {
                    (yyval.node) = makeSetOp(SETOP_INTERSECT, (yyvsp[-1].setquantifier) == SET_QUANTIFIER_ALL, (yyvsp[-3].node), (yyvsp[0].node));
                }
#line 46760 "gram.c"
    break;

  case 1714: /* simple_select: select_clause EXCEPT set_quantifier select_clause  */
#line 12944 "gram.y"
                                {
                    (yyval.node) = makeSetOp(SETOP_EXCEPT, (yyvsp[-1].setquantifier) == SET_QUANTIFIER_ALL, (yyvsp[-3].node), (yyvsp[0].node));
                }
#line 46768 "gram.c"
    break;

  case 1715: /* with_clause: WITH cte_list  */
#line 12959 "gram.y"
                        {
                (yyval.with) = makeNode(WithClause);
                (yyval.with)->ctes = (yyvsp[0].list);
                (yyval.with)->recursive = false;
                (yyval.with)->location = (yylsp[-1]);
            }
#line 46779 "gram.c"
    break;

  case 1716: /* with_clause: WITH_LA cte_list  */
#line 12966 "gram.y"
                        {
                (yyval.with) = makeNode(WithClause);
                (yyval.with)->ctes = (yyvsp[0].list);
                (yyval.with)->recursive = false;
                (yyval.with)->location = (yylsp[-1]);
            }
#line 46790 "gram.c"
    break;

  case 1717: /* with_clause: WITH RECURSIVE cte_list  */
#line 12973 "gram.y"
                        {
                (yyval.with) = makeNode(WithClause);
                (yyval.with)->ctes = (yyvsp[0].list);
                (yyval.with)->recursive = true;
                (yyval.with)->location = (yylsp[-2]);
            }
#line 46801 "gram.c"
    break;

  case 1718: /* cte_list: common_table_expr  */
#line 12982 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 46807 "gram.c"
    break;

  case 1719: /* cte_list: cte_list ',' common_table_expr  */
#line 12983 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 46813 "gram.c"
    break;

  case 1720: /* common_table_expr: name opt_name_list AS opt_materialized '(' PreparableStmt ')' opt_search_clause opt_cycle_clause  */
#line 12987 "gram.y"
                        {
                CommonTableExpr *n = makeNode(CommonTableExpr);

                n->ctename = (yyvsp[-8].str);
                n->aliascolnames = (yyvsp[-7].list);
                n->ctematerialized = (yyvsp[-5].ival);
                n->ctequery = (yyvsp[-3].node);
                n->search_clause = castNode(CTESearchClause, (yyvsp[-1].node));
                n->cycle_clause = castNode(CTECycleClause, (yyvsp[0].node));
                n->location = (yylsp[-8]);
                (yyval.node) = (Node *) n;
            }
#line 46830 "gram.c"
    break;

  case 1721: /* opt_materialized: MATERIALIZED  */
#line 13002 "gram.y"
                                                                                { (yyval.ival) = CTEMaterializeAlways; }
#line 46836 "gram.c"
    break;

  case 1722: /* opt_materialized: NOT MATERIALIZED  */
#line 13003 "gram.y"
                                                                                { (yyval.ival) = CTEMaterializeNever; }
#line 46842 "gram.c"
    break;

  case 1723: /* opt_materialized: %empty  */
#line 13004 "gram.y"
                                                                                        { (yyval.ival) = CTEMaterializeDefault; }
#line 46848 "gram.c"
    break;

  case 1724: /* opt_search_clause: SEARCH DEPTH FIRST_P BY columnList SET ColId  */
#line 13009 "gram.y"
                        {
                CTESearchClause *n = makeNode(CTESearchClause);

                n->search_col_list = (yyvsp[-2].list);
                n->search_breadth_first = false;
                n->search_seq_column = (yyvsp[0].str);
                n->location = (yylsp[-6]);
                (yyval.node) = (Node *) n;
            }
#line 46862 "gram.c"
    break;

  case 1725: /* opt_search_clause: SEARCH BREADTH FIRST_P BY columnList SET ColId  */
#line 13019 "gram.y"
                        {
                CTESearchClause *n = makeNode(CTESearchClause);

                n->search_col_list = (yyvsp[-2].list);
                n->search_breadth_first = true;
                n->search_seq_column = (yyvsp[0].str);
                n->location = (yylsp[-6]);
                (yyval.node) = (Node *) n;
            }
#line 46876 "gram.c"
    break;

  case 1726: /* opt_search_clause: %empty  */
#line 13029 "gram.y"
                        {
                (yyval.node) = NULL;
            }
#line 46884 "gram.c"
    break;

  case 1727: /* opt_cycle_clause: CYCLE columnList SET ColId TO AexprConst DEFAULT AexprConst USING ColId  */
#line 13036 "gram.y"
                        {
                CTECycleClause *n = makeNode(CTECycleClause);

                n->cycle_col_list = (yyvsp[-8].list);
                n->cycle_mark_column = (yyvsp[-6].str);
                n->cycle_mark_value = (yyvsp[-4].node);
                n->cycle_mark_default = (yyvsp[-2].node);
                n->cycle_path_column = (yyvsp[0].str);
                n->location = (yylsp[-9]);
                (yyval.node) = (Node *) n;
            }
#line 46900 "gram.c"
    break;

  case 1728: /* opt_cycle_clause: CYCLE columnList SET ColId USING ColId  */
#line 13048 "gram.y"
                        {
                CTECycleClause *n = makeNode(CTECycleClause);

                n->cycle_col_list = (yyvsp[-4].list);
                n->cycle_mark_column = (yyvsp[-2].str);
                n->cycle_mark_value = makeBoolAConst(true, -1);
                n->cycle_mark_default = makeBoolAConst(false, -1);
                n->cycle_path_column = (yyvsp[0].str);
                n->location = (yylsp[-5]);
                (yyval.node) = (Node *) n;
            }
#line 46916 "gram.c"
    break;

  case 1729: /* opt_cycle_clause: %empty  */
#line 13060 "gram.y"
                        {
                (yyval.node) = NULL;
            }
#line 46924 "gram.c"
    break;

  case 1730: /* opt_with_clause: with_clause  */
#line 13066 "gram.y"
                                                                                        { (yyval.with) = (yyvsp[0].with); }
#line 46930 "gram.c"
    break;

  case 1731: /* opt_with_clause: %empty  */
#line 13067 "gram.y"
                                                                                        { (yyval.with) = NULL; }
#line 46936 "gram.c"
    break;

  case 1732: /* into_clause: INTO OptTempTableName  */
#line 13072 "gram.y"
                                {
                    (yyval.into) = makeNode(IntoClause);
                    (yyval.into)->rel = (yyvsp[0].range);
                    (yyval.into)->colNames = NIL;
                    (yyval.into)->options = NIL;
                    (yyval.into)->onCommit = ONCOMMIT_NOOP;
                    (yyval.into)->tableSpaceName = NULL;
                    (yyval.into)->viewQuery = NULL;
                    (yyval.into)->skipData = false;
                }
#line 46951 "gram.c"
    break;

  case 1733: /* into_clause: %empty  */
#line 13083 "gram.y"
                                { (yyval.into) = NULL; }
#line 46957 "gram.c"
    break;

  case 1734: /* OptTempTableName: TEMPORARY opt_table qualified_name  */
#line 13092 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 46966 "gram.c"
    break;

  case 1735: /* OptTempTableName: TEMP opt_table qualified_name  */
#line 13097 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 46975 "gram.c"
    break;

  case 1736: /* OptTempTableName: LOCAL TEMPORARY opt_table qualified_name  */
#line 13102 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 46984 "gram.c"
    break;

  case 1737: /* OptTempTableName: LOCAL TEMP opt_table qualified_name  */
#line 13107 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 46993 "gram.c"
    break;

  case 1738: /* OptTempTableName: GLOBAL TEMPORARY opt_table qualified_name  */
#line 13112 "gram.y"
                                {
                    ereport(WARNING,
                            (errmsg("GLOBAL is deprecated in temporary table creation"),
                             parser_errposition((yylsp[-3]))));
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 47005 "gram.c"
    break;

  case 1739: /* OptTempTableName: GLOBAL TEMP opt_table qualified_name  */
#line 13120 "gram.y"
                                {
                    ereport(WARNING,
                            (errmsg("GLOBAL is deprecated in temporary table creation"),
                             parser_errposition((yylsp[-3]))));
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_TEMP;
                }
#line 47017 "gram.c"
    break;

  case 1740: /* OptTempTableName: UNLOGGED opt_table qualified_name  */
#line 13128 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_UNLOGGED;
                }
#line 47026 "gram.c"
    break;

  case 1741: /* OptTempTableName: TABLE qualified_name  */
#line 13133 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_PERMANENT;
                }
#line 47035 "gram.c"
    break;

  case 1742: /* OptTempTableName: qualified_name  */
#line 13138 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->relpersistence = RELPERSISTENCE_PERMANENT;
                }
#line 47044 "gram.c"
    break;

  case 1745: /* set_quantifier: ALL  */
#line 13149 "gram.y"
                                                                                                        { (yyval.setquantifier) = SET_QUANTIFIER_ALL; }
#line 47050 "gram.c"
    break;

  case 1746: /* set_quantifier: DISTINCT  */
#line 13150 "gram.y"
                                                                                                { (yyval.setquantifier) = SET_QUANTIFIER_DISTINCT; }
#line 47056 "gram.c"
    break;

  case 1747: /* set_quantifier: %empty  */
#line 13151 "gram.y"
                                                                                                { (yyval.setquantifier) = SET_QUANTIFIER_DEFAULT; }
#line 47062 "gram.c"
    break;

  case 1748: /* distinct_clause: DISTINCT  */
#line 13158 "gram.y"
                                                                                                { (yyval.list) = list_make1(NIL); }
#line 47068 "gram.c"
    break;

  case 1749: /* distinct_clause: DISTINCT ON '(' expr_list ')'  */
#line 13159 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 47074 "gram.c"
    break;

  case 1752: /* opt_distinct_clause: distinct_clause  */
#line 13168 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 47080 "gram.c"
    break;

  case 1753: /* opt_distinct_clause: opt_all_clause  */
#line 13169 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 47086 "gram.c"
    break;

  case 1754: /* opt_sort_clause: sort_clause  */
#line 13173 "gram.y"
                                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 47092 "gram.c"
    break;

  case 1755: /* opt_sort_clause: %empty  */
#line 13174 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 47098 "gram.c"
    break;

  case 1756: /* sort_clause: ORDER BY sortby_list  */
#line 13178 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 47104 "gram.c"
    break;

  case 1757: /* sortby_list: sortby  */
#line 13182 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].sortby)); }
#line 47110 "gram.c"
    break;

  case 1758: /* sortby_list: sortby_list ',' sortby  */
#line 13183 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].sortby)); }
#line 47116 "gram.c"
    break;

  case 1759: /* sortby: a_expr USING qual_all_Op opt_nulls_order  */
#line 13187 "gram.y"
                                {
                    (yyval.sortby) = makeNode(SortBy);
                    (yyval.sortby)->node = (yyvsp[-3].node);
                    (yyval.sortby)->sortby_dir = SORTBY_USING;
                    (yyval.sortby)->sortby_nulls = (yyvsp[0].ival);
                    (yyval.sortby)->useOp = (yyvsp[-1].list);
                    (yyval.sortby)->location = (yylsp[-1]);
                }
#line 47129 "gram.c"
    break;

  case 1760: /* sortby: a_expr opt_asc_desc opt_nulls_order  */
#line 13196 "gram.y"
                                {
                    (yyval.sortby) = makeNode(SortBy);
                    (yyval.sortby)->node = (yyvsp[-2].node);
                    (yyval.sortby)->sortby_dir = (yyvsp[-1].ival);
                    (yyval.sortby)->sortby_nulls = (yyvsp[0].ival);
                    (yyval.sortby)->useOp = NIL;
                    (yyval.sortby)->location = -1;      /* no operator */
                }
#line 47142 "gram.c"
    break;

  case 1761: /* select_limit: limit_clause offset_clause  */
#line 13209 "gram.y"
                                {
                    (yyval.selectlimit) = (yyvsp[-1].selectlimit);
                    ((yyval.selectlimit))->limitOffset = (yyvsp[0].node);
                }
#line 47151 "gram.c"
    break;

  case 1762: /* select_limit: offset_clause limit_clause  */
#line 13214 "gram.y"
                                {
                    (yyval.selectlimit) = (yyvsp[0].selectlimit);
                    ((yyval.selectlimit))->limitOffset = (yyvsp[-1].node);
                }
#line 47160 "gram.c"
    break;

  case 1763: /* select_limit: limit_clause  */
#line 13219 "gram.y"
                                {
                    (yyval.selectlimit) = (yyvsp[0].selectlimit);
                }
#line 47168 "gram.c"
    break;

  case 1764: /* select_limit: offset_clause  */
#line 13223 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = (yyvsp[0].node);
                    n->limitCount = NULL;
                    n->limitOption = LIMIT_OPTION_COUNT;
                    (yyval.selectlimit) = n;
                }
#line 47181 "gram.c"
    break;

  case 1765: /* opt_select_limit: select_limit  */
#line 13234 "gram.y"
                                                                                { (yyval.selectlimit) = (yyvsp[0].selectlimit); }
#line 47187 "gram.c"
    break;

  case 1766: /* opt_select_limit: %empty  */
#line 13235 "gram.y"
                                                                                { (yyval.selectlimit) = NULL; }
#line 47193 "gram.c"
    break;

  case 1767: /* limit_clause: LIMIT select_limit_value  */
#line 13240 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = NULL;
                    n->limitCount = (yyvsp[0].node);
                    n->limitOption = LIMIT_OPTION_COUNT;
                    (yyval.selectlimit) = n;
                }
#line 47206 "gram.c"
    break;

  case 1768: /* limit_clause: LIMIT select_limit_value ',' select_offset_value  */
#line 13249 "gram.y"
                                {
                    /* Disabled because it was too confusing, bjm 2002-02-18 */
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("LIMIT #,# syntax is not supported"),
                             errhint("Use separate LIMIT and OFFSET clauses."),
                             parser_errposition((yylsp[-3]))));
                }
#line 47219 "gram.c"
    break;

  case 1769: /* limit_clause: FETCH first_or_next select_fetch_first_value row_or_rows ONLY  */
#line 13265 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = NULL;
                    n->limitCount = (yyvsp[-2].node);
                    n->limitOption = LIMIT_OPTION_COUNT;
                    (yyval.selectlimit) = n;
                }
#line 47232 "gram.c"
    break;

  case 1770: /* limit_clause: FETCH first_or_next select_fetch_first_value row_or_rows WITH TIES  */
#line 13274 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = NULL;
                    n->limitCount = (yyvsp[-3].node);
                    n->limitOption = LIMIT_OPTION_WITH_TIES;
                    (yyval.selectlimit) = n;
                }
#line 47245 "gram.c"
    break;

  case 1771: /* limit_clause: FETCH first_or_next row_or_rows ONLY  */
#line 13283 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = NULL;
                    n->limitCount = makeIntConst(1, -1);
                    n->limitOption = LIMIT_OPTION_COUNT;
                    (yyval.selectlimit) = n;
                }
#line 47258 "gram.c"
    break;

  case 1772: /* limit_clause: FETCH first_or_next row_or_rows WITH TIES  */
#line 13292 "gram.y"
                                {
                    SelectLimit *n = (SelectLimit *) palloc(sizeof(SelectLimit));

                    n->limitOffset = NULL;
                    n->limitCount = makeIntConst(1, -1);
                    n->limitOption = LIMIT_OPTION_WITH_TIES;
                    (yyval.selectlimit) = n;
                }
#line 47271 "gram.c"
    break;

  case 1773: /* offset_clause: OFFSET select_offset_value  */
#line 13304 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 47277 "gram.c"
    break;

  case 1774: /* offset_clause: OFFSET select_fetch_first_value row_or_rows  */
#line 13307 "gram.y"
                                { (yyval.node) = (yyvsp[-1].node); }
#line 47283 "gram.c"
    break;

  case 1775: /* select_limit_value: a_expr  */
#line 13311 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47289 "gram.c"
    break;

  case 1776: /* select_limit_value: ALL  */
#line 13313 "gram.y"
                                {
                    /* LIMIT ALL is represented as a NULL constant */
                    (yyval.node) = makeNullAConst((yylsp[0]));
                }
#line 47298 "gram.c"
    break;

  case 1777: /* select_offset_value: a_expr  */
#line 13320 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47304 "gram.c"
    break;

  case 1778: /* select_fetch_first_value: c_expr  */
#line 13340 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47310 "gram.c"
    break;

  case 1779: /* select_fetch_first_value: '+' I_or_F_const  */
#line 13342 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "+", NULL, (yyvsp[0].node), (yylsp[-1])); }
#line 47316 "gram.c"
    break;

  case 1780: /* select_fetch_first_value: '-' I_or_F_const  */
#line 13344 "gram.y"
                                { (yyval.node) = doNegate((yyvsp[0].node), (yylsp[-1])); }
#line 47322 "gram.c"
    break;

  case 1781: /* I_or_F_const: Iconst  */
#line 13348 "gram.y"
                                                                                                { (yyval.node) = makeIntConst((yyvsp[0].ival),(yylsp[0])); }
#line 47328 "gram.c"
    break;

  case 1782: /* I_or_F_const: FCONST  */
#line 13349 "gram.y"
                                                                                                { (yyval.node) = makeFloatConst((yyvsp[0].str),(yylsp[0])); }
#line 47334 "gram.c"
    break;

  case 1783: /* row_or_rows: ROW  */
#line 13353 "gram.y"
                                                                                        { (yyval.ival) = 0; }
#line 47340 "gram.c"
    break;

  case 1784: /* row_or_rows: ROWS  */
#line 13354 "gram.y"
                                                                                                { (yyval.ival) = 0; }
#line 47346 "gram.c"
    break;

  case 1785: /* first_or_next: FIRST_P  */
#line 13357 "gram.y"
                                                                                { (yyval.ival) = 0; }
#line 47352 "gram.c"
    break;

  case 1786: /* first_or_next: NEXT  */
#line 13358 "gram.y"
                                                                                                { (yyval.ival) = 0; }
#line 47358 "gram.c"
    break;

  case 1787: /* group_clause: GROUP_P BY set_quantifier group_by_list  */
#line 13384 "gram.y"
                                {
                    GroupClause *n = (GroupClause *) palloc(sizeof(GroupClause));

                    n->distinct = (yyvsp[-1].setquantifier) == SET_QUANTIFIER_DISTINCT;
                    n->list = (yyvsp[0].list);
                    (yyval.groupclause) = n;
                }
#line 47370 "gram.c"
    break;

  case 1788: /* group_clause: %empty  */
#line 13392 "gram.y"
                                {
                    GroupClause *n = (GroupClause *) palloc(sizeof(GroupClause));

                    n->distinct = false;
                    n->list = NIL;
                    (yyval.groupclause) = n;
                }
#line 47382 "gram.c"
    break;

  case 1789: /* group_by_list: group_by_item  */
#line 13402 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 47388 "gram.c"
    break;

  case 1790: /* group_by_list: group_by_list ',' group_by_item  */
#line 13403 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list),(yyvsp[0].node)); }
#line 47394 "gram.c"
    break;

  case 1791: /* group_by_item: a_expr  */
#line 13407 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47400 "gram.c"
    break;

  case 1792: /* group_by_item: empty_grouping_set  */
#line 13408 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47406 "gram.c"
    break;

  case 1793: /* group_by_item: cube_clause  */
#line 13409 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 47412 "gram.c"
    break;

  case 1794: /* group_by_item: rollup_clause  */
#line 13410 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 47418 "gram.c"
    break;

  case 1795: /* group_by_item: grouping_sets_clause  */
#line 13411 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 47424 "gram.c"
    break;

  case 1796: /* empty_grouping_set: '(' ')'  */
#line 13416 "gram.y"
                                {
                    (yyval.node) = (Node *) makeGroupingSet(GROUPING_SET_EMPTY, NIL, (yylsp[-1]));
                }
#line 47432 "gram.c"
    break;

  case 1797: /* rollup_clause: ROLLUP '(' expr_list ')'  */
#line 13429 "gram.y"
                                {
                    (yyval.node) = (Node *) makeGroupingSet(GROUPING_SET_ROLLUP, (yyvsp[-1].list), (yylsp[-3]));
                }
#line 47440 "gram.c"
    break;

  case 1798: /* cube_clause: CUBE '(' expr_list ')'  */
#line 13436 "gram.y"
                                {
                    (yyval.node) = (Node *) makeGroupingSet(GROUPING_SET_CUBE, (yyvsp[-1].list), (yylsp[-3]));
                }
#line 47448 "gram.c"
    break;

  case 1799: /* grouping_sets_clause: GROUPING SETS '(' group_by_list ')'  */
#line 13443 "gram.y"
                                {
                    (yyval.node) = (Node *) makeGroupingSet(GROUPING_SET_SETS, (yyvsp[-1].list), (yylsp[-4]));
                }
#line 47456 "gram.c"
    break;

  case 1800: /* having_clause: HAVING a_expr  */
#line 13449 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 47462 "gram.c"
    break;

  case 1801: /* having_clause: %empty  */
#line 13450 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 47468 "gram.c"
    break;

  case 1802: /* for_locking_clause: for_locking_items  */
#line 13454 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 47474 "gram.c"
    break;

  case 1803: /* for_locking_clause: FOR READ ONLY  */
#line 13455 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 47480 "gram.c"
    break;

  case 1804: /* opt_for_locking_clause: for_locking_clause  */
#line 13459 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 47486 "gram.c"
    break;

  case 1805: /* opt_for_locking_clause: %empty  */
#line 13460 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 47492 "gram.c"
    break;

  case 1806: /* for_locking_items: for_locking_item  */
#line 13464 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 47498 "gram.c"
    break;

  case 1807: /* for_locking_items: for_locking_items for_locking_item  */
#line 13465 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 47504 "gram.c"
    break;

  case 1808: /* for_locking_item: for_locking_strength locked_rels_list opt_nowait_or_skip  */
#line 13470 "gram.y"
                                {
                    LockingClause *n = makeNode(LockingClause);

                    n->lockedRels = (yyvsp[-1].list);
                    n->strength = (yyvsp[-2].ival);
                    n->waitPolicy = (yyvsp[0].ival);
                    (yyval.node) = (Node *) n;
                }
#line 47517 "gram.c"
    break;

  case 1809: /* for_locking_strength: FOR UPDATE  */
#line 13481 "gram.y"
                                                                                        { (yyval.ival) = LCS_FORUPDATE; }
#line 47523 "gram.c"
    break;

  case 1810: /* for_locking_strength: FOR NO KEY UPDATE  */
#line 13482 "gram.y"
                                                                                { (yyval.ival) = LCS_FORNOKEYUPDATE; }
#line 47529 "gram.c"
    break;

  case 1811: /* for_locking_strength: FOR SHARE  */
#line 13483 "gram.y"
                                                                                        { (yyval.ival) = LCS_FORSHARE; }
#line 47535 "gram.c"
    break;

  case 1812: /* for_locking_strength: FOR KEY SHARE  */
#line 13484 "gram.y"
                                                                                { (yyval.ival) = LCS_FORKEYSHARE; }
#line 47541 "gram.c"
    break;

  case 1813: /* locked_rels_list: OF qualified_name_list  */
#line 13488 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 47547 "gram.c"
    break;

  case 1814: /* locked_rels_list: %empty  */
#line 13489 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 47553 "gram.c"
    break;

  case 1815: /* values_clause: VALUES '(' expr_list ')'  */
#line 13500 "gram.y"
                                {
                    SelectStmt *n = makeNode(SelectStmt);

                    n->valuesLists = list_make1((yyvsp[-1].list));
                    (yyval.node) = (Node *) n;
                }
#line 47564 "gram.c"
    break;

  case 1816: /* values_clause: values_clause ',' '(' expr_list ')'  */
#line 13507 "gram.y"
                                {
                    SelectStmt *n = (SelectStmt *) (yyvsp[-4].node);

                    n->valuesLists = lappend(n->valuesLists, (yyvsp[-1].list));
                    (yyval.node) = (Node *) n;
                }
#line 47575 "gram.c"
    break;

  case 1817: /* from_clause: FROM from_list  */
#line 13525 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 47581 "gram.c"
    break;

  case 1818: /* from_clause: %empty  */
#line 13526 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 47587 "gram.c"
    break;

  case 1819: /* from_list: table_ref  */
#line 13530 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 47593 "gram.c"
    break;

  case 1820: /* from_list: from_list ',' table_ref  */
#line 13531 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 47599 "gram.c"
    break;

  case 1821: /* table_ref: relation_expr opt_alias_clause  */
#line 13538 "gram.y"
                                {
                    (yyvsp[-1].range)->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) (yyvsp[-1].range);
                }
#line 47608 "gram.c"
    break;

  case 1822: /* table_ref: relation_expr opt_alias_clause tablesample_clause  */
#line 13543 "gram.y"
                                {
                    RangeTableSample *n = (RangeTableSample *) (yyvsp[0].node);

                    (yyvsp[-2].range)->alias = (yyvsp[-1].alias);
                    /* relation_expr goes inside the RangeTableSample node */
                    n->relation = (Node *) (yyvsp[-2].range);
                    (yyval.node) = (Node *) n;
                }
#line 47621 "gram.c"
    break;

  case 1823: /* table_ref: func_table func_alias_clause  */
#line 13552 "gram.y"
                                {
                    RangeFunction *n = (RangeFunction *) (yyvsp[-1].node);

                    n->alias = linitial((yyvsp[0].list));
                    n->coldeflist = lsecond((yyvsp[0].list));
                    (yyval.node) = (Node *) n;
                }
#line 47633 "gram.c"
    break;

  case 1824: /* table_ref: LATERAL_P func_table func_alias_clause  */
#line 13560 "gram.y"
                                {
                    RangeFunction *n = (RangeFunction *) (yyvsp[-1].node);

                    n->lateral = true;
                    n->alias = linitial((yyvsp[0].list));
                    n->coldeflist = lsecond((yyvsp[0].list));
                    (yyval.node) = (Node *) n;
                }
#line 47646 "gram.c"
    break;

  case 1825: /* table_ref: xmltable opt_alias_clause  */
#line 13569 "gram.y"
                                {
                    RangeTableFunc *n = (RangeTableFunc *) (yyvsp[-1].node);

                    n->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) n;
                }
#line 47657 "gram.c"
    break;

  case 1826: /* table_ref: LATERAL_P xmltable opt_alias_clause  */
#line 13576 "gram.y"
                                {
                    RangeTableFunc *n = (RangeTableFunc *) (yyvsp[-1].node);

                    n->lateral = true;
                    n->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) n;
                }
#line 47669 "gram.c"
    break;

  case 1827: /* table_ref: select_with_parens opt_alias_clause  */
#line 13584 "gram.y"
                                {
                    RangeSubselect *n = makeNode(RangeSubselect);

                    n->lateral = false;
                    n->subquery = (yyvsp[-1].node);
                    n->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) n;
                }
#line 47682 "gram.c"
    break;

  case 1828: /* table_ref: LATERAL_P select_with_parens opt_alias_clause  */
#line 13593 "gram.y"
                                {
                    RangeSubselect *n = makeNode(RangeSubselect);

                    n->lateral = true;
                    n->subquery = (yyvsp[-1].node);
                    n->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) n;
                }
#line 47695 "gram.c"
    break;

  case 1829: /* table_ref: joined_table  */
#line 13602 "gram.y"
                                {
                    (yyval.node) = (Node *) (yyvsp[0].jexpr);
                }
#line 47703 "gram.c"
    break;

  case 1830: /* table_ref: '(' joined_table ')' alias_clause  */
#line 13606 "gram.y"
                                {
                    (yyvsp[-2].jexpr)->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) (yyvsp[-2].jexpr);
                }
#line 47712 "gram.c"
    break;

  case 1831: /* table_ref: json_table opt_alias_clause  */
#line 13611 "gram.y"
                                {
                    JsonTable  *jt = castNode(JsonTable, (yyvsp[-1].node));

                    jt->alias = (yyvsp[0].alias);
                    (yyval.node) = (Node *) jt;
                }
#line 47723 "gram.c"
    break;

  case 1832: /* table_ref: LATERAL_P json_table opt_alias_clause  */
#line 13618 "gram.y"
                                {
                    JsonTable  *jt = castNode(JsonTable, (yyvsp[-1].node));

                    jt->alias = (yyvsp[0].alias);
                    jt->lateral = true;
                    (yyval.node) = (Node *) jt;
                }
#line 47735 "gram.c"
    break;

  case 1833: /* joined_table: '(' joined_table ')'  */
#line 13647 "gram.y"
                                {
                    (yyval.jexpr) = (yyvsp[-1].jexpr);
                }
#line 47743 "gram.c"
    break;

  case 1834: /* joined_table: table_ref CROSS JOIN table_ref  */
#line 13651 "gram.y"
                                {
                    /* CROSS JOIN is same as unqualified inner join */
                    JoinExpr   *n = makeNode(JoinExpr);

                    n->jointype = JOIN_INNER;
                    n->isNatural = false;
                    n->larg = (yyvsp[-3].node);
                    n->rarg = (yyvsp[0].node);
                    n->usingClause = NIL;
                    n->join_using_alias = NULL;
                    n->quals = NULL;
                    (yyval.jexpr) = n;
                }
#line 47761 "gram.c"
    break;

  case 1835: /* joined_table: table_ref join_type JOIN table_ref join_qual  */
#line 13665 "gram.y"
                                {
                    JoinExpr   *n = makeNode(JoinExpr);

                    n->jointype = (yyvsp[-3].jtype);
                    n->isNatural = false;
                    n->larg = (yyvsp[-4].node);
                    n->rarg = (yyvsp[-1].node);
                    if ((yyvsp[0].node) != NULL && IsA((yyvsp[0].node), List))
                    {
                         /* USING clause */
                        n->usingClause = linitial_node(List, castNode(List, (yyvsp[0].node)));
                        n->join_using_alias = lsecond_node(Alias, castNode(List, (yyvsp[0].node)));
                    }
                    else
                    {
                        /* ON clause */
                        n->quals = (yyvsp[0].node);
                    }
                    (yyval.jexpr) = n;
                }
#line 47786 "gram.c"
    break;

  case 1836: /* joined_table: table_ref JOIN table_ref join_qual  */
#line 13686 "gram.y"
                                {
                    /* letting join_type reduce to empty doesn't work */
                    JoinExpr   *n = makeNode(JoinExpr);

                    n->jointype = JOIN_INNER;
                    n->isNatural = false;
                    n->larg = (yyvsp[-3].node);
                    n->rarg = (yyvsp[-1].node);
                    if ((yyvsp[0].node) != NULL && IsA((yyvsp[0].node), List))
                    {
                        /* USING clause */
                        n->usingClause = linitial_node(List, castNode(List, (yyvsp[0].node)));
                        n->join_using_alias = lsecond_node(Alias, castNode(List, (yyvsp[0].node)));
                    }
                    else
                    {
                        /* ON clause */
                        n->quals = (yyvsp[0].node);
                    }
                    (yyval.jexpr) = n;
                }
#line 47812 "gram.c"
    break;

  case 1837: /* joined_table: table_ref NATURAL join_type JOIN table_ref  */
#line 13708 "gram.y"
                                {
                    JoinExpr   *n = makeNode(JoinExpr);

                    n->jointype = (yyvsp[-2].jtype);
                    n->isNatural = true;
                    n->larg = (yyvsp[-4].node);
                    n->rarg = (yyvsp[0].node);
                    n->usingClause = NIL; /* figure out which columns later... */
                    n->join_using_alias = NULL;
                    n->quals = NULL; /* fill later */
                    (yyval.jexpr) = n;
                }
#line 47829 "gram.c"
    break;

  case 1838: /* joined_table: table_ref NATURAL JOIN table_ref  */
#line 13721 "gram.y"
                                {
                    /* letting join_type reduce to empty doesn't work */
                    JoinExpr   *n = makeNode(JoinExpr);

                    n->jointype = JOIN_INNER;
                    n->isNatural = true;
                    n->larg = (yyvsp[-3].node);
                    n->rarg = (yyvsp[0].node);
                    n->usingClause = NIL; /* figure out which columns later... */
                    n->join_using_alias = NULL;
                    n->quals = NULL; /* fill later */
                    (yyval.jexpr) = n;
                }
#line 47847 "gram.c"
    break;

  case 1839: /* alias_clause: AS ColId '(' name_list ')'  */
#line 13738 "gram.y"
                                {
                    (yyval.alias) = makeNode(Alias);
                    (yyval.alias)->aliasname = (yyvsp[-3].str);
                    (yyval.alias)->colnames = (yyvsp[-1].list);
                }
#line 47857 "gram.c"
    break;

  case 1840: /* alias_clause: AS ColId  */
#line 13744 "gram.y"
                                {
                    (yyval.alias) = makeNode(Alias);
                    (yyval.alias)->aliasname = (yyvsp[0].str);
                }
#line 47866 "gram.c"
    break;

  case 1841: /* alias_clause: ColId '(' name_list ')'  */
#line 13749 "gram.y"
                                {
                    (yyval.alias) = makeNode(Alias);
                    (yyval.alias)->aliasname = (yyvsp[-3].str);
                    (yyval.alias)->colnames = (yyvsp[-1].list);
                }
#line 47876 "gram.c"
    break;

  case 1842: /* alias_clause: ColId  */
#line 13755 "gram.y"
                                {
                    (yyval.alias) = makeNode(Alias);
                    (yyval.alias)->aliasname = (yyvsp[0].str);
                }
#line 47885 "gram.c"
    break;

  case 1843: /* opt_alias_clause: alias_clause  */
#line 13761 "gram.y"
                                                                        { (yyval.alias) = (yyvsp[0].alias); }
#line 47891 "gram.c"
    break;

  case 1844: /* opt_alias_clause: %empty  */
#line 13762 "gram.y"
                                                                                                { (yyval.alias) = NULL; }
#line 47897 "gram.c"
    break;

  case 1845: /* opt_alias_clause_for_join_using: AS ColId  */
#line 13773 "gram.y"
                                {
                    (yyval.alias) = makeNode(Alias);
                    (yyval.alias)->aliasname = (yyvsp[0].str);
                    /* the column name list will be inserted later */
                }
#line 47907 "gram.c"
    break;

  case 1846: /* opt_alias_clause_for_join_using: %empty  */
#line 13778 "gram.y"
                                                                                                { (yyval.alias) = NULL; }
#line 47913 "gram.c"
    break;

  case 1847: /* func_alias_clause: alias_clause  */
#line 13787 "gram.y"
                                {
                    (yyval.list) = list_make2((yyvsp[0].alias), NIL);
                }
#line 47921 "gram.c"
    break;

  case 1848: /* func_alias_clause: AS '(' TableFuncElementList ')'  */
#line 13791 "gram.y"
                                {
                    (yyval.list) = list_make2(NULL, (yyvsp[-1].list));
                }
#line 47929 "gram.c"
    break;

  case 1849: /* func_alias_clause: AS ColId '(' TableFuncElementList ')'  */
#line 13795 "gram.y"
                                {
                    Alias      *a = makeNode(Alias);

                    a->aliasname = (yyvsp[-3].str);
                    (yyval.list) = list_make2(a, (yyvsp[-1].list));
                }
#line 47940 "gram.c"
    break;

  case 1850: /* func_alias_clause: ColId '(' TableFuncElementList ')'  */
#line 13802 "gram.y"
                                {
                    Alias      *a = makeNode(Alias);

                    a->aliasname = (yyvsp[-3].str);
                    (yyval.list) = list_make2(a, (yyvsp[-1].list));
                }
#line 47951 "gram.c"
    break;

  case 1851: /* func_alias_clause: %empty  */
#line 13809 "gram.y"
                                {
                    (yyval.list) = list_make2(NULL, NIL);
                }
#line 47959 "gram.c"
    break;

  case 1852: /* join_type: FULL opt_outer  */
#line 13814 "gram.y"
                                                                                { (yyval.jtype) = JOIN_FULL; }
#line 47965 "gram.c"
    break;

  case 1853: /* join_type: LEFT opt_outer  */
#line 13815 "gram.y"
                                                                                        { (yyval.jtype) = JOIN_LEFT; }
#line 47971 "gram.c"
    break;

  case 1854: /* join_type: RIGHT opt_outer  */
#line 13816 "gram.y"
                                                                                        { (yyval.jtype) = JOIN_RIGHT; }
#line 47977 "gram.c"
    break;

  case 1855: /* join_type: INNER_P  */
#line 13817 "gram.y"
                                                                                                { (yyval.jtype) = JOIN_INNER; }
#line 47983 "gram.c"
    break;

  case 1858: /* join_qual: USING '(' name_list ')' opt_alias_clause_for_join_using  */
#line 13838 "gram.y"
                                {
                    (yyval.node) = (Node *) list_make2((yyvsp[-2].list), (yyvsp[0].alias));
                }
#line 47991 "gram.c"
    break;

  case 1859: /* join_qual: ON a_expr  */
#line 13842 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 47999 "gram.c"
    break;

  case 1860: /* relation_expr: qualified_name  */
#line 13850 "gram.y"
                                {
                    /* inheritance query, implicitly */
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->inh = true;
                    (yyval.range)->alias = NULL;
                }
#line 48010 "gram.c"
    break;

  case 1861: /* relation_expr: extended_relation_expr  */
#line 13857 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                }
#line 48018 "gram.c"
    break;

  case 1862: /* extended_relation_expr: qualified_name '*'  */
#line 13864 "gram.y"
                                {
                    /* inheritance query, explicitly */
                    (yyval.range) = (yyvsp[-1].range);
                    (yyval.range)->inh = true;
                    (yyval.range)->alias = NULL;
                }
#line 48029 "gram.c"
    break;

  case 1863: /* extended_relation_expr: ONLY qualified_name  */
#line 13871 "gram.y"
                                {
                    /* no inheritance */
                    (yyval.range) = (yyvsp[0].range);
                    (yyval.range)->inh = false;
                    (yyval.range)->alias = NULL;
                }
#line 48040 "gram.c"
    break;

  case 1864: /* extended_relation_expr: ONLY '(' qualified_name ')'  */
#line 13878 "gram.y"
                                {
                    /* no inheritance, SQL99-style syntax */
                    (yyval.range) = (yyvsp[-1].range);
                    (yyval.range)->inh = false;
                    (yyval.range)->alias = NULL;
                }
#line 48051 "gram.c"
    break;

  case 1865: /* relation_expr_list: relation_expr  */
#line 13888 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].range)); }
#line 48057 "gram.c"
    break;

  case 1866: /* relation_expr_list: relation_expr_list ',' relation_expr  */
#line 13889 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].range)); }
#line 48063 "gram.c"
    break;

  case 1867: /* relation_expr_opt_alias: relation_expr  */
#line 13903 "gram.y"
                                {
                    (yyval.range) = (yyvsp[0].range);
                }
#line 48071 "gram.c"
    break;

  case 1868: /* relation_expr_opt_alias: relation_expr ColId  */
#line 13907 "gram.y"
                                {
                    Alias      *alias = makeNode(Alias);

                    alias->aliasname = (yyvsp[0].str);
                    (yyvsp[-1].range)->alias = alias;
                    (yyval.range) = (yyvsp[-1].range);
                }
#line 48083 "gram.c"
    break;

  case 1869: /* relation_expr_opt_alias: relation_expr AS ColId  */
#line 13915 "gram.y"
                                {
                    Alias      *alias = makeNode(Alias);

                    alias->aliasname = (yyvsp[0].str);
                    (yyvsp[-2].range)->alias = alias;
                    (yyval.range) = (yyvsp[-2].range);
                }
#line 48095 "gram.c"
    break;

  case 1870: /* tablesample_clause: TABLESAMPLE func_name '(' expr_list ')' opt_repeatable_clause  */
#line 13929 "gram.y"
                                {
                    RangeTableSample *n = makeNode(RangeTableSample);

                    /* n->relation will be filled in later */
                    n->method = (yyvsp[-4].list);
                    n->args = (yyvsp[-2].list);
                    n->repeatable = (yyvsp[0].node);
                    n->location = (yylsp[-4]);
                    (yyval.node) = (Node *) n;
                }
#line 48110 "gram.c"
    break;

  case 1871: /* opt_repeatable_clause: REPEATABLE '(' a_expr ')'  */
#line 13942 "gram.y"
                                                        { (yyval.node) = (Node *) (yyvsp[-1].node); }
#line 48116 "gram.c"
    break;

  case 1872: /* opt_repeatable_clause: %empty  */
#line 13943 "gram.y"
                                                                        { (yyval.node) = NULL; }
#line 48122 "gram.c"
    break;

  case 1873: /* func_table: func_expr_windowless opt_ordinality  */
#line 13959 "gram.y"
                                {
                    RangeFunction *n = makeNode(RangeFunction);

                    n->lateral = false;
                    n->ordinality = (yyvsp[0].boolean);
                    n->is_rowsfrom = false;
                    n->functions = list_make1(list_make2((yyvsp[-1].node), NIL));
                    /* alias and coldeflist are set by table_ref production */
                    (yyval.node) = (Node *) n;
                }
#line 48137 "gram.c"
    break;

  case 1874: /* func_table: ROWS FROM '(' rowsfrom_list ')' opt_ordinality  */
#line 13970 "gram.y"
                                {
                    RangeFunction *n = makeNode(RangeFunction);

                    n->lateral = false;
                    n->ordinality = (yyvsp[0].boolean);
                    n->is_rowsfrom = true;
                    n->functions = (yyvsp[-2].list);
                    /* alias and coldeflist are set by table_ref production */
                    (yyval.node) = (Node *) n;
                }
#line 48152 "gram.c"
    break;

  case 1875: /* rowsfrom_item: func_expr_windowless opt_col_def_list  */
#line 13983 "gram.y"
                                { (yyval.list) = list_make2((yyvsp[-1].node), (yyvsp[0].list)); }
#line 48158 "gram.c"
    break;

  case 1876: /* rowsfrom_list: rowsfrom_item  */
#line 13987 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].list)); }
#line 48164 "gram.c"
    break;

  case 1877: /* rowsfrom_list: rowsfrom_list ',' rowsfrom_item  */
#line 13988 "gram.y"
                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].list)); }
#line 48170 "gram.c"
    break;

  case 1878: /* opt_col_def_list: AS '(' TableFuncElementList ')'  */
#line 13991 "gram.y"
                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 48176 "gram.c"
    break;

  case 1879: /* opt_col_def_list: %empty  */
#line 13992 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 48182 "gram.c"
    break;

  case 1880: /* opt_ordinality: WITH_LA ORDINALITY  */
#line 13995 "gram.y"
                                                                        { (yyval.boolean) = true; }
#line 48188 "gram.c"
    break;

  case 1881: /* opt_ordinality: %empty  */
#line 13996 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 48194 "gram.c"
    break;

  case 1882: /* where_clause: WHERE a_expr  */
#line 14001 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 48200 "gram.c"
    break;

  case 1883: /* where_clause: %empty  */
#line 14002 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 48206 "gram.c"
    break;

  case 1884: /* where_or_current_clause: WHERE a_expr  */
#line 14007 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 48212 "gram.c"
    break;

  case 1885: /* where_or_current_clause: WHERE CURRENT_P OF cursor_name  */
#line 14009 "gram.y"
                                {
                    CurrentOfExpr *n = makeNode(CurrentOfExpr);

                    /* cvarno is filled in by parse analysis */
                    n->cursor_name = (yyvsp[0].str);
                    n->cursor_param = 0;
                    (yyval.node) = (Node *) n;
                }
#line 48225 "gram.c"
    break;

  case 1886: /* where_or_current_clause: %empty  */
#line 14017 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 48231 "gram.c"
    break;

  case 1887: /* OptTableFuncElementList: TableFuncElementList  */
#line 14022 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 48237 "gram.c"
    break;

  case 1888: /* OptTableFuncElementList: %empty  */
#line 14023 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 48243 "gram.c"
    break;

  case 1889: /* TableFuncElementList: TableFuncElement  */
#line 14028 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 48251 "gram.c"
    break;

  case 1890: /* TableFuncElementList: TableFuncElementList ',' TableFuncElement  */
#line 14032 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 48259 "gram.c"
    break;

  case 1891: /* TableFuncElement: ColId Typename opt_collate_clause  */
#line 14038 "gram.y"
                                {
                    ColumnDef *n = makeNode(ColumnDef);

                    n->colname = (yyvsp[-2].str);
                    n->typeName = (yyvsp[-1].typnam);
                    n->inhcount = 0;
                    n->is_local = true;
                    n->is_not_null = false;
                    n->is_from_type = false;
                    n->storage = 0;
                    n->raw_default = NULL;
                    n->cooked_default = NULL;
                    n->collClause = (CollateClause *) (yyvsp[0].node);
                    n->collOid = InvalidOid;
                    n->constraints = NIL;
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 48282 "gram.c"
    break;

  case 1892: /* xmltable: XMLTABLE '(' c_expr xmlexists_argument COLUMNS xmltable_column_list ')'  */
#line 14063 "gram.y"
                                {
                    RangeTableFunc *n = makeNode(RangeTableFunc);

                    n->rowexpr = (yyvsp[-4].node);
                    n->docexpr = (yyvsp[-3].node);
                    n->columns = (yyvsp[-1].list);
                    n->namespaces = NIL;
                    n->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 48297 "gram.c"
    break;

  case 1893: /* xmltable: XMLTABLE '(' XMLNAMESPACES '(' xml_namespace_list ')' ',' c_expr xmlexists_argument COLUMNS xmltable_column_list ')'  */
#line 14075 "gram.y"
                                {
                    RangeTableFunc *n = makeNode(RangeTableFunc);

                    n->rowexpr = (yyvsp[-4].node);
                    n->docexpr = (yyvsp[-3].node);
                    n->columns = (yyvsp[-1].list);
                    n->namespaces = (yyvsp[-7].list);
                    n->location = (yylsp[-11]);
                    (yyval.node) = (Node *) n;
                }
#line 48312 "gram.c"
    break;

  case 1894: /* xmltable_column_list: xmltable_column_el  */
#line 14087 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 48318 "gram.c"
    break;

  case 1895: /* xmltable_column_list: xmltable_column_list ',' xmltable_column_el  */
#line 14088 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 48324 "gram.c"
    break;

  case 1896: /* xmltable_column_el: ColId Typename  */
#line 14093 "gram.y"
                                {
                    RangeTableFuncCol *fc = makeNode(RangeTableFuncCol);

                    fc->colname = (yyvsp[-1].str);
                    fc->for_ordinality = false;
                    fc->typeName = (yyvsp[0].typnam);
                    fc->is_not_null = false;
                    fc->colexpr = NULL;
                    fc->coldefexpr = NULL;
                    fc->location = (yylsp[-1]);

                    (yyval.node) = (Node *) fc;
                }
#line 48342 "gram.c"
    break;

  case 1897: /* xmltable_column_el: ColId Typename xmltable_column_option_list  */
#line 14107 "gram.y"
                                {
                    RangeTableFuncCol *fc = makeNode(RangeTableFuncCol);
                    ListCell   *option;
                    bool        nullability_seen = false;

                    fc->colname = (yyvsp[-2].str);
                    fc->typeName = (yyvsp[-1].typnam);
                    fc->for_ordinality = false;
                    fc->is_not_null = false;
                    fc->colexpr = NULL;
                    fc->coldefexpr = NULL;
                    fc->location = (yylsp[-2]);

                    foreach(option, (yyvsp[0].list))
                    {
                        DefElem   *defel = (DefElem *) lfirst(option);

                        if (strcmp(defel->defname, "default") == 0)
                        {
                            if (fc->coldefexpr != NULL)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("only one DEFAULT value is allowed"),
                                         parser_errposition(defel->location)));
                            fc->coldefexpr = defel->arg;
                        }
                        else if (strcmp(defel->defname, "path") == 0)
                        {
                            if (fc->colexpr != NULL)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("only one PATH value per column is allowed"),
                                         parser_errposition(defel->location)));
                            fc->colexpr = defel->arg;
                        }
                        else if (strcmp(defel->defname, "is_not_null") == 0)
                        {
                            if (nullability_seen)
                                ereport(ERROR,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("conflicting or redundant NULL / NOT NULL declarations for column \"%s\"", fc->colname),
                                         parser_errposition(defel->location)));
                            fc->is_not_null = boolVal(defel->arg);
                            nullability_seen = true;
                        }
                        else
                        {
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("unrecognized column option \"%s\"",
                                            defel->defname),
                                     parser_errposition(defel->location)));
                        }
                    }
                    (yyval.node) = (Node *) fc;
                }
#line 48403 "gram.c"
    break;

  case 1898: /* xmltable_column_el: ColId FOR ORDINALITY  */
#line 14164 "gram.y"
                                {
                    RangeTableFuncCol *fc = makeNode(RangeTableFuncCol);

                    fc->colname = (yyvsp[-2].str);
                    fc->for_ordinality = true;
                    /* other fields are ignored, initialized by makeNode */
                    fc->location = (yylsp[-2]);

                    (yyval.node) = (Node *) fc;
                }
#line 48418 "gram.c"
    break;

  case 1899: /* xmltable_column_option_list: xmltable_column_option_el  */
#line 14178 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].defelt)); }
#line 48424 "gram.c"
    break;

  case 1900: /* xmltable_column_option_list: xmltable_column_option_list xmltable_column_option_el  */
#line 14180 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].defelt)); }
#line 48430 "gram.c"
    break;

  case 1901: /* xmltable_column_option_el: IDENT b_expr  */
#line 14185 "gram.y"
                                { (yyval.defelt) = makeDefElem((yyvsp[-1].str), (yyvsp[0].node), (yylsp[-1])); }
#line 48436 "gram.c"
    break;

  case 1902: /* xmltable_column_option_el: DEFAULT b_expr  */
#line 14187 "gram.y"
                                { (yyval.defelt) = makeDefElem("default", (yyvsp[0].node), (yylsp[-1])); }
#line 48442 "gram.c"
    break;

  case 1903: /* xmltable_column_option_el: NOT NULL_P  */
#line 14189 "gram.y"
                                { (yyval.defelt) = makeDefElem("is_not_null", (Node *) makeBoolean(true), (yylsp[-1])); }
#line 48448 "gram.c"
    break;

  case 1904: /* xmltable_column_option_el: NULL_P  */
#line 14191 "gram.y"
                                { (yyval.defelt) = makeDefElem("is_not_null", (Node *) makeBoolean(false), (yylsp[0])); }
#line 48454 "gram.c"
    break;

  case 1905: /* xmltable_column_option_el: PATH b_expr  */
#line 14193 "gram.y"
                                { (yyval.defelt) = makeDefElem("path", (yyvsp[0].node), (yylsp[-1])); }
#line 48460 "gram.c"
    break;

  case 1906: /* xml_namespace_list: xml_namespace_el  */
#line 14198 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].target)); }
#line 48466 "gram.c"
    break;

  case 1907: /* xml_namespace_list: xml_namespace_list ',' xml_namespace_el  */
#line 14200 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].target)); }
#line 48472 "gram.c"
    break;

  case 1908: /* xml_namespace_el: b_expr AS ColLabel  */
#line 14205 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[0].str);
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (yyvsp[-2].node);
                    (yyval.target)->location = (yylsp[-2]);
                }
#line 48484 "gram.c"
    break;

  case 1909: /* xml_namespace_el: DEFAULT b_expr  */
#line 14213 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = NULL;
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (yyvsp[0].node);
                    (yyval.target)->location = (yylsp[-1]);
                }
#line 48496 "gram.c"
    break;

  case 1910: /* json_table: JSON_TABLE '(' json_value_expr ',' a_expr json_table_path_name_opt json_passing_clause_opt COLUMNS '(' json_table_column_definition_list ')' json_on_error_clause_opt ')'  */
#line 14229 "gram.y"
                                {
                    JsonTable *n = makeNode(JsonTable);
                    char      *pathstring;

                    n->context_item = (JsonValueExpr *) (yyvsp[-10].node);
                    if (!IsA((yyvsp[-8].node), A_Const) ||
                        castNode(A_Const, (yyvsp[-8].node))->val.node.type != T_String)
                        ereport(ERROR,
                                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("only string constants are supported in JSON_TABLE path specification"),
                                parser_errposition((yylsp[-8])));
                    pathstring = castNode(A_Const, (yyvsp[-8].node))->val.sval.sval;
                    n->pathspec = makeJsonTablePathSpec(pathstring, (yyvsp[-7].str), (yylsp[-8]), (yylsp[-7]));
                    n->passing = (yyvsp[-6].list);
                    n->columns = (yyvsp[-3].list);
                    n->on_error = (JsonBehavior *) (yyvsp[-1].node);
                    n->location = (yylsp[-12]);
                    (yyval.node) = (Node *) n;
                }
#line 48520 "gram.c"
    break;

  case 1911: /* json_table_path_name_opt: AS name  */
#line 14251 "gram.y"
                                                { (yyval.str) = (yyvsp[0].str); }
#line 48526 "gram.c"
    break;

  case 1912: /* json_table_path_name_opt: %empty  */
#line 14252 "gram.y"
                                        { (yyval.str) = NULL; }
#line 48532 "gram.c"
    break;

  case 1913: /* json_table_column_definition_list: json_table_column_definition  */
#line 14257 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 48538 "gram.c"
    break;

  case 1914: /* json_table_column_definition_list: json_table_column_definition_list ',' json_table_column_definition  */
#line 14259 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 48544 "gram.c"
    break;

  case 1915: /* json_table_column_definition: ColId FOR ORDINALITY  */
#line 14264 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_FOR_ORDINALITY;
                    n->name = (yyvsp[-2].str);
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 48557 "gram.c"
    break;

  case 1916: /* json_table_column_definition: ColId Typename json_table_column_path_clause_opt json_wrapper_behavior json_quotes_clause_opt json_behavior_clause_opt  */
#line 14277 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_REGULAR;
                    n->name = (yyvsp[-5].str);
                    n->typeName = (yyvsp[-4].typnam);
                    n->format = makeJsonFormat(JS_FORMAT_DEFAULT, JS_ENC_DEFAULT, -1);
                    n->pathspec = (JsonTablePathSpec *) (yyvsp[-3].node);
                    n->wrapper = (yyvsp[-2].ival);
                    n->quotes = (yyvsp[-1].ival);
                    n->on_empty = (JsonBehavior *) linitial((yyvsp[0].list));
                    n->on_error = (JsonBehavior *) lsecond((yyvsp[0].list));
                    n->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 48577 "gram.c"
    break;

  case 1917: /* json_table_column_definition: ColId Typename json_format_clause json_table_column_path_clause_opt json_wrapper_behavior json_quotes_clause_opt json_behavior_clause_opt  */
#line 14297 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_FORMATTED;
                    n->name = (yyvsp[-6].str);
                    n->typeName = (yyvsp[-5].typnam);
                    n->format = (JsonFormat *) (yyvsp[-4].node);
                    n->pathspec = (JsonTablePathSpec *) (yyvsp[-3].node);
                    n->wrapper = (yyvsp[-2].ival);
                    n->quotes = (yyvsp[-1].ival);
                    n->on_empty = (JsonBehavior *) linitial((yyvsp[0].list));
                    n->on_error = (JsonBehavior *) lsecond((yyvsp[0].list));
                    n->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 48597 "gram.c"
    break;

  case 1918: /* json_table_column_definition: ColId Typename EXISTS json_table_column_path_clause_opt json_behavior_clause_opt  */
#line 14315 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_EXISTS;
                    n->name = (yyvsp[-4].str);
                    n->typeName = (yyvsp[-3].typnam);
                    n->format = makeJsonFormat(JS_FORMAT_DEFAULT, JS_ENC_DEFAULT, -1);
                    n->wrapper = JSW_NONE;
                    n->quotes = JS_QUOTES_UNSPEC;
                    n->pathspec = (JsonTablePathSpec *) (yyvsp[-1].node);
                    n->on_empty = (JsonBehavior *) linitial((yyvsp[0].list));
                    n->on_error = (JsonBehavior *) lsecond((yyvsp[0].list));
                    n->location = (yylsp[-4]);
                    (yyval.node) = (Node *) n;
                }
#line 48617 "gram.c"
    break;

  case 1919: /* json_table_column_definition: NESTED path_opt Sconst COLUMNS '(' json_table_column_definition_list ')'  */
#line 14332 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_NESTED;
                    n->pathspec = (JsonTablePathSpec *)
                        makeJsonTablePathSpec((yyvsp[-4].str), NULL, (yylsp[-4]), -1);
                    n->columns = (yyvsp[-1].list);
                    n->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 48632 "gram.c"
    break;

  case 1920: /* json_table_column_definition: NESTED path_opt Sconst AS name COLUMNS '(' json_table_column_definition_list ')'  */
#line 14344 "gram.y"
                                {
                    JsonTableColumn *n = makeNode(JsonTableColumn);

                    n->coltype = JTC_NESTED;
                    n->pathspec = (JsonTablePathSpec *)
                        makeJsonTablePathSpec((yyvsp[-6].str), (yyvsp[-4].str), (yylsp[-6]), (yylsp[-4]));
                    n->columns = (yyvsp[-1].list);
                    n->location = (yylsp[-8]);
                    (yyval.node) = (Node *) n;
                }
#line 48647 "gram.c"
    break;

  case 1923: /* json_table_column_path_clause_opt: PATH Sconst  */
#line 14363 "gram.y"
                                { (yyval.node) = (Node *) makeJsonTablePathSpec((yyvsp[0].str), NULL, (yylsp[0]), -1); }
#line 48653 "gram.c"
    break;

  case 1924: /* json_table_column_path_clause_opt: %empty  */
#line 14365 "gram.y"
                                { (yyval.node) = NULL; }
#line 48659 "gram.c"
    break;

  case 1925: /* Typename: SimpleTypename opt_array_bounds  */
#line 14379 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-1].typnam);
                    (yyval.typnam)->arrayBounds = (yyvsp[0].list);
                }
#line 48668 "gram.c"
    break;

  case 1926: /* Typename: SETOF SimpleTypename opt_array_bounds  */
#line 14384 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-1].typnam);
                    (yyval.typnam)->arrayBounds = (yyvsp[0].list);
                    (yyval.typnam)->setof = true;
                }
#line 48678 "gram.c"
    break;

  case 1927: /* Typename: SimpleTypename ARRAY '[' Iconst ']'  */
#line 14391 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-4].typnam);
                    (yyval.typnam)->arrayBounds = list_make1(makeInteger((yyvsp[-1].ival)));
                }
#line 48687 "gram.c"
    break;

  case 1928: /* Typename: SETOF SimpleTypename ARRAY '[' Iconst ']'  */
#line 14396 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-4].typnam);
                    (yyval.typnam)->arrayBounds = list_make1(makeInteger((yyvsp[-1].ival)));
                    (yyval.typnam)->setof = true;
                }
#line 48697 "gram.c"
    break;

  case 1929: /* Typename: SimpleTypename ARRAY  */
#line 14402 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-1].typnam);
                    (yyval.typnam)->arrayBounds = list_make1(makeInteger(-1));
                }
#line 48706 "gram.c"
    break;

  case 1930: /* Typename: SETOF SimpleTypename ARRAY  */
#line 14407 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-1].typnam);
                    (yyval.typnam)->arrayBounds = list_make1(makeInteger(-1));
                    (yyval.typnam)->setof = true;
                }
#line 48716 "gram.c"
    break;

  case 1931: /* opt_array_bounds: opt_array_bounds '[' ']'  */
#line 14416 "gram.y"
                                        {  (yyval.list) = lappend((yyvsp[-2].list), makeInteger(-1)); }
#line 48722 "gram.c"
    break;

  case 1932: /* opt_array_bounds: opt_array_bounds '[' Iconst ']'  */
#line 14418 "gram.y"
                                        {  (yyval.list) = lappend((yyvsp[-3].list), makeInteger((yyvsp[-1].ival))); }
#line 48728 "gram.c"
    break;

  case 1933: /* opt_array_bounds: %empty  */
#line 14420 "gram.y"
                                        {  (yyval.list) = NIL; }
#line 48734 "gram.c"
    break;

  case 1934: /* SimpleTypename: GenericType  */
#line 14424 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48740 "gram.c"
    break;

  case 1935: /* SimpleTypename: Numeric  */
#line 14425 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48746 "gram.c"
    break;

  case 1936: /* SimpleTypename: Bit  */
#line 14426 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48752 "gram.c"
    break;

  case 1937: /* SimpleTypename: Character  */
#line 14427 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48758 "gram.c"
    break;

  case 1938: /* SimpleTypename: ConstDatetime  */
#line 14428 "gram.y"
                                                                                        { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48764 "gram.c"
    break;

  case 1939: /* SimpleTypename: ConstInterval opt_interval  */
#line 14430 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-1].typnam);
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                }
#line 48773 "gram.c"
    break;

  case 1940: /* SimpleTypename: ConstInterval '(' Iconst ')'  */
#line 14435 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[-3].typnam);
                    (yyval.typnam)->typmods = list_make2(makeIntConst(INTERVAL_FULL_RANGE, -1),
                                             makeIntConst((yyvsp[-1].ival), (yylsp[-1])));
                }
#line 48783 "gram.c"
    break;

  case 1941: /* SimpleTypename: JsonType  */
#line 14440 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48789 "gram.c"
    break;

  case 1942: /* ConstTypename: Numeric  */
#line 14455 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48795 "gram.c"
    break;

  case 1943: /* ConstTypename: ConstBit  */
#line 14456 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48801 "gram.c"
    break;

  case 1944: /* ConstTypename: ConstCharacter  */
#line 14457 "gram.y"
                                                                                        { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48807 "gram.c"
    break;

  case 1945: /* ConstTypename: ConstDatetime  */
#line 14458 "gram.y"
                                                                                        { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48813 "gram.c"
    break;

  case 1946: /* ConstTypename: JsonType  */
#line 14459 "gram.y"
                                                                                                { (yyval.typnam) = (yyvsp[0].typnam); }
#line 48819 "gram.c"
    break;

  case 1947: /* GenericType: type_function_name opt_type_modifiers  */
#line 14471 "gram.y"
                                {
                    (yyval.typnam) = makeTypeName((yyvsp[-1].str));
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48829 "gram.c"
    break;

  case 1948: /* GenericType: type_function_name attrs opt_type_modifiers  */
#line 14477 "gram.y"
                                {
                    (yyval.typnam) = makeTypeNameFromNameList(lcons(makeString((yyvsp[-2].str)), (yyvsp[-1].list)));
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                    (yyval.typnam)->location = (yylsp[-2]);
                }
#line 48839 "gram.c"
    break;

  case 1949: /* opt_type_modifiers: '(' expr_list ')'  */
#line 14484 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 48845 "gram.c"
    break;

  case 1950: /* opt_type_modifiers: %empty  */
#line 14485 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 48851 "gram.c"
    break;

  case 1951: /* Numeric: INT_P  */
#line 14492 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("int4");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48860 "gram.c"
    break;

  case 1952: /* Numeric: INTEGER  */
#line 14497 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("int4");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48869 "gram.c"
    break;

  case 1953: /* Numeric: SMALLINT  */
#line 14502 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("int2");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48878 "gram.c"
    break;

  case 1954: /* Numeric: BIGINT  */
#line 14507 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("int8");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48887 "gram.c"
    break;

  case 1955: /* Numeric: REAL  */
#line 14512 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("float4");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48896 "gram.c"
    break;

  case 1956: /* Numeric: FLOAT_P opt_float  */
#line 14517 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48905 "gram.c"
    break;

  case 1957: /* Numeric: DOUBLE_P PRECISION  */
#line 14522 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("float8");
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48914 "gram.c"
    break;

  case 1958: /* Numeric: DECIMAL_P opt_type_modifiers  */
#line 14527 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("numeric");
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48924 "gram.c"
    break;

  case 1959: /* Numeric: DEC opt_type_modifiers  */
#line 14533 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("numeric");
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48934 "gram.c"
    break;

  case 1960: /* Numeric: NUMERIC opt_type_modifiers  */
#line 14539 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("numeric");
                    (yyval.typnam)->typmods = (yyvsp[0].list);
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 48944 "gram.c"
    break;

  case 1961: /* Numeric: BOOLEAN_P  */
#line 14545 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("bool");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 48953 "gram.c"
    break;

  case 1962: /* opt_float: '(' Iconst ')'  */
#line 14552 "gram.y"
                                {
                    /*
                     * Check FLOAT() precision limits assuming IEEE floating
                     * types - thomas 1997-09-18
                     */
                    if ((yyvsp[-1].ival) < 1)
                        ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                 errmsg("precision for type float must be at least 1 bit"),
                                 parser_errposition((yylsp[-1]))));
                    else if ((yyvsp[-1].ival) <= 24)
                        (yyval.typnam) = SystemTypeName("float4");
                    else if ((yyvsp[-1].ival) <= 53)
                        (yyval.typnam) = SystemTypeName("float8");
                    else
                        ereport(ERROR,
                                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                 errmsg("precision for type float must be less than 54 bits"),
                                 parser_errposition((yylsp[-1]))));
                }
#line 48978 "gram.c"
    break;

  case 1963: /* opt_float: %empty  */
#line 14573 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("float8");
                }
#line 48986 "gram.c"
    break;

  case 1964: /* Bit: BitWithLength  */
#line 14583 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 48994 "gram.c"
    break;

  case 1965: /* Bit: BitWithoutLength  */
#line 14587 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 49002 "gram.c"
    break;

  case 1966: /* ConstBit: BitWithLength  */
#line 14595 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 49010 "gram.c"
    break;

  case 1967: /* ConstBit: BitWithoutLength  */
#line 14599 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                    (yyval.typnam)->typmods = NIL;
                }
#line 49019 "gram.c"
    break;

  case 1968: /* BitWithLength: BIT opt_varying '(' expr_list ')'  */
#line 14607 "gram.y"
                                {
                    char *typname;

                    typname = (yyvsp[-3].boolean) ? "varbit" : "bit";
                    (yyval.typnam) = SystemTypeName(typname);
                    (yyval.typnam)->typmods = (yyvsp[-1].list);
                    (yyval.typnam)->location = (yylsp[-4]);
                }
#line 49032 "gram.c"
    break;

  case 1969: /* BitWithoutLength: BIT opt_varying  */
#line 14619 "gram.y"
                                {
                    /* bit defaults to bit(1), varbit to no limit */
                    if ((yyvsp[0].boolean))
                    {
                        (yyval.typnam) = SystemTypeName("varbit");
                    }
                    else
                    {
                        (yyval.typnam) = SystemTypeName("bit");
                        (yyval.typnam)->typmods = list_make1(makeIntConst(1, -1));
                    }
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 49050 "gram.c"
    break;

  case 1970: /* Character: CharacterWithLength  */
#line 14640 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 49058 "gram.c"
    break;

  case 1971: /* Character: CharacterWithoutLength  */
#line 14644 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 49066 "gram.c"
    break;

  case 1972: /* ConstCharacter: CharacterWithLength  */
#line 14650 "gram.y"
                                {
                    (yyval.typnam) = (yyvsp[0].typnam);
                }
#line 49074 "gram.c"
    break;

  case 1973: /* ConstCharacter: CharacterWithoutLength  */
#line 14654 "gram.y"
                                {
                    /* Length was not specified so allow to be unrestricted.
                     * This handles problems with fixed-length (bpchar) strings
                     * which in column definitions must default to a length
                     * of one, but should not be constrained if the length
                     * was not specified.
                     */
                    (yyval.typnam) = (yyvsp[0].typnam);
                    (yyval.typnam)->typmods = NIL;
                }
#line 49089 "gram.c"
    break;

  case 1974: /* CharacterWithLength: character '(' Iconst ')'  */
#line 14667 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName((yyvsp[-3].str));
                    (yyval.typnam)->typmods = list_make1(makeIntConst((yyvsp[-1].ival), (yylsp[-1])));
                    (yyval.typnam)->location = (yylsp[-3]);
                }
#line 49099 "gram.c"
    break;

  case 1975: /* CharacterWithoutLength: character  */
#line 14675 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName((yyvsp[0].str));
                    /* char defaults to char(1), varchar to no limit */
                    if (strcmp((yyvsp[0].str), "bpchar") == 0)
                        (yyval.typnam)->typmods = list_make1(makeIntConst(1, -1));
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 49111 "gram.c"
    break;

  case 1976: /* character: CHARACTER opt_varying  */
#line 14685 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].boolean) ? "varchar": "bpchar"; }
#line 49117 "gram.c"
    break;

  case 1977: /* character: CHAR_P opt_varying  */
#line 14687 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].boolean) ? "varchar": "bpchar"; }
#line 49123 "gram.c"
    break;

  case 1978: /* character: VARCHAR  */
#line 14689 "gram.y"
                                                                                { (yyval.str) = "varchar"; }
#line 49129 "gram.c"
    break;

  case 1979: /* character: NATIONAL CHARACTER opt_varying  */
#line 14691 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].boolean) ? "varchar": "bpchar"; }
#line 49135 "gram.c"
    break;

  case 1980: /* character: NATIONAL CHAR_P opt_varying  */
#line 14693 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].boolean) ? "varchar": "bpchar"; }
#line 49141 "gram.c"
    break;

  case 1981: /* character: NCHAR opt_varying  */
#line 14695 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].boolean) ? "varchar": "bpchar"; }
#line 49147 "gram.c"
    break;

  case 1982: /* opt_varying: VARYING  */
#line 14699 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 49153 "gram.c"
    break;

  case 1983: /* opt_varying: %empty  */
#line 14700 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 49159 "gram.c"
    break;

  case 1984: /* ConstDatetime: TIMESTAMP '(' Iconst ')' opt_timezone  */
#line 14708 "gram.y"
                                {
                    if ((yyvsp[0].boolean))
                        (yyval.typnam) = SystemTypeName("timestamptz");
                    else
                        (yyval.typnam) = SystemTypeName("timestamp");
                    (yyval.typnam)->typmods = list_make1(makeIntConst((yyvsp[-2].ival), (yylsp[-2])));
                    (yyval.typnam)->location = (yylsp[-4]);
                }
#line 49172 "gram.c"
    break;

  case 1985: /* ConstDatetime: TIMESTAMP opt_timezone  */
#line 14717 "gram.y"
                                {
                    if ((yyvsp[0].boolean))
                        (yyval.typnam) = SystemTypeName("timestamptz");
                    else
                        (yyval.typnam) = SystemTypeName("timestamp");
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 49184 "gram.c"
    break;

  case 1986: /* ConstDatetime: TIME '(' Iconst ')' opt_timezone  */
#line 14725 "gram.y"
                                {
                    if ((yyvsp[0].boolean))
                        (yyval.typnam) = SystemTypeName("timetz");
                    else
                        (yyval.typnam) = SystemTypeName("time");
                    (yyval.typnam)->typmods = list_make1(makeIntConst((yyvsp[-2].ival), (yylsp[-2])));
                    (yyval.typnam)->location = (yylsp[-4]);
                }
#line 49197 "gram.c"
    break;

  case 1987: /* ConstDatetime: TIME opt_timezone  */
#line 14734 "gram.y"
                                {
                    if ((yyvsp[0].boolean))
                        (yyval.typnam) = SystemTypeName("timetz");
                    else
                        (yyval.typnam) = SystemTypeName("time");
                    (yyval.typnam)->location = (yylsp[-1]);
                }
#line 49209 "gram.c"
    break;

  case 1988: /* ConstInterval: INTERVAL  */
#line 14745 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("interval");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 49218 "gram.c"
    break;

  case 1989: /* opt_timezone: WITH_LA TIME ZONE  */
#line 14752 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 49224 "gram.c"
    break;

  case 1990: /* opt_timezone: WITHOUT_LA TIME ZONE  */
#line 14753 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 49230 "gram.c"
    break;

  case 1991: /* opt_timezone: %empty  */
#line 14754 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 49236 "gram.c"
    break;

  case 1992: /* opt_interval: YEAR_P  */
#line 14759 "gram.y"
                                { (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(YEAR), (yylsp[0]))); }
#line 49242 "gram.c"
    break;

  case 1993: /* opt_interval: MONTH_P  */
#line 14761 "gram.y"
                                { (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(MONTH), (yylsp[0]))); }
#line 49248 "gram.c"
    break;

  case 1994: /* opt_interval: DAY_P  */
#line 14763 "gram.y"
                                { (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(DAY), (yylsp[0]))); }
#line 49254 "gram.c"
    break;

  case 1995: /* opt_interval: HOUR_P  */
#line 14765 "gram.y"
                                { (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(HOUR), (yylsp[0]))); }
#line 49260 "gram.c"
    break;

  case 1996: /* opt_interval: MINUTE_P  */
#line 14767 "gram.y"
                                { (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(MINUTE), (yylsp[0]))); }
#line 49266 "gram.c"
    break;

  case 1997: /* opt_interval: interval_second  */
#line 14769 "gram.y"
                                { (yyval.list) = (yyvsp[0].list); }
#line 49272 "gram.c"
    break;

  case 1998: /* opt_interval: YEAR_P TO MONTH_P  */
#line 14771 "gram.y"
                                {
                    (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(YEAR) |
                                                 INTERVAL_MASK(MONTH), (yylsp[-2])));
                }
#line 49281 "gram.c"
    break;

  case 1999: /* opt_interval: DAY_P TO HOUR_P  */
#line 14776 "gram.y"
                                {
                    (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(DAY) |
                                                 INTERVAL_MASK(HOUR), (yylsp[-2])));
                }
#line 49290 "gram.c"
    break;

  case 2000: /* opt_interval: DAY_P TO MINUTE_P  */
#line 14781 "gram.y"
                                {
                    (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(DAY) |
                                                 INTERVAL_MASK(HOUR) |
                                                 INTERVAL_MASK(MINUTE), (yylsp[-2])));
                }
#line 49300 "gram.c"
    break;

  case 2001: /* opt_interval: DAY_P TO interval_second  */
#line 14787 "gram.y"
                                {
                    (yyval.list) = (yyvsp[0].list);
                    linitial((yyval.list)) = makeIntConst(INTERVAL_MASK(DAY) |
                                                INTERVAL_MASK(HOUR) |
                                                INTERVAL_MASK(MINUTE) |
                                                INTERVAL_MASK(SECOND), (yylsp[-2]));
                }
#line 49312 "gram.c"
    break;

  case 2002: /* opt_interval: HOUR_P TO MINUTE_P  */
#line 14795 "gram.y"
                                {
                    (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(HOUR) |
                                                 INTERVAL_MASK(MINUTE), (yylsp[-2])));
                }
#line 49321 "gram.c"
    break;

  case 2003: /* opt_interval: HOUR_P TO interval_second  */
#line 14800 "gram.y"
                                {
                    (yyval.list) = (yyvsp[0].list);
                    linitial((yyval.list)) = makeIntConst(INTERVAL_MASK(HOUR) |
                                                INTERVAL_MASK(MINUTE) |
                                                INTERVAL_MASK(SECOND), (yylsp[-2]));
                }
#line 49332 "gram.c"
    break;

  case 2004: /* opt_interval: MINUTE_P TO interval_second  */
#line 14807 "gram.y"
                                {
                    (yyval.list) = (yyvsp[0].list);
                    linitial((yyval.list)) = makeIntConst(INTERVAL_MASK(MINUTE) |
                                                INTERVAL_MASK(SECOND), (yylsp[-2]));
                }
#line 49342 "gram.c"
    break;

  case 2005: /* opt_interval: %empty  */
#line 14813 "gram.y"
                                { (yyval.list) = NIL; }
#line 49348 "gram.c"
    break;

  case 2006: /* interval_second: SECOND_P  */
#line 14818 "gram.y"
                                {
                    (yyval.list) = list_make1(makeIntConst(INTERVAL_MASK(SECOND), (yylsp[0])));
                }
#line 49356 "gram.c"
    break;

  case 2007: /* interval_second: SECOND_P '(' Iconst ')'  */
#line 14822 "gram.y"
                                {
                    (yyval.list) = list_make2(makeIntConst(INTERVAL_MASK(SECOND), (yylsp[-3])),
                                    makeIntConst((yyvsp[-1].ival), (yylsp[-1])));
                }
#line 49365 "gram.c"
    break;

  case 2008: /* JsonType: JSON  */
#line 14830 "gram.y"
                                {
                    (yyval.typnam) = SystemTypeName("json");
                    (yyval.typnam)->location = (yylsp[0]);
                }
#line 49374 "gram.c"
    break;

  case 2009: /* a_expr: c_expr  */
#line 14864 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 49380 "gram.c"
    break;

  case 2010: /* a_expr: a_expr TYPECAST Typename  */
#line 14866 "gram.y"
                                        { (yyval.node) = makeTypeCast((yyvsp[-2].node), (yyvsp[0].typnam), (yylsp[-1])); }
#line 49386 "gram.c"
    break;

  case 2011: /* a_expr: a_expr COLLATE any_name  */
#line 14868 "gram.y"
                                {
                    CollateClause *n = makeNode(CollateClause);

                    n->arg = (yyvsp[-2].node);
                    n->collname = (yyvsp[0].list);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 49399 "gram.c"
    break;

  case 2012: /* a_expr: a_expr AT TIME ZONE a_expr  */
#line 14877 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("timezone"),
                                               list_make2((yyvsp[0].node), (yyvsp[-4].node)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 49410 "gram.c"
    break;

  case 2013: /* a_expr: a_expr AT LOCAL  */
#line 14884 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("timezone"),
                                               list_make1((yyvsp[-2].node)),
                                               COERCE_SQL_SYNTAX,
                                               -1);
                }
#line 49421 "gram.c"
    break;

  case 2014: /* a_expr: '+' a_expr  */
#line 14900 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "+", NULL, (yyvsp[0].node), (yylsp[-1])); }
#line 49427 "gram.c"
    break;

  case 2015: /* a_expr: '-' a_expr  */
#line 14902 "gram.y"
                                { (yyval.node) = doNegate((yyvsp[0].node), (yylsp[-1])); }
#line 49433 "gram.c"
    break;

  case 2016: /* a_expr: a_expr '+' a_expr  */
#line 14904 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "+", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49439 "gram.c"
    break;

  case 2017: /* a_expr: a_expr '-' a_expr  */
#line 14906 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "-", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49445 "gram.c"
    break;

  case 2018: /* a_expr: a_expr '*' a_expr  */
#line 14908 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "*", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49451 "gram.c"
    break;

  case 2019: /* a_expr: a_expr '/' a_expr  */
#line 14910 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "/", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49457 "gram.c"
    break;

  case 2020: /* a_expr: a_expr '%' a_expr  */
#line 14912 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "%", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49463 "gram.c"
    break;

  case 2021: /* a_expr: a_expr '^' a_expr  */
#line 14914 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "^", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49469 "gram.c"
    break;

  case 2022: /* a_expr: a_expr '<' a_expr  */
#line 14916 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49475 "gram.c"
    break;

  case 2023: /* a_expr: a_expr '>' a_expr  */
#line 14918 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, ">", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49481 "gram.c"
    break;

  case 2024: /* a_expr: a_expr '=' a_expr  */
#line 14920 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49487 "gram.c"
    break;

  case 2025: /* a_expr: a_expr LESS_EQUALS a_expr  */
#line 14922 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49493 "gram.c"
    break;

  case 2026: /* a_expr: a_expr GREATER_EQUALS a_expr  */
#line 14924 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, ">=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49499 "gram.c"
    break;

  case 2027: /* a_expr: a_expr NOT_EQUALS a_expr  */
#line 14926 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<>", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49505 "gram.c"
    break;

  case 2028: /* a_expr: a_expr qual_Op a_expr  */
#line 14929 "gram.y"
                                { (yyval.node) = (Node *) makeA_Expr(AEXPR_OP, (yyvsp[-1].list), (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49511 "gram.c"
    break;

  case 2029: /* a_expr: qual_Op a_expr  */
#line 14931 "gram.y"
                                { (yyval.node) = (Node *) makeA_Expr(AEXPR_OP, (yyvsp[-1].list), NULL, (yyvsp[0].node), (yylsp[-1])); }
#line 49517 "gram.c"
    break;

  case 2030: /* a_expr: a_expr AND a_expr  */
#line 14934 "gram.y"
                                { (yyval.node) = makeAndExpr((yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49523 "gram.c"
    break;

  case 2031: /* a_expr: a_expr OR a_expr  */
#line 14936 "gram.y"
                                { (yyval.node) = makeOrExpr((yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 49529 "gram.c"
    break;

  case 2032: /* a_expr: NOT a_expr  */
#line 14938 "gram.y"
                                { (yyval.node) = makeNotExpr((yyvsp[0].node), (yylsp[-1])); }
#line 49535 "gram.c"
    break;

  case 2033: /* a_expr: NOT_LA a_expr  */
#line 14940 "gram.y"
                                { (yyval.node) = makeNotExpr((yyvsp[0].node), (yylsp[-1])); }
#line 49541 "gram.c"
    break;

  case 2034: /* a_expr: a_expr LIKE a_expr  */
#line 14943 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_LIKE, "~~",
                                                   (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]));
                }
#line 49550 "gram.c"
    break;

  case 2035: /* a_expr: a_expr LIKE a_expr ESCAPE a_expr  */
#line 14948 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("like_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-3]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_LIKE, "~~",
                                                   (yyvsp[-4].node), (Node *) n, (yylsp[-3]));
                }
#line 49563 "gram.c"
    break;

  case 2036: /* a_expr: a_expr NOT_LA LIKE a_expr  */
#line 14957 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_LIKE, "!~~",
                                                   (yyvsp[-3].node), (yyvsp[0].node), (yylsp[-2]));
                }
#line 49572 "gram.c"
    break;

  case 2037: /* a_expr: a_expr NOT_LA LIKE a_expr ESCAPE a_expr  */
#line 14962 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("like_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-4]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_LIKE, "!~~",
                                                   (yyvsp[-5].node), (Node *) n, (yylsp[-4]));
                }
#line 49585 "gram.c"
    break;

  case 2038: /* a_expr: a_expr ILIKE a_expr  */
#line 14971 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_ILIKE, "~~*",
                                                   (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]));
                }
#line 49594 "gram.c"
    break;

  case 2039: /* a_expr: a_expr ILIKE a_expr ESCAPE a_expr  */
#line 14976 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("like_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-3]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_ILIKE, "~~*",
                                                   (yyvsp[-4].node), (Node *) n, (yylsp[-3]));
                }
#line 49607 "gram.c"
    break;

  case 2040: /* a_expr: a_expr NOT_LA ILIKE a_expr  */
#line 14985 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_ILIKE, "!~~*",
                                                   (yyvsp[-3].node), (yyvsp[0].node), (yylsp[-2]));
                }
#line 49616 "gram.c"
    break;

  case 2041: /* a_expr: a_expr NOT_LA ILIKE a_expr ESCAPE a_expr  */
#line 14990 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("like_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-4]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_ILIKE, "!~~*",
                                                   (yyvsp[-5].node), (Node *) n, (yylsp[-4]));
                }
#line 49629 "gram.c"
    break;

  case 2042: /* a_expr: a_expr SIMILAR TO a_expr  */
#line 15000 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("similar_to_escape"),
                                                 list_make1((yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-2]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_SIMILAR, "~",
                                                   (yyvsp[-3].node), (Node *) n, (yylsp[-2]));
                }
#line 49642 "gram.c"
    break;

  case 2043: /* a_expr: a_expr SIMILAR TO a_expr ESCAPE a_expr  */
#line 15009 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("similar_to_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-4]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_SIMILAR, "~",
                                                   (yyvsp[-5].node), (Node *) n, (yylsp[-4]));
                }
#line 49655 "gram.c"
    break;

  case 2044: /* a_expr: a_expr NOT_LA SIMILAR TO a_expr  */
#line 15018 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("similar_to_escape"),
                                                 list_make1((yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-3]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_SIMILAR, "!~",
                                                   (yyvsp[-4].node), (Node *) n, (yylsp[-3]));
                }
#line 49668 "gram.c"
    break;

  case 2045: /* a_expr: a_expr NOT_LA SIMILAR TO a_expr ESCAPE a_expr  */
#line 15027 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall(SystemFuncName("similar_to_escape"),
                                                 list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-5]));
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_SIMILAR, "!~",
                                                   (yyvsp[-6].node), (Node *) n, (yylsp[-5]));
                }
#line 49681 "gram.c"
    break;

  case 2046: /* a_expr: a_expr IS NULL_P  */
#line 15046 "gram.y"
                                {
                    NullTest   *n = makeNode(NullTest);

                    n->arg = (Expr *) (yyvsp[-2].node);
                    n->nulltesttype = IS_NULL;
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 49694 "gram.c"
    break;

  case 2047: /* a_expr: a_expr ISNULL  */
#line 15055 "gram.y"
                                {
                    NullTest   *n = makeNode(NullTest);

                    n->arg = (Expr *) (yyvsp[-1].node);
                    n->nulltesttype = IS_NULL;
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 49707 "gram.c"
    break;

  case 2048: /* a_expr: a_expr IS NOT NULL_P  */
#line 15064 "gram.y"
                                {
                    NullTest   *n = makeNode(NullTest);

                    n->arg = (Expr *) (yyvsp[-3].node);
                    n->nulltesttype = IS_NOT_NULL;
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 49720 "gram.c"
    break;

  case 2049: /* a_expr: a_expr NOTNULL  */
#line 15073 "gram.y"
                                {
                    NullTest   *n = makeNode(NullTest);

                    n->arg = (Expr *) (yyvsp[-1].node);
                    n->nulltesttype = IS_NOT_NULL;
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 49733 "gram.c"
    break;

  case 2050: /* a_expr: row OVERLAPS row  */
#line 15082 "gram.y"
                                {
                    if (list_length((yyvsp[-2].list)) != 2)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("wrong number of parameters on left side of OVERLAPS expression"),
                                 parser_errposition((yylsp[-2]))));
                    if (list_length((yyvsp[0].list)) != 2)
                        ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("wrong number of parameters on right side of OVERLAPS expression"),
                                 parser_errposition((yylsp[0]))));
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("overlaps"),
                                               list_concat((yyvsp[-2].list), (yyvsp[0].list)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-1]));
                }
#line 49754 "gram.c"
    break;

  case 2051: /* a_expr: a_expr IS TRUE_P  */
#line 15099 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-2].node);
                    b->booltesttype = IS_TRUE;
                    b->location = (yylsp[-1]);
                    (yyval.node) = (Node *) b;
                }
#line 49767 "gram.c"
    break;

  case 2052: /* a_expr: a_expr IS NOT TRUE_P  */
#line 15108 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-3].node);
                    b->booltesttype = IS_NOT_TRUE;
                    b->location = (yylsp[-2]);
                    (yyval.node) = (Node *) b;
                }
#line 49780 "gram.c"
    break;

  case 2053: /* a_expr: a_expr IS FALSE_P  */
#line 15117 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-2].node);
                    b->booltesttype = IS_FALSE;
                    b->location = (yylsp[-1]);
                    (yyval.node) = (Node *) b;
                }
#line 49793 "gram.c"
    break;

  case 2054: /* a_expr: a_expr IS NOT FALSE_P  */
#line 15126 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-3].node);
                    b->booltesttype = IS_NOT_FALSE;
                    b->location = (yylsp[-2]);
                    (yyval.node) = (Node *) b;
                }
#line 49806 "gram.c"
    break;

  case 2055: /* a_expr: a_expr IS UNKNOWN  */
#line 15135 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-2].node);
                    b->booltesttype = IS_UNKNOWN;
                    b->location = (yylsp[-1]);
                    (yyval.node) = (Node *) b;
                }
#line 49819 "gram.c"
    break;

  case 2056: /* a_expr: a_expr IS NOT UNKNOWN  */
#line 15144 "gram.y"
                                {
                    BooleanTest *b = makeNode(BooleanTest);

                    b->arg = (Expr *) (yyvsp[-3].node);
                    b->booltesttype = IS_NOT_UNKNOWN;
                    b->location = (yylsp[-2]);
                    (yyval.node) = (Node *) b;
                }
#line 49832 "gram.c"
    break;

  case 2057: /* a_expr: a_expr IS DISTINCT FROM a_expr  */
#line 15153 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_DISTINCT, "=", (yyvsp[-4].node), (yyvsp[0].node), (yylsp[-3]));
                }
#line 49840 "gram.c"
    break;

  case 2058: /* a_expr: a_expr IS NOT DISTINCT FROM a_expr  */
#line 15157 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_NOT_DISTINCT, "=", (yyvsp[-5].node), (yyvsp[0].node), (yylsp[-4]));
                }
#line 49848 "gram.c"
    break;

  case 2059: /* a_expr: a_expr BETWEEN opt_asymmetric b_expr AND a_expr  */
#line 15161 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_BETWEEN,
                                                   "BETWEEN",
                                                   (yyvsp[-5].node),
                                                   (Node *) list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                   (yylsp[-4]));
                }
#line 49860 "gram.c"
    break;

  case 2060: /* a_expr: a_expr NOT_LA BETWEEN opt_asymmetric b_expr AND a_expr  */
#line 15169 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_NOT_BETWEEN,
                                                   "NOT BETWEEN",
                                                   (yyvsp[-6].node),
                                                   (Node *) list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                   (yylsp[-5]));
                }
#line 49872 "gram.c"
    break;

  case 2061: /* a_expr: a_expr BETWEEN SYMMETRIC b_expr AND a_expr  */
#line 15177 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_BETWEEN_SYM,
                                                   "BETWEEN SYMMETRIC",
                                                   (yyvsp[-5].node),
                                                   (Node *) list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                   (yylsp[-4]));
                }
#line 49884 "gram.c"
    break;

  case 2062: /* a_expr: a_expr NOT_LA BETWEEN SYMMETRIC b_expr AND a_expr  */
#line 15185 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_NOT_BETWEEN_SYM,
                                                   "NOT BETWEEN SYMMETRIC",
                                                   (yyvsp[-6].node),
                                                   (Node *) list_make2((yyvsp[-2].node), (yyvsp[0].node)),
                                                   (yylsp[-5]));
                }
#line 49896 "gram.c"
    break;

  case 2063: /* a_expr: a_expr IN_P in_expr  */
#line 15193 "gram.y"
                                {
                    /* in_expr returns a SubLink or a list of a_exprs */
                    if (IsA((yyvsp[0].node), SubLink))
                    {
                        /* generate foo = ANY (subquery) */
                        SubLink    *n = (SubLink *) (yyvsp[0].node);

                        n->subLinkType = ANY_SUBLINK;
                        n->subLinkId = 0;
                        n->testexpr = (yyvsp[-2].node);
                        n->operName = NIL;      /* show it's IN not = ANY */
                        n->location = (yylsp[-1]);
                        (yyval.node) = (Node *) n;
                    }
                    else
                    {
                        /* generate scalar IN expression */
                        (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_IN, "=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]));
                    }
                }
#line 49921 "gram.c"
    break;

  case 2064: /* a_expr: a_expr NOT_LA IN_P in_expr  */
#line 15214 "gram.y"
                                {
                    /* in_expr returns a SubLink or a list of a_exprs */
                    if (IsA((yyvsp[0].node), SubLink))
                    {
                        /* generate NOT (foo = ANY (subquery)) */
                        /* Make an = ANY node */
                        SubLink    *n = (SubLink *) (yyvsp[0].node);

                        n->subLinkType = ANY_SUBLINK;
                        n->subLinkId = 0;
                        n->testexpr = (yyvsp[-3].node);
                        n->operName = NIL;      /* show it's IN not = ANY */
                        n->location = (yylsp[-2]);
                        /* Stick a NOT on top; must have same parse location */
                        (yyval.node) = makeNotExpr((Node *) n, (yylsp[-2]));
                    }
                    else
                    {
                        /* generate scalar NOT IN expression */
                        (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_IN, "<>", (yyvsp[-3].node), (yyvsp[0].node), (yylsp[-2]));
                    }
                }
#line 49948 "gram.c"
    break;

  case 2065: /* a_expr: a_expr subquery_Op sub_type select_with_parens  */
#line 15237 "gram.y"
                                {
                    SubLink    *n = makeNode(SubLink);

                    n->subLinkType = (yyvsp[-1].ival);
                    n->subLinkId = 0;
                    n->testexpr = (yyvsp[-3].node);
                    n->operName = (yyvsp[-2].list);
                    n->subselect = (yyvsp[0].node);
                    n->location = (yylsp[-2]);
                    (yyval.node) = (Node *) n;
                }
#line 49964 "gram.c"
    break;

  case 2066: /* a_expr: a_expr subquery_Op sub_type '(' a_expr ')'  */
#line 15249 "gram.y"
                                {
                    if ((yyvsp[-3].ival) == ANY_SUBLINK)
                        (yyval.node) = (Node *) makeA_Expr(AEXPR_OP_ANY, (yyvsp[-4].list), (yyvsp[-5].node), (yyvsp[-1].node), (yylsp[-4]));
                    else
                        (yyval.node) = (Node *) makeA_Expr(AEXPR_OP_ALL, (yyvsp[-4].list), (yyvsp[-5].node), (yyvsp[-1].node), (yylsp[-4]));
                }
#line 49975 "gram.c"
    break;

  case 2067: /* a_expr: UNIQUE opt_unique_null_treatment select_with_parens  */
#line 15256 "gram.y"
                                {
                    /* Not sure how to get rid of the parentheses
                     * but there are lots of shift/reduce errors without them.
                     *
                     * Should be able to implement this by plopping the entire
                     * select into a node, then transforming the target expressions
                     * from whatever they are into count(*), and testing the
                     * entire result equal to one.
                     * But, will probably implement a separate node in the executor.
                     */
                    ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                             errmsg("UNIQUE predicate is not yet implemented"),
                             parser_errposition((yylsp[-2]))));
                }
#line 49995 "gram.c"
    break;

  case 2068: /* a_expr: a_expr IS DOCUMENT_P  */
#line 15272 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_DOCUMENT, NULL, NIL,
                                     list_make1((yyvsp[-2].node)), (yylsp[-1]));
                }
#line 50004 "gram.c"
    break;

  case 2069: /* a_expr: a_expr IS NOT DOCUMENT_P  */
#line 15277 "gram.y"
                                {
                    (yyval.node) = makeNotExpr(makeXmlExpr(IS_DOCUMENT, NULL, NIL,
                                                 list_make1((yyvsp[-3].node)), (yylsp[-2])),
                                     (yylsp[-2]));
                }
#line 50014 "gram.c"
    break;

  case 2070: /* a_expr: a_expr IS NORMALIZED  */
#line 15283 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("is_normalized"),
                                               list_make1((yyvsp[-2].node)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-1]));
                }
#line 50025 "gram.c"
    break;

  case 2071: /* a_expr: a_expr IS unicode_normal_form NORMALIZED  */
#line 15290 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("is_normalized"),
                                               list_make2((yyvsp[-3].node), makeStringConst((yyvsp[-1].str), (yylsp[-1]))),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-2]));
                }
#line 50036 "gram.c"
    break;

  case 2072: /* a_expr: a_expr IS NOT NORMALIZED  */
#line 15297 "gram.y"
                                {
                    (yyval.node) = makeNotExpr((Node *) makeFuncCall(SystemFuncName("is_normalized"),
                                                           list_make1((yyvsp[-3].node)),
                                                           COERCE_SQL_SYNTAX,
                                                           (yylsp[-2])),
                                     (yylsp[-2]));
                }
#line 50048 "gram.c"
    break;

  case 2073: /* a_expr: a_expr IS NOT unicode_normal_form NORMALIZED  */
#line 15305 "gram.y"
                                {
                    (yyval.node) = makeNotExpr((Node *) makeFuncCall(SystemFuncName("is_normalized"),
                                                           list_make2((yyvsp[-4].node), makeStringConst((yyvsp[-1].str), (yylsp[-1]))),
                                                           COERCE_SQL_SYNTAX,
                                                           (yylsp[-3])),
                                     (yylsp[-3]));
                }
#line 50060 "gram.c"
    break;

  case 2074: /* a_expr: a_expr IS json_predicate_type_constraint json_key_uniqueness_constraint_opt  */
#line 15314 "gram.y"
                                {
                    JsonFormat *format = makeJsonFormat(JS_FORMAT_DEFAULT, JS_ENC_DEFAULT, -1);

                    (yyval.node) = makeJsonIsPredicate((yyvsp[-3].node), format, (yyvsp[-1].ival), (yyvsp[0].boolean), (yylsp[-3]));
                }
#line 50070 "gram.c"
    break;

  case 2075: /* a_expr: a_expr IS NOT json_predicate_type_constraint json_key_uniqueness_constraint_opt  */
#line 15332 "gram.y"
                                {
                    JsonFormat *format = makeJsonFormat(JS_FORMAT_DEFAULT, JS_ENC_DEFAULT, -1);

                    (yyval.node) = makeNotExpr(makeJsonIsPredicate((yyvsp[-4].node), format, (yyvsp[-1].ival), (yyvsp[0].boolean), (yylsp[-4])), (yylsp[-4]));
                }
#line 50080 "gram.c"
    break;

  case 2076: /* a_expr: DEFAULT  */
#line 15349 "gram.y"
                                {
                    /*
                     * The SQL spec only allows DEFAULT in "contextually typed
                     * expressions", but for us, it's easier to allow it in
                     * any a_expr and then throw error during parse analysis
                     * if it's in an inappropriate context.  This way also
                     * lets us say something smarter than "syntax error".
                     */
                    SetToDefault *n = makeNode(SetToDefault);

                    /* parse analysis will fill in the rest */
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 50099 "gram.c"
    break;

  case 2077: /* b_expr: c_expr  */
#line 15375 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 50105 "gram.c"
    break;

  case 2078: /* b_expr: b_expr TYPECAST Typename  */
#line 15377 "gram.y"
                                { (yyval.node) = makeTypeCast((yyvsp[-2].node), (yyvsp[0].typnam), (yylsp[-1])); }
#line 50111 "gram.c"
    break;

  case 2079: /* b_expr: '+' b_expr  */
#line 15379 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "+", NULL, (yyvsp[0].node), (yylsp[-1])); }
#line 50117 "gram.c"
    break;

  case 2080: /* b_expr: '-' b_expr  */
#line 15381 "gram.y"
                                { (yyval.node) = doNegate((yyvsp[0].node), (yylsp[-1])); }
#line 50123 "gram.c"
    break;

  case 2081: /* b_expr: b_expr '+' b_expr  */
#line 15383 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "+", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50129 "gram.c"
    break;

  case 2082: /* b_expr: b_expr '-' b_expr  */
#line 15385 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "-", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50135 "gram.c"
    break;

  case 2083: /* b_expr: b_expr '*' b_expr  */
#line 15387 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "*", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50141 "gram.c"
    break;

  case 2084: /* b_expr: b_expr '/' b_expr  */
#line 15389 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "/", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50147 "gram.c"
    break;

  case 2085: /* b_expr: b_expr '%' b_expr  */
#line 15391 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "%", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50153 "gram.c"
    break;

  case 2086: /* b_expr: b_expr '^' b_expr  */
#line 15393 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "^", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50159 "gram.c"
    break;

  case 2087: /* b_expr: b_expr '<' b_expr  */
#line 15395 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50165 "gram.c"
    break;

  case 2088: /* b_expr: b_expr '>' b_expr  */
#line 15397 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, ">", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50171 "gram.c"
    break;

  case 2089: /* b_expr: b_expr '=' b_expr  */
#line 15399 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50177 "gram.c"
    break;

  case 2090: /* b_expr: b_expr LESS_EQUALS b_expr  */
#line 15401 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50183 "gram.c"
    break;

  case 2091: /* b_expr: b_expr GREATER_EQUALS b_expr  */
#line 15403 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, ">=", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50189 "gram.c"
    break;

  case 2092: /* b_expr: b_expr NOT_EQUALS b_expr  */
#line 15405 "gram.y"
                                { (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_OP, "<>", (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50195 "gram.c"
    break;

  case 2093: /* b_expr: b_expr qual_Op b_expr  */
#line 15407 "gram.y"
                                { (yyval.node) = (Node *) makeA_Expr(AEXPR_OP, (yyvsp[-1].list), (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1])); }
#line 50201 "gram.c"
    break;

  case 2094: /* b_expr: qual_Op b_expr  */
#line 15409 "gram.y"
                                { (yyval.node) = (Node *) makeA_Expr(AEXPR_OP, (yyvsp[-1].list), NULL, (yyvsp[0].node), (yylsp[-1])); }
#line 50207 "gram.c"
    break;

  case 2095: /* b_expr: b_expr IS DISTINCT FROM b_expr  */
#line 15411 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_DISTINCT, "=", (yyvsp[-4].node), (yyvsp[0].node), (yylsp[-3]));
                }
#line 50215 "gram.c"
    break;

  case 2096: /* b_expr: b_expr IS NOT DISTINCT FROM b_expr  */
#line 15415 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_NOT_DISTINCT, "=", (yyvsp[-5].node), (yyvsp[0].node), (yylsp[-4]));
                }
#line 50223 "gram.c"
    break;

  case 2097: /* b_expr: b_expr IS DOCUMENT_P  */
#line 15419 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_DOCUMENT, NULL, NIL,
                                     list_make1((yyvsp[-2].node)), (yylsp[-1]));
                }
#line 50232 "gram.c"
    break;

  case 2098: /* b_expr: b_expr IS NOT DOCUMENT_P  */
#line 15424 "gram.y"
                                {
                    (yyval.node) = makeNotExpr(makeXmlExpr(IS_DOCUMENT, NULL, NIL,
                                                 list_make1((yyvsp[-3].node)), (yylsp[-2])),
                                     (yylsp[-2]));
                }
#line 50242 "gram.c"
    break;

  case 2099: /* c_expr: columnref  */
#line 15439 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 50248 "gram.c"
    break;

  case 2100: /* c_expr: AexprConst  */
#line 15440 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 50254 "gram.c"
    break;

  case 2101: /* c_expr: PARAM opt_indirection  */
#line 15442 "gram.y"
                                {
                    ParamRef   *p = makeNode(ParamRef);

                    p->number = (yyvsp[-1].ival);
                    p->location = (yylsp[-1]);
                    if ((yyvsp[0].list))
                    {
                        A_Indirection *n = makeNode(A_Indirection);

                        n->arg = (Node *) p;
                        n->indirection = check_indirection((yyvsp[0].list), yyscanner);
                        (yyval.node) = (Node *) n;
                    }
                    else
                        (yyval.node) = (Node *) p;
                }
#line 50275 "gram.c"
    break;

  case 2102: /* c_expr: '(' a_expr ')' opt_indirection  */
#line 15459 "gram.y"
                                {
                    if ((yyvsp[0].list))
                    {
                        A_Indirection *n = makeNode(A_Indirection);

                        n->arg = (yyvsp[-2].node);
                        n->indirection = check_indirection((yyvsp[0].list), yyscanner);
                        (yyval.node) = (Node *) n;
                    }
                    else
                        (yyval.node) = (yyvsp[-2].node);
                }
#line 50292 "gram.c"
    break;

  case 2103: /* c_expr: case_expr  */
#line 15472 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 50298 "gram.c"
    break;

  case 2104: /* c_expr: func_expr  */
#line 15474 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 50304 "gram.c"
    break;

  case 2105: /* c_expr: select_with_parens  */
#line 15476 "gram.y"
                                {
                    SubLink    *n = makeNode(SubLink);

                    n->subLinkType = EXPR_SUBLINK;
                    n->subLinkId = 0;
                    n->testexpr = NULL;
                    n->operName = NIL;
                    n->subselect = (yyvsp[0].node);
                    n->location = (yylsp[0]);
                    (yyval.node) = (Node *) n;
                }
#line 50320 "gram.c"
    break;

  case 2106: /* c_expr: select_with_parens indirection  */
#line 15488 "gram.y"
                                {
                    /*
                     * Because the select_with_parens nonterminal is designed
                     * to "eat" as many levels of parens as possible, the
                     * '(' a_expr ')' opt_indirection production above will
                     * fail to match a sub-SELECT with indirection decoration;
                     * the sub-SELECT won't be regarded as an a_expr as long
                     * as there are parens around it.  To support applying
                     * subscripting or field selection to a sub-SELECT result,
                     * we need this redundant-looking production.
                     */
                    SubLink    *n = makeNode(SubLink);
                    A_Indirection *a = makeNode(A_Indirection);

                    n->subLinkType = EXPR_SUBLINK;
                    n->subLinkId = 0;
                    n->testexpr = NULL;
                    n->operName = NIL;
                    n->subselect = (yyvsp[-1].node);
                    n->location = (yylsp[-1]);
                    a->arg = (Node *) n;
                    a->indirection = check_indirection((yyvsp[0].list), yyscanner);
                    (yyval.node) = (Node *) a;
                }
#line 50349 "gram.c"
    break;

  case 2107: /* c_expr: EXISTS select_with_parens  */
#line 15513 "gram.y"
                                {
                    SubLink    *n = makeNode(SubLink);

                    n->subLinkType = EXISTS_SUBLINK;
                    n->subLinkId = 0;
                    n->testexpr = NULL;
                    n->operName = NIL;
                    n->subselect = (yyvsp[0].node);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 50365 "gram.c"
    break;

  case 2108: /* c_expr: ARRAY select_with_parens  */
#line 15525 "gram.y"
                                {
                    SubLink    *n = makeNode(SubLink);

                    n->subLinkType = ARRAY_SUBLINK;
                    n->subLinkId = 0;
                    n->testexpr = NULL;
                    n->operName = NIL;
                    n->subselect = (yyvsp[0].node);
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 50381 "gram.c"
    break;

  case 2109: /* c_expr: ARRAY array_expr  */
#line 15537 "gram.y"
                                {
                    A_ArrayExpr *n = castNode(A_ArrayExpr, (yyvsp[0].node));

                    /* point outermost A_ArrayExpr to the ARRAY keyword */
                    n->location = (yylsp[-1]);
                    (yyval.node) = (Node *) n;
                }
#line 50393 "gram.c"
    break;

  case 2110: /* c_expr: explicit_row  */
#line 15545 "gram.y"
                                {
                    RowExpr    *r = makeNode(RowExpr);

                    r->args = (yyvsp[0].list);
                    r->row_typeid = InvalidOid; /* not analyzed yet */
                    r->colnames = NIL;  /* to be filled in during analysis */
                    r->row_format = COERCE_EXPLICIT_CALL; /* abuse */
                    r->location = (yylsp[0]);
                    (yyval.node) = (Node *) r;
                }
#line 50408 "gram.c"
    break;

  case 2111: /* c_expr: implicit_row  */
#line 15556 "gram.y"
                                {
                    RowExpr    *r = makeNode(RowExpr);

                    r->args = (yyvsp[0].list);
                    r->row_typeid = InvalidOid; /* not analyzed yet */
                    r->colnames = NIL;  /* to be filled in during analysis */
                    r->row_format = COERCE_IMPLICIT_CAST; /* abuse */
                    r->location = (yylsp[0]);
                    (yyval.node) = (Node *) r;
                }
#line 50423 "gram.c"
    break;

  case 2112: /* c_expr: GROUPING '(' expr_list ')'  */
#line 15567 "gram.y"
                          {
                  GroupingFunc *g = makeNode(GroupingFunc);

                  g->args = (yyvsp[-1].list);
                  g->location = (yylsp[-3]);
                  (yyval.node) = (Node *) g;
              }
#line 50435 "gram.c"
    break;

  case 2113: /* func_application: func_name '(' ')'  */
#line 15577 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall((yyvsp[-2].list), NIL,
                                               COERCE_EXPLICIT_CALL,
                                               (yylsp[-2]));
                }
#line 50445 "gram.c"
    break;

  case 2114: /* func_application: func_name '(' func_arg_list opt_sort_clause ')'  */
#line 15583 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall((yyvsp[-4].list), (yyvsp[-2].list),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-4]));

                    n->agg_order = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 50458 "gram.c"
    break;

  case 2115: /* func_application: func_name '(' VARIADIC func_arg_expr opt_sort_clause ')'  */
#line 15592 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall((yyvsp[-5].list), list_make1((yyvsp[-2].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-5]));

                    n->func_variadic = true;
                    n->agg_order = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 50472 "gram.c"
    break;

  case 2116: /* func_application: func_name '(' func_arg_list ',' VARIADIC func_arg_expr opt_sort_clause ')'  */
#line 15602 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall((yyvsp[-7].list), lappend((yyvsp[-5].list), (yyvsp[-2].node)),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-7]));

                    n->func_variadic = true;
                    n->agg_order = (yyvsp[-1].list);
                    (yyval.node) = (Node *) n;
                }
#line 50486 "gram.c"
    break;

  case 2117: /* func_application: func_name '(' ALL func_arg_list opt_sort_clause ')'  */
#line 15612 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall((yyvsp[-5].list), (yyvsp[-2].list),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-5]));

                    n->agg_order = (yyvsp[-1].list);
                    /* Ideally we'd mark the FuncCall node to indicate
                     * "must be an aggregate", but there's no provision
                     * for that in FuncCall at the moment.
                     */
                    (yyval.node) = (Node *) n;
                }
#line 50503 "gram.c"
    break;

  case 2118: /* func_application: func_name '(' DISTINCT func_arg_list opt_sort_clause ')'  */
#line 15625 "gram.y"
                                {
                    FuncCall   *n = makeFuncCall((yyvsp[-5].list), (yyvsp[-2].list),
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-5]));

                    n->agg_order = (yyvsp[-1].list);
                    n->agg_distinct = true;
                    (yyval.node) = (Node *) n;
                }
#line 50517 "gram.c"
    break;

  case 2119: /* func_application: func_name '(' '*' ')'  */
#line 15635 "gram.y"
                                {
                    /*
                     * We consider AGGREGATE(*) to invoke a parameterless
                     * aggregate.  This does the right thing for COUNT(*),
                     * and there are no other aggregates in SQL that accept
                     * '*' as parameter.
                     *
                     * The FuncCall node is also marked agg_star = true,
                     * so that later processing can detect what the argument
                     * really was.
                     */
                    FuncCall   *n = makeFuncCall((yyvsp[-3].list), NIL,
                                                 COERCE_EXPLICIT_CALL,
                                                 (yylsp[-3]));

                    n->agg_star = true;
                    (yyval.node) = (Node *) n;
                }
#line 50540 "gram.c"
    break;

  case 2120: /* func_expr: func_application within_group_clause filter_clause over_clause  */
#line 15666 "gram.y"
                                {
                    FuncCall   *n = (FuncCall *) (yyvsp[-3].node);

                    /*
                     * The order clause for WITHIN GROUP and the one for
                     * plain-aggregate ORDER BY share a field, so we have to
                     * check here that at most one is present.  We also check
                     * for DISTINCT and VARIADIC here to give a better error
                     * location.  Other consistency checks are deferred to
                     * parse analysis.
                     */
                    if ((yyvsp[-2].list) != NIL)
                    {
                        if (n->agg_order != NIL)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("cannot use multiple ORDER BY clauses with WITHIN GROUP"),
                                     parser_errposition((yylsp[-2]))));
                        if (n->agg_distinct)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("cannot use DISTINCT with WITHIN GROUP"),
                                     parser_errposition((yylsp[-2]))));
                        if (n->func_variadic)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("cannot use VARIADIC with WITHIN GROUP"),
                                     parser_errposition((yylsp[-2]))));
                        n->agg_order = (yyvsp[-2].list);
                        n->agg_within_group = true;
                    }
                    n->agg_filter = (yyvsp[-1].node);
                    n->over = (yyvsp[0].windef);
                    (yyval.node) = (Node *) n;
                }
#line 50580 "gram.c"
    break;

  case 2121: /* func_expr: json_aggregate_func filter_clause over_clause  */
#line 15702 "gram.y"
                                {
                    JsonAggConstructor *n = IsA((yyvsp[-2].node), JsonObjectAgg) ?
                        ((JsonObjectAgg *) (yyvsp[-2].node))->constructor :
                        ((JsonArrayAgg *) (yyvsp[-2].node))->constructor;

                    n->agg_filter = (yyvsp[-1].node);
                    n->over = (yyvsp[0].windef);
                    (yyval.node) = (Node *) (yyvsp[-2].node);
                }
#line 50594 "gram.c"
    break;

  case 2122: /* func_expr: func_expr_common_subexpr  */
#line 15712 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 50600 "gram.c"
    break;

  case 2123: /* func_expr_windowless: func_application  */
#line 15722 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 50606 "gram.c"
    break;

  case 2124: /* func_expr_windowless: func_expr_common_subexpr  */
#line 15723 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 50612 "gram.c"
    break;

  case 2125: /* func_expr_windowless: json_aggregate_func  */
#line 15724 "gram.y"
                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 50618 "gram.c"
    break;

  case 2126: /* func_expr_common_subexpr: COLLATION FOR '(' a_expr ')'  */
#line 15732 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("pg_collation_for"),
                                               list_make1((yyvsp[-1].node)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-4]));
                }
#line 50629 "gram.c"
    break;

  case 2127: /* func_expr_common_subexpr: CURRENT_DATE  */
#line 15739 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_DATE, -1, (yylsp[0]));
                }
#line 50637 "gram.c"
    break;

  case 2128: /* func_expr_common_subexpr: CURRENT_TIME  */
#line 15743 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_TIME, -1, (yylsp[0]));
                }
#line 50645 "gram.c"
    break;

  case 2129: /* func_expr_common_subexpr: CURRENT_TIME '(' Iconst ')'  */
#line 15747 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_TIME_N, (yyvsp[-1].ival), (yylsp[-3]));
                }
#line 50653 "gram.c"
    break;

  case 2130: /* func_expr_common_subexpr: CURRENT_TIMESTAMP  */
#line 15751 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_TIMESTAMP, -1, (yylsp[0]));
                }
#line 50661 "gram.c"
    break;

  case 2131: /* func_expr_common_subexpr: CURRENT_TIMESTAMP '(' Iconst ')'  */
#line 15755 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_TIMESTAMP_N, (yyvsp[-1].ival), (yylsp[-3]));
                }
#line 50669 "gram.c"
    break;

  case 2132: /* func_expr_common_subexpr: LOCALTIME  */
#line 15759 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_LOCALTIME, -1, (yylsp[0]));
                }
#line 50677 "gram.c"
    break;

  case 2133: /* func_expr_common_subexpr: LOCALTIME '(' Iconst ')'  */
#line 15763 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_LOCALTIME_N, (yyvsp[-1].ival), (yylsp[-3]));
                }
#line 50685 "gram.c"
    break;

  case 2134: /* func_expr_common_subexpr: LOCALTIMESTAMP  */
#line 15767 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_LOCALTIMESTAMP, -1, (yylsp[0]));
                }
#line 50693 "gram.c"
    break;

  case 2135: /* func_expr_common_subexpr: LOCALTIMESTAMP '(' Iconst ')'  */
#line 15771 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_LOCALTIMESTAMP_N, (yyvsp[-1].ival), (yylsp[-3]));
                }
#line 50701 "gram.c"
    break;

  case 2136: /* func_expr_common_subexpr: CURRENT_ROLE  */
#line 15775 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_ROLE, -1, (yylsp[0]));
                }
#line 50709 "gram.c"
    break;

  case 2137: /* func_expr_common_subexpr: CURRENT_USER  */
#line 15779 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_USER, -1, (yylsp[0]));
                }
#line 50717 "gram.c"
    break;

  case 2138: /* func_expr_common_subexpr: SESSION_USER  */
#line 15783 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_SESSION_USER, -1, (yylsp[0]));
                }
#line 50725 "gram.c"
    break;

  case 2139: /* func_expr_common_subexpr: SYSTEM_USER  */
#line 15787 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("system_user"),
                                               NIL,
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[0]));
                }
#line 50736 "gram.c"
    break;

  case 2140: /* func_expr_common_subexpr: USER  */
#line 15794 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_USER, -1, (yylsp[0]));
                }
#line 50744 "gram.c"
    break;

  case 2141: /* func_expr_common_subexpr: CURRENT_CATALOG  */
#line 15798 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_CATALOG, -1, (yylsp[0]));
                }
#line 50752 "gram.c"
    break;

  case 2142: /* func_expr_common_subexpr: CURRENT_SCHEMA  */
#line 15802 "gram.y"
                                {
                    (yyval.node) = makeSQLValueFunction(SVFOP_CURRENT_SCHEMA, -1, (yylsp[0]));
                }
#line 50760 "gram.c"
    break;

  case 2143: /* func_expr_common_subexpr: CAST '(' a_expr AS Typename ')'  */
#line 15806 "gram.y"
                                { (yyval.node) = makeTypeCast((yyvsp[-3].node), (yyvsp[-1].typnam), (yylsp[-5])); }
#line 50766 "gram.c"
    break;

  case 2144: /* func_expr_common_subexpr: EXTRACT '(' extract_list ')'  */
#line 15808 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("extract"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50777 "gram.c"
    break;

  case 2145: /* func_expr_common_subexpr: NORMALIZE '(' a_expr ')'  */
#line 15815 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("normalize"),
                                               list_make1((yyvsp[-1].node)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50788 "gram.c"
    break;

  case 2146: /* func_expr_common_subexpr: NORMALIZE '(' a_expr ',' unicode_normal_form ')'  */
#line 15822 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("normalize"),
                                               list_make2((yyvsp[-3].node), makeStringConst((yyvsp[-1].str), (yylsp[-1]))),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-5]));
                }
#line 50799 "gram.c"
    break;

  case 2147: /* func_expr_common_subexpr: OVERLAY '(' overlay_list ')'  */
#line 15829 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("overlay"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50810 "gram.c"
    break;

  case 2148: /* func_expr_common_subexpr: OVERLAY '(' func_arg_list_opt ')'  */
#line 15836 "gram.y"
                                {
                    /*
                     * allow functions named overlay() to be called without
                     * special syntax
                     */
                    (yyval.node) = (Node *) makeFuncCall(list_make1(makeString("overlay")),
                                               (yyvsp[-1].list),
                                               COERCE_EXPLICIT_CALL,
                                               (yylsp[-3]));
                }
#line 50825 "gram.c"
    break;

  case 2149: /* func_expr_common_subexpr: POSITION '(' position_list ')'  */
#line 15847 "gram.y"
                                {
                    /*
                     * position(A in B) is converted to position(B, A)
                     *
                     * We deliberately don't offer a "plain syntax" option
                     * for position(), because the reversal of the arguments
                     * creates too much risk of confusion.
                     */
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("position"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50843 "gram.c"
    break;

  case 2150: /* func_expr_common_subexpr: SUBSTRING '(' substr_list ')'  */
#line 15861 "gram.y"
                                {
                    /* substring(A from B for C) is converted to
                     * substring(A, B, C) - thomas 2000-11-28
                     */
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("substring"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50857 "gram.c"
    break;

  case 2151: /* func_expr_common_subexpr: SUBSTRING '(' func_arg_list_opt ')'  */
#line 15871 "gram.y"
                                {
                    /*
                     * allow functions named substring() to be called without
                     * special syntax
                     */
                    (yyval.node) = (Node *) makeFuncCall(list_make1(makeString("substring")),
                                               (yyvsp[-1].list),
                                               COERCE_EXPLICIT_CALL,
                                               (yylsp[-3]));
                }
#line 50872 "gram.c"
    break;

  case 2152: /* func_expr_common_subexpr: TREAT '(' a_expr AS Typename ')'  */
#line 15882 "gram.y"
                                {
                    /* TREAT(expr AS target) converts expr of a particular type to target,
                     * which is defined to be a subtype of the original expression.
                     * In SQL99, this is intended for use with structured UDTs,
                     * but let's make this a generally useful form allowing stronger
                     * coercions than are handled by implicit casting.
                     *
                     * Convert SystemTypeName() to SystemFuncName() even though
                     * at the moment they result in the same thing.
                     */
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName(strVal(llast((yyvsp[-1].typnam)->names))),
                                               list_make1((yyvsp[-3].node)),
                                               COERCE_EXPLICIT_CALL,
                                               (yylsp[-5]));
                }
#line 50892 "gram.c"
    break;

  case 2153: /* func_expr_common_subexpr: TRIM '(' BOTH trim_list ')'  */
#line 15898 "gram.y"
                                {
                    /* various trim expressions are defined in SQL
                     * - thomas 1997-07-19
                     */
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("btrim"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-4]));
                }
#line 50906 "gram.c"
    break;

  case 2154: /* func_expr_common_subexpr: TRIM '(' LEADING trim_list ')'  */
#line 15908 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("ltrim"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-4]));
                }
#line 50917 "gram.c"
    break;

  case 2155: /* func_expr_common_subexpr: TRIM '(' TRAILING trim_list ')'  */
#line 15915 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("rtrim"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-4]));
                }
#line 50928 "gram.c"
    break;

  case 2156: /* func_expr_common_subexpr: TRIM '(' trim_list ')'  */
#line 15922 "gram.y"
                                {
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("btrim"),
                                               (yyvsp[-1].list),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-3]));
                }
#line 50939 "gram.c"
    break;

  case 2157: /* func_expr_common_subexpr: NULLIF '(' a_expr ',' a_expr ')'  */
#line 15929 "gram.y"
                                {
                    (yyval.node) = (Node *) makeSimpleA_Expr(AEXPR_NULLIF, "=", (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]));
                }
#line 50947 "gram.c"
    break;

  case 2158: /* func_expr_common_subexpr: COALESCE '(' expr_list ')'  */
#line 15933 "gram.y"
                                {
                    CoalesceExpr *c = makeNode(CoalesceExpr);

                    c->args = (yyvsp[-1].list);
                    c->location = (yylsp[-3]);
                    (yyval.node) = (Node *) c;
                }
#line 50959 "gram.c"
    break;

  case 2159: /* func_expr_common_subexpr: GREATEST '(' expr_list ')'  */
#line 15941 "gram.y"
                                {
                    MinMaxExpr *v = makeNode(MinMaxExpr);

                    v->args = (yyvsp[-1].list);
                    v->op = IS_GREATEST;
                    v->location = (yylsp[-3]);
                    (yyval.node) = (Node *) v;
                }
#line 50972 "gram.c"
    break;

  case 2160: /* func_expr_common_subexpr: LEAST '(' expr_list ')'  */
#line 15950 "gram.y"
                                {
                    MinMaxExpr *v = makeNode(MinMaxExpr);

                    v->args = (yyvsp[-1].list);
                    v->op = IS_LEAST;
                    v->location = (yylsp[-3]);
                    (yyval.node) = (Node *) v;
                }
#line 50985 "gram.c"
    break;

  case 2161: /* func_expr_common_subexpr: XMLCONCAT '(' expr_list ')'  */
#line 15959 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLCONCAT, NULL, NIL, (yyvsp[-1].list), (yylsp[-3]));
                }
#line 50993 "gram.c"
    break;

  case 2162: /* func_expr_common_subexpr: XMLELEMENT '(' NAME_P ColLabel ')'  */
#line 15963 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLELEMENT, (yyvsp[-1].str), NIL, NIL, (yylsp[-4]));
                }
#line 51001 "gram.c"
    break;

  case 2163: /* func_expr_common_subexpr: XMLELEMENT '(' NAME_P ColLabel ',' xml_attributes ')'  */
#line 15967 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLELEMENT, (yyvsp[-3].str), (yyvsp[-1].list), NIL, (yylsp[-6]));
                }
#line 51009 "gram.c"
    break;

  case 2164: /* func_expr_common_subexpr: XMLELEMENT '(' NAME_P ColLabel ',' expr_list ')'  */
#line 15971 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLELEMENT, (yyvsp[-3].str), NIL, (yyvsp[-1].list), (yylsp[-6]));
                }
#line 51017 "gram.c"
    break;

  case 2165: /* func_expr_common_subexpr: XMLELEMENT '(' NAME_P ColLabel ',' xml_attributes ',' expr_list ')'  */
#line 15975 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLELEMENT, (yyvsp[-5].str), (yyvsp[-3].list), (yyvsp[-1].list), (yylsp[-8]));
                }
#line 51025 "gram.c"
    break;

  case 2166: /* func_expr_common_subexpr: XMLEXISTS '(' c_expr xmlexists_argument ')'  */
#line 15979 "gram.y"
                                {
                    /* xmlexists(A PASSING [BY REF] B [BY REF]) is
                     * converted to xmlexists(A, B)*/
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("xmlexists"),
                                               list_make2((yyvsp[-2].node), (yyvsp[-1].node)),
                                               COERCE_SQL_SYNTAX,
                                               (yylsp[-4]));
                }
#line 51038 "gram.c"
    break;

  case 2167: /* func_expr_common_subexpr: XMLFOREST '(' xml_attribute_list ')'  */
#line 15988 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLFOREST, NULL, (yyvsp[-1].list), NIL, (yylsp[-3]));
                }
#line 51046 "gram.c"
    break;

  case 2168: /* func_expr_common_subexpr: XMLPARSE '(' document_or_content a_expr xml_whitespace_option ')'  */
#line 15992 "gram.y"
                                {
                    XmlExpr *x = (XmlExpr *)
                        makeXmlExpr(IS_XMLPARSE, NULL, NIL,
                                    list_make2((yyvsp[-2].node), makeBoolAConst((yyvsp[-1].boolean), -1)),
                                    (yylsp[-5]));

                    x->xmloption = (yyvsp[-3].ival);
                    (yyval.node) = (Node *) x;
                }
#line 51060 "gram.c"
    break;

  case 2169: /* func_expr_common_subexpr: XMLPI '(' NAME_P ColLabel ')'  */
#line 16002 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLPI, (yyvsp[-1].str), NULL, NIL, (yylsp[-4]));
                }
#line 51068 "gram.c"
    break;

  case 2170: /* func_expr_common_subexpr: XMLPI '(' NAME_P ColLabel ',' a_expr ')'  */
#line 16006 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLPI, (yyvsp[-3].str), NULL, list_make1((yyvsp[-1].node)), (yylsp[-6]));
                }
#line 51076 "gram.c"
    break;

  case 2171: /* func_expr_common_subexpr: XMLROOT '(' a_expr ',' xml_root_version opt_xml_root_standalone ')'  */
#line 16010 "gram.y"
                                {
                    (yyval.node) = makeXmlExpr(IS_XMLROOT, NULL, NIL,
                                     list_make3((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[-1].node)), (yylsp[-6]));
                }
#line 51085 "gram.c"
    break;

  case 2172: /* func_expr_common_subexpr: XMLSERIALIZE '(' document_or_content a_expr AS SimpleTypename xml_indent_option ')'  */
#line 16015 "gram.y"
                                {
                    XmlSerialize *n = makeNode(XmlSerialize);

                    n->xmloption = (yyvsp[-5].ival);
                    n->expr = (yyvsp[-4].node);
                    n->typeName = (yyvsp[-2].typnam);
                    n->indent = (yyvsp[-1].boolean);
                    n->location = (yylsp[-7]);
                    (yyval.node) = (Node *) n;
                }
#line 51100 "gram.c"
    break;

  case 2173: /* func_expr_common_subexpr: JSON_OBJECT '(' func_arg_list ')'  */
#line 16026 "gram.y"
                                {
                    /* Support for legacy (non-standard) json_object() */
                    (yyval.node) = (Node *) makeFuncCall(SystemFuncName("json_object"),
                                               (yyvsp[-1].list), COERCE_EXPLICIT_CALL, (yylsp[-3]));
                }
#line 51110 "gram.c"
    break;

  case 2174: /* func_expr_common_subexpr: JSON_OBJECT '(' json_name_and_value_list json_object_constructor_null_clause_opt json_key_uniqueness_constraint_opt json_returning_clause_opt ')'  */
#line 16035 "gram.y"
                                {
                    JsonObjectConstructor *n = makeNode(JsonObjectConstructor);

                    n->exprs = (yyvsp[-4].list);
                    n->absent_on_null = (yyvsp[-3].boolean);
                    n->unique = (yyvsp[-2].boolean);
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 51125 "gram.c"
    break;

  case 2175: /* func_expr_common_subexpr: JSON_OBJECT '(' json_returning_clause_opt ')'  */
#line 16046 "gram.y"
                                {
                    JsonObjectConstructor *n = makeNode(JsonObjectConstructor);

                    n->exprs = NULL;
                    n->absent_on_null = false;
                    n->unique = false;
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-3]);
                    (yyval.node) = (Node *) n;
                }
#line 51140 "gram.c"
    break;

  case 2176: /* func_expr_common_subexpr: JSON_ARRAY '(' json_value_expr_list json_array_constructor_null_clause_opt json_returning_clause_opt ')'  */
#line 16061 "gram.y"
                                {
                    JsonArrayConstructor *n = makeNode(JsonArrayConstructor);

                    n->exprs = (yyvsp[-3].list);
                    n->absent_on_null = (yyvsp[-2].boolean);
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 51154 "gram.c"
    break;

  case 2177: /* func_expr_common_subexpr: JSON_ARRAY '(' select_no_parens json_format_clause_opt json_returning_clause_opt ')'  */
#line 16076 "gram.y"
                                {
                    JsonArrayQueryConstructor *n = makeNode(JsonArrayQueryConstructor);

                    n->query = (yyvsp[-3].node);
                    n->format = (JsonFormat *) (yyvsp[-2].node);
                    n->absent_on_null = true;   /* XXX */
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-5]);
                    (yyval.node) = (Node *) n;
                }
#line 51169 "gram.c"
    break;

  case 2178: /* func_expr_common_subexpr: JSON_ARRAY '(' json_returning_clause_opt ')'  */
#line 16089 "gram.y"
                                {
                    JsonArrayConstructor *n = makeNode(JsonArrayConstructor);

                    n->exprs = NIL;
                    n->absent_on_null = true;
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-3]);
                    (yyval.node) = (Node *) n;
                }
#line 51183 "gram.c"
    break;

  case 2179: /* func_expr_common_subexpr: JSON '(' json_value_expr json_key_uniqueness_constraint_opt ')'  */
#line 16099 "gram.y"
                                {
                    JsonParseExpr *n = makeNode(JsonParseExpr);

                    n->expr = (JsonValueExpr *) (yyvsp[-2].node);
                    n->unique_keys = (yyvsp[-1].boolean);
                    n->output = NULL;
                    n->location = (yylsp[-4]);
                    (yyval.node) = (Node *) n;
                }
#line 51197 "gram.c"
    break;

  case 2180: /* func_expr_common_subexpr: JSON_SCALAR '(' a_expr ')'  */
#line 16109 "gram.y"
                                {
                    JsonScalarExpr *n = makeNode(JsonScalarExpr);

                    n->expr = (Expr *) (yyvsp[-1].node);
                    n->output = NULL;
                    n->location = (yylsp[-3]);
                    (yyval.node) = (Node *) n;
                }
#line 51210 "gram.c"
    break;

  case 2181: /* func_expr_common_subexpr: JSON_SERIALIZE '(' json_value_expr json_returning_clause_opt ')'  */
#line 16118 "gram.y"
                                {
                    JsonSerializeExpr *n = makeNode(JsonSerializeExpr);

                    n->expr = (JsonValueExpr *) (yyvsp[-2].node);
                    n->output = (JsonOutput *) (yyvsp[-1].node);
                    n->location = (yylsp[-4]);
                    (yyval.node) = (Node *) n;
                }
#line 51223 "gram.c"
    break;

  case 2182: /* func_expr_common_subexpr: MERGE_ACTION '(' ')'  */
#line 16127 "gram.y"
                                {
                    MergeSupportFunc *m = makeNode(MergeSupportFunc);

                    m->msftype = TEXTOID;
                    m->location = (yylsp[-2]);
                    (yyval.node) = (Node *) m;
                }
#line 51235 "gram.c"
    break;

  case 2183: /* func_expr_common_subexpr: JSON_QUERY '(' json_value_expr ',' a_expr json_passing_clause_opt json_returning_clause_opt json_wrapper_behavior json_quotes_clause_opt json_behavior_clause_opt ')'  */
#line 16141 "gram.y"
                                {
                    JsonFuncExpr *n = makeNode(JsonFuncExpr);

                    n->op = JSON_QUERY_OP;
                    n->context_item = (JsonValueExpr *) (yyvsp[-8].node);
                    n->pathspec = (yyvsp[-6].node);
                    n->passing = (yyvsp[-5].list);
                    n->output = (JsonOutput *) (yyvsp[-4].node);
                    n->wrapper = (yyvsp[-3].ival);
                    n->quotes = (yyvsp[-2].ival);
                    n->on_empty = (JsonBehavior *) linitial((yyvsp[-1].list));
                    n->on_error = (JsonBehavior *) lsecond((yyvsp[-1].list));
                    n->location = (yylsp[-10]);
                    (yyval.node) = (Node *) n;
                }
#line 51255 "gram.c"
    break;

  case 2184: /* func_expr_common_subexpr: JSON_EXISTS '(' json_value_expr ',' a_expr json_passing_clause_opt json_on_error_clause_opt ')'  */
#line 16160 "gram.y"
                                {
                    JsonFuncExpr *n = makeNode(JsonFuncExpr);

                    n->op = JSON_EXISTS_OP;
                    n->context_item = (JsonValueExpr *) (yyvsp[-5].node);
                    n->pathspec = (yyvsp[-3].node);
                    n->passing = (yyvsp[-2].list);
                    n->output = NULL;
                    n->on_error = (JsonBehavior *) (yyvsp[-1].node);
                    n->location = (yylsp[-7]);
                    (yyval.node) = (Node *) n;
                }
#line 51272 "gram.c"
    break;

  case 2185: /* func_expr_common_subexpr: JSON_VALUE '(' json_value_expr ',' a_expr json_passing_clause_opt json_returning_clause_opt json_behavior_clause_opt ')'  */
#line 16177 "gram.y"
                                {
                    JsonFuncExpr *n = makeNode(JsonFuncExpr);

                    n->op = JSON_VALUE_OP;
                    n->context_item = (JsonValueExpr *) (yyvsp[-6].node);
                    n->pathspec = (yyvsp[-4].node);
                    n->passing = (yyvsp[-3].list);
                    n->output = (JsonOutput *) (yyvsp[-2].node);
                    n->on_empty = (JsonBehavior *) linitial((yyvsp[-1].list));
                    n->on_error = (JsonBehavior *) lsecond((yyvsp[-1].list));
                    n->location = (yylsp[-8]);
                    (yyval.node) = (Node *) n;
                }
#line 51290 "gram.c"
    break;

  case 2186: /* xml_root_version: VERSION_P a_expr  */
#line 16197 "gram.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 51296 "gram.c"
    break;

  case 2187: /* xml_root_version: VERSION_P NO VALUE_P  */
#line 16199 "gram.y"
                                { (yyval.node) = makeNullAConst(-1); }
#line 51302 "gram.c"
    break;

  case 2188: /* opt_xml_root_standalone: ',' STANDALONE_P YES_P  */
#line 16203 "gram.y"
                                { (yyval.node) = makeIntConst(XML_STANDALONE_YES, -1); }
#line 51308 "gram.c"
    break;

  case 2189: /* opt_xml_root_standalone: ',' STANDALONE_P NO  */
#line 16205 "gram.y"
                                { (yyval.node) = makeIntConst(XML_STANDALONE_NO, -1); }
#line 51314 "gram.c"
    break;

  case 2190: /* opt_xml_root_standalone: ',' STANDALONE_P NO VALUE_P  */
#line 16207 "gram.y"
                                { (yyval.node) = makeIntConst(XML_STANDALONE_NO_VALUE, -1); }
#line 51320 "gram.c"
    break;

  case 2191: /* opt_xml_root_standalone: %empty  */
#line 16209 "gram.y"
                                { (yyval.node) = makeIntConst(XML_STANDALONE_OMITTED, -1); }
#line 51326 "gram.c"
    break;

  case 2192: /* xml_attributes: XMLATTRIBUTES '(' xml_attribute_list ')'  */
#line 16212 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 51332 "gram.c"
    break;

  case 2193: /* xml_attribute_list: xml_attribute_el  */
#line 16215 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].target)); }
#line 51338 "gram.c"
    break;

  case 2194: /* xml_attribute_list: xml_attribute_list ',' xml_attribute_el  */
#line 16216 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].target)); }
#line 51344 "gram.c"
    break;

  case 2195: /* xml_attribute_el: a_expr AS ColLabel  */
#line 16220 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[0].str);
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) (yyvsp[-2].node);
                    (yyval.target)->location = (yylsp[-2]);
                }
#line 51356 "gram.c"
    break;

  case 2196: /* xml_attribute_el: a_expr  */
#line 16228 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = NULL;
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) (yyvsp[0].node);
                    (yyval.target)->location = (yylsp[0]);
                }
#line 51368 "gram.c"
    break;

  case 2197: /* document_or_content: DOCUMENT_P  */
#line 16237 "gram.y"
                                                                        { (yyval.ival) = XMLOPTION_DOCUMENT; }
#line 51374 "gram.c"
    break;

  case 2198: /* document_or_content: CONTENT_P  */
#line 16238 "gram.y"
                                                                                                { (yyval.ival) = XMLOPTION_CONTENT; }
#line 51380 "gram.c"
    break;

  case 2199: /* xml_indent_option: INDENT  */
#line 16241 "gram.y"
                                                                                { (yyval.boolean) = true; }
#line 51386 "gram.c"
    break;

  case 2200: /* xml_indent_option: NO INDENT  */
#line 16242 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 51392 "gram.c"
    break;

  case 2201: /* xml_indent_option: %empty  */
#line 16243 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 51398 "gram.c"
    break;

  case 2202: /* xml_whitespace_option: PRESERVE WHITESPACE_P  */
#line 16246 "gram.y"
                                                        { (yyval.boolean) = true; }
#line 51404 "gram.c"
    break;

  case 2203: /* xml_whitespace_option: STRIP_P WHITESPACE_P  */
#line 16247 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 51410 "gram.c"
    break;

  case 2204: /* xml_whitespace_option: %empty  */
#line 16248 "gram.y"
                                                                                                { (yyval.boolean) = false; }
#line 51416 "gram.c"
    break;

  case 2205: /* xmlexists_argument: PASSING c_expr  */
#line 16254 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 51424 "gram.c"
    break;

  case 2206: /* xmlexists_argument: PASSING c_expr xml_passing_mech  */
#line 16258 "gram.y"
                                {
                    (yyval.node) = (yyvsp[-1].node);
                }
#line 51432 "gram.c"
    break;

  case 2207: /* xmlexists_argument: PASSING xml_passing_mech c_expr  */
#line 16262 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 51440 "gram.c"
    break;

  case 2208: /* xmlexists_argument: PASSING xml_passing_mech c_expr xml_passing_mech  */
#line 16266 "gram.y"
                                {
                    (yyval.node) = (yyvsp[-1].node);
                }
#line 51448 "gram.c"
    break;

  case 2211: /* within_group_clause: WITHIN GROUP_P '(' sort_clause ')'  */
#line 16281 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 51454 "gram.c"
    break;

  case 2212: /* within_group_clause: %empty  */
#line 16282 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 51460 "gram.c"
    break;

  case 2213: /* filter_clause: FILTER '(' WHERE a_expr ')'  */
#line 16286 "gram.y"
                                                                                { (yyval.node) = (yyvsp[-1].node); }
#line 51466 "gram.c"
    break;

  case 2214: /* filter_clause: %empty  */
#line 16287 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 51472 "gram.c"
    break;

  case 2215: /* window_clause: WINDOW window_definition_list  */
#line 16295 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 51478 "gram.c"
    break;

  case 2216: /* window_clause: %empty  */
#line 16296 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 51484 "gram.c"
    break;

  case 2217: /* window_definition_list: window_definition  */
#line 16300 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].windef)); }
#line 51490 "gram.c"
    break;

  case 2218: /* window_definition_list: window_definition_list ',' window_definition  */
#line 16302 "gram.y"
                                                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].windef)); }
#line 51496 "gram.c"
    break;

  case 2219: /* window_definition: ColId AS window_specification  */
#line 16307 "gram.y"
                                {
                    WindowDef  *n = (yyvsp[0].windef);

                    n->name = (yyvsp[-2].str);
                    (yyval.windef) = n;
                }
#line 51507 "gram.c"
    break;

  case 2220: /* over_clause: OVER window_specification  */
#line 16316 "gram.y"
                                { (yyval.windef) = (yyvsp[0].windef); }
#line 51513 "gram.c"
    break;

  case 2221: /* over_clause: OVER ColId  */
#line 16318 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->name = (yyvsp[0].str);
                    n->refname = NULL;
                    n->partitionClause = NIL;
                    n->orderClause = NIL;
                    n->frameOptions = FRAMEOPTION_DEFAULTS;
                    n->startOffset = NULL;
                    n->endOffset = NULL;
                    n->location = (yylsp[0]);
                    (yyval.windef) = n;
                }
#line 51531 "gram.c"
    break;

  case 2222: /* over_clause: %empty  */
#line 16332 "gram.y"
                                { (yyval.windef) = NULL; }
#line 51537 "gram.c"
    break;

  case 2223: /* window_specification: '(' opt_existing_window_name opt_partition_clause opt_sort_clause opt_frame_clause ')'  */
#line 16337 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->name = NULL;
                    n->refname = (yyvsp[-4].str);
                    n->partitionClause = (yyvsp[-3].list);
                    n->orderClause = (yyvsp[-2].list);
                    /* copy relevant fields of opt_frame_clause */
                    n->frameOptions = (yyvsp[-1].windef)->frameOptions;
                    n->startOffset = (yyvsp[-1].windef)->startOffset;
                    n->endOffset = (yyvsp[-1].windef)->endOffset;
                    n->location = (yylsp[-5]);
                    (yyval.windef) = n;
                }
#line 51556 "gram.c"
    break;

  case 2224: /* opt_existing_window_name: ColId  */
#line 16363 "gram.y"
                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 51562 "gram.c"
    break;

  case 2225: /* opt_existing_window_name: %empty  */
#line 16364 "gram.y"
                                                                                        { (yyval.str) = NULL; }
#line 51568 "gram.c"
    break;

  case 2226: /* opt_partition_clause: PARTITION BY expr_list  */
#line 16367 "gram.y"
                                                        { (yyval.list) = (yyvsp[0].list); }
#line 51574 "gram.c"
    break;

  case 2227: /* opt_partition_clause: %empty  */
#line 16368 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 51580 "gram.c"
    break;

  case 2228: /* opt_frame_clause: RANGE frame_extent opt_window_exclusion_clause  */
#line 16377 "gram.y"
                                {
                    WindowDef  *n = (yyvsp[-1].windef);

                    n->frameOptions |= FRAMEOPTION_NONDEFAULT | FRAMEOPTION_RANGE;
                    n->frameOptions |= (yyvsp[0].ival);
                    (yyval.windef) = n;
                }
#line 51592 "gram.c"
    break;

  case 2229: /* opt_frame_clause: ROWS frame_extent opt_window_exclusion_clause  */
#line 16385 "gram.y"
                                {
                    WindowDef  *n = (yyvsp[-1].windef);

                    n->frameOptions |= FRAMEOPTION_NONDEFAULT | FRAMEOPTION_ROWS;
                    n->frameOptions |= (yyvsp[0].ival);
                    (yyval.windef) = n;
                }
#line 51604 "gram.c"
    break;

  case 2230: /* opt_frame_clause: GROUPS frame_extent opt_window_exclusion_clause  */
#line 16393 "gram.y"
                                {
                    WindowDef  *n = (yyvsp[-1].windef);

                    n->frameOptions |= FRAMEOPTION_NONDEFAULT | FRAMEOPTION_GROUPS;
                    n->frameOptions |= (yyvsp[0].ival);
                    (yyval.windef) = n;
                }
#line 51616 "gram.c"
    break;

  case 2231: /* opt_frame_clause: %empty  */
#line 16401 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_DEFAULTS;
                    n->startOffset = NULL;
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51629 "gram.c"
    break;

  case 2232: /* frame_extent: frame_bound  */
#line 16412 "gram.y"
                                {
                    WindowDef  *n = (yyvsp[0].windef);

                    /* reject invalid cases */
                    if (n->frameOptions & FRAMEOPTION_START_UNBOUNDED_FOLLOWING)
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame start cannot be UNBOUNDED FOLLOWING"),
                                 parser_errposition((yylsp[0]))));
                    if (n->frameOptions & FRAMEOPTION_START_OFFSET_FOLLOWING)
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame starting from following row cannot end with current row"),
                                 parser_errposition((yylsp[0]))));
                    n->frameOptions |= FRAMEOPTION_END_CURRENT_ROW;
                    (yyval.windef) = n;
                }
#line 51651 "gram.c"
    break;

  case 2233: /* frame_extent: BETWEEN frame_bound AND frame_bound  */
#line 16430 "gram.y"
                                {
                    WindowDef  *n1 = (yyvsp[-2].windef);
                    WindowDef  *n2 = (yyvsp[0].windef);

                    /* form merged options */
                    int     frameOptions = n1->frameOptions;
                    /* shift converts START_ options to END_ options */
                    frameOptions |= n2->frameOptions << 1;
                    frameOptions |= FRAMEOPTION_BETWEEN;
                    /* reject invalid cases */
                    if (frameOptions & FRAMEOPTION_START_UNBOUNDED_FOLLOWING)
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame start cannot be UNBOUNDED FOLLOWING"),
                                 parser_errposition((yylsp[-2]))));
                    if (frameOptions & FRAMEOPTION_END_UNBOUNDED_PRECEDING)
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame end cannot be UNBOUNDED PRECEDING"),
                                 parser_errposition((yylsp[0]))));
                    if ((frameOptions & FRAMEOPTION_START_CURRENT_ROW) &&
                        (frameOptions & FRAMEOPTION_END_OFFSET_PRECEDING))
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame starting from current row cannot have preceding rows"),
                                 parser_errposition((yylsp[0]))));
                    if ((frameOptions & FRAMEOPTION_START_OFFSET_FOLLOWING) &&
                        (frameOptions & (FRAMEOPTION_END_OFFSET_PRECEDING |
                                         FRAMEOPTION_END_CURRENT_ROW)))
                        ereport(ERROR,
                                (errcode(ERRCODE_WINDOWING_ERROR),
                                 errmsg("frame starting from following row cannot have preceding rows"),
                                 parser_errposition((yylsp[0]))));
                    n1->frameOptions = frameOptions;
                    n1->endOffset = n2->startOffset;
                    (yyval.windef) = n1;
                }
#line 51693 "gram.c"
    break;

  case 2234: /* frame_bound: UNBOUNDED PRECEDING  */
#line 16476 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_START_UNBOUNDED_PRECEDING;
                    n->startOffset = NULL;
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51706 "gram.c"
    break;

  case 2235: /* frame_bound: UNBOUNDED FOLLOWING  */
#line 16485 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_START_UNBOUNDED_FOLLOWING;
                    n->startOffset = NULL;
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51719 "gram.c"
    break;

  case 2236: /* frame_bound: CURRENT_P ROW  */
#line 16494 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_START_CURRENT_ROW;
                    n->startOffset = NULL;
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51732 "gram.c"
    break;

  case 2237: /* frame_bound: a_expr PRECEDING  */
#line 16503 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_START_OFFSET_PRECEDING;
                    n->startOffset = (yyvsp[-1].node);
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51745 "gram.c"
    break;

  case 2238: /* frame_bound: a_expr FOLLOWING  */
#line 16512 "gram.y"
                                {
                    WindowDef  *n = makeNode(WindowDef);

                    n->frameOptions = FRAMEOPTION_START_OFFSET_FOLLOWING;
                    n->startOffset = (yyvsp[-1].node);
                    n->endOffset = NULL;
                    (yyval.windef) = n;
                }
#line 51758 "gram.c"
    break;

  case 2239: /* opt_window_exclusion_clause: EXCLUDE CURRENT_P ROW  */
#line 16523 "gram.y"
                                                { (yyval.ival) = FRAMEOPTION_EXCLUDE_CURRENT_ROW; }
#line 51764 "gram.c"
    break;

  case 2240: /* opt_window_exclusion_clause: EXCLUDE GROUP_P  */
#line 16524 "gram.y"
                                                        { (yyval.ival) = FRAMEOPTION_EXCLUDE_GROUP; }
#line 51770 "gram.c"
    break;

  case 2241: /* opt_window_exclusion_clause: EXCLUDE TIES  */
#line 16525 "gram.y"
                                                        { (yyval.ival) = FRAMEOPTION_EXCLUDE_TIES; }
#line 51776 "gram.c"
    break;

  case 2242: /* opt_window_exclusion_clause: EXCLUDE NO OTHERS  */
#line 16526 "gram.y"
                                                        { (yyval.ival) = 0; }
#line 51782 "gram.c"
    break;

  case 2243: /* opt_window_exclusion_clause: %empty  */
#line 16527 "gram.y"
                                                                { (yyval.ival) = 0; }
#line 51788 "gram.c"
    break;

  case 2244: /* row: ROW '(' expr_list ')'  */
#line 16541 "gram.y"
                                                                        { (yyval.list) = (yyvsp[-1].list); }
#line 51794 "gram.c"
    break;

  case 2245: /* row: ROW '(' ')'  */
#line 16542 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 51800 "gram.c"
    break;

  case 2246: /* row: '(' expr_list ',' a_expr ')'  */
#line 16543 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-3].list), (yyvsp[-1].node)); }
#line 51806 "gram.c"
    break;

  case 2247: /* explicit_row: ROW '(' expr_list ')'  */
#line 16546 "gram.y"
                                                                { (yyval.list) = (yyvsp[-1].list); }
#line 51812 "gram.c"
    break;

  case 2248: /* explicit_row: ROW '(' ')'  */
#line 16547 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 51818 "gram.c"
    break;

  case 2249: /* implicit_row: '(' expr_list ',' a_expr ')'  */
#line 16550 "gram.y"
                                                        { (yyval.list) = lappend((yyvsp[-3].list), (yyvsp[-1].node)); }
#line 51824 "gram.c"
    break;

  case 2250: /* sub_type: ANY  */
#line 16553 "gram.y"
                                                                                                { (yyval.ival) = ANY_SUBLINK; }
#line 51830 "gram.c"
    break;

  case 2251: /* sub_type: SOME  */
#line 16554 "gram.y"
                                                                                                { (yyval.ival) = ANY_SUBLINK; }
#line 51836 "gram.c"
    break;

  case 2252: /* sub_type: ALL  */
#line 16555 "gram.y"
                                                                                                { (yyval.ival) = ALL_SUBLINK; }
#line 51842 "gram.c"
    break;

  case 2253: /* all_Op: Op  */
#line 16558 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 51848 "gram.c"
    break;

  case 2254: /* all_Op: MathOp  */
#line 16559 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 51854 "gram.c"
    break;

  case 2255: /* MathOp: '+'  */
#line 16562 "gram.y"
                                                                                        { (yyval.str) = "+"; }
#line 51860 "gram.c"
    break;

  case 2256: /* MathOp: '-'  */
#line 16563 "gram.y"
                                                                                                { (yyval.str) = "-"; }
#line 51866 "gram.c"
    break;

  case 2257: /* MathOp: '*'  */
#line 16564 "gram.y"
                                                                                                { (yyval.str) = "*"; }
#line 51872 "gram.c"
    break;

  case 2258: /* MathOp: '/'  */
#line 16565 "gram.y"
                                                                                                { (yyval.str) = "/"; }
#line 51878 "gram.c"
    break;

  case 2259: /* MathOp: '%'  */
#line 16566 "gram.y"
                                                                                                { (yyval.str) = "%"; }
#line 51884 "gram.c"
    break;

  case 2260: /* MathOp: '^'  */
#line 16567 "gram.y"
                                                                                                { (yyval.str) = "^"; }
#line 51890 "gram.c"
    break;

  case 2261: /* MathOp: '<'  */
#line 16568 "gram.y"
                                                                                                { (yyval.str) = "<"; }
#line 51896 "gram.c"
    break;

  case 2262: /* MathOp: '>'  */
#line 16569 "gram.y"
                                                                                                { (yyval.str) = ">"; }
#line 51902 "gram.c"
    break;

  case 2263: /* MathOp: '='  */
#line 16570 "gram.y"
                                                                                                { (yyval.str) = "="; }
#line 51908 "gram.c"
    break;

  case 2264: /* MathOp: LESS_EQUALS  */
#line 16571 "gram.y"
                                                                                        { (yyval.str) = "<="; }
#line 51914 "gram.c"
    break;

  case 2265: /* MathOp: GREATER_EQUALS  */
#line 16572 "gram.y"
                                                                                        { (yyval.str) = ">="; }
#line 51920 "gram.c"
    break;

  case 2266: /* MathOp: NOT_EQUALS  */
#line 16573 "gram.y"
                                                                                        { (yyval.str) = "<>"; }
#line 51926 "gram.c"
    break;

  case 2267: /* qual_Op: Op  */
#line 16577 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 51932 "gram.c"
    break;

  case 2268: /* qual_Op: OPERATOR '(' any_operator ')'  */
#line 16579 "gram.y"
                                        { (yyval.list) = (yyvsp[-1].list); }
#line 51938 "gram.c"
    break;

  case 2269: /* qual_all_Op: all_Op  */
#line 16584 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 51944 "gram.c"
    break;

  case 2270: /* qual_all_Op: OPERATOR '(' any_operator ')'  */
#line 16586 "gram.y"
                                        { (yyval.list) = (yyvsp[-1].list); }
#line 51950 "gram.c"
    break;

  case 2271: /* subquery_Op: all_Op  */
#line 16591 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 51956 "gram.c"
    break;

  case 2272: /* subquery_Op: OPERATOR '(' any_operator ')'  */
#line 16593 "gram.y"
                                        { (yyval.list) = (yyvsp[-1].list); }
#line 51962 "gram.c"
    break;

  case 2273: /* subquery_Op: LIKE  */
#line 16595 "gram.y"
                                        { (yyval.list) = list_make1(makeString("~~")); }
#line 51968 "gram.c"
    break;

  case 2274: /* subquery_Op: NOT_LA LIKE  */
#line 16597 "gram.y"
                                        { (yyval.list) = list_make1(makeString("!~~")); }
#line 51974 "gram.c"
    break;

  case 2275: /* subquery_Op: ILIKE  */
#line 16599 "gram.y"
                                        { (yyval.list) = list_make1(makeString("~~*")); }
#line 51980 "gram.c"
    break;

  case 2276: /* subquery_Op: NOT_LA ILIKE  */
#line 16601 "gram.y"
                                        { (yyval.list) = list_make1(makeString("!~~*")); }
#line 51986 "gram.c"
    break;

  case 2277: /* expr_list: a_expr  */
#line 16613 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 51994 "gram.c"
    break;

  case 2278: /* expr_list: expr_list ',' a_expr  */
#line 16617 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 52002 "gram.c"
    break;

  case 2279: /* func_arg_list: func_arg_expr  */
#line 16624 "gram.y"
                                {
                    (yyval.list) = list_make1((yyvsp[0].node));
                }
#line 52010 "gram.c"
    break;

  case 2280: /* func_arg_list: func_arg_list ',' func_arg_expr  */
#line 16628 "gram.y"
                                {
                    (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));
                }
#line 52018 "gram.c"
    break;

  case 2281: /* func_arg_expr: a_expr  */
#line 16634 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 52026 "gram.c"
    break;

  case 2282: /* func_arg_expr: param_name COLON_EQUALS a_expr  */
#line 16638 "gram.y"
                                {
                    NamedArgExpr *na = makeNode(NamedArgExpr);

                    na->name = (yyvsp[-2].str);
                    na->arg = (Expr *) (yyvsp[0].node);
                    na->argnumber = -1;     /* until determined */
                    na->location = (yylsp[-2]);
                    (yyval.node) = (Node *) na;
                }
#line 52040 "gram.c"
    break;

  case 2283: /* func_arg_expr: param_name EQUALS_GREATER a_expr  */
#line 16648 "gram.y"
                                {
                    NamedArgExpr *na = makeNode(NamedArgExpr);

                    na->name = (yyvsp[-2].str);
                    na->arg = (Expr *) (yyvsp[0].node);
                    na->argnumber = -1;     /* until determined */
                    na->location = (yylsp[-2]);
                    (yyval.node) = (Node *) na;
                }
#line 52054 "gram.c"
    break;

  case 2284: /* func_arg_list_opt: func_arg_list  */
#line 16659 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 52060 "gram.c"
    break;

  case 2285: /* func_arg_list_opt: %empty  */
#line 16660 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 52066 "gram.c"
    break;

  case 2286: /* type_list: Typename  */
#line 16663 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].typnam)); }
#line 52072 "gram.c"
    break;

  case 2287: /* type_list: type_list ',' Typename  */
#line 16664 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].typnam)); }
#line 52078 "gram.c"
    break;

  case 2288: /* array_expr: '[' expr_list ']'  */
#line 16668 "gram.y"
                                {
                    (yyval.node) = makeAArrayExpr((yyvsp[-1].list), (yylsp[-2]));
                }
#line 52086 "gram.c"
    break;

  case 2289: /* array_expr: '[' array_expr_list ']'  */
#line 16672 "gram.y"
                                {
                    (yyval.node) = makeAArrayExpr((yyvsp[-1].list), (yylsp[-2]));
                }
#line 52094 "gram.c"
    break;

  case 2290: /* array_expr: '[' ']'  */
#line 16676 "gram.y"
                                {
                    (yyval.node) = makeAArrayExpr(NIL, (yylsp[-1]));
                }
#line 52102 "gram.c"
    break;

  case 2291: /* array_expr_list: array_expr  */
#line 16681 "gram.y"
                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52108 "gram.c"
    break;

  case 2292: /* array_expr_list: array_expr_list ',' array_expr  */
#line 16682 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 52114 "gram.c"
    break;

  case 2293: /* extract_list: extract_arg FROM a_expr  */
#line 16688 "gram.y"
                                {
                    (yyval.list) = list_make2(makeStringConst((yyvsp[-2].str), (yylsp[-2])), (yyvsp[0].node));
                }
#line 52122 "gram.c"
    break;

  case 2294: /* extract_arg: IDENT  */
#line 16697 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 52128 "gram.c"
    break;

  case 2295: /* extract_arg: YEAR_P  */
#line 16698 "gram.y"
                                                                                                { (yyval.str) = "year"; }
#line 52134 "gram.c"
    break;

  case 2296: /* extract_arg: MONTH_P  */
#line 16699 "gram.y"
                                                                                                { (yyval.str) = "month"; }
#line 52140 "gram.c"
    break;

  case 2297: /* extract_arg: DAY_P  */
#line 16700 "gram.y"
                                                                                                { (yyval.str) = "day"; }
#line 52146 "gram.c"
    break;

  case 2298: /* extract_arg: HOUR_P  */
#line 16701 "gram.y"
                                                                                                { (yyval.str) = "hour"; }
#line 52152 "gram.c"
    break;

  case 2299: /* extract_arg: MINUTE_P  */
#line 16702 "gram.y"
                                                                                                { (yyval.str) = "minute"; }
#line 52158 "gram.c"
    break;

  case 2300: /* extract_arg: SECOND_P  */
#line 16703 "gram.y"
                                                                                                { (yyval.str) = "second"; }
#line 52164 "gram.c"
    break;

  case 2301: /* extract_arg: Sconst  */
#line 16704 "gram.y"
                                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 52170 "gram.c"
    break;

  case 2302: /* unicode_normal_form: NFC  */
#line 16708 "gram.y"
                                                                                                        { (yyval.str) = "NFC"; }
#line 52176 "gram.c"
    break;

  case 2303: /* unicode_normal_form: NFD  */
#line 16709 "gram.y"
                                                                                                { (yyval.str) = "NFD"; }
#line 52182 "gram.c"
    break;

  case 2304: /* unicode_normal_form: NFKC  */
#line 16710 "gram.y"
                                                                                                { (yyval.str) = "NFKC"; }
#line 52188 "gram.c"
    break;

  case 2305: /* unicode_normal_form: NFKD  */
#line 16711 "gram.y"
                                                                                                { (yyval.str) = "NFKD"; }
#line 52194 "gram.c"
    break;

  case 2306: /* overlay_list: a_expr PLACING a_expr FROM a_expr FOR a_expr  */
#line 16717 "gram.y"
                                {
                    /* overlay(A PLACING B FROM C FOR D) is converted to overlay(A, B, C, D) */
                    (yyval.list) = list_make4((yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
                }
#line 52203 "gram.c"
    break;

  case 2307: /* overlay_list: a_expr PLACING a_expr FROM a_expr  */
#line 16722 "gram.y"
                                {
                    /* overlay(A PLACING B FROM C) is converted to overlay(A, B, C) */
                    (yyval.list) = list_make3((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
                }
#line 52212 "gram.c"
    break;

  case 2308: /* position_list: b_expr IN_P b_expr  */
#line 16730 "gram.y"
                                                                                        { (yyval.list) = list_make2((yyvsp[0].node), (yyvsp[-2].node)); }
#line 52218 "gram.c"
    break;

  case 2309: /* substr_list: a_expr FROM a_expr FOR a_expr  */
#line 16752 "gram.y"
                                {
                    (yyval.list) = list_make3((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
                }
#line 52226 "gram.c"
    break;

  case 2310: /* substr_list: a_expr FOR a_expr FROM a_expr  */
#line 16756 "gram.y"
                                {
                    /* not legal per SQL, but might as well allow it */
                    (yyval.list) = list_make3((yyvsp[-4].node), (yyvsp[0].node), (yyvsp[-2].node));
                }
#line 52235 "gram.c"
    break;

  case 2311: /* substr_list: a_expr FROM a_expr  */
#line 16761 "gram.y"
                                {
                    /*
                     * Because we aren't restricting data types here, this
                     * syntax can end up resolving to textregexsubstr().
                     * We've historically allowed that to happen, so continue
                     * to accept it.  However, ruleutils.c will reverse-list
                     * such a call in regular function call syntax.
                     */
                    (yyval.list) = list_make2((yyvsp[-2].node), (yyvsp[0].node));
                }
#line 52250 "gram.c"
    break;

  case 2312: /* substr_list: a_expr FOR a_expr  */
#line 16772 "gram.y"
                                {
                    /* not legal per SQL */

                    /*
                     * Since there are no cases where this syntax allows
                     * a textual FOR value, we forcibly cast the argument
                     * to int4.  The possible matches in pg_proc are
                     * substring(text,int4) and substring(text,text),
                     * and we don't want the parser to choose the latter,
                     * which it is likely to do if the second argument
                     * is unknown or doesn't have an implicit cast to int4.
                     */
                    (yyval.list) = list_make3((yyvsp[-2].node), makeIntConst(1, -1),
                                    makeTypeCast((yyvsp[0].node),
                                                 SystemTypeName("int4"), -1));
                }
#line 52271 "gram.c"
    break;

  case 2313: /* substr_list: a_expr SIMILAR a_expr ESCAPE a_expr  */
#line 16789 "gram.y"
                                {
                    (yyval.list) = list_make3((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
                }
#line 52279 "gram.c"
    break;

  case 2314: /* trim_list: a_expr FROM expr_list  */
#line 16794 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[0].list), (yyvsp[-2].node)); }
#line 52285 "gram.c"
    break;

  case 2315: /* trim_list: FROM expr_list  */
#line 16795 "gram.y"
                                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 52291 "gram.c"
    break;

  case 2316: /* trim_list: expr_list  */
#line 16796 "gram.y"
                                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 52297 "gram.c"
    break;

  case 2317: /* in_expr: select_with_parens  */
#line 16800 "gram.y"
                                {
                    SubLink    *n = makeNode(SubLink);

                    n->subselect = (yyvsp[0].node);
                    /* other fields will be filled later */
                    (yyval.node) = (Node *) n;
                }
#line 52309 "gram.c"
    break;

  case 2318: /* in_expr: '(' expr_list ')'  */
#line 16807 "gram.y"
                                                                                        { (yyval.node) = (Node *) (yyvsp[-1].list); }
#line 52315 "gram.c"
    break;

  case 2319: /* case_expr: CASE case_arg when_clause_list case_default END_P  */
#line 16818 "gram.y"
                                {
                    CaseExpr   *c = makeNode(CaseExpr);

                    c->casetype = InvalidOid; /* not analyzed yet */
                    c->arg = (Expr *) (yyvsp[-3].node);
                    c->args = (yyvsp[-2].list);
                    c->defresult = (Expr *) (yyvsp[-1].node);
                    c->location = (yylsp[-4]);
                    (yyval.node) = (Node *) c;
                }
#line 52330 "gram.c"
    break;

  case 2320: /* when_clause_list: when_clause  */
#line 16832 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52336 "gram.c"
    break;

  case 2321: /* when_clause_list: when_clause_list when_clause  */
#line 16833 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 52342 "gram.c"
    break;

  case 2322: /* when_clause: WHEN a_expr THEN a_expr  */
#line 16838 "gram.y"
                                {
                    CaseWhen   *w = makeNode(CaseWhen);

                    w->expr = (Expr *) (yyvsp[-2].node);
                    w->result = (Expr *) (yyvsp[0].node);
                    w->location = (yylsp[-3]);
                    (yyval.node) = (Node *) w;
                }
#line 52355 "gram.c"
    break;

  case 2323: /* case_default: ELSE a_expr  */
#line 16849 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 52361 "gram.c"
    break;

  case 2324: /* case_default: %empty  */
#line 16850 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 52367 "gram.c"
    break;

  case 2325: /* case_arg: a_expr  */
#line 16853 "gram.y"
                                                                                        { (yyval.node) = (yyvsp[0].node); }
#line 52373 "gram.c"
    break;

  case 2326: /* case_arg: %empty  */
#line 16854 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 52379 "gram.c"
    break;

  case 2327: /* columnref: ColId  */
#line 16858 "gram.y"
                                {
                    (yyval.node) = makeColumnRef((yyvsp[0].str), NIL, (yylsp[0]), yyscanner);
                }
#line 52387 "gram.c"
    break;

  case 2328: /* columnref: ColId indirection  */
#line 16862 "gram.y"
                                {
                    (yyval.node) = makeColumnRef((yyvsp[-1].str), (yyvsp[0].list), (yylsp[-1]), yyscanner);
                }
#line 52395 "gram.c"
    break;

  case 2329: /* indirection_el: '.' attr_name  */
#line 16869 "gram.y"
                                {
                    (yyval.node) = (Node *) makeString((yyvsp[0].str));
                }
#line 52403 "gram.c"
    break;

  case 2330: /* indirection_el: '.' '*'  */
#line 16873 "gram.y"
                                {
                    (yyval.node) = (Node *) makeNode(A_Star);
                }
#line 52411 "gram.c"
    break;

  case 2331: /* indirection_el: '[' a_expr ']'  */
#line 16877 "gram.y"
                                {
                    A_Indices *ai = makeNode(A_Indices);

                    ai->is_slice = false;
                    ai->lidx = NULL;
                    ai->uidx = (yyvsp[-1].node);
                    (yyval.node) = (Node *) ai;
                }
#line 52424 "gram.c"
    break;

  case 2332: /* indirection_el: '[' opt_slice_bound ':' opt_slice_bound ']'  */
#line 16886 "gram.y"
                                {
                    A_Indices *ai = makeNode(A_Indices);

                    ai->is_slice = true;
                    ai->lidx = (yyvsp[-3].node);
                    ai->uidx = (yyvsp[-1].node);
                    (yyval.node) = (Node *) ai;
                }
#line 52437 "gram.c"
    break;

  case 2333: /* opt_slice_bound: a_expr  */
#line 16897 "gram.y"
                                                                                                { (yyval.node) = (yyvsp[0].node); }
#line 52443 "gram.c"
    break;

  case 2334: /* opt_slice_bound: %empty  */
#line 16898 "gram.y"
                                                                                                { (yyval.node) = NULL; }
#line 52449 "gram.c"
    break;

  case 2335: /* indirection: indirection_el  */
#line 16902 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52455 "gram.c"
    break;

  case 2336: /* indirection: indirection indirection_el  */
#line 16903 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 52461 "gram.c"
    break;

  case 2337: /* opt_indirection: %empty  */
#line 16907 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 52467 "gram.c"
    break;

  case 2338: /* opt_indirection: opt_indirection indirection_el  */
#line 16908 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].node)); }
#line 52473 "gram.c"
    break;

  case 2341: /* json_passing_clause_opt: PASSING json_arguments  */
#line 16917 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 52479 "gram.c"
    break;

  case 2342: /* json_passing_clause_opt: %empty  */
#line 16918 "gram.y"
                                                                                                { (yyval.list) = NIL; }
#line 52485 "gram.c"
    break;

  case 2343: /* json_arguments: json_argument  */
#line 16922 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52491 "gram.c"
    break;

  case 2344: /* json_arguments: json_arguments ',' json_argument  */
#line 16923 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 52497 "gram.c"
    break;

  case 2345: /* json_argument: json_value_expr AS ColLabel  */
#line 16928 "gram.y"
                        {
                JsonArgument *n = makeNode(JsonArgument);

                n->val = (JsonValueExpr *) (yyvsp[-2].node);
                n->name = (yyvsp[0].str);
                (yyval.node) = (Node *) n;
            }
#line 52509 "gram.c"
    break;

  case 2346: /* json_wrapper_behavior: WITHOUT WRAPPER  */
#line 16939 "gram.y"
                                                                                { (yyval.ival) = JSW_NONE; }
#line 52515 "gram.c"
    break;

  case 2347: /* json_wrapper_behavior: WITHOUT ARRAY WRAPPER  */
#line 16940 "gram.y"
                                                                        { (yyval.ival) = JSW_NONE; }
#line 52521 "gram.c"
    break;

  case 2348: /* json_wrapper_behavior: WITH WRAPPER  */
#line 16941 "gram.y"
                                                                                { (yyval.ival) = JSW_UNCONDITIONAL; }
#line 52527 "gram.c"
    break;

  case 2349: /* json_wrapper_behavior: WITH ARRAY WRAPPER  */
#line 16942 "gram.y"
                                                                        { (yyval.ival) = JSW_UNCONDITIONAL; }
#line 52533 "gram.c"
    break;

  case 2350: /* json_wrapper_behavior: WITH CONDITIONAL ARRAY WRAPPER  */
#line 16943 "gram.y"
                                                                { (yyval.ival) = JSW_CONDITIONAL; }
#line 52539 "gram.c"
    break;

  case 2351: /* json_wrapper_behavior: WITH UNCONDITIONAL ARRAY WRAPPER  */
#line 16944 "gram.y"
                                                                { (yyval.ival) = JSW_UNCONDITIONAL; }
#line 52545 "gram.c"
    break;

  case 2352: /* json_wrapper_behavior: WITH CONDITIONAL WRAPPER  */
#line 16945 "gram.y"
                                                                        { (yyval.ival) = JSW_CONDITIONAL; }
#line 52551 "gram.c"
    break;

  case 2353: /* json_wrapper_behavior: WITH UNCONDITIONAL WRAPPER  */
#line 16946 "gram.y"
                                                                { (yyval.ival) = JSW_UNCONDITIONAL; }
#line 52557 "gram.c"
    break;

  case 2354: /* json_wrapper_behavior: %empty  */
#line 16947 "gram.y"
                                                                                { (yyval.ival) = JSW_UNSPEC; }
#line 52563 "gram.c"
    break;

  case 2355: /* json_behavior: DEFAULT a_expr  */
#line 16952 "gram.y"
                                { (yyval.node) = (Node *) makeJsonBehavior(JSON_BEHAVIOR_DEFAULT, (yyvsp[0].node), (yylsp[-1])); }
#line 52569 "gram.c"
    break;

  case 2356: /* json_behavior: json_behavior_type  */
#line 16954 "gram.y"
                                { (yyval.node) = (Node *) makeJsonBehavior((yyvsp[0].ival), NULL, (yylsp[0])); }
#line 52575 "gram.c"
    break;

  case 2357: /* json_behavior_type: ERROR_P  */
#line 16958 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_ERROR; }
#line 52581 "gram.c"
    break;

  case 2358: /* json_behavior_type: NULL_P  */
#line 16959 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_NULL; }
#line 52587 "gram.c"
    break;

  case 2359: /* json_behavior_type: TRUE_P  */
#line 16960 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_TRUE; }
#line 52593 "gram.c"
    break;

  case 2360: /* json_behavior_type: FALSE_P  */
#line 16961 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_FALSE; }
#line 52599 "gram.c"
    break;

  case 2361: /* json_behavior_type: UNKNOWN  */
#line 16962 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_UNKNOWN; }
#line 52605 "gram.c"
    break;

  case 2362: /* json_behavior_type: EMPTY_P ARRAY  */
#line 16963 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_EMPTY_ARRAY; }
#line 52611 "gram.c"
    break;

  case 2363: /* json_behavior_type: EMPTY_P OBJECT_P  */
#line 16964 "gram.y"
                                                { (yyval.ival) = JSON_BEHAVIOR_EMPTY_OBJECT; }
#line 52617 "gram.c"
    break;

  case 2364: /* json_behavior_type: EMPTY_P  */
#line 16966 "gram.y"
                                        { (yyval.ival) = JSON_BEHAVIOR_EMPTY_ARRAY; }
#line 52623 "gram.c"
    break;

  case 2365: /* json_behavior_clause_opt: json_behavior ON EMPTY_P  */
#line 16971 "gram.y"
                                { (yyval.list) = list_make2((yyvsp[-2].node), NULL); }
#line 52629 "gram.c"
    break;

  case 2366: /* json_behavior_clause_opt: json_behavior ON ERROR_P  */
#line 16973 "gram.y"
                                { (yyval.list) = list_make2(NULL, (yyvsp[-2].node)); }
#line 52635 "gram.c"
    break;

  case 2367: /* json_behavior_clause_opt: json_behavior ON EMPTY_P json_behavior ON ERROR_P  */
#line 16975 "gram.y"
                                { (yyval.list) = list_make2((yyvsp[-5].node), (yyvsp[-2].node)); }
#line 52641 "gram.c"
    break;

  case 2368: /* json_behavior_clause_opt: %empty  */
#line 16977 "gram.y"
                                { (yyval.list) = list_make2(NULL, NULL); }
#line 52647 "gram.c"
    break;

  case 2369: /* json_on_error_clause_opt: json_behavior ON ERROR_P  */
#line 16982 "gram.y"
                                { (yyval.node) = (yyvsp[-2].node); }
#line 52653 "gram.c"
    break;

  case 2370: /* json_on_error_clause_opt: %empty  */
#line 16984 "gram.y"
                                { (yyval.node) = NULL; }
#line 52659 "gram.c"
    break;

  case 2371: /* json_value_expr: a_expr json_format_clause_opt  */
#line 16989 "gram.y"
                        {
                /* formatted_expr will be set during parse-analysis. */
                (yyval.node) = (Node *) makeJsonValueExpr((Expr *) (yyvsp[-1].node), NULL,
                                                castNode(JsonFormat, (yyvsp[0].node)));
            }
#line 52669 "gram.c"
    break;

  case 2372: /* json_format_clause: FORMAT_LA JSON ENCODING name  */
#line 16998 "gram.y"
                                {
                    int     encoding;

                    if (!pg_strcasecmp((yyvsp[0].str), "utf8"))
                        encoding = JS_ENC_UTF8;
                    else if (!pg_strcasecmp((yyvsp[0].str), "utf16"))
                        encoding = JS_ENC_UTF16;
                    else if (!pg_strcasecmp((yyvsp[0].str), "utf32"))
                        encoding = JS_ENC_UTF32;
                    else
                        ereport(ERROR,
                                errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                errmsg("unrecognized JSON encoding: %s", (yyvsp[0].str)));

                    (yyval.node) = (Node *) makeJsonFormat(JS_FORMAT_JSON, encoding, (yylsp[-3]));
                }
#line 52690 "gram.c"
    break;

  case 2373: /* json_format_clause: FORMAT_LA JSON  */
#line 17015 "gram.y"
                                {
                    (yyval.node) = (Node *) makeJsonFormat(JS_FORMAT_JSON, JS_ENC_DEFAULT, (yylsp[-1]));
                }
#line 52698 "gram.c"
    break;

  case 2374: /* json_format_clause_opt: json_format_clause  */
#line 17022 "gram.y"
                                {
                    (yyval.node) = (yyvsp[0].node);
                }
#line 52706 "gram.c"
    break;

  case 2375: /* json_format_clause_opt: %empty  */
#line 17026 "gram.y"
                                {
                    (yyval.node) = (Node *) makeJsonFormat(JS_FORMAT_DEFAULT, JS_ENC_DEFAULT, -1);
                }
#line 52714 "gram.c"
    break;

  case 2376: /* json_quotes_clause_opt: KEEP QUOTES ON SCALAR STRING_P  */
#line 17032 "gram.y"
                                                                { (yyval.ival) = JS_QUOTES_KEEP; }
#line 52720 "gram.c"
    break;

  case 2377: /* json_quotes_clause_opt: KEEP QUOTES  */
#line 17033 "gram.y"
                                                                                { (yyval.ival) = JS_QUOTES_KEEP; }
#line 52726 "gram.c"
    break;

  case 2378: /* json_quotes_clause_opt: OMIT QUOTES ON SCALAR STRING_P  */
#line 17034 "gram.y"
                                                                { (yyval.ival) = JS_QUOTES_OMIT; }
#line 52732 "gram.c"
    break;

  case 2379: /* json_quotes_clause_opt: OMIT QUOTES  */
#line 17035 "gram.y"
                                                                                { (yyval.ival) = JS_QUOTES_OMIT; }
#line 52738 "gram.c"
    break;

  case 2380: /* json_quotes_clause_opt: %empty  */
#line 17036 "gram.y"
                                                                                { (yyval.ival) = JS_QUOTES_UNSPEC; }
#line 52744 "gram.c"
    break;

  case 2381: /* json_returning_clause_opt: RETURNING Typename json_format_clause_opt  */
#line 17041 "gram.y"
                                {
                    JsonOutput *n = makeNode(JsonOutput);

                    n->typeName = (yyvsp[-1].typnam);
                    n->returning = makeNode(JsonReturning);
                    n->returning->format = (JsonFormat *) (yyvsp[0].node);
                    (yyval.node) = (Node *) n;
                }
#line 52757 "gram.c"
    break;

  case 2382: /* json_returning_clause_opt: %empty  */
#line 17049 "gram.y"
                                                                                        { (yyval.node) = NULL; }
#line 52763 "gram.c"
    break;

  case 2383: /* json_predicate_type_constraint: JSON  */
#line 17063 "gram.y"
                                                                                { (yyval.ival) = JS_TYPE_ANY; }
#line 52769 "gram.c"
    break;

  case 2384: /* json_predicate_type_constraint: JSON VALUE_P  */
#line 17064 "gram.y"
                                                                                        { (yyval.ival) = JS_TYPE_ANY; }
#line 52775 "gram.c"
    break;

  case 2385: /* json_predicate_type_constraint: JSON ARRAY  */
#line 17065 "gram.y"
                                                                                        { (yyval.ival) = JS_TYPE_ARRAY; }
#line 52781 "gram.c"
    break;

  case 2386: /* json_predicate_type_constraint: JSON OBJECT_P  */
#line 17066 "gram.y"
                                                                                        { (yyval.ival) = JS_TYPE_OBJECT; }
#line 52787 "gram.c"
    break;

  case 2387: /* json_predicate_type_constraint: JSON SCALAR  */
#line 17067 "gram.y"
                                                                                        { (yyval.ival) = JS_TYPE_SCALAR; }
#line 52793 "gram.c"
    break;

  case 2388: /* json_key_uniqueness_constraint_opt: WITH UNIQUE KEYS  */
#line 17076 "gram.y"
                                                                                                { (yyval.boolean) = true; }
#line 52799 "gram.c"
    break;

  case 2389: /* json_key_uniqueness_constraint_opt: WITH UNIQUE  */
#line 17077 "gram.y"
                                                                                { (yyval.boolean) = true; }
#line 52805 "gram.c"
    break;

  case 2390: /* json_key_uniqueness_constraint_opt: WITHOUT UNIQUE KEYS  */
#line 17078 "gram.y"
                                                                                        { (yyval.boolean) = false; }
#line 52811 "gram.c"
    break;

  case 2391: /* json_key_uniqueness_constraint_opt: WITHOUT UNIQUE  */
#line 17079 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 52817 "gram.c"
    break;

  case 2392: /* json_key_uniqueness_constraint_opt: %empty  */
#line 17080 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 52823 "gram.c"
    break;

  case 2393: /* json_name_and_value_list: json_name_and_value  */
#line 17085 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52829 "gram.c"
    break;

  case 2394: /* json_name_and_value_list: json_name_and_value_list ',' json_name_and_value  */
#line 17087 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node)); }
#line 52835 "gram.c"
    break;

  case 2395: /* json_name_and_value: c_expr VALUE_P json_value_expr  */
#line 17097 "gram.y"
                                { (yyval.node) = makeJsonKeyValue((yyvsp[-2].node), (yyvsp[0].node)); }
#line 52841 "gram.c"
    break;

  case 2396: /* json_name_and_value: a_expr ':' json_value_expr  */
#line 17100 "gram.y"
                                { (yyval.node) = makeJsonKeyValue((yyvsp[-2].node), (yyvsp[0].node)); }
#line 52847 "gram.c"
    break;

  case 2397: /* json_object_constructor_null_clause_opt: NULL_P ON NULL_P  */
#line 17105 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 52853 "gram.c"
    break;

  case 2398: /* json_object_constructor_null_clause_opt: ABSENT ON NULL_P  */
#line 17106 "gram.y"
                                                                                { (yyval.boolean) = true; }
#line 52859 "gram.c"
    break;

  case 2399: /* json_object_constructor_null_clause_opt: %empty  */
#line 17107 "gram.y"
                                                                                { (yyval.boolean) = false; }
#line 52865 "gram.c"
    break;

  case 2400: /* json_array_constructor_null_clause_opt: NULL_P ON NULL_P  */
#line 17111 "gram.y"
                                                                                        { (yyval.boolean) = false; }
#line 52871 "gram.c"
    break;

  case 2401: /* json_array_constructor_null_clause_opt: ABSENT ON NULL_P  */
#line 17112 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 52877 "gram.c"
    break;

  case 2402: /* json_array_constructor_null_clause_opt: %empty  */
#line 17113 "gram.y"
                                                                                        { (yyval.boolean) = true; }
#line 52883 "gram.c"
    break;

  case 2403: /* json_value_expr_list: json_value_expr  */
#line 17117 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].node)); }
#line 52889 "gram.c"
    break;

  case 2404: /* json_value_expr_list: json_value_expr_list ',' json_value_expr  */
#line 17118 "gram.y"
                                                                        { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].node));}
#line 52895 "gram.c"
    break;

  case 2405: /* json_aggregate_func: JSON_OBJECTAGG '(' json_name_and_value json_object_constructor_null_clause_opt json_key_uniqueness_constraint_opt json_returning_clause_opt ')'  */
#line 17128 "gram.y"
                                {
                    JsonObjectAgg *n = makeNode(JsonObjectAgg);

                    n->arg = (JsonKeyValue *) (yyvsp[-4].node);
                    n->absent_on_null = (yyvsp[-3].boolean);
                    n->unique = (yyvsp[-2].boolean);
                    n->constructor = makeNode(JsonAggConstructor);
                    n->constructor->output = (JsonOutput *) (yyvsp[-1].node);
                    n->constructor->agg_order = NULL;
                    n->constructor->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 52912 "gram.c"
    break;

  case 2406: /* json_aggregate_func: JSON_ARRAYAGG '(' json_value_expr json_array_aggregate_order_by_clause_opt json_array_constructor_null_clause_opt json_returning_clause_opt ')'  */
#line 17146 "gram.y"
                                {
                    JsonArrayAgg *n = makeNode(JsonArrayAgg);

                    n->arg = (JsonValueExpr *) (yyvsp[-4].node);
                    n->absent_on_null = (yyvsp[-2].boolean);
                    n->constructor = makeNode(JsonAggConstructor);
                    n->constructor->agg_order = (yyvsp[-3].list);
                    n->constructor->output = (JsonOutput *) (yyvsp[-1].node);
                    n->constructor->location = (yylsp[-6]);
                    (yyval.node) = (Node *) n;
                }
#line 52928 "gram.c"
    break;

  case 2407: /* json_array_aggregate_order_by_clause_opt: ORDER BY sortby_list  */
#line 17160 "gram.y"
                                                                                { (yyval.list) = (yyvsp[0].list); }
#line 52934 "gram.c"
    break;

  case 2408: /* json_array_aggregate_order_by_clause_opt: %empty  */
#line 17161 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 52940 "gram.c"
    break;

  case 2409: /* opt_target_list: target_list  */
#line 17170 "gram.y"
                                                                        { (yyval.list) = (yyvsp[0].list); }
#line 52946 "gram.c"
    break;

  case 2410: /* opt_target_list: %empty  */
#line 17171 "gram.y"
                                                                                        { (yyval.list) = NIL; }
#line 52952 "gram.c"
    break;

  case 2411: /* target_list: target_el  */
#line 17175 "gram.y"
                                                                                                { (yyval.list) = list_make1((yyvsp[0].target)); }
#line 52958 "gram.c"
    break;

  case 2412: /* target_list: target_list ',' target_el  */
#line 17176 "gram.y"
                                                                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].target)); }
#line 52964 "gram.c"
    break;

  case 2413: /* target_el: a_expr AS ColLabel  */
#line 17180 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[0].str);
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) (yyvsp[-2].node);
                    (yyval.target)->location = (yylsp[-2]);
                }
#line 52976 "gram.c"
    break;

  case 2414: /* target_el: a_expr BareColLabel  */
#line 17188 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = (yyvsp[0].str);
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) (yyvsp[-1].node);
                    (yyval.target)->location = (yylsp[-1]);
                }
#line 52988 "gram.c"
    break;

  case 2415: /* target_el: a_expr  */
#line 17196 "gram.y"
                                {
                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = NULL;
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) (yyvsp[0].node);
                    (yyval.target)->location = (yylsp[0]);
                }
#line 53000 "gram.c"
    break;

  case 2416: /* target_el: '*'  */
#line 17204 "gram.y"
                                {
                    ColumnRef  *n = makeNode(ColumnRef);

                    n->fields = list_make1(makeNode(A_Star));
                    n->location = (yylsp[0]);

                    (yyval.target) = makeNode(ResTarget);
                    (yyval.target)->name = NULL;
                    (yyval.target)->indirection = NIL;
                    (yyval.target)->val = (Node *) n;
                    (yyval.target)->location = (yylsp[0]);
                }
#line 53017 "gram.c"
    break;

  case 2417: /* qualified_name_list: qualified_name  */
#line 17226 "gram.y"
                                                                                        { (yyval.list) = list_make1((yyvsp[0].range)); }
#line 53023 "gram.c"
    break;

  case 2418: /* qualified_name_list: qualified_name_list ',' qualified_name  */
#line 17227 "gram.y"
                                                                 { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].range)); }
#line 53029 "gram.c"
    break;

  case 2419: /* qualified_name: ColId  */
#line 17239 "gram.y"
                                {
                    (yyval.range) = makeRangeVar(NULL, (yyvsp[0].str), (yylsp[0]));
                }
#line 53037 "gram.c"
    break;

  case 2420: /* qualified_name: ColId indirection  */
#line 17243 "gram.y"
                                {
                    (yyval.range) = makeRangeVarFromQualifiedName((yyvsp[-1].str), (yyvsp[0].list), (yylsp[-1]), yyscanner);
                }
#line 53045 "gram.c"
    break;

  case 2421: /* name_list: name  */
#line 17249 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 53051 "gram.c"
    break;

  case 2422: /* name_list: name_list ',' name  */
#line 17251 "gram.y"
                                        { (yyval.list) = lappend((yyvsp[-2].list), makeString((yyvsp[0].str))); }
#line 53057 "gram.c"
    break;

  case 2423: /* name: ColId  */
#line 17255 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53063 "gram.c"
    break;

  case 2424: /* attr_name: ColLabel  */
#line 17257 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53069 "gram.c"
    break;

  case 2425: /* file_name: Sconst  */
#line 17259 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53075 "gram.c"
    break;

  case 2426: /* func_name: type_function_name  */
#line 17270 "gram.y"
                                        { (yyval.list) = list_make1(makeString((yyvsp[0].str))); }
#line 53081 "gram.c"
    break;

  case 2427: /* func_name: ColId indirection  */
#line 17272 "gram.y"
                                        {
                        (yyval.list) = check_func_name(lcons(makeString((yyvsp[-1].str)), (yyvsp[0].list)),
                                             yyscanner);
                    }
#line 53090 "gram.c"
    break;

  case 2428: /* AexprConst: Iconst  */
#line 17283 "gram.y"
                                {
                    (yyval.node) = makeIntConst((yyvsp[0].ival), (yylsp[0]));
                }
#line 53098 "gram.c"
    break;

  case 2429: /* AexprConst: FCONST  */
#line 17287 "gram.y"
                                {
                    (yyval.node) = makeFloatConst((yyvsp[0].str), (yylsp[0]));
                }
#line 53106 "gram.c"
    break;

  case 2430: /* AexprConst: Sconst  */
#line 17291 "gram.y"
                                {
                    (yyval.node) = makeStringConst((yyvsp[0].str), (yylsp[0]));
                }
#line 53114 "gram.c"
    break;

  case 2431: /* AexprConst: BCONST  */
#line 17295 "gram.y"
                                {
                    (yyval.node) = makeBitStringConst((yyvsp[0].str), (yylsp[0]));
                }
#line 53122 "gram.c"
    break;

  case 2432: /* AexprConst: XCONST  */
#line 17299 "gram.y"
                                {
                    /* This is a bit constant per SQL99:
                     * Without Feature F511, "BIT data type",
                     * a <general literal> shall not be a
                     * <bit string literal> or a <hex string literal>.
                     */
                    (yyval.node) = makeBitStringConst((yyvsp[0].str), (yylsp[0]));
                }
#line 53135 "gram.c"
    break;

  case 2433: /* AexprConst: func_name Sconst  */
#line 17308 "gram.y"
                                {
                    /* generic type 'literal' syntax */
                    TypeName   *t = makeTypeNameFromNameList((yyvsp[-1].list));

                    t->location = (yylsp[-1]);
                    (yyval.node) = makeStringConstCast((yyvsp[0].str), (yylsp[0]), t);
                }
#line 53147 "gram.c"
    break;

  case 2434: /* AexprConst: func_name '(' func_arg_list opt_sort_clause ')' Sconst  */
#line 17316 "gram.y"
                                {
                    /* generic syntax with a type modifier */
                    TypeName   *t = makeTypeNameFromNameList((yyvsp[-5].list));
                    ListCell   *lc;

                    /*
                     * We must use func_arg_list and opt_sort_clause in the
                     * production to avoid reduce/reduce conflicts, but we
                     * don't actually wish to allow NamedArgExpr in this
                     * context, nor ORDER BY.
                     */
                    foreach(lc, (yyvsp[-3].list))
                    {
                        NamedArgExpr *arg = (NamedArgExpr *) lfirst(lc);

                        if (IsA(arg, NamedArgExpr))
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("type modifier cannot have parameter name"),
                                     parser_errposition(arg->location)));
                    }
                    if ((yyvsp[-2].list) != NIL)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("type modifier cannot have ORDER BY"),
                                     parser_errposition((yylsp[-2]))));

                    t->typmods = (yyvsp[-3].list);
                    t->location = (yylsp[-5]);
                    (yyval.node) = makeStringConstCast((yyvsp[0].str), (yylsp[0]), t);
                }
#line 53183 "gram.c"
    break;

  case 2435: /* AexprConst: ConstTypename Sconst  */
#line 17348 "gram.y"
                                {
                    (yyval.node) = makeStringConstCast((yyvsp[0].str), (yylsp[0]), (yyvsp[-1].typnam));
                }
#line 53191 "gram.c"
    break;

  case 2436: /* AexprConst: ConstInterval Sconst opt_interval  */
#line 17352 "gram.y"
                                {
                    TypeName   *t = (yyvsp[-2].typnam);

                    t->typmods = (yyvsp[0].list);
                    (yyval.node) = makeStringConstCast((yyvsp[-1].str), (yylsp[-1]), t);
                }
#line 53202 "gram.c"
    break;

  case 2437: /* AexprConst: ConstInterval '(' Iconst ')' Sconst  */
#line 17359 "gram.y"
                                {
                    TypeName   *t = (yyvsp[-4].typnam);

                    t->typmods = list_make2(makeIntConst(INTERVAL_FULL_RANGE, -1),
                                            makeIntConst((yyvsp[-2].ival), (yylsp[-2])));
                    (yyval.node) = makeStringConstCast((yyvsp[0].str), (yylsp[0]), t);
                }
#line 53214 "gram.c"
    break;

  case 2438: /* AexprConst: TRUE_P  */
#line 17367 "gram.y"
                                {
                    (yyval.node) = makeBoolAConst(true, (yylsp[0]));
                }
#line 53222 "gram.c"
    break;

  case 2439: /* AexprConst: FALSE_P  */
#line 17371 "gram.y"
                                {
                    (yyval.node) = makeBoolAConst(false, (yylsp[0]));
                }
#line 53230 "gram.c"
    break;

  case 2440: /* AexprConst: NULL_P  */
#line 17375 "gram.y"
                                {
                    (yyval.node) = makeNullAConst((yylsp[0]));
                }
#line 53238 "gram.c"
    break;

  case 2441: /* Iconst: ICONST  */
#line 17380 "gram.y"
                                                                                        { (yyval.ival) = (yyvsp[0].ival); }
#line 53244 "gram.c"
    break;

  case 2442: /* Sconst: SCONST  */
#line 17381 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53250 "gram.c"
    break;

  case 2443: /* SignedIconst: Iconst  */
#line 17383 "gram.y"
                                                                                { (yyval.ival) = (yyvsp[0].ival); }
#line 53256 "gram.c"
    break;

  case 2444: /* SignedIconst: '+' Iconst  */
#line 17384 "gram.y"
                                                                                        { (yyval.ival) = + (yyvsp[0].ival); }
#line 53262 "gram.c"
    break;

  case 2445: /* SignedIconst: '-' Iconst  */
#line 17385 "gram.y"
                                                                                        { (yyval.ival) = - (yyvsp[0].ival); }
#line 53268 "gram.c"
    break;

  case 2446: /* RoleId: RoleSpec  */
#line 17390 "gram.y"
                                {
                    RoleSpec   *spc = (RoleSpec *) (yyvsp[0].rolespec);

                    switch (spc->roletype)
                    {
                        case ROLESPEC_CSTRING:
                            (yyval.str) = spc->rolename;
                            break;
                        case ROLESPEC_PUBLIC:
                            ereport(ERROR,
                                    (errcode(ERRCODE_RESERVED_NAME),
                                     errmsg("role name \"%s\" is reserved",
                                            "public"),
                                     parser_errposition((yylsp[0]))));
                            break;
                        case ROLESPEC_SESSION_USER:
                            ereport(ERROR,
                                    (errcode(ERRCODE_RESERVED_NAME),
                                     errmsg("%s cannot be used as a role name here",
                                            "SESSION_USER"),
                                     parser_errposition((yylsp[0]))));
                            break;
                        case ROLESPEC_CURRENT_USER:
                            ereport(ERROR,
                                    (errcode(ERRCODE_RESERVED_NAME),
                                     errmsg("%s cannot be used as a role name here",
                                            "CURRENT_USER"),
                                     parser_errposition((yylsp[0]))));
                            break;
                        case ROLESPEC_CURRENT_ROLE:
                            ereport(ERROR,
                                    (errcode(ERRCODE_RESERVED_NAME),
                                     errmsg("%s cannot be used as a role name here",
                                            "CURRENT_ROLE"),
                                     parser_errposition((yylsp[0]))));
                            break;
                    }
                }
#line 53311 "gram.c"
    break;

  case 2447: /* RoleSpec: NonReservedWord  */
#line 17431 "gram.y"
                                {
                    /*
                     * "public" and "none" are not keywords, but they must
                     * be treated specially here.
                     */
                    RoleSpec   *n;

                    if (strcmp((yyvsp[0].str), "public") == 0)
                    {
                        n = (RoleSpec *) makeRoleSpec(ROLESPEC_PUBLIC, (yylsp[0]));
                        n->roletype = ROLESPEC_PUBLIC;
                    }
                    else if (strcmp((yyvsp[0].str), "none") == 0)
                    {
                        ereport(ERROR,
                                (errcode(ERRCODE_RESERVED_NAME),
                                 errmsg("role name \"%s\" is reserved",
                                        "none"),
                                 parser_errposition((yylsp[0]))));
                    }
                    else
                    {
                        n = makeRoleSpec(ROLESPEC_CSTRING, (yylsp[0]));
                        n->rolename = pstrdup((yyvsp[0].str));
                    }
                    (yyval.rolespec) = n;
                }
#line 53343 "gram.c"
    break;

  case 2448: /* RoleSpec: CURRENT_ROLE  */
#line 17459 "gram.y"
                                {
                    (yyval.rolespec) = makeRoleSpec(ROLESPEC_CURRENT_ROLE, (yylsp[0]));
                }
#line 53351 "gram.c"
    break;

  case 2449: /* RoleSpec: CURRENT_USER  */
#line 17463 "gram.y"
                                {
                    (yyval.rolespec) = makeRoleSpec(ROLESPEC_CURRENT_USER, (yylsp[0]));
                }
#line 53359 "gram.c"
    break;

  case 2450: /* RoleSpec: SESSION_USER  */
#line 17467 "gram.y"
                                {
                    (yyval.rolespec) = makeRoleSpec(ROLESPEC_SESSION_USER, (yylsp[0]));
                }
#line 53367 "gram.c"
    break;

  case 2451: /* role_list: RoleSpec  */
#line 17473 "gram.y"
                                { (yyval.list) = list_make1((yyvsp[0].rolespec)); }
#line 53373 "gram.c"
    break;

  case 2452: /* role_list: role_list ',' RoleSpec  */
#line 17475 "gram.y"
                                { (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].rolespec)); }
#line 53379 "gram.c"
    break;

  case 2453: /* PLpgSQL_Expr: opt_distinct_clause opt_target_list from_clause where_clause group_clause having_clause window_clause opt_sort_clause opt_select_limit opt_for_locking_clause  */
#line 17492 "gram.y"
                                {
                    SelectStmt *n = makeNode(SelectStmt);

                    n->distinctClause = (yyvsp[-9].list);
                    n->targetList = (yyvsp[-8].list);
                    n->fromClause = (yyvsp[-7].list);
                    n->whereClause = (yyvsp[-6].node);
                    n->groupClause = ((yyvsp[-5].groupclause))->list;
                    n->groupDistinct = ((yyvsp[-5].groupclause))->distinct;
                    n->havingClause = (yyvsp[-4].node);
                    n->windowClause = (yyvsp[-3].list);
                    n->sortClause = (yyvsp[-2].list);
                    if ((yyvsp[-1].selectlimit))
                    {
                        n->limitOffset = (yyvsp[-1].selectlimit)->limitOffset;
                        n->limitCount = (yyvsp[-1].selectlimit)->limitCount;
                        if (!n->sortClause &&
                            (yyvsp[-1].selectlimit)->limitOption == LIMIT_OPTION_WITH_TIES)
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("WITH TIES cannot be specified without ORDER BY clause")));
                        n->limitOption = (yyvsp[-1].selectlimit)->limitOption;
                    }
                    n->lockingClause = (yyvsp[0].list);
                    (yyval.node) = (Node *) n;
                }
#line 53410 "gram.c"
    break;

  case 2454: /* PLAssignStmt: plassign_target opt_indirection plassign_equals PLpgSQL_Expr  */
#line 17525 "gram.y"
                                {
                    PLAssignStmt *n = makeNode(PLAssignStmt);

                    n->name = (yyvsp[-3].str);
                    n->indirection = check_indirection((yyvsp[-2].list), yyscanner);
                    /* nnames will be filled by calling production */
                    n->val = (SelectStmt *) (yyvsp[0].node);
                    n->location = (yylsp[-3]);
                    (yyval.node) = (Node *) n;
                }
#line 53425 "gram.c"
    break;

  case 2455: /* plassign_target: ColId  */
#line 17537 "gram.y"
                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53431 "gram.c"
    break;

  case 2456: /* plassign_target: PARAM  */
#line 17538 "gram.y"
                                                                                        { (yyval.str) = psprintf("$%d", (yyvsp[0].ival)); }
#line 53437 "gram.c"
    break;

  case 2459: /* ColId: IDENT  */
#line 17559 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53443 "gram.c"
    break;

  case 2460: /* ColId: unreserved_keyword  */
#line 17560 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53449 "gram.c"
    break;

  case 2461: /* ColId: col_name_keyword  */
#line 17561 "gram.y"
                                                                                        { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53455 "gram.c"
    break;

  case 2462: /* type_function_name: IDENT  */
#line 17566 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 53461 "gram.c"
    break;

  case 2463: /* type_function_name: unreserved_keyword  */
#line 17567 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53467 "gram.c"
    break;

  case 2464: /* type_function_name: type_func_name_keyword  */
#line 17568 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53473 "gram.c"
    break;

  case 2465: /* NonReservedWord: IDENT  */
#line 17573 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 53479 "gram.c"
    break;

  case 2466: /* NonReservedWord: unreserved_keyword  */
#line 17574 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53485 "gram.c"
    break;

  case 2467: /* NonReservedWord: col_name_keyword  */
#line 17575 "gram.y"
                                                                                        { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53491 "gram.c"
    break;

  case 2468: /* NonReservedWord: type_func_name_keyword  */
#line 17576 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53497 "gram.c"
    break;

  case 2469: /* ColLabel: IDENT  */
#line 17582 "gram.y"
                                                                                        { (yyval.str) = (yyvsp[0].str); }
#line 53503 "gram.c"
    break;

  case 2470: /* ColLabel: unreserved_keyword  */
#line 17583 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53509 "gram.c"
    break;

  case 2471: /* ColLabel: col_name_keyword  */
#line 17584 "gram.y"
                                                                                        { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53515 "gram.c"
    break;

  case 2472: /* ColLabel: type_func_name_keyword  */
#line 17585 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53521 "gram.c"
    break;

  case 2473: /* ColLabel: reserved_keyword  */
#line 17586 "gram.y"
                                                                                        { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53527 "gram.c"
    break;

  case 2474: /* BareColLabel: IDENT  */
#line 17592 "gram.y"
                                                                                { (yyval.str) = (yyvsp[0].str); }
#line 53533 "gram.c"
    break;

  case 2475: /* BareColLabel: bare_label_keyword  */
#line 17593 "gram.y"
                                                                                { (yyval.str) = pstrdup((yyvsp[0].keyword)); }
#line 53539 "gram.c"
    break;


#line 53543 "gram.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (&yylloc, yyscanner, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, yyscanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, yyscanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, yyscanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, yyscanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, yyscanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 18611 "gram.y"


/*=======================================================================*/

/*
 * The signature of this function is required by bison.  However, we
 * ignore the passed yylloc and instead use the last token position
 * available from the scanner.
 */
static void
base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner, const char *msg)
{
    parser_yyerror(msg);
}

static RawStmt *
makeRawStmt(Node *stmt, int stmt_location)
{
    RawStmt    *rs = makeNode(RawStmt);

    rs->stmt = stmt;
    rs->stmt_location = stmt_location;
    rs->stmt_len = 0;           /* might get changed later */
    return rs;
}

/* Adjust a RawStmt to reflect that it doesn't run to the end of the string */
static void
updateRawStmtEnd(RawStmt *rs, int end_location)
{
    /*
     * If we already set the length, don't change it.  This is for situations
     * like "select foo ;; select bar" where the same statement will be last
     * in the string for more than one semicolon.
     */
    if (rs->stmt_len > 0)
        return;

    /* OK, update length of RawStmt */
    rs->stmt_len = end_location - rs->stmt_location;
}

static Node *
makeColumnRef(char *colname, List *indirection,
              int location, core_yyscan_t yyscanner)
{
    /*
     * Generate a ColumnRef node, with an A_Indirection node added if there
     * is any subscripting in the specified indirection list.  However,
     * any field selection at the start of the indirection list must be
     * transposed into the "fields" part of the ColumnRef node.
     */
    ColumnRef  *c = makeNode(ColumnRef);
    int         nfields = 0;
    ListCell   *l;

    c->location = location;
    foreach(l, indirection)
    {
        if (IsA(lfirst(l), A_Indices))
        {
            A_Indirection *i = makeNode(A_Indirection);

            if (nfields == 0)
            {
                /* easy case - all indirection goes to A_Indirection */
                c->fields = list_make1(makeString(colname));
                i->indirection = check_indirection(indirection, yyscanner);
            }
            else
            {
                /* got to split the list in two */
                i->indirection = check_indirection(list_copy_tail(indirection,
                                                                  nfields),
                                                   yyscanner);
                indirection = list_truncate(indirection, nfields);
                c->fields = lcons(makeString(colname), indirection);
            }
            i->arg = (Node *) c;
            return (Node *) i;
        }
        else if (IsA(lfirst(l), A_Star))
        {
            /* We only allow '*' at the end of a ColumnRef */
            if (lnext(indirection, l) != NULL)
                parser_yyerror("improper use of \"*\"");
        }
        nfields++;
    }
    /* No subscripting, so all indirection gets added to field list */
    c->fields = lcons(makeString(colname), indirection);
    return (Node *) c;
}

static Node *
makeTypeCast(Node *arg, TypeName *typename, int location)
{
    TypeCast   *n = makeNode(TypeCast);

    n->arg = arg;
    n->typeName = typename;
    n->location = location;
    return (Node *) n;
}

static Node *
makeStringConstCast(char *str, int location, TypeName *typename)
{
    Node       *s = makeStringConst(str, location);

    return makeTypeCast(s, typename, -1);
}

static Node *
makeIntConst(int val, int location)
{
    A_Const    *n = makeNode(A_Const);

    n->val.ival.type = T_Integer;
    n->val.ival.ival = val;
    n->location = location;

   return (Node *) n;
}

static Node *
makeFloatConst(char *str, int location)
{
    A_Const    *n = makeNode(A_Const);

    n->val.fval.type = T_Float;
    n->val.fval.fval = str;
    n->location = location;

   return (Node *) n;
}

static Node *
makeBoolAConst(bool state, int location)
{
    A_Const    *n = makeNode(A_Const);

    n->val.boolval.type = T_Boolean;
    n->val.boolval.boolval = state;
    n->location = location;

   return (Node *) n;
}

static Node *
makeBitStringConst(char *str, int location)
{
    A_Const    *n = makeNode(A_Const);

    n->val.bsval.type = T_BitString;
    n->val.bsval.bsval = str;
    n->location = location;

   return (Node *) n;
}

static Node *
makeNullAConst(int location)
{
    A_Const    *n = makeNode(A_Const);

    n->isnull = true;
    n->location = location;

    return (Node *) n;
}

static Node *
makeAConst(Node *v, int location)
{
    Node       *n;

    switch (v->type)
    {
        case T_Float:
            n = makeFloatConst(castNode(Float, v)->fval, location);
            break;

        case T_Integer:
            n = makeIntConst(castNode(Integer, v)->ival, location);
            break;

        default:
            /* currently not used */
            Assert(false);
            n = NULL;
    }

    return n;
}

/* makeRoleSpec
 * Create a RoleSpec with the given type
 */
static RoleSpec *
makeRoleSpec(RoleSpecType type, int location)
{
    RoleSpec   *spec = makeNode(RoleSpec);

    spec->roletype = type;
    spec->location = location;

    return spec;
}

/* check_qualified_name --- check the result of qualified_name production
 *
 * It's easiest to let the grammar production for qualified_name allow
 * subscripts and '*', which we then must reject here.
 */
static void
check_qualified_name(List *names, core_yyscan_t yyscanner)
{
    ListCell   *i;

    foreach(i, names)
    {
        if (!IsA(lfirst(i), String))
            parser_yyerror("syntax error");
    }
}

/* check_func_name --- check the result of func_name production
 *
 * It's easiest to let the grammar production for func_name allow subscripts
 * and '*', which we then must reject here.
 */
static List *
check_func_name(List *names, core_yyscan_t yyscanner)
{
    ListCell   *i;

    foreach(i, names)
    {
        if (!IsA(lfirst(i), String))
            parser_yyerror("syntax error");
    }
    return names;
}

/* check_indirection --- check the result of indirection production
 *
 * We only allow '*' at the end of the list, but it's hard to enforce that
 * in the grammar, so do it here.
 */
static List *
check_indirection(List *indirection, core_yyscan_t yyscanner)
{
    ListCell *l;

    foreach(l, indirection)
    {
        if (IsA(lfirst(l), A_Star))
        {
            if (lnext(indirection, l) != NULL)
                parser_yyerror("improper use of \"*\"");
        }
    }
    return indirection;
}

/* extractArgTypes()
 * Given a list of FunctionParameter nodes, extract a list of just the
 * argument types (TypeNames) for input parameters only.  This is what
 * is needed to look up an existing function, which is what is wanted by
 * the productions that use this call.
 */
static List *
extractArgTypes(List *parameters)
{
    List       *result = NIL;
    ListCell   *i;

    foreach(i, parameters)
    {
        FunctionParameter *p = (FunctionParameter *) lfirst(i);

        if (p->mode != FUNC_PARAM_OUT && p->mode != FUNC_PARAM_TABLE)
            result = lappend(result, p->argType);
    }
    return result;
}

/* extractAggrArgTypes()
 * As above, but work from the output of the aggr_args production.
 */
static List *
extractAggrArgTypes(List *aggrargs)
{
    Assert(list_length(aggrargs) == 2);
    return extractArgTypes((List *) linitial(aggrargs));
}

/* makeOrderedSetArgs()
 * Build the result of the aggr_args production (which see the comments for).
 * This handles only the case where both given lists are nonempty, so that
 * we have to deal with multiple VARIADIC arguments.
 */
static List *
makeOrderedSetArgs(List *directargs, List *orderedargs,
                   core_yyscan_t yyscanner)
{
    FunctionParameter *lastd = (FunctionParameter *) llast(directargs);
    Integer    *ndirectargs;

    /* No restriction unless last direct arg is VARIADIC */
    if (lastd->mode == FUNC_PARAM_VARIADIC)
    {
        FunctionParameter *firsto = (FunctionParameter *) linitial(orderedargs);

        /*
         * We ignore the names, though the aggr_arg production allows them;
         * it doesn't allow default values, so those need not be checked.
         */
        if (list_length(orderedargs) != 1 ||
            firsto->mode != FUNC_PARAM_VARIADIC ||
            !equal(lastd->argType, firsto->argType))
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("an ordered-set aggregate with a VARIADIC direct argument must have one VARIADIC aggregated argument of the same data type"),
                     parser_errposition(exprLocation((Node *) firsto))));

        /* OK, drop the duplicate VARIADIC argument from the internal form */
        orderedargs = NIL;
    }

    /* don't merge into the next line, as list_concat changes directargs */
    ndirectargs = makeInteger(list_length(directargs));

    return list_make2(list_concat(directargs, orderedargs),
                      ndirectargs);
}

/* insertSelectOptions()
 * Insert ORDER BY, etc into an already-constructed SelectStmt.
 *
 * This routine is just to avoid duplicating code in SelectStmt productions.
 */
static void
insertSelectOptions(SelectStmt *stmt,
                    List *sortClause, List *lockingClause,
                    SelectLimit *limitClause,
                    WithClause *withClause,
                    core_yyscan_t yyscanner)
{
    Assert(IsA(stmt, SelectStmt));

    /*
     * Tests here are to reject constructs like
     *  (SELECT foo ORDER BY bar) ORDER BY baz
     */
    if (sortClause)
    {
        if (stmt->sortClause)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("multiple ORDER BY clauses not allowed"),
                     parser_errposition(exprLocation((Node *) sortClause))));
        stmt->sortClause = sortClause;
    }
    /* We can handle multiple locking clauses, though */
    stmt->lockingClause = list_concat(stmt->lockingClause, lockingClause);
    if (limitClause && limitClause->limitOffset)
    {
        if (stmt->limitOffset)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("multiple OFFSET clauses not allowed"),
                     parser_errposition(exprLocation(limitClause->limitOffset))));
        stmt->limitOffset = limitClause->limitOffset;
    }
    if (limitClause && limitClause->limitCount)
    {
        if (stmt->limitCount)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("multiple LIMIT clauses not allowed"),
                     parser_errposition(exprLocation(limitClause->limitCount))));
        stmt->limitCount = limitClause->limitCount;
    }
    if (limitClause)
    {
        if (stmt->limitOption)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("multiple limit options not allowed")));
        if (!stmt->sortClause && limitClause->limitOption == LIMIT_OPTION_WITH_TIES)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("WITH TIES cannot be specified without ORDER BY clause")));
        if (limitClause->limitOption == LIMIT_OPTION_WITH_TIES && stmt->lockingClause)
        {
            ListCell   *lc;

            foreach(lc, stmt->lockingClause)
            {
                LockingClause *lock = lfirst_node(LockingClause, lc);

                if (lock->waitPolicy == LockWaitSkip)
                    ereport(ERROR,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("%s and %s options cannot be used together",
                                    "SKIP LOCKED", "WITH TIES")));
            }
        }
        stmt->limitOption = limitClause->limitOption;
    }
    if (withClause)
    {
        if (stmt->withClause)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("multiple WITH clauses not allowed"),
                     parser_errposition(exprLocation((Node *) withClause))));
        stmt->withClause = withClause;
    }
}

static Node *
makeSetOp(SetOperation op, bool all, Node *larg, Node *rarg)
{
    SelectStmt *n = makeNode(SelectStmt);

    n->op = op;
    n->all = all;
    n->larg = (SelectStmt *) larg;
    n->rarg = (SelectStmt *) rarg;
    return (Node *) n;
}

/* SystemFuncName()
 * Build a properly-qualified reference to a built-in function.
 */
List *
SystemFuncName(char *name)
{
    return list_make2(makeString("pg_catalog"), makeString(name));
}

/* SystemTypeName()
 * Build a properly-qualified reference to a built-in type.
 *
 * typmod is defaulted, but may be changed afterwards by caller.
 * Likewise for the location.
 */
TypeName *
SystemTypeName(char *name)
{
    return makeTypeNameFromNameList(list_make2(makeString("pg_catalog"),
                                               makeString(name)));
}

/* doNegate()
 * Handle negation of a numeric constant.
 *
 * Formerly, we did this here because the optimizer couldn't cope with
 * indexquals that looked like "var = -4" --- it wants "var = const"
 * and a unary minus operator applied to a constant didn't qualify.
 * As of Postgres 7.0, that problem doesn't exist anymore because there
 * is a constant-subexpression simplifier in the optimizer.  However,
 * there's still a good reason for doing this here, which is that we can
 * postpone committing to a particular internal representation for simple
 * negative constants.  It's better to leave "-123.456" in string form
 * until we know what the desired type is.
 */
static Node *
doNegate(Node *n, int location)
{
    if (IsA(n, A_Const))
    {
        A_Const    *con = (A_Const *) n;

        /* report the constant's location as that of the '-' sign */
        con->location = location;

        if (IsA(&con->val, Integer))
        {
            con->val.ival.ival = -con->val.ival.ival;
            return n;
        }
        if (IsA(&con->val, Float))
        {
            doNegateFloat(&con->val.fval);
            return n;
        }
    }

    return (Node *) makeSimpleA_Expr(AEXPR_OP, "-", NULL, n, location);
}

static void
doNegateFloat(Float *v)
{
    char       *oldval = v->fval;

    if (*oldval == '+')
        oldval++;
    if (*oldval == '-')
        v->fval = oldval+1; /* just strip the '-' */
    else
        v->fval = psprintf("-%s", oldval);
}

static Node *
makeAndExpr(Node *lexpr, Node *rexpr, int location)
{
    /* Flatten "a AND b AND c ..." to a single BoolExpr on sight */
    if (IsA(lexpr, BoolExpr))
    {
        BoolExpr   *blexpr = (BoolExpr *) lexpr;

        if (blexpr->boolop == AND_EXPR)
        {
            blexpr->args = lappend(blexpr->args, rexpr);
            return (Node *) blexpr;
        }
    }
    return (Node *) makeBoolExpr(AND_EXPR, list_make2(lexpr, rexpr), location);
}

static Node *
makeOrExpr(Node *lexpr, Node *rexpr, int location)
{
    /* Flatten "a OR b OR c ..." to a single BoolExpr on sight */
    if (IsA(lexpr, BoolExpr))
    {
        BoolExpr   *blexpr = (BoolExpr *) lexpr;

        if (blexpr->boolop == OR_EXPR)
        {
            blexpr->args = lappend(blexpr->args, rexpr);
            return (Node *) blexpr;
        }
    }
    return (Node *) makeBoolExpr(OR_EXPR, list_make2(lexpr, rexpr), location);
}

static Node *
makeNotExpr(Node *expr, int location)
{
    return (Node *) makeBoolExpr(NOT_EXPR, list_make1(expr), location);
}

static Node *
makeAArrayExpr(List *elements, int location)
{
    A_ArrayExpr *n = makeNode(A_ArrayExpr);

    n->elements = elements;
    n->location = location;
    return (Node *) n;
}

static Node *
makeSQLValueFunction(SQLValueFunctionOp op, int32 typmod, int location)
{
    SQLValueFunction *svf = makeNode(SQLValueFunction);

    svf->op = op;
    /* svf->type will be filled during parse analysis */
    svf->typmod = typmod;
    svf->location = location;
    return (Node *) svf;
}

static Node *
makeXmlExpr(XmlExprOp op, char *name, List *named_args, List *args,
            int location)
{
    XmlExpr     *x = makeNode(XmlExpr);

    x->op = op;
    x->name = name;
    /*
     * named_args is a list of ResTarget; it'll be split apart into separate
     * expression and name lists in transformXmlExpr().
     */
    x->named_args = named_args;
    x->arg_names = NIL;
    x->args = args;
    /* xmloption, if relevant, must be filled in by caller */
    /* type and typmod will be filled in during parse analysis */
    x->type = InvalidOid;           /* marks the node as not analyzed */
    x->location = location;
    return (Node *) x;
}

/*
 * Merge the input and output parameters of a table function.
 */
static List *
mergeTableFuncParameters(List *func_args, List *columns)
{
    ListCell   *lc;

    /* Explicit OUT and INOUT parameters shouldn't be used in this syntax */
    foreach(lc, func_args)
    {
        FunctionParameter *p = (FunctionParameter *) lfirst(lc);

        if (p->mode != FUNC_PARAM_DEFAULT &&
            p->mode != FUNC_PARAM_IN &&
            p->mode != FUNC_PARAM_VARIADIC)
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("OUT and INOUT arguments aren't allowed in TABLE functions")));
    }

    return list_concat(func_args, columns);
}

/*
 * Determine return type of a TABLE function.  A single result column
 * returns setof that column's type; otherwise return setof record.
 */
static TypeName *
TableFuncTypeName(List *columns)
{
    TypeName   *result;

    if (list_length(columns) == 1)
    {
        FunctionParameter *p = (FunctionParameter *) linitial(columns);

        result = copyObject(p->argType);
    }
    else
        result = SystemTypeName("record");

    result->setof = true;

    return result;
}

/*
 * Convert a list of (dotted) names to a RangeVar (like
 * makeRangeVarFromNameList, but with position support).  The
 * "AnyName" refers to the any_name production in the grammar.
 */
static RangeVar *
makeRangeVarFromAnyName(List *names, int position, core_yyscan_t yyscanner)
{
    RangeVar   *r = makeNode(RangeVar);

    switch (list_length(names))
    {
        case 1:
            r->catalogname = NULL;
            r->schemaname = NULL;
            r->relname = strVal(linitial(names));
            break;
        case 2:
            r->catalogname = NULL;
            r->schemaname = strVal(linitial(names));
            r->relname = strVal(lsecond(names));
            break;
        case 3:
            r->catalogname = strVal(linitial(names));
            r->schemaname = strVal(lsecond(names));
            r->relname = strVal(lthird(names));
            break;
        default:
            ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("improper qualified name (too many dotted names): %s",
                            NameListToString(names)),
                     parser_errposition(position)));
            break;
    }

    r->relpersistence = RELPERSISTENCE_PERMANENT;
    r->location = position;

    return r;
}

/*
 * Convert a relation_name with name and namelist to a RangeVar using
 * makeRangeVar.
 */
static RangeVar *
makeRangeVarFromQualifiedName(char *name, List *namelist, int location,
                              core_yyscan_t yyscanner)
{
    RangeVar   *r;

    check_qualified_name(namelist, yyscanner);
    r = makeRangeVar(NULL, NULL, location);

    switch (list_length(namelist))
    {
        case 1:
            r->catalogname = NULL;
            r->schemaname = name;
            r->relname = strVal(linitial(namelist));
            break;
        case 2:
            r->catalogname = name;
            r->schemaname = strVal(linitial(namelist));
            r->relname = strVal(lsecond(namelist));
            break;
        default:
            ereport(ERROR,
                    errcode(ERRCODE_SYNTAX_ERROR),
                    errmsg("improper qualified name (too many dotted names): %s",
                           NameListToString(lcons(makeString(name), namelist))),
                           parser_errposition(location));
            break;
    }

    return r;
}

/* Separate Constraint nodes from COLLATE clauses in a ColQualList */
static void
SplitColQualList(List *qualList,
                 List **constraintList, CollateClause **collClause,
                 core_yyscan_t yyscanner)
{
    ListCell   *cell;

    *collClause = NULL;
    foreach(cell, qualList)
    {
        Node       *n = (Node *) lfirst(cell);

        if (IsA(n, Constraint))
        {
            /* keep it in list */
            continue;
        }
        if (IsA(n, CollateClause))
        {
            CollateClause *c = (CollateClause *) n;

            if (*collClause)
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("multiple COLLATE clauses not allowed"),
                         parser_errposition(c->location)));
            *collClause = c;
        }
        else
            elog(ERROR, "unexpected node type %d", (int) n->type);
        /* remove non-Constraint nodes from qualList */
        qualList = foreach_delete_current(qualList, cell);
    }
    *constraintList = qualList;
}

/*
 * Process result of ConstraintAttributeSpec, and set appropriate bool flags
 * in the output command node.  Pass NULL for any flags the particular
 * command doesn't support.
 */
static void
processCASbits(int cas_bits, int location, const char *constrType,
               bool *deferrable, bool *initdeferred, bool *not_valid,
               bool *no_inherit, core_yyscan_t yyscanner)
{
    /* defaults */
    if (deferrable)
        *deferrable = false;
    if (initdeferred)
        *initdeferred = false;
    if (not_valid)
        *not_valid = false;

    if (cas_bits & (CAS_DEFERRABLE | CAS_INITIALLY_DEFERRED))
    {
        if (deferrable)
            *deferrable = true;
        else
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     /* translator: %s is CHECK, UNIQUE, or similar */
                     errmsg("%s constraints cannot be marked DEFERRABLE",
                            constrType),
                     parser_errposition(location)));
    }

    if (cas_bits & CAS_INITIALLY_DEFERRED)
    {
        if (initdeferred)
            *initdeferred = true;
        else
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     /* translator: %s is CHECK, UNIQUE, or similar */
                     errmsg("%s constraints cannot be marked DEFERRABLE",
                            constrType),
                     parser_errposition(location)));
    }

    if (cas_bits & CAS_NOT_VALID)
    {
        if (not_valid)
            *not_valid = true;
        else
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     /* translator: %s is CHECK, UNIQUE, or similar */
                     errmsg("%s constraints cannot be marked NOT VALID",
                            constrType),
                     parser_errposition(location)));
    }

    if (cas_bits & CAS_NO_INHERIT)
    {
        if (no_inherit)
            *no_inherit = true;
        else
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     /* translator: %s is CHECK, UNIQUE, or similar */
                     errmsg("%s constraints cannot be marked NO INHERIT",
                            constrType),
                     parser_errposition(location)));
    }
}

/*
 * Parse a user-supplied partition strategy string into parse node
 * PartitionStrategy representation, or die trying.
 */
static PartitionStrategy
parsePartitionStrategy(char *strategy)
{
    if (pg_strcasecmp(strategy, "list") == 0)
        return PARTITION_STRATEGY_LIST;
    else if (pg_strcasecmp(strategy, "range") == 0)
        return PARTITION_STRATEGY_RANGE;
    else if (pg_strcasecmp(strategy, "hash") == 0)
        return PARTITION_STRATEGY_HASH;

    ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
             errmsg("unrecognized partitioning strategy \"%s\"",
                    strategy)));
    return PARTITION_STRATEGY_LIST;     /* keep compiler quiet */

}

/*
 * Process pubobjspec_list to check for errors in any of the objects and
 * convert PUBLICATIONOBJ_CONTINUATION into appropriate PublicationObjSpecType.
 */
static void
preprocess_pubobj_list(List *pubobjspec_list, core_yyscan_t yyscanner)
{
    ListCell   *cell;
    PublicationObjSpec *pubobj;
    PublicationObjSpecType prevobjtype = PUBLICATIONOBJ_CONTINUATION;

    if (!pubobjspec_list)
        return;

    pubobj = (PublicationObjSpec *) linitial(pubobjspec_list);
    if (pubobj->pubobjtype == PUBLICATIONOBJ_CONTINUATION)
        ereport(ERROR,
                errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("invalid publication object list"),
                errdetail("One of TABLE or TABLES IN SCHEMA must be specified before a standalone table or schema name."),
                parser_errposition(pubobj->location));

    foreach(cell, pubobjspec_list)
    {
        pubobj = (PublicationObjSpec *) lfirst(cell);

        if (pubobj->pubobjtype == PUBLICATIONOBJ_CONTINUATION)
            pubobj->pubobjtype = prevobjtype;

        if (pubobj->pubobjtype == PUBLICATIONOBJ_TABLE)
        {
            /* relation name or pubtable must be set for this type of object */
            if (!pubobj->name && !pubobj->pubtable)
                ereport(ERROR,
                        errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("invalid table name"),
                        parser_errposition(pubobj->location));

            if (pubobj->name)
            {
                /* convert it to PublicationTable */
                PublicationTable *pubtable = makeNode(PublicationTable);

                pubtable->relation =
                    makeRangeVar(NULL, pubobj->name, pubobj->location);
                pubobj->pubtable = pubtable;
                pubobj->name = NULL;
            }
        }
        else if (pubobj->pubobjtype == PUBLICATIONOBJ_TABLES_IN_SCHEMA ||
                 pubobj->pubobjtype == PUBLICATIONOBJ_TABLES_IN_CUR_SCHEMA)
        {
            /* WHERE clause is not allowed on a schema object */
            if (pubobj->pubtable && pubobj->pubtable->whereClause)
                ereport(ERROR,
                        errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("WHERE clause not allowed for schema"),
                        parser_errposition(pubobj->location));

            /* Column list is not allowed on a schema object */
            if (pubobj->pubtable && pubobj->pubtable->columns)
                ereport(ERROR,
                        errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("column specification not allowed for schema"),
                        parser_errposition(pubobj->location));

            /*
             * We can distinguish between the different type of schema
             * objects based on whether name and pubtable is set.
             */
            if (pubobj->name)
                pubobj->pubobjtype = PUBLICATIONOBJ_TABLES_IN_SCHEMA;
            else if (!pubobj->name && !pubobj->pubtable)
                pubobj->pubobjtype = PUBLICATIONOBJ_TABLES_IN_CUR_SCHEMA;
            else
                ereport(ERROR,
                        errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("invalid schema name"),
                        parser_errposition(pubobj->location));
        }

        prevobjtype = pubobj->pubobjtype;
    }
}

/*----------
 * Recursive view transformation
 *
 * Convert
 *
 *     CREATE RECURSIVE VIEW relname (aliases) AS query
 *
 * to
 *
 *     CREATE VIEW relname (aliases) AS
 *         WITH RECURSIVE relname (aliases) AS (query)
 *         SELECT aliases FROM relname
 *
 * Actually, just the WITH ... part, which is then inserted into the original
 * view definition as the query.
 * ----------
 */
static Node *
makeRecursiveViewSelect(char *relname, List *aliases, Node *query)
{
    SelectStmt *s = makeNode(SelectStmt);
    WithClause *w = makeNode(WithClause);
    CommonTableExpr *cte = makeNode(CommonTableExpr);
    List       *tl = NIL;
    ListCell   *lc;

    /* create common table expression */
    cte->ctename = relname;
    cte->aliascolnames = aliases;
    cte->ctematerialized = CTEMaterializeDefault;
    cte->ctequery = query;
    cte->location = -1;

    /* create WITH clause and attach CTE */
    w->recursive = true;
    w->ctes = list_make1(cte);
    w->location = -1;

    /* create target list for the new SELECT from the alias list of the
     * recursive view specification */
    foreach (lc, aliases)
    {
        ResTarget *rt = makeNode(ResTarget);

        rt->name = NULL;
        rt->indirection = NIL;
        rt->val = makeColumnRef(strVal(lfirst(lc)), NIL, -1, 0);
        rt->location = -1;

        tl = lappend(tl, rt);
    }

    /* create new SELECT combining WITH clause, target list, and fake FROM
     * clause */
    s->withClause = w;
    s->targetList = tl;
    s->fromClause = list_make1(makeRangeVar(NULL, relname, -1));

    return (Node *) s;
}

/* parser_init()
 * Initialize to parse one query string
 */
void
parser_init(base_yy_extra_type *yyext)
{
    yyext->parsetree = NIL;     /* in case grammar forgets to set it */
}
