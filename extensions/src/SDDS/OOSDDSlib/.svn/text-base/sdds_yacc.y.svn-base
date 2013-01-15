/* $Log: not supported by cvs2svn $
 * Revision 1.2  1997/02/12 23:34:10  saunders
 * Intermediate.
 * */
/*************************************************************************/
/* FILE:      sdds_yacc.y                                                */
/* Author:    Claude Saunders                                            */
/* Purpose:   yacc input file                                            */
/*                                                                       */
/*************************************************************************/

%{
#include <stdio.h>
#include <string.h>
#include "sddsLayout.hh"

/* Parsing support functions */
int headerParse(FILE *fp, sddsLayout *layout);
int yyerror(char *err);
int semanticError(char *msg);
int yylex(void);
int yyreset();

/* Keep track of input line number */
static int line_num;
static char errBuf[SDDS_MAX_STRING_SIZE];

/* When we parse header, put information into layout */
static sddsLayout *parseLayout;
static sddsParameterDefinition *parameterDef;

%}

%start header

%token <Str> WORD
%left SDDS1
%left COMMA 
%left O_DELIMITER C_DELIMITER EQUALS
%left DESCRIPTION PARAMETER COLUMN DATA
%left MODE TEXT CONTENTS FORMAT_STRING NO_ROW_COUNTS NAME TYPE
%left WORD
%left NOWHERE 

%union
{
    int	Int;
    char Char;
    char Str[SDDS_MAX_STRING_SIZE];
    double Real;
}

%%

header:
  SDDS1 {parseLayout->setVersion(1);} sdds1Declarations sdds1DataStatement
  ;

sdds1Declarations:
  /* null */
  |
  sdds1Declarations sdds1Declaration
  ;

sdds1DataStatement:
  O_DELIMITER DATA sdds1DataEntries C_DELIMITER
  {
    return(0);
  }
  ;

sdds1Declaration:
  O_DELIMITER sdds1Group C_DELIMITER
  ;

sdds1Group:
  DESCRIPTION {printf("found description-->\n");} sdds1DescriptionEntries |
  PARAMETER {
    parameterDef = new sddsParameterDefinition;
  } sdds1ParameterEntries {
    parseLayout->commitParameterDefinition(parameterDef);
  } |
  COLUMN {printf("found column-->\n");} sdds1ColumnEntries
  ;
  
sdds1DataEntries:
  /* null */
  |
  sdds1DataEntries sdds1DataEntry |
  sdds1DataEntries COMMA sdds1DataEntry
  ;

sdds1DataEntry:
  MODE EQUALS WORD {printf("mode equals %s\n",$<Str>3);} |
  NO_ROW_COUNTS EQUALS WORD {printf("no_row_counts equals 1");}
  ;

sdds1DescriptionEntries:
  /* null */
  |
  sdds1DescriptionEntries sdds1DescriptionEntry |
  sdds1DescriptionEntries COMMA sdds1DescriptionEntry
  ;

sdds1DescriptionEntry:
  TEXT EQUALS WORD {parseLayout->setDescription($<Str>3);} |
  CONTENTS EQUALS WORD {parseLayout->setContents($<Str>3);}
  ;

sdds1ParameterEntries:
  /* null */
  |
  sdds1ParameterEntries sdds1ParameterEntry |
  sdds1ParameterEntries COMMA sdds1ParameterEntry
  ;

sdds1ParameterEntry:
  NAME EQUALS WORD {
    parameterDef->setName($<Str>3);
  } |
  TYPE EQUALS WORD {
    if (parameterDef->setType($<Str>3)) {
      sprintf(errBuf,"%s is an illegal type",$<Str>3);
      semanticError(errBuf);
      return(-1);
    }
  } |
  FORMAT_STRING EQUALS WORD {
    parameterDef->setFormatString($<Str>3);
  }
  ;

sdds1ColumnEntries:
  /* null */
  |
  sdds1ColumnEntries sdds1ColumnEntry |
  sdds1ColumnEntries COMMA sdds1ColumnEntry
  ;

sdds1ColumnEntry:
  NAME EQUALS WORD |
  TYPE EQUALS WORD |
  FORMAT_STRING EQUALS WORD
  ;


%%
 
#include "sdds_lex.cc"
 
int yyerror(char *str)
{
  char buf[SDDS_MAX_STRING_SIZE];
  sprintf(buf,"Unable to read layout--syntax error, line %d : %s",line_num,yytext);
  parseLayout->err.SDDS_SetError(buf);
  return(0);
}

int semanticError(char *str) {
  char buf[SDDS_MAX_STRING_SIZE];
  sprintf(buf,"Unable to read layout--%s, line %d",str,line_num);
  parseLayout->err.SDDS_SetError(buf);
  return(0);
}

int headerParse(FILE *fp, sddsLayout *layout)
{
  parseLayout = layout;
  yyin = fp;
  yyreset();
  return(yyparse());
}

yywrap() {
   return(1);
}


