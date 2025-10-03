#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new AST node
Ast* ast_create(AstKind kind, const char* identifier, uint32_t line, uint32_t col) {
    Ast* node = malloc(sizeof(Ast));
    if (!node) return NULL;
    
    node->kind = kind;
    node->identifier = identifier ? strdup(identifier) : NULL;
    node->text = NULL;
    node->line = line;
    node->col = col;
    
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    
    node->type_index = 0;
    node->is_resolved = false;
    node->is_mutable = false;
    node->is_static = false;
    
    // Initialize literal union
    memset(&node->literal, 0, sizeof(node->literal));
    node->operator = TK_EOF;
    
    return node;
}

// Destroy an AST node and all its children
void ast_destroy(Ast* node) {
    if (!node) return;
    
    // Free identifier
    if (node->identifier) {
        free(node->identifier);
    }
    
    // Free text
    if (node->text) {
        free((void*)node->text);
    }
    
    // Free string literal if it's a string
    if (node->kind == AST_LITERAL && node->literal.string_offset == 0) {
        // This is a raw string pointer, not an offset
        // We need to be careful here - only free if it's not pointing to source text
        // For now, we'll leave this to the caller to manage
    }
    
    // Recursively destroy children
    for (uint32_t i = 0; i < node->child_count; i++) {
        ast_destroy(node->children[i]);
    }
    
    // Free children array
    if (node->children) {
        free(node->children);
    }
    
    free(node);
}

// Add a child to an AST node
void ast_add_child(Ast* parent, Ast* child) {
    if (!parent || !child) return;
    
    // Grow children array if needed
    if (parent->child_count >= parent->child_capacity) {
        uint32_t new_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        Ast** new_children = realloc(parent->children, sizeof(Ast*) * new_capacity);
        if (!new_children) return;
        
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count] = child;
    parent->child_count++;
}

// Get a child by index
Ast* ast_get_child(Ast* parent, uint32_t index) {
    if (!parent || index >= parent->child_count) return NULL;
    return parent->children[index];
}

// Set literal values
void ast_set_literal_int(Ast* node, int64_t value) {
    if (!node) return;
    node->literal.int_value = value;
}

void ast_set_literal_float(Ast* node, double value) {
    if (!node) return;
    node->literal.float_value = value;
}

void ast_set_literal_bool(Ast* node, bool value) {
    if (!node) return;
    node->literal.bool_value = value;
}

void ast_set_literal_string(Ast* node, const char* value) {
    if (!node) return;
    // Store as string offset (0 for now, will be set during bytecode generation)
    node->literal.string_offset = 0;
    // Store the actual string in text field for now
    node->text = value;
}

// Print AST node (for debugging)
void ast_print(Ast* node, int depth) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Print node info
    printf("%s", ast_kind_to_string(node->kind));
    
    if (node->identifier) {
        printf(" (%s)", node->identifier);
    }
    
    if (node->kind == AST_LITERAL) {
        printf(" = ");
        if (node->literal.int_value != 0 || node->literal.float_value != 0.0 || 
            node->literal.bool_value || node->literal.string_offset != 0) {
            // Print literal value based on what's set
            if (node->literal.string_offset != 0) {
                printf("string_offset:%u", node->literal.string_offset);
            } else if (node->text) {
                printf("\"%s\"", node->text);
            } else {
                printf("0");
            }
        } else {
            printf("0");
        }
    }
    
    printf(" [%u:%u]\n", node->line, node->col);
    
    // Print children
    for (uint32_t i = 0; i < node->child_count; i++) {
        ast_print(node->children[i], depth + 1);
    }
}

// Convert AST kind to string
const char* ast_kind_to_string(AstKind kind) {
    switch (kind) {
        case AST_COMPUNIT: return "COMPUNIT";
        case AST_DOMAIN: return "DOMAIN";
        case AST_IMPORT: return "IMPORT";
        case AST_CLASS: return "CLASS";
        case AST_RECORD: return "RECORD";
        case AST_ENUM: return "ENUM";
        case AST_INTERFACE: return "INTERFACE";
        case AST_FIELD: return "FIELD";
        case AST_METHOD: return "METHOD";
        case AST_CONSTRUCTOR: return "CONSTRUCTOR";
        case AST_PROPERTY: return "PROPERTY";
        case AST_BLOCK: return "BLOCK";
        case AST_VAR_DECL: return "VAR_DECL";
        case AST_ASSIGN: return "ASSIGN";
        case AST_IF: return "IF";
        case AST_WHILE: return "WHILE";
        case AST_FOR: return "FOR";
        case AST_MATCH: return "MATCH";
        case AST_RETURN: return "RETURN";
        case AST_EXPR_STMT: return "EXPR_STMT";
        case AST_CASE: return "CASE";
        case AST_BINARY: return "BINARY";
        case AST_UNARY: return "UNARY";
        case AST_CALL: return "CALL";
        case AST_FIELD_ACCESS: return "FIELD_ACCESS";
        case AST_INDEX_ACCESS: return "INDEX_ACCESS";
        case AST_LITERAL: return "LITERAL";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_LAMBDA: return "LAMBDA";
        case AST_ARGUMENTS: return "ARGUMENTS";
        case AST_TYPE: return "TYPE";
        case AST_TYPE_ARGS: return "TYPE_ARGS";
        case AST_NULLABLE_TYPE: return "NULLABLE_TYPE";
        case AST_GENERIC_TYPE: return "GENERIC_TYPE";
        case AST_PATTERN: return "PATTERN";
        case AST_LITERAL_PATTERN: return "LITERAL_PATTERN";
        case AST_IDENTIFIER_PATTERN: return "IDENTIFIER_PATTERN";
        case AST_CONSTRUCTOR_PATTERN: return "CONSTRUCTOR_PATTERN";
        case AST_SECURITY_KEY: return "SECURITY_KEY";
        case AST_SECURE_DOMAIN: return "SECURE_DOMAIN";
        case AST_HANDSHAKE: return "HANDSHAKE";
        case AST_EVENT_DECL: return "EVENT_DECL";
        case AST_PUBLISH: return "PUBLISH";
        case AST_SUBSCRIBE: return "SUBSCRIBE";
        case AST_UNSUBSCRIBE: return "UNSUBSCRIBE";
        case AST_QUALIFIED_NAME: return "QUALIFIED_NAME";
        default: return "UNKNOWN";
    }
}

// Check if AST kind is an expression
bool ast_is_expression(AstKind kind) {
    return kind == AST_BINARY || kind == AST_UNARY || kind == AST_CALL ||
           kind == AST_FIELD_ACCESS || kind == AST_INDEX_ACCESS ||
           kind == AST_LITERAL || kind == AST_IDENTIFIER || kind == AST_LAMBDA;
}

// Check if AST kind is a statement
bool ast_is_statement(AstKind kind) {
    return kind == AST_BLOCK || kind == AST_VAR_DECL || kind == AST_ASSIGN ||
           kind == AST_IF || kind == AST_WHILE || kind == AST_FOR ||
           kind == AST_MATCH || kind == AST_RETURN || kind == AST_EXPR_STMT;
}

// Check if AST kind is a declaration
bool ast_is_declaration(AstKind kind) {
    return kind == AST_DOMAIN || kind == AST_IMPORT || kind == AST_CLASS ||
           kind == AST_RECORD || kind == AST_ENUM || kind == AST_INTERFACE ||
           kind == AST_FIELD || kind == AST_METHOD || kind == AST_CONSTRUCTOR ||
           kind == AST_PROPERTY || kind == AST_VAR_DECL;
}
