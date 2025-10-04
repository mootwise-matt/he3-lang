#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../lexer/lexer.h"
#include "../../shared/ast/ast.h"

// Parser state
typedef struct {
    Lexer* lexer;
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
} Parser;

// Error handling
typedef struct {
    char message[256];
    uint32_t line;
    uint32_t column;
} ParseError;

typedef struct {
    ParseError* errors;
    uint32_t count;
    uint32_t capacity;
} ErrorList;

// Parser creation and destruction
Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);

// Main parsing functions
Ast* parse_compilation_unit(Parser* parser);
Ast* parse_domain_declaration(Parser* parser);
Ast* parse_import_declaration(Parser* parser);
Ast* parse_class_declaration(Parser* parser);
Ast* parse_record_declaration(Parser* parser);
Ast* parse_enum_declaration(Parser* parser);
Ast* parse_interface_declaration(Parser* parser);

// Method and property parsing
Ast* parse_method_declaration(Parser* parser);
Ast* parse_property_declaration(Parser* parser);
Ast* parse_constructor_declaration(Parser* parser);

// Statement parsing
Ast* parse_statement(Parser* parser);
Ast* parse_block_statement(Parser* parser);
Ast* parse_variable_declaration(Parser* parser);
Ast* parse_if_statement(Parser* parser);
Ast* parse_while_statement(Parser* parser);
Ast* parse_for_statement(Parser* parser);
Ast* parse_match_statement(Parser* parser);
Ast* parse_case_statement(Parser* parser);
Ast* parse_return_statement(Parser* parser);
Ast* parse_expression_statement(Parser* parser);

// Expression parsing
Ast* parse_expression(Parser* parser);
Ast* parse_new_expression(Parser* parser);
Ast* parse_assignment(Parser* parser);
Ast* parse_or_expression(Parser* parser);
Ast* parse_and_expression(Parser* parser);
Ast* parse_equality_expression(Parser* parser);
Ast* parse_comparison_expression(Parser* parser);
Ast* parse_term_expression(Parser* parser);
Ast* parse_factor_expression(Parser* parser);
Ast* parse_unary_expression(Parser* parser);
Ast* parse_call_expression(Parser* parser);
Ast* parse_primary_expression(Parser* parser);
Ast* finish_call_expression(Parser* parser, Ast* callee);

// Type parsing
Ast* parse_type(Parser* parser);
Ast* parse_type_arguments(Parser* parser);
Ast* parse_qualified_name(Parser* parser);
Ast* parse_type_list(Parser* parser);

// Parameter and field parsing
Ast* parse_parameter_list(Parser* parser);
Ast* parse_parameter(Parser* parser);
Ast* parse_field_declaration(Parser* parser);

// Declaration parsing
Ast* parse_class_declaration(Parser* parser);
Ast* parse_method_declaration(Parser* parser);
Ast* parse_record_declaration(Parser* parser);
Ast* parse_enum_declaration(Parser* parser);
Ast* parse_enum_variant(Parser* parser);
Ast* parse_interface_declaration(Parser* parser);
Ast* parse_interface_member(Parser* parser);
Ast* parse_property_declaration(Parser* parser);
Ast* parse_constructor_declaration(Parser* parser);

// Pattern matching
Ast* parse_pattern(Parser* parser);
Ast* parse_literal_pattern(Parser* parser);
Ast* parse_identifier_pattern(Parser* parser);
Ast* parse_constructor_pattern(Parser* parser);

// Utility functions
void parser_advance(Parser* parser);
Token parser_consume(Parser* parser, TokenKind kind, const char* message);
bool parser_check(Parser* parser, TokenKind kind);
bool parser_match(Parser* parser, TokenKind kind);
bool parser_is_at_end(Parser* parser);

// Error handling
void parser_error(Parser* parser, const char* message);
void parser_error_at_current(Parser* parser, const char* message);
void parser_error_at_previous(Parser* parser, const char* message);
void parser_synchronize(Parser* parser);

// AST creation helpers
Ast* create_ast_node(AstKind kind);
Ast* create_ast_literal(TokenKind token_kind, Token token);
Ast* create_ast_identifier(Token token);
Ast* create_ast_binary(Ast* left, Token operator, Ast* right);
Ast* create_ast_unary(Token operator, Ast* right);
Ast* create_ast_call(Ast* callee, Ast* arguments);
Ast* create_ast_field_access(Ast* object, Token field);
