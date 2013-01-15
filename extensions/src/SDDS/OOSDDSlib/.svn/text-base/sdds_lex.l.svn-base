/* $Log: not supported by cvs2svn $
 * Revision 1.1  1997/02/11 17:31:39  saunders
 * Initial work.
 * */
/*************************************************************************/
/* FILE:      sdds_flex.l                                                */
/* Author:    Claude Saunders                                            */
/* Purpose:   flex input file                                            */
/*            Lexicon Definition for:                                    */
/*		sdds1 layout                                             */
/*************************************************************************/

%{
int yyreset()
{
    line_num = 1;
    BEGIN INITIAL;
    return 0;
}
			  
%}

string [^ \n\t,&\"]+
qstring \"[^\n,&\"]+\"
end [,]*[ \t]*&end

%s COMMENT
%%


"SDDS1"		{ return(SDDS1); }
"description"	{ return(DESCRIPTION); }
"parameter"	{ return(PARAMETER); }
"column"	{ return(COLUMN); }
"data"		{ return(DATA); }
"="		{ return(EQUALS); }

"mode"		{ return(MODE); }
"text"		{ return(TEXT); }
"contents"	{ return(CONTENTS); }
"format_string" { return(FORMAT_STRING); }
"no_row_counts" { return(NO_ROW_COUNTS); }
"name"		{ return(NAME); }
"type"		{ return(TYPE); }

{qstring} { 
	   /* strip off quotes */
	   strcpy(yylval.Str,yytext+1);
	   yylval.Str[strlen(yylval.Str)-1] = '\0';
           return(WORD);	
          }

{string} { 
           strcpy(yylval.Str,yytext);
           return(WORD);	
         }

"&"	{ return(O_DELIMITER); }
{end}	{ return(C_DELIMITER); }
","	{ return(COMMA);	}


^!.*\n { line_num++; } /* single line comments only */
.	;
\n	{ line_num++; }

%%
