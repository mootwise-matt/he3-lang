#pragma once
#include <stdint.h>
#include <stdbool.h>

// Token kinds for He³ lexer
typedef enum {
  // End of file
  TK_EOF = 0,
  
  // Identifiers and literals
  TK_IDENTIFIER, TK_INT, TK_FLOAT, TK_STRING, TK_BOOL, TK_NULL,
  
  // Keywords - Core language
  TK_DOMAIN, TK_CROWD, TK_IMPORT, 
  TK_CLASS, TK_RECORD, TK_ENUM, TK_INTERFACE,
  TK_EXTENDS, TK_IMPLEMENTS, TK_CONSTRUCTOR, TK_FUNCTION, TK_PROCEDURE,
  TK_VAR, TK_LET, TK_NEW, TK_RETURN, TK_PROPERTY, TK_STATIC,
  TK_IF, TK_ELSE, TK_WHILE, TK_FOR, TK_IN, TK_MATCH, TK_WHEN,
  TK_ASYNC, TK_AWAIT, TK_AS,
  TK_SOME, TK_NONE, TK_OK, TK_ERR,
  TK_TRUE, TK_FALSE,
  
  // Type keywords
  TK_INTEGER, TK_FLOAT_TYPE, TK_BOOLEAN, TK_STRING_TYPE, TK_VOID, TK_OBJECT,
  
  // Operators - Arithmetic
  TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_MODULO,
  
  // Operators - Comparison
  TK_EQUAL, TK_NOT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_GREATER, TK_GREATER_EQUAL,
  
  // Operators - Logical
  TK_AND, TK_OR, TK_NOT,
  
  // Operators - Bitwise
  TK_BIT_AND, TK_BIT_OR, TK_BIT_XOR, TK_BIT_NOT,
  
  // Operators - Assignment
  TK_ASSIGN, TK_PLUS_ASSIGN, TK_MINUS_ASSIGN, TK_STAR_ASSIGN, TK_SLASH_ASSIGN,
  TK_MODULO_ASSIGN, TK_AND_ASSIGN, TK_OR_ASSIGN, TK_XOR_ASSIGN,
  
  // Operators - Other
  TK_QUESTION, TK_COLON, TK_ARROW, TK_DOUBLE_ARROW,
  
  // Delimiters
  TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_LBRACK, TK_RBRACK,
  TK_SEMICOLON, TK_DOT, TK_COMMA, TK_UNDERSCORE,
  
  // Comments
  TK_COMMENT_LINE, TK_COMMENT_BLOCK,
  
  // Security keywords
  TK_SECURITY, TK_KEY, TK_HANDSHAKE, TK_AT, TK_SECURE,
  
  // Event keywords
  TK_EVENT, TK_PUBLISH, TK_SUBSCRIBE, TK_UNSUBSCRIBE,
  
  // Lambda syntax
  TK_LAMBDA_START, TK_LAMBDA_END,
  
  // Pattern matching
  TK_WILDCARD, TK_PATTERN_MATCH,
  
  // Generic type syntax
  TK_LT, TK_GT,
  
  // Nullable type syntax
  TK_QUESTION_MARK,
  
  // String interpolation
  TK_STRING_START, TK_STRING_END, TK_STRING_INTERP,
  
  // Error handling
  TK_THROW, TK_TRY, TK_CATCH, TK_FINALLY,
  
  // Memory management
  TK_DELETE, TK_GC,
  
  // Module system
  TK_MODULE, TK_EXPORT, TK_PUBLIC, TK_PRIVATE, TK_PROTECTED,
  
  // Debugging
  TK_DEBUG, TK_BREAKPOINT, TK_ASSERT,
  
  // Documentation
  TK_DOC_COMMENT, TK_DOC_START, TK_DOC_END
} TokenKind;

// Token structure
typedef struct {
  TokenKind kind;
  const char* start;
  uint32_t len;
  uint32_t line, col;
  union {
    int64_t int_value;
    double float_value;
    bool bool_value;
    const char* string_value;
  } literal;
} Token;

// Function declarations
const char* token_kind_to_string(TokenKind kind);
bool token_is_keyword(const char* text, uint32_t len);
TokenKind token_keyword_to_kind(const char* text, uint32_t len);
bool token_is_operator(TokenKind kind);
bool token_is_literal(TokenKind kind);
bool token_is_type_keyword(TokenKind kind);
bool token_is_delimiter(TokenKind kind);
bool token_is_security_keyword(TokenKind kind);
bool token_is_event_keyword(TokenKind kind);
