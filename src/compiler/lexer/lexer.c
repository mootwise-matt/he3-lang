#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Lexer creation and destruction
Lexer* lexer_create(const char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->col = 1;
    
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

// Helper functions
char lexer_peek(Lexer* lexer) {
    if (lexer_is_at_end(lexer)) return '\0';
    return *lexer->current;
}

char lexer_peek_next(Lexer* lexer) {
    if (lexer_is_at_end(lexer)) return '\0';
    return *(lexer->current + 1);
}

char lexer_advance(Lexer* lexer) {
    if (lexer_is_at_end(lexer)) return '\0';
    char c = *lexer->current;
    lexer->current++;
    if (c == '\n') {
        lexer->line++;
        lexer->col = 1;
    } else {
        lexer->col++;
    }
    return c;
}

bool lexer_is_at_end(Lexer* lexer) {
    return *lexer->current == '\0';
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (!lexer_is_at_end(lexer)) {
        char c = lexer_peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                lexer_advance(lexer);
                break;
            case '\n':
                lexer_advance(lexer);
                lexer->line++;
                lexer->col = 1;
                break;
            case '/':
                if (lexer_peek_next(lexer) == '/') {
                    // Line comment
                    while (lexer_peek(lexer) != '\n' && !lexer_is_at_end(lexer)) {
                        lexer_advance(lexer);
                    }
                } else if (lexer_peek_next(lexer) == '*') {
                    // Block comment
                    lexer_advance(lexer); // consume '/'
                    lexer_advance(lexer); // consume '*'
                    while (!lexer_is_at_end(lexer)) {
                        if (lexer_peek(lexer) == '*' && lexer_peek_next(lexer) == '/') {
                            lexer_advance(lexer); // consume '*'
                            lexer_advance(lexer); // consume '/'
                            break;
                        }
                        lexer_advance(lexer);
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token lexer_make_token(Lexer* lexer, TokenKind kind) {
    Token token;
    token.kind = kind;
    token.start = lexer->current - 1;
    token.len = 1;
    token.line = lexer->line;
    token.col = lexer->col - 1;
    return token;
}

Token lexer_make_literal_token(Lexer* lexer, TokenKind kind) {
    Token token = lexer_make_token(lexer, kind);
    
    // Set literal values
    switch (kind) {
        case TK_INT:
            token.literal.int_value = strtol(token.start, NULL, 10);
            break;
        case TK_FLOAT:
            token.literal.float_value = strtod(token.start, NULL);
            break;
        case TK_STRING:
            // Remove quotes and handle escapes
            token.start++;
            token.len -= 2;
            token.literal.string_value = token.start;
            break;
        case TK_TRUE:
            token.literal.bool_value = true;
            break;
        case TK_FALSE:
            token.literal.bool_value = false;
            break;
        default:
            break;
    }
    
    return token;
}

Token lexer_make_number_literal_token(Lexer* lexer, TokenKind kind, const char* start, size_t len) {
    Token token;
    token.kind = kind;
    token.start = start;
    token.len = len;
    token.line = lexer->line;
    token.col = lexer->col - len;
    
    // Set literal values
    switch (kind) {
        case TK_INT:
            token.literal.int_value = strtol(token.start, NULL, 10);
            break;
        case TK_FLOAT:
            token.literal.float_value = strtod(token.start, NULL);
            break;
        default:
            break;
    }
    
    return token;
}

Token lexer_make_identifier_token(Lexer* lexer) {
    const char* start = lexer->current - 1;
    while (lexer_is_alphanumeric(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }
    
    TokenKind kind = TK_IDENTIFIER;
    uint32_t len = lexer->current - start;
    
    // Check for keywords
    if (len == 2) {
        if (strncmp(start, "if", 2) == 0) kind = TK_IF;
        else if (strncmp(start, "in", 2) == 0) kind = TK_IN;
        else if (strncmp(start, "as", 2) == 0) kind = TK_AS;
        else if (strncmp(start, "Ok", 2) == 0) kind = TK_OK;
    } else if (len == 3) {
        if (strncmp(start, "var", 3) == 0) kind = TK_VAR;
        else if (strncmp(start, "let", 3) == 0) kind = TK_LET;
        else if (strncmp(start, "for", 3) == 0) kind = TK_FOR;
        else if (strncmp(start, "new", 3) == 0) kind = TK_NEW;
        else if (strncmp(start, "try", 3) == 0) kind = TK_TRY;
        else if (strncmp(start, "Err", 3) == 0) kind = TK_ERR;
    } else if (len == 4) {
        if (strncmp(start, "true", 4) == 0) kind = TK_TRUE;
        else if (strncmp(start, "null", 4) == 0) kind = TK_NULL;
        else if (strncmp(start, "void", 4) == 0) kind = TK_VOID;
        else if (strncmp(start, "enum", 4) == 0) kind = TK_ENUM;
        else if (strncmp(start, "else", 4) == 0) kind = TK_ELSE;
        else if (strncmp(start, "when", 4) == 0) kind = TK_WHEN;
        else if (strncmp(start, "Some", 4) == 0) kind = TK_SOME;
        else if (strncmp(start, "None", 4) == 0) kind = TK_NONE;
        else if (strncmp(start, "bool", 4) == 0) kind = TK_BOOL;
    } else if (len == 5) {
        if (strncmp(start, "false", 5) == 0) kind = TK_FALSE;
        else if (strncmp(start, "while", 5) == 0) kind = TK_WHILE;
        else if (strncmp(start, "match", 5) == 0) kind = TK_MATCH;
        else if (strncmp(start, "async", 5) == 0) kind = TK_ASYNC;
        else if (strncmp(start, "await", 5) == 0) kind = TK_AWAIT;
        else if (strncmp(start, "class", 5) == 0) kind = TK_CLASS;
        else if (strncmp(start, "float", 5) == 0) kind = TK_FLOAT_TYPE;
        else if (strncmp(start, "throw", 5) == 0) kind = TK_THROW;
        else if (strncmp(start, "catch", 5) == 0) kind = TK_CATCH;
        else if (strncmp(start, "debug", 5) == 0) kind = TK_DEBUG;
    } else if (len == 6) {
        if (strncmp(start, "domain", 6) == 0) kind = TK_DOMAIN;
        else if (strncmp(start, "import", 6) == 0) kind = TK_IMPORT;
        else if (strncmp(start, "record", 6) == 0) kind = TK_RECORD;
        else if (strncmp(start, "return", 6) == 0) kind = TK_RETURN;
        else if (strncmp(start, "public", 6) == 0) kind = TK_PUBLIC;
        else if (strncmp(start, "string", 6) == 0) kind = TK_STRING_TYPE;
        else if (strncmp(start, "object", 6) == 0) kind = TK_OBJECT;
        else if (strncmp(start, "delete", 6) == 0) kind = TK_DELETE;
        else if (strncmp(start, "module", 6) == 0) kind = TK_MODULE;
        else if (strncmp(start, "export", 6) == 0) kind = TK_EXPORT;
        else if (strncmp(start, "static", 6) == 0) kind = TK_STATIC;
        else if (strncmp(start, "Option", 6) == 0) {
            printf("DEBUG: Lexer found Option token\n");
            kind = TK_OPTION;
        }
        else if (strncmp(start, "Result", 6) == 0) kind = TK_RESULT;
    } else if (len == 7) {
        if (strncmp(start, "boolean", 7) == 0) kind = TK_BOOLEAN;
        else if (strncmp(start, "integer", 7) == 0) kind = TK_INTEGER;
        else if (strncmp(start, "finally", 7) == 0) kind = TK_FINALLY;
        else if (strncmp(start, "private", 7) == 0) kind = TK_PRIVATE;
        else if (strncmp(start, "publish", 7) == 0) kind = TK_PUBLISH;
    } else if (len == 8) {
        if (strncmp(start, "function", 8) == 0) kind = TK_FUNCTION;
        else if (strncmp(start, "property", 8) == 0) kind = TK_PROPERTY;
        else if (strncmp(start, "security", 8) == 0) kind = TK_SECURITY;
        else if (strncmp(start, "handshake", 8) == 0) kind = TK_HANDSHAKE;
        else if (strncmp(start, "subscribe", 8) == 0) kind = TK_SUBSCRIBE;
        else if (strncmp(start, "protected", 8) == 0) kind = TK_PROTECTED;
    } else if (len == 9) {
        if (strncmp(start, "interface", 9) == 0) kind = TK_INTERFACE;
        else if (strncmp(start, "procedure", 9) == 0) kind = TK_PROCEDURE;
        else if (strncmp(start, "unsubscribe", 9) == 0) kind = TK_UNSUBSCRIBE;
    } else if (len == 10) {
        if (strncmp(start, "constructor", 10) == 0) kind = TK_CONSTRUCTOR;
    }
    
    Token token;
    token.kind = kind;
    token.start = start;
    token.len = len;
    token.line = lexer->line;
    token.col = lexer->col - len;
    return token;
}

Token lexer_make_number_token(Lexer* lexer) {
    const char* start = lexer->current - 1;
    
    while (lexer_is_digit(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }
    
    // Look for decimal point
    if (lexer_peek(lexer) == '.' && lexer_is_digit(lexer_peek_next(lexer))) {
        lexer_advance(lexer); // consume '.'
        while (lexer_is_digit(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }
        return lexer_make_number_literal_token(lexer, TK_FLOAT, start, lexer->current - start);
    }
    
    return lexer_make_number_literal_token(lexer, TK_INT, start, lexer->current - start);
}

Token lexer_make_string_token(Lexer* lexer) {
    const char* start = lexer->current - 1; // Points to opening quote
    
    while (lexer_peek(lexer) != '"' && !lexer_is_at_end(lexer)) {
        if (lexer_peek(lexer) == '\n') {
            lexer->line++;
            lexer->col = 1;
        }
        lexer_advance(lexer);
    }
    
    if (lexer_is_at_end(lexer)) {
        // Unterminated string
        return lexer_make_token(lexer, TK_EOF);
    }
    
    lexer_advance(lexer); // consume closing quote
    
    // Create token with proper length (including quotes)
    Token token;
    token.kind = TK_STRING;
    token.start = start;
    token.len = lexer->current - start;
    token.line = lexer->line;
    token.col = lexer->col - token.len;
    
    // Set string value (without quotes)
    token.literal.string_value = start + 1; // Skip opening quote
    // Note: We don't null-terminate here since string_value is const char*
    // The actual string content is from start+1 to start+token.len-2
    
    return token;
}

Token lexer_make_operator_token(Lexer* lexer, char c) {
    switch (c) {
        case '(': return lexer_make_token(lexer, TK_LPAREN);
        case ')': return lexer_make_token(lexer, TK_RPAREN);
        case '{': return lexer_make_token(lexer, TK_LBRACE);
        case '}': return lexer_make_token(lexer, TK_RBRACE);
        case '[': return lexer_make_token(lexer, TK_LBRACK);
        case ']': return lexer_make_token(lexer, TK_RBRACK);
        case ';': return lexer_make_token(lexer, TK_SEMICOLON);
        case ',': return lexer_make_token(lexer, TK_COMMA);
        case '.': return lexer_make_token(lexer, TK_DOT);
        case '_': return lexer_make_token(lexer, TK_UNDERSCORE);
        case '@': return lexer_make_token(lexer, TK_AT);
        case '?': return lexer_make_token(lexer, TK_QUESTION);
        case ':': return lexer_make_token(lexer, TK_COLON);
        case '+': return lexer_make_token(lexer, TK_PLUS);
        case '-': 
            if (lexer_peek(lexer) == '>') {
                lexer_advance(lexer);
                return lexer_make_token(lexer, TK_ARROW);
            }
            return lexer_make_token(lexer, TK_MINUS);
        case '*': return lexer_make_token(lexer, TK_STAR);
        case '/': return lexer_make_token(lexer, TK_SLASH);
        case '%': return lexer_make_token(lexer, TK_MODULO);
        case '=': 
            if (lexer_peek(lexer) == '=') {
                lexer_advance(lexer);
                return lexer_make_token(lexer, TK_EQUAL);
            }
            if (lexer_peek(lexer) == '>') {
                lexer_advance(lexer);
                return lexer_make_token(lexer, TK_ARROW);
            }
            return lexer_make_token(lexer, TK_ASSIGN);
        case '!': return lexer_make_token(lexer, TK_NOT);
        case '<': return lexer_make_token(lexer, TK_LESS);
        case '>': return lexer_make_token(lexer, TK_GREATER);
        case '&': return lexer_make_token(lexer, TK_BIT_AND);
        case '|': return lexer_make_token(lexer, TK_BIT_OR);
        case '^': return lexer_make_token(lexer, TK_BIT_XOR);
        case '~': return lexer_make_token(lexer, TK_BIT_NOT);
        default: return lexer_make_token(lexer, TK_EOF);
    }
}

bool lexer_is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool lexer_is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool lexer_is_alphanumeric(char c) {
    return lexer_is_alpha(c) || lexer_is_digit(c);
}

bool lexer_match(Lexer* lexer, char expected) {
    if (lexer_is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer_advance(lexer);
    return true;
}

bool lexer_string_equals(const char* str1, const char* str2, uint32_t len) {
    return strncmp(str1, str2, len) == 0;
}

// Main lexer function
Token lexer_next_token(Lexer* lexer) {
    lexer_skip_whitespace(lexer);
    
    if (lexer_is_at_end(lexer)) {
        return lexer_make_token(lexer, TK_EOF);
    }
    
    char c = lexer_advance(lexer);
    
    if (lexer_is_alpha(c)) {
        return lexer_make_identifier_token(lexer);
    }
    
    if (lexer_is_digit(c)) {
        return lexer_make_number_token(lexer);
    }
    
    switch (c) {
        case '"':
            return lexer_make_string_token(lexer);
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ';':
        case ',':
        case '.':
        case '_':
        case '@':
        case '?':
        case ':':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '!':
        case '<':
        case '>':
        case '&':
        case '|':
        case '^':
        case '~':
            return lexer_make_operator_token(lexer, c);
        default:
            return lexer_make_token(lexer, TK_EOF);
    }
}

Token lexer_peek_token(Lexer* lexer) {
    // This is a simplified implementation
    // In a real lexer, you'd want to cache the peek token
    return lexer_next_token(lexer);
}

// Additional helper functions
const char* token_kind_to_string(TokenKind kind) {
    switch (kind) {
        case TK_EOF: return "EOF";
        case TK_IDENTIFIER: return "IDENTIFIER";
        case TK_INT: return "INT";
        case TK_FLOAT: return "FLOAT";
        case TK_STRING: return "STRING";
        case TK_BOOL: return "BOOL";
        case TK_NULL: return "NULL";
        case TK_DOMAIN: return "DOMAIN";
        case TK_CROWD: return "CROWD";
        case TK_IMPORT: return "IMPORT";
        case TK_CLASS: return "CLASS";
        case TK_RECORD: return "RECORD";
        case TK_ENUM: return "ENUM";
        case TK_INTERFACE: return "INTERFACE";
        case TK_EXTENDS: return "EXTENDS";
        case TK_IMPLEMENTS: return "IMPLEMENTS";
        case TK_CONSTRUCTOR: return "CONSTRUCTOR";
        case TK_FUNCTION: return "FUNCTION";
        case TK_PROCEDURE: return "PROCEDURE";
        case TK_VAR: return "VAR";
        case TK_LET: return "LET";
        case TK_RETURN: return "RETURN";
        case TK_PROPERTY: return "PROPERTY";
        case TK_STATIC: return "STATIC";
        case TK_IF: return "IF";
        case TK_ELSE: return "ELSE";
        case TK_WHILE: return "WHILE";
        case TK_FOR: return "FOR";
        case TK_IN: return "IN";
        case TK_MATCH: return "MATCH";
        case TK_WHEN: return "WHEN";
        case TK_ASYNC: return "ASYNC";
        case TK_AWAIT: return "AWAIT";
        case TK_AS: return "AS";
        case TK_SOME: return "SOME";
        case TK_NONE: return "NONE";
        case TK_OK: return "OK";
        case TK_ERR: return "ERR";
        case TK_TRUE: return "TRUE";
        case TK_FALSE: return "FALSE";
        case TK_INTEGER: return "INTEGER";
        case TK_FLOAT_TYPE: return "FLOAT_TYPE";
        case TK_BOOLEAN: return "BOOLEAN";
        case TK_STRING_TYPE: return "STRING_TYPE";
        case TK_VOID: return "VOID";
        case TK_OBJECT: return "OBJECT";
        case TK_PLUS: return "PLUS";
        case TK_MINUS: return "MINUS";
        case TK_STAR: return "STAR";
        case TK_SLASH: return "SLASH";
        case TK_MODULO: return "MODULO";
        case TK_EQUAL: return "EQUAL";
        case TK_NOT_EQUAL: return "NOT_EQUAL";
        case TK_LESS: return "LESS";
        case TK_LESS_EQUAL: return "LESS_EQUAL";
        case TK_GREATER: return "GREATER";
        case TK_GREATER_EQUAL: return "GREATER_EQUAL";
        case TK_AND: return "AND";
        case TK_OR: return "OR";
        case TK_NOT: return "NOT";
        case TK_BIT_AND: return "BIT_AND";
        case TK_BIT_OR: return "BIT_OR";
        case TK_BIT_XOR: return "BIT_XOR";
        case TK_BIT_NOT: return "BIT_NOT";
        case TK_ASSIGN: return "ASSIGN";
        case TK_QUESTION: return "QUESTION";
        case TK_COLON: return "COLON";
        case TK_ARROW: return "ARROW";
        case TK_DOUBLE_ARROW: return "DOUBLE_ARROW";
        case TK_LPAREN: return "LPAREN";
        case TK_RPAREN: return "RPAREN";
        case TK_LBRACE: return "LBRACE";
        case TK_RBRACE: return "RBRACE";
        case TK_LBRACK: return "LBRACK";
        case TK_RBRACK: return "RBRACK";
        case TK_SEMICOLON: return "SEMICOLON";
        case TK_DOT: return "DOT";
        case TK_COMMA: return "COMMA";
        case TK_UNDERSCORE: return "UNDERSCORE";
        case TK_AT: return "AT";
        case TK_SECURITY: return "SECURITY";
        case TK_KEY: return "KEY";
        case TK_HANDSHAKE: return "HANDSHAKE";
        case TK_EVENT: return "EVENT";
        case TK_PUBLISH: return "PUBLISH";
        case TK_SUBSCRIBE: return "SUBSCRIBE";
        case TK_UNSUBSCRIBE: return "UNSUBSCRIBE";
        default: return "UNKNOWN";
    }
}

bool token_is_keyword(const char* text, uint32_t len) {
    (void)text;
    (void)len;
    // This is a simplified implementation
    // In a real lexer, you'd want to use a hash table or trie
    return false;
}

TokenKind token_keyword_to_kind(const char* text, uint32_t len) {
    (void)text;
    (void)len;
    // This is a simplified implementation
    // In a real lexer, you'd want to use a hash table or trie
    return TK_IDENTIFIER;
}

bool token_is_operator(TokenKind kind) {
    switch (kind) {
        case TK_PLUS:
        case TK_MINUS:
        case TK_STAR:
        case TK_SLASH:
        case TK_MODULO:
        case TK_EQUAL:
        case TK_NOT_EQUAL:
        case TK_LESS:
        case TK_LESS_EQUAL:
        case TK_GREATER:
        case TK_GREATER_EQUAL:
        case TK_AND:
        case TK_OR:
        case TK_NOT:
        case TK_BIT_AND:
        case TK_BIT_OR:
        case TK_BIT_XOR:
        case TK_BIT_NOT:
        case TK_ASSIGN:
        case TK_QUESTION:
        case TK_COLON:
        case TK_ARROW:
        case TK_DOUBLE_ARROW:
            return true;
        default:
            return false;
    }
}

bool token_is_literal(TokenKind kind) {
    switch (kind) {
        case TK_INT:
        case TK_FLOAT:
        case TK_STRING:
        case TK_BOOL:
        case TK_NULL:
        case TK_TRUE:
        case TK_FALSE:
            return true;
        default:
            return false;
    }
}

bool token_is_type_keyword(TokenKind kind) {
    switch (kind) {
        case TK_INTEGER:
        case TK_FLOAT_TYPE:
        case TK_BOOLEAN:
        case TK_STRING_TYPE:
        case TK_VOID:
        case TK_OBJECT:
            return true;
        default:
            return false;
    }
}

bool token_is_delimiter(TokenKind kind) {
    switch (kind) {
        case TK_LPAREN:
        case TK_RPAREN:
        case TK_LBRACE:
        case TK_RBRACE:
        case TK_LBRACK:
        case TK_RBRACK:
        case TK_SEMICOLON:
        case TK_DOT:
        case TK_COMMA:
        case TK_UNDERSCORE:
            return true;
        default:
            return false;
    }
}

bool token_is_security_keyword(TokenKind kind) {
    switch (kind) {
        case TK_SECURITY:
        case TK_KEY:
        case TK_HANDSHAKE:
        case TK_AT:
            return true;
        default:
            return false;
    }
}

bool token_is_event_keyword(TokenKind kind) {
    switch (kind) {
        case TK_EVENT:
        case TK_PUBLISH:
        case TK_SUBSCRIBE:
        case TK_UNSUBSCRIBE:
            return true;
        default:
            return false;
    }
}
