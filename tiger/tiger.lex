%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;
int commentLevel = 0;
int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}
%Start NORMAL COMMENT STRING
%%
<NORMAL> " "	 {adjust(); continue;}
<NORMAL> \n	 {adjust(); EM_newline(); continue;}
<NORMAL> ","	 {adjust(); return COMMA;}
<NORMAL> ":"      {adjust(); return COLON;}
<NORMAL> ";"      {adjust(); return SEMICOLON;}
<NORMAL> "("      {adjust(); return LPAREN;}
<NORMAL> ")"      {adjust(); return RPAREN;}
<NORMAL> "["      {adjust(); return LBRACK;}
<NORMAL> "]"      {adjust(); return RBRACK;}
<NORMAL> "{"      {adjust(); return LBRACE;}
<NORMAL> "}"      {adjust(); return RBRACE;}
<NORMAL> "."      {adjust(); return DOT;}
<NORMAL> "+"      {adjust(); return PLUS;}
<NORMAL> "-"      {adjust(); return MINUS;}
<NORMAL> "*"      {adjust(); return TIMES;}
<NORMAL> "/"      {adjust(); return DIVIDE;}
<NORMAL> "="      {adjust(); return EQ;}
<NORMAL> "<>"     {adjust(); return NEQ;}
<NORMAL> "<"      {adjust(); return LT;}
<NORMAL> "<="     {adjust(); return LE;}
<NORMAL> ">"      {adjust(); return GT;}
<NORMAL> ">="     {adjust(); return GE;}
<NORMAL> "&"      {adjust(); return AND;}
<NORMAL> "|"      {adjust(); return OR;}
<NORMAL> ":="     {adjust(); return ASSIGN;}
<NORMAL> "/*"     {adjust(); commentLevel ++; BEGIN COMMENT;}

<NORMAL> "\""     {adjust(); } //string?????

<NORMAL> "*/"     {adjust(); EM_error(EM_tokPos,"illegal token");}
<NORMAL> while    {adjust(); return WHILE;}
<NORMAL> for  	 {adjust(); return FOR;}
<NORMAL> to       {adjust(); return TO;}
<NORMAL> break    {adjust(); return BREAK;}
<NORMAL> let      {adjust(); return LET;}
<NORMAL> in       {adjust(); return IN;}
<NORMAL> end      {adjust(); return END;}
<NORMAL> function {adjust(); return FUNCTION;}
<NORMAL> var      {adjust(); return VAR;}
<NORMAL> type     {adjust(); return TYPE;}
<NORMAL> array    {adjust(); return ARRAY;}
<NORMAL> if       {adjust(); return IF;}
<NORMAL> then     {adjust(); return THEN;}
<NORMAL> else     {adjust(); return ELSE;}
<NORMAL> do       {adjust(); return DO;}
<NORMAL> of       {adjust(); return OF;}
<NORMAL> nil      {adjust(); return NIL;}
<NORMAL> [0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
<NORMAL> [a-zA-Z][a-zA-Z0-9_]* {adjust(); yylval.sval = String(yytext); return ID;}
<COMMENT> "/*"   {adjust(); commentLevel ++;}
<COMMENT> "*/"   {adjust(); commentLevel --; if (commentLevel == 0) {BEGIN NORMAL;}}
<COMMENT> "."    {adjust();}
.	 {BEGIN NORMAL; yyless(1);}


