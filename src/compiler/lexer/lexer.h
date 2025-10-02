#pragma once
#include <stdbool.h>
#include "../../shared/tokens.h"

// Lexer state
typedef struct {
    const char* source;        // Source code
    const char* current;       // Current position
    uint32_t line;             // Current line number
    uint32_t col;              // Current column number
    Token current_token;       // The most recently scanned token
    Token peek_token;          // The next token after current_token
} Lexer;

// Function declarations
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
bool lexer_is_at_end(Lexer* lexer);

// Internal helper functions (for lexer.c)
char lexer_peek(Lexer* lexer);
char lexer_peek_next(Lexer* lexer);
char lexer_advance(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
Token lexer_make_token(Lexer* lexer, TokenKind kind);
Token lexer_make_literal_token(Lexer* lexer, TokenKind kind);
Token lexer_make_identifier_token(Lexer* lexer);
Token lexer_make_number_token(Lexer* lexer);
Token lexer_make_string_token(Lexer* lexer);
Token lexer_make_operator_token(Lexer* lexer, char c);
bool lexer_is_alpha(char c);
bool lexer_is_digit(char c);
bool lexer_is_alphanumeric(char c);
bool lexer_match(Lexer* lexer, char expected);
bool lexer_string_equals(const char* str1, const char* str2, uint32_t len);
