# He³ EBNF Grammar

```ebnf
(* ===== LEXICAL ===== *)
letter      = 'A'..'Z' | 'a'..'z' | '_' ;
digit       = '0'..'9' ;
identifier  = letter { letter | digit } ;

int_lit     = digit { digit | '_' } ;
float_lit   = digit { digit | '_' } "." digit { digit | '_' } ;
string_lit  = '"' { ? UTF-8 char except " and unescaped NL ? | escape } '"' ;
escape      = "\" ( '"' | "\" | "n" | "r" | "t" | "u{" 1*6 ?hex? "}" ) ;
bool_lit    = "true" | "false" ;
null_lit    = "null" ;

comment     = "//" { ? not NL ? } | "/*" { ? any, nestable ? } "*/" ;

qname       = identifier { "." identifier } ;

(* ===== KEYWORDS ===== *)
keyword =
  "domain" | "crowd" | "import" |
  "class" | "record" | "enum" | "interface" |
  "extends" | "implements" |
  "constructor" | "function" | "procedure" |
  "var" | "let" | "return" | "property" |
  "if" | "else" | "while" | "for" | "in" |
  "match" | "when" | "async" | "await" |
  "Some" | "None" | "Ok" | "Err" |
  "object" | "integer" | "float" | "boolean" | "string" | "void" ;

(* ===== COMPILATION UNIT ===== *)
compilation_unit = domain_decl { import_decl } { top_level_decl } ;

domain_decl = "domain" qname [ "crowd" "=" crowd_list ] ";" ;
crowd_list  = identifier { "," identifier } ;
import_decl = "import" qname [ ".*" | "." identifier ] ";" ;

(* ===== DECLARATIONS ===== *)
top_level_decl =
    class_decl | record_decl | enum_decl | interface_decl ;

class_decl =
  "class" identifier [ type_params ]
  [ "extends" qname ] [ "implements" type_list ]
  class_body ;

record_decl =
  "record" identifier [ type_params ]
  class_body ;

enum_decl =
  "enum" identifier [ type_params ]
  "{" enum_variant { "," enum_variant } [ "," ] "}" ;

interface_decl =
  "interface" identifier [ type_params ]
  "{" { iface_member } "}" ;

type_params   = "<" type_param { "," type_param } ">" ;
type_param    = identifier [ ":" type_list ] ;
type_list     = type { "," type } ;

enum_variant  = identifier [ "(" [ param_types ] ")" ] ;
param_types   = type { "," type } ;

class_body    = "{" { class_member } "}" ;

class_member  =
    field_decl
  | ctor_decl
  | method_decl
  | prop_decl ;

field_decl    = ( "var" | "let" ) identifier ":" type [ "=" expr ] ";" ;

ctor_decl     = "constructor" "(" [ param_list ] ")" block ;

method_decl   =
  [ "async" ] ( "function" | "procedure" ) identifier [ type_params ]
  "(" [ param_list ] ")" [ ":" type ] block ;

prop_decl     = "property" identifier ":" type
                "{" [ "get" block ] [ "set" block ] "}" ;

iface_member  =
  [ "async" ] ( "function" | "procedure" ) identifier [ type_params ]
  "(" [ param_list ] ")" [ ":" type ] ";" ;

param_list    = param { "," param } ;
param         = identifier ":" type ;

(* ===== TYPES ===== *)
type =
    qname [ type_args ] [ nullable ]
  | "integer" [ nullable ]
  | "float"   [ nullable ]
  | "boolean" [ nullable ]
  | "string"  [ nullable ]
  | "object"  [ nullable ]
  | "void" ;
nullable    = "?" ;
type_args   = "<" type { "," type } ">" ;

(* ===== STATEMENTS ===== *)
block       = "{" { stmt } "}" ;

stmt =
    var_decl | assign_stmt | expr_stmt | return_stmt
  | if_stmt | while_stmt | for_stmt | match_stmt
  | block ;

var_decl      = ( "var" | "let" ) identifier ":" type [ "=" expr ] ";" ;
assign_stmt   = lvalue ":=" expr ";" ;
lvalue        = qname | qname "[" expr "]" | qname "." identifier ;
expr_stmt     = expr ";" ;
return_stmt   = "return" [ expr ] ";" ;

if_stmt       = "if" "(" expr ")" block [ "else" block ] ;
while_stmt    = "while" "(" expr ")" block ;
for_stmt      = "for" identifier "in" expr block ;

match_stmt    = "match" "(" expr ")" "{" match_arm { "," match_arm } [ "," ] "}" ;
match_arm     = pattern [ "when" expr ] "=>" ( expr | block ) ;

(* ===== EXPRESSIONS ===== *)
expr          = logic_or ;

logic_or      = logic_and { "||" logic_and } ;
logic_and     = bit_or    { "&&" bit_or } ;
bit_or        = bit_xor   { "|"  bit_xor } ;
bit_xor       = bit_and   { "^"  bit_and } ;
bit_and       = equality  { "&"  equality } ;
equality      = compare   { ( "==" | "!=" ) compare } ;
compare       = additive  { ( "<" | "<=" | ">" | ">=" ) additive } ;
additive      = multiplicative { ( "+" | "-" ) multiplicative } ;
multiplicative= unary { ( "*" | "/" | "%" ) unary } ;

unary         = [ "!" | "-" ] unary
              | "await" unary
              | postfix ;

postfix       = primary { call_suffix | index_suffix | field_suffix } ;
call_suffix   = "(" [ arg_list ] ")" ;
index_suffix  = "[" expr "]" ;
field_suffix  = "." identifier ;

primary       =
    int_lit | float_lit | string_lit | bool_lit | null_lit
  | "Some" "(" expr ")" | "None"
  | "Ok" "(" expr ")"   | "Err" "(" expr ")"
  | "(" expr ")"
  | qname
  | lambda ;

arg_list      = expr { "," expr } ;

lambda        = "|" [ lambda_params ] "|" ( expr | block ) ;
lambda_params = identifier { "," identifier } ;

(* ===== PATTERNS ===== *)
pattern =
    "_"
  | literal_pat
  | ident_pat
  | qname "(" [ pat_list ] ")" ;

literal_pat = int_lit | float_lit | string_lit | bool_lit | "None" | "null" ;
ident_pat   = identifier ;
pat_list    = pattern { "," pattern } ;
```

# End of He³ Grammar
