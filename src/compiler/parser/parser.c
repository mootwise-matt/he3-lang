#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parser creation and destruction
Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->had_error = false;
    parser->panic_mode = false;
    
    // Initialize current and previous tokens
    parser_advance(parser);
    // Only advance once to start with the first token as current
    
    return parser;
}

void parser_destroy(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

// Utility functions
void parser_advance(Parser* parser) {
    parser->previous = parser->current;
    parser->current = lexer_next_token(parser->lexer);
}

Token parser_consume(Parser* parser, TokenKind kind, const char* message) {
    if (parser->current.kind == kind) {
        parser_advance(parser);
        return parser->previous;
    }
    
    parser_error_at_current(parser, message);
    return parser->current; // Return current token on error
}

bool parser_check(Parser* parser, TokenKind kind) {
    if (parser_is_at_end(parser)) return false;
    return parser->current.kind == kind;
}

bool parser_match(Parser* parser, TokenKind kind) {
    if (parser_check(parser, kind)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

bool parser_is_at_end(Parser* parser) {
    return parser->current.kind == TK_EOF;
}

// Error handling
void parser_error(Parser* parser, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    parser->had_error = true;
    
    fprintf(stderr, "Parse error at line %d: %s\n", parser->current.line, message);
}

void parser_error_at_current(Parser* parser, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    parser->had_error = true;
    
    fprintf(stderr, "Parse error at line %d: %s\n", parser->current.line, message);
}

void parser_error_at_previous(Parser* parser, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    parser->had_error = true;
    
    fprintf(stderr, "Parse error at line %d: %s\n", parser->previous.line, message);
}

void parser_synchronize(Parser* parser) {
    parser->panic_mode = false;
    
    while (!parser_is_at_end(parser)) {
        if (parser->previous.kind == TK_SEMICOLON) return;
        
        switch (parser->current.kind) {
            case TK_CLASS:
            case TK_FUNCTION:
            case TK_VAR:
            case TK_LET:
            case TK_IF:
            case TK_WHILE:
            case TK_FOR:
            case TK_RETURN:
                return;
            default:
                break;
        }
        
        parser_advance(parser);
    }
}

// AST creation helpers
Ast* create_ast_node(AstKind kind) {
    return ast_create_node(kind);
}

Ast* create_ast_literal(TokenKind token_kind, Token token) {
    Ast* node = ast_create_node(AST_LITERAL);
    if (!node) return NULL;
    
    node->literal.token = token;
    node->line = token.line;
    node->col = token.col;
    
    // Set literal values based on token kind
    switch (token_kind) {
        case TK_INT:
            node->literal.int_value = strtol(token.start, NULL, 10);
            break;
        case TK_FLOAT:
            node->literal.float_value = strtod(token.start, NULL);
            break;
        case TK_STRING:
            // Copy string value
            node->literal.string_value = malloc(token.len + 1);
            if (node->literal.string_value) {
                strncpy(node->literal.string_value, token.start, token.len);
                node->literal.string_value[token.len] = '\0';
            }
            break;
        case TK_TRUE:
            node->literal.bool_value = true;
            break;
        case TK_FALSE:
            node->literal.bool_value = false;
            break;
        default:
            break;
    }
    
    return node;
}

Ast* create_ast_identifier(Token token) {
    Ast* node = ast_create_node(AST_IDENTIFIER);
    if (!node) return NULL;
    
    node->identifier = malloc(token.len + 1);
    if (node->identifier) {
        strncpy(node->identifier, token.start, token.len);
        node->identifier[token.len] = '\0';
    }
    node->line = token.line;
    node->col = token.col;
    
    return node;
}

Ast* create_ast_binary(Ast* left, Token operator, Ast* right) {
    Ast* node = ast_create_node(AST_BINARY);
    if (!node) return NULL;
    
    ast_add_child(node, left);
    ast_add_child(node, right);
    node->line = operator.line;
    node->col = operator.col;
    
    return node;
}

Ast* create_ast_unary(Token operator, Ast* right) {
    Ast* node = ast_create_node(AST_UNARY);
    if (!node) return NULL;
    
    ast_add_child(node, right);
    node->line = operator.line;
    node->col = operator.col;
    
    return node;
}

Ast* create_ast_call(Ast* callee, Ast* arguments) {
    Ast* node = ast_create_node(AST_CALL);
    if (!node) return NULL;
    
    ast_add_child(node, callee);
    if (arguments) {
        ast_add_child(node, arguments);
    }
    
    return node;
}

Ast* create_ast_field_access(Ast* object, Token field) {
    Ast* node = ast_create_node(AST_FIELD_ACCESS);
    if (!node) return NULL;
    
    ast_add_child(node, object);
    node->identifier = malloc(field.len + 1);
    if (node->identifier) {
        strncpy(node->identifier, field.start, field.len);
        node->identifier[field.len] = '\0';
    }
    node->line = field.line;
    node->col = field.col;
    
    return node;
}

// Main parsing functions
Ast* parse_compilation_unit(Parser* parser) {
    Ast* compunit = ast_create_node(AST_COMPUNIT);
    if (!compunit) return NULL;
    
    // Parse top-level declarations
    while (!parser_is_at_end(parser)) {
        if (parser_match(parser, TK_DOMAIN)) {
            Ast* domain = parse_domain_declaration(parser);
            if (domain) {
                ast_add_child(compunit, domain);
            }
        } else if (parser_match(parser, TK_IMPORT)) {
            Ast* import = parse_import_declaration(parser);
            if (import) {
                ast_add_child(compunit, import);
            }
        } else if (parser_match(parser, TK_CLASS)) {
            Ast* class = parse_class_declaration(parser);
            if (class) {
                ast_add_child(compunit, class);
            }
        } else if (parser_match(parser, TK_RECORD)) {
            Ast* record = parse_record_declaration(parser);
            if (record) {
                ast_add_child(compunit, record);
            }
        } else if (parser_match(parser, TK_ENUM)) {
            Ast* enum_decl = parse_enum_declaration(parser);
            if (enum_decl) {
                ast_add_child(compunit, enum_decl);
            }
        } else if (parser_match(parser, TK_INTERFACE)) {
            Ast* interface = parse_interface_declaration(parser);
            if (interface) {
                ast_add_child(compunit, interface);
            }
        } else {
            // Skip unexpected tokens
            parser_advance(parser);
        }
    }
    
    return compunit;
}

Ast* parse_domain_declaration(Parser* parser) {
    Ast* domain = ast_create_node(AST_DOMAIN);
    if (!domain) return NULL;
    
    // Parse domain name (qualified name)
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected domain name");
    
    // Build qualified name by collecting all parts
    char* qualified_name = malloc(name.len + 1);
    if (!qualified_name) return NULL;
    strncpy(qualified_name, name.start, name.len);
    qualified_name[name.len] = '\0';
    
    // Parse additional parts of qualified name (e.g., "app.main")
    while (parser_match(parser, TK_DOT)) {
        Token part = parser_consume(parser, TK_IDENTIFIER, "Expected domain name part after '.'");
        
        // Reallocate and append the new part
        size_t new_len = strlen(qualified_name) + 1 + part.len + 1; // +1 for dot, +1 for null terminator
        char* new_name = realloc(qualified_name, new_len);
        if (!new_name) {
            free(qualified_name);
            return NULL;
        }
        qualified_name = new_name;
        strcat(qualified_name, ".");
        strncat(qualified_name, part.start, part.len);
    }
    
    domain->identifier = qualified_name;
    
    // Parse semicolon
    parser_consume(parser, TK_SEMICOLON, "Expected ';' after domain declaration");
    return domain;
}

Ast* parse_class_declaration(Parser* parser) {
    Ast* class = ast_create_node(AST_CLASS);
    if (!class) return NULL;
    
    // Parse class name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected class name");
    class->identifier = malloc(name.len + 1);
    if (class->identifier) {
        strncpy(class->identifier, name.start, name.len);
        class->identifier[name.len] = '\0';
    }
    
    // Parse optional type parameters
    if (parser_match(parser, TK_LT)) {
        Ast* type_params = parse_type_arguments(parser);
        if (type_params) {
            ast_add_child(class, type_params);
        }
    }
    
    // Parse optional extends clause
    if (parser_match(parser, TK_EXTENDS)) {
        Ast* extends = parse_qualified_name(parser);
        if (extends) {
            ast_add_child(class, extends);
        }
    }
    
    // Parse optional implements clause
    if (parser_match(parser, TK_IMPLEMENTS)) {
        Ast* implements = parse_type_list(parser);
        if (implements) {
            ast_add_child(class, implements);
        }
    }
    
    // Parse class body
    parser_consume(parser, TK_LBRACE, "Expected '{' after class name");
    
    while (!parser_check(parser, TK_RBRACE) && !parser_is_at_end(parser)) {
        if (parser_match(parser, TK_FUNCTION) || parser_match(parser, TK_PROCEDURE)) {
            Ast* method = parse_method_declaration(parser);
            if (method) {
                ast_add_child(class, method);
            }
        } else if (parser_match(parser, TK_CONSTRUCTOR)) {
            Ast* constructor = parse_constructor_declaration(parser);
            if (constructor) {
                ast_add_child(class, constructor);
            }
        } else if (parser_match(parser, TK_VAR) || parser_match(parser, TK_LET)) {
            Ast* field = parse_field_declaration(parser);
            if (field) {
                ast_add_child(class, field);
            }
        } else if (parser_match(parser, TK_PROPERTY)) {
            Ast* property = parse_property_declaration(parser);
            if (property) {
                ast_add_child(class, property);
            }
        } else {
            // Skip unexpected tokens
            parser_advance(parser);
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after class body");
    return class;
}

Ast* parse_method_declaration(Parser* parser) {
    Ast* method = ast_create_node(AST_METHOD);
    if (!method) return NULL;
    
    // Check if it's async
    bool is_async = (parser->previous.kind == TK_ASYNC);
    
    // Parse method name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected method name");
    method->identifier = malloc(name.len + 1);
    if (method->identifier) {
        strncpy(method->identifier, name.start, name.len);
        method->identifier[name.len] = '\0';
    }
    
    // Parse optional type parameters
    if (parser_match(parser, TK_LT)) {
        Ast* type_params = parse_type_arguments(parser);
        if (type_params) {
            ast_add_child(method, type_params);
        }
    }
    
    // Parse parameters
    parser_consume(parser, TK_LPAREN, "Expected '(' after method name");
    Ast* params = parse_parameter_list(parser);
    if (params) {
        ast_add_child(method, params);
    }
    parser_consume(parser, TK_RPAREN, "Expected ')' after method parameters");
    
    // Parse return type
    if (parser_match(parser, TK_COLON)) {
        Ast* return_type = parse_type(parser);
        if (return_type) {
            ast_add_child(method, return_type);
        }
    }
    
    // Parse method body
    Ast* body = parse_block_statement(parser);
    if (body) {
        ast_add_child(method, body);
    }
    
    return method;
}

Ast* parse_block_statement(Parser* parser) {
    Ast* block = ast_create_node(AST_BLOCK);
    if (!block) return NULL;
    
    parser_consume(parser, TK_LBRACE, "Expected '{' after block");
    
    while (!parser_check(parser, TK_RBRACE) && !parser_is_at_end(parser)) {
        Ast* stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(block, stmt);
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after block");
    return block;
}

Ast* parse_statement(Parser* parser) {
    if (parser_match(parser, TK_VAR) || parser_match(parser, TK_LET)) {
        return parse_variable_declaration(parser);
    } else if (parser_match(parser, TK_IF)) {
        return parse_if_statement(parser);
    } else if (parser_match(parser, TK_WHILE)) {
        return parse_while_statement(parser);
    } else if (parser_match(parser, TK_FOR)) {
        return parse_for_statement(parser);
    } else if (parser_match(parser, TK_RETURN)) {
        return parse_return_statement(parser);
    } else {
        return parse_expression_statement(parser);
    }
}

Ast* parse_variable_declaration(Parser* parser) {
    TokenKind keyword = parser->previous.kind; // var or let
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected variable name");
    
    Ast* decl = ast_create_node(AST_VAR_DECL);
    if (!decl) return NULL;
    
    decl->identifier = malloc(name.len + 1);
    if (decl->identifier) {
        strncpy(decl->identifier, name.start, name.len);
        decl->identifier[name.len] = '\0';
    }
    decl->is_mutable = (keyword == TK_VAR);
    decl->line = name.line;
    decl->col = name.col;
    
    // Parse type annotation
    if (parser_match(parser, TK_COLON)) {
        Ast* type = parse_type(parser);
        if (type) {
            ast_add_child(decl, type);
        }
    }
    
    // Parse initializer
    if (parser_match(parser, TK_ASSIGN)) {
        Ast* initializer = parse_expression(parser);
        if (initializer) {
            ast_add_child(decl, initializer);
        }
    }
    
    parser_consume(parser, TK_SEMICOLON, "Expected ';' after variable declaration");
    return decl;
}

Ast* parse_expression_statement(Parser* parser) {
    Ast* expr = parse_expression(parser);
    if (expr) {
        Ast* stmt = ast_create_node(AST_EXPR_STMT);
        if (stmt) {
            ast_add_child(stmt, expr);
        }
        parser_consume(parser, TK_SEMICOLON, "Expected ';' after expression");
        return stmt;
    }
    return NULL;
}

Ast* parse_expression(Parser* parser) {
    return parse_assignment(parser);
}

Ast* parse_assignment(Parser* parser) {
    Ast* expr = parse_or_expression(parser);
    
    if (parser_match(parser, TK_ASSIGN)) {
        Token equals = parser->previous;
        Ast* value = parse_assignment(parser);
        
        if (expr && value) {
            return create_ast_binary(expr, equals, value);
        }
    }
    
    return expr;
}

Ast* parse_or_expression(Parser* parser) {
    Ast* expr = parse_and_expression(parser);
    
    while (parser_match(parser, TK_OR)) {
        Token operator = parser->previous;
        Ast* right = parse_and_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_and_expression(Parser* parser) {
    Ast* expr = parse_equality_expression(parser);
    
    while (parser_match(parser, TK_AND)) {
        Token operator = parser->previous;
        Ast* right = parse_equality_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_equality_expression(Parser* parser) {
    Ast* expr = parse_comparison_expression(parser);
    
    while (parser_match(parser, TK_EQUAL) || parser_match(parser, TK_NOT_EQUAL)) {
        Token operator = parser->previous;
        Ast* right = parse_comparison_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_comparison_expression(Parser* parser) {
    Ast* expr = parse_term_expression(parser);
    
    while (parser_match(parser, TK_GREATER) || parser_match(parser, TK_GREATER_EQUAL) ||
           parser_match(parser, TK_LESS) || parser_match(parser, TK_LESS_EQUAL)) {
        Token operator = parser->previous;
        Ast* right = parse_term_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_term_expression(Parser* parser) {
    Ast* expr = parse_factor_expression(parser);
    
    while (parser_match(parser, TK_PLUS) || parser_match(parser, TK_MINUS)) {
        Token operator = parser->previous;
        Ast* right = parse_factor_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_factor_expression(Parser* parser) {
    Ast* expr = parse_unary_expression(parser);
    
    while (parser_match(parser, TK_STAR) || parser_match(parser, TK_SLASH)) {
        Token operator = parser->previous;
        Ast* right = parse_unary_expression(parser);
        if (right) {
            expr = create_ast_binary(expr, operator, right);
        }
    }
    
    return expr;
}

Ast* parse_unary_expression(Parser* parser) {
    if (parser_match(parser, TK_MINUS) || parser_match(parser, TK_NOT)) {
        Token operator = parser->previous;
        Ast* right = parse_unary_expression(parser);
        if (right) {
            return create_ast_unary(operator, right);
        }
    }
    
    return parse_call_expression(parser);
}

Ast* parse_call_expression(Parser* parser) {
    Ast* expr = parse_primary_expression(parser);
    
    while (true) {
        if (parser_match(parser, TK_LPAREN)) {
            expr = finish_call_expression(parser, expr);
        } else if (parser_match(parser, TK_DOT)) {
            Token name = parser_consume(parser, TK_IDENTIFIER, "Expected property name after '.'");
            expr = create_ast_field_access(expr, name);
        } else {
            break;
        }
    }
    
    return expr;
}

Ast* finish_call_expression(Parser* parser, Ast* callee) {
    Ast* arguments = ast_create_node(AST_ARGUMENTS);
    if (!arguments) return callee;
    
    if (!parser_check(parser, TK_RPAREN)) {
        do {
            Ast* arg = parse_expression(parser);
            if (arg) {
                ast_add_child(arguments, arg);
            }
        } while (parser_match(parser, TK_COMMA));
    }
    
    parser_consume(parser, TK_RPAREN, "Expected ')' after arguments");
    return create_ast_call(callee, arguments);
}

Ast* parse_primary_expression(Parser* parser) {
    if (parser_match(parser, TK_FALSE)) return create_ast_literal(TK_FALSE, parser->previous);
    if (parser_match(parser, TK_TRUE)) return create_ast_literal(TK_TRUE, parser->previous);
    if (parser_match(parser, TK_NULL)) return create_ast_literal(TK_NULL, parser->previous);
    
    if (parser_match(parser, TK_INT)) return create_ast_literal(TK_INT, parser->previous);
    if (parser_match(parser, TK_FLOAT)) return create_ast_literal(TK_FLOAT, parser->previous);
    if (parser_match(parser, TK_STRING)) return create_ast_literal(TK_STRING, parser->previous);
    
    if (parser_match(parser, TK_IDENTIFIER)) return create_ast_identifier(parser->previous);
    
    if (parser_match(parser, TK_LPAREN)) {
        Ast* expr = parse_expression(parser);
        parser_consume(parser, TK_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    parser_error_at_current(parser, "Expected expression");
    return NULL;
}

Ast* parse_type(Parser* parser) {
    if (parser_match(parser, TK_INTEGER) || parser_match(parser, TK_FLOAT_TYPE) ||
        parser_match(parser, TK_BOOLEAN) || parser_match(parser, TK_STRING_TYPE) ||
        parser_match(parser, TK_VOID) || parser_match(parser, TK_OBJECT)) {
        return create_ast_literal(parser->previous.kind, parser->previous);
    }
    
    if (parser_match(parser, TK_IDENTIFIER)) {
        return create_ast_identifier(parser->previous);
    }
    
    return NULL;
}

// Helper function for parameter list parsing
Ast* parse_parameter_list(Parser* parser) {
    Ast* params = ast_create_node(AST_ARGUMENTS);
    if (!params) return NULL;
    
    if (!parser_check(parser, TK_RPAREN)) {
        do {
            Ast* param = parse_parameter(parser);
            if (param) {
                ast_add_child(params, param);
            }
        } while (parser_match(parser, TK_COMMA));
    }
    
    return params;
}

Ast* parse_parameter(Parser* parser) {
    Ast* param = ast_create_node(AST_VAR_DECL);
    if (!param) return NULL;
    
    // Parse parameter name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected parameter name");
    param->identifier = malloc(name.len + 1);
    if (param->identifier) {
        strncpy(param->identifier, name.start, name.len);
        param->identifier[name.len] = '\0';
    }
    
    // Parse parameter type
    parser_consume(parser, TK_COLON, "Expected ':' after parameter name");
    Ast* type = parse_type(parser);
    if (type) {
        ast_add_child(param, type);
    }
    
    return param;
}

Ast* parse_field_declaration(Parser* parser) {
    Ast* field = ast_create_node(AST_VAR_DECL);
    if (!field) return NULL;
    
    // Parse field name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected field name");
    field->identifier = malloc(name.len + 1);
    if (field->identifier) {
        strncpy(field->identifier, name.start, name.len);
        field->identifier[name.len] = '\0';
    }
    
    // Parse field type
    parser_consume(parser, TK_COLON, "Expected ':' after field name");
    Ast* type = parse_type(parser);
    if (type) {
        ast_add_child(field, type);
    }
    
    // Parse optional initializer
    if (parser_match(parser, TK_ASSIGN)) {
        Ast* initializer = parse_expression(parser);
        if (initializer) {
            ast_add_child(field, initializer);
        }
    }
    
    parser_consume(parser, TK_SEMICOLON, "Expected ';' after field declaration");
    return field;
}

Ast* parse_type_list(Parser* parser) {
    Ast* type_list = ast_create_node(AST_TYPE_ARGS);
    if (!type_list) return NULL;
    
    do {
        Ast* type = parse_type(parser);
        if (type) {
            ast_add_child(type_list, type);
        }
    } while (parser_match(parser, TK_COMMA));
    
    return type_list;
}

// Placeholder implementations for remaining functions
Ast* parse_import_declaration(Parser* parser) { return NULL; }
Ast* parse_record_declaration(Parser* parser) {
    Ast* record = ast_create_node(AST_RECORD);
    if (!record) return NULL;
    
    // Parse record name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected record name");
    record->identifier = malloc(name.len + 1);
    if (record->identifier) {
        strncpy(record->identifier, name.start, name.len);
        record->identifier[name.len] = '\0';
    }
    
    // Parse optional type parameters
    if (parser_match(parser, TK_LT)) {
        Ast* type_params = parse_type_arguments(parser);
        if (type_params) {
            ast_add_child(record, type_params);
        }
    }
    
    // Parse record body (same as class body)
    parser_consume(parser, TK_LBRACE, "Expected '{' after record name");
    
    while (!parser_check(parser, TK_RBRACE) && !parser_is_at_end(parser)) {
        if (parser_match(parser, TK_FUNCTION) || parser_match(parser, TK_PROCEDURE)) {
            Ast* method = parse_method_declaration(parser);
            if (method) {
                ast_add_child(record, method);
            }
        } else if (parser_match(parser, TK_CONSTRUCTOR)) {
            Ast* constructor = parse_constructor_declaration(parser);
            if (constructor) {
                ast_add_child(record, constructor);
            }
        } else if (parser_match(parser, TK_VAR) || parser_match(parser, TK_LET)) {
            Ast* field = parse_field_declaration(parser);
            if (field) {
                ast_add_child(record, field);
            }
        } else if (parser_match(parser, TK_PROPERTY)) {
            Ast* property = parse_property_declaration(parser);
            if (property) {
                ast_add_child(record, property);
            }
        } else {
            // Skip unexpected tokens
            parser_advance(parser);
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after record body");
    return record;
}
Ast* parse_enum_declaration(Parser* parser) {
    Ast* enum_decl = ast_create_node(AST_ENUM);
    if (!enum_decl) return NULL;
    
    // Parse enum name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected enum name");
    enum_decl->identifier = malloc(name.len + 1);
    if (enum_decl->identifier) {
        strncpy(enum_decl->identifier, name.start, name.len);
        enum_decl->identifier[name.len] = '\0';
    }
    
    // Parse optional type parameters
    if (parser_match(parser, TK_LT)) {
        Ast* type_params = parse_type_arguments(parser);
        if (type_params) {
            ast_add_child(enum_decl, type_params);
        }
    }
    
    // Parse enum body
    parser_consume(parser, TK_LBRACE, "Expected '{' after enum name");
    
    while (!parser_check(parser, TK_RBRACE) && !parser_is_at_end(parser)) {
        Ast* variant = parse_enum_variant(parser);
        if (variant) {
            ast_add_child(enum_decl, variant);
        }
        
        if (parser_match(parser, TK_COMMA)) {
            // Continue parsing variants
        } else if (!parser_check(parser, TK_RBRACE)) {
            parser_error_at_current(parser, "Expected ',' or '}' after enum variant");
            break;
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after enum body");
    return enum_decl;
}

Ast* parse_enum_variant(Parser* parser) {
    Ast* variant = ast_create_node(AST_IDENTIFIER);
    if (!variant) return NULL;
    
    // Parse variant name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected enum variant name");
    variant->identifier = malloc(name.len + 1);
    if (variant->identifier) {
        strncpy(variant->identifier, name.start, name.len);
        variant->identifier[name.len] = '\0';
    }
    
    // Parse optional parameters (type list, not parameter list)
    if (parser_match(parser, TK_LPAREN)) {
        Ast* param_types = ast_create_node(AST_TYPE_ARGS);
        if (param_types) {
            do {
                Ast* type = parse_type(parser);
                if (type) {
                    ast_add_child(param_types, type);
                }
            } while (parser_match(parser, TK_COMMA));
            ast_add_child(variant, param_types);
        }
        parser_consume(parser, TK_RPAREN, "Expected ')' after enum variant parameters");
    }
    
    return variant;
}
Ast* parse_interface_declaration(Parser* parser) {
    Ast* interface = ast_create_node(AST_INTERFACE);
    if (!interface) return NULL;
    
    // Parse interface name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected interface name");
    interface->identifier = malloc(name.len + 1);
    if (interface->identifier) {
        strncpy(interface->identifier, name.start, name.len);
        interface->identifier[name.len] = '\0';
    }
    
    // Parse optional type parameters
    if (parser_match(parser, TK_LT)) {
        Ast* type_params = parse_type_arguments(parser);
        if (type_params) {
            ast_add_child(interface, type_params);
        }
    }
    
    // Parse interface body
    parser_consume(parser, TK_LBRACE, "Expected '{' after interface name");
    
    while (!parser_check(parser, TK_RBRACE) && !parser_is_at_end(parser)) {
        Ast* member = parse_interface_member(parser);
        if (member) {
            ast_add_child(interface, member);
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after interface body");
    return interface;
}

Ast* parse_interface_member(Parser* parser) {
    Ast* member = ast_create_node(AST_METHOD);
    if (!member) return NULL;
    
    // Check if it's async
    bool is_async = parser_match(parser, TK_ASYNC);
    
    // Parse method signature
    if (parser_match(parser, TK_FUNCTION) || parser_match(parser, TK_PROCEDURE)) {
        // Parse method name
        Token name = parser_consume(parser, TK_IDENTIFIER, "Expected method name");
        member->identifier = malloc(name.len + 1);
        if (member->identifier) {
            strncpy(member->identifier, name.start, name.len);
            member->identifier[name.len] = '\0';
        }
        
        // Parse optional type parameters
        if (parser_match(parser, TK_LT)) {
            Ast* type_params = parse_type_arguments(parser);
            if (type_params) {
                ast_add_child(member, type_params);
            }
        }
        
        // Parse parameters
        parser_consume(parser, TK_LPAREN, "Expected '(' after method name");
        Ast* params = parse_parameter_list(parser);
        if (params) {
            ast_add_child(member, params);
        }
        parser_consume(parser, TK_RPAREN, "Expected ')' after method parameters");
        
        // Parse return type
        if (parser_match(parser, TK_COLON)) {
            Ast* return_type = parse_type(parser);
            if (return_type) {
                ast_add_child(member, return_type);
            }
        }
        
        parser_consume(parser, TK_SEMICOLON, "Expected ';' after interface method");
    }
    
    return member;
}
Ast* parse_property_declaration(Parser* parser) {
    Ast* property = ast_create_node(AST_PROPERTY);
    if (!property) return NULL;
    
    // Parse property name
    Token name = parser_consume(parser, TK_IDENTIFIER, "Expected property name");
    property->identifier = malloc(name.len + 1);
    if (property->identifier) {
        strncpy(property->identifier, name.start, name.len);
        property->identifier[name.len] = '\0';
    }
    
    // Parse property type
    parser_consume(parser, TK_COLON, "Expected ':' after property name");
    Ast* type = parse_type(parser);
    if (type) {
        ast_add_child(property, type);
    }
    
    // Parse property body
    parser_consume(parser, TK_LBRACE, "Expected '{' after property type");
    
    // Parse getter
    if (parser_match(parser, TK_IDENTIFIER) && 
        strncmp(parser->previous.start, "get", parser->previous.len) == 0) {
        Ast* getter = parse_block_statement(parser);
        if (getter) {
            ast_add_child(property, getter);
        }
    }
    
    // Parse setter
    if (parser_match(parser, TK_IDENTIFIER) && 
        strncmp(parser->previous.start, "set", parser->previous.len) == 0) {
        Ast* setter = parse_block_statement(parser);
        if (setter) {
            ast_add_child(property, setter);
        }
    }
    
    parser_consume(parser, TK_RBRACE, "Expected '}' after property body");
    return property;
}

Ast* parse_constructor_declaration(Parser* parser) {
    Ast* constructor = ast_create_node(AST_CONSTRUCTOR);
    if (!constructor) return NULL;
    
    // Parse parameters
    parser_consume(parser, TK_LPAREN, "Expected '(' after constructor");
    Ast* params = parse_parameter_list(parser);
    if (params) {
        ast_add_child(constructor, params);
    }
    parser_consume(parser, TK_RPAREN, "Expected ')' after constructor parameters");
    
    // Parse constructor body
    Ast* body = parse_block_statement(parser);
    if (body) {
        ast_add_child(constructor, body);
    }
    
    return constructor;
}
Ast* parse_if_statement(Parser* parser) { return NULL; }
Ast* parse_while_statement(Parser* parser) { return NULL; }
Ast* parse_for_statement(Parser* parser) { return NULL; }
Ast* parse_match_statement(Parser* parser) { return NULL; }
Ast* parse_return_statement(Parser* parser) { return NULL; }
Ast* parse_type_arguments(Parser* parser) { return NULL; }
Ast* parse_qualified_name(Parser* parser) { return NULL; }
Ast* parse_pattern(Parser* parser) { return NULL; }
Ast* parse_literal_pattern(Parser* parser) { return NULL; }
Ast* parse_identifier_pattern(Parser* parser) { return NULL; }
Ast* parse_constructor_pattern(Parser* parser) { return NULL; }
