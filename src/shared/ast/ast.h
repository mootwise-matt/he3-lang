#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../tokens.h"

// AST node kinds for He³ parser
typedef enum {
  // Compilation unit
  AST_COMPUNIT,
  
  // Top-level declarations
  AST_DOMAIN, AST_IMPORT, AST_CLASS, AST_RECORD, AST_ENUM, AST_INTERFACE,
  
  // Class members
  AST_FIELD, AST_METHOD, AST_CONSTRUCTOR, AST_PROPERTY,
  
  // Statements
  AST_BLOCK, AST_VAR_DECL, AST_ASSIGN, AST_IF, AST_WHILE, AST_FOR, AST_MATCH,
  AST_RETURN, AST_EXPR_STMT, AST_CASE,
  
  // Expressions
  AST_BINARY, AST_UNARY, AST_CALL, AST_FIELD_ACCESS, AST_INDEX_ACCESS, AST_NEW,
  AST_LITERAL, AST_IDENTIFIER, AST_LAMBDA, AST_ARGUMENTS,
  
  // Types
  AST_TYPE, AST_TYPE_ARGS, AST_NULLABLE_TYPE, AST_GENERIC_TYPE,
  
  // Patterns
  AST_PATTERN, AST_LITERAL_PATTERN, AST_IDENTIFIER_PATTERN, AST_CONSTRUCTOR_PATTERN,
  
  // Security
  AST_SECURITY_KEY, AST_SECURE_DOMAIN, AST_HANDSHAKE,
  
  // Events
  AST_EVENT_DECL, AST_PUBLISH, AST_SUBSCRIBE, AST_UNSUBSCRIBE,
  
  // Qualified names
  AST_QUALIFIED_NAME
} AstKind;

// AST node structure
typedef struct Ast Ast;
struct Ast {
  AstKind kind;
  char* identifier;    // Name or identifier
  const char* text;          // Source text for literals
  uint32_t line, col;        // Source location
  
  // Children nodes
  Ast** children;
  uint32_t child_count;
  uint32_t child_capacity;
  
  // Type information (filled during resolution)
  uint32_t type_index;       // Index in type table
  bool is_resolved;          // Whether type has been resolved
  bool is_mutable;           // Whether variable is mutable (var vs let)
  
  // Additional data
  union {
    int64_t int_value;
    double float_value;
    bool bool_value;
    uint32_t string_offset;  // Offset in string table
    Token token;             // For literal patterns
  } literal;
  
  // Operator for binary/unary expressions
  TokenKind operator;
};

// Function declarations
Ast* ast_create(AstKind kind, const char* identifier, uint32_t line, uint32_t col);
void ast_destroy(Ast* node);
void ast_add_child(Ast* parent, Ast* child);
Ast* ast_get_child(Ast* parent, uint32_t index);
void ast_set_literal_int(Ast* node, int64_t value);
void ast_set_literal_float(Ast* node, double value);
void ast_set_literal_bool(Ast* node, bool value);
void ast_set_literal_string(Ast* node, const char* value);

// AST traversal
void ast_print(Ast* node, int depth);
const char* ast_kind_to_string(AstKind kind);
bool ast_is_expression(AstKind kind);
bool ast_is_statement(AstKind kind);
bool ast_is_declaration(AstKind kind);
