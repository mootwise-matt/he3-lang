#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AST creation and destruction
Ast* ast_create_node(AstKind kind) {
    Ast* node = ast_malloc(sizeof(Ast));
    if (!node) return NULL;
    
    node->kind = kind;
    node->children = ast_malloc(sizeof(Ast*) * 4);
    if (!node->children) {
        ast_free(node);
        return NULL;
    }
    node->child_count = 0;
    node->child_capacity = 4;
    node->identifier = NULL;
    node->type_index = 0;
    node->is_resolved = false;
    node->is_mutable = false;
    node->line = 0;
    node->col = 0;
    
    return node;
}

void ast_destroy_node(Ast* node) {
    if (!node) return;
    
    // Destroy children first
    for (uint32_t i = 0; i < node->child_count; i++) {
        ast_destroy_node(node->children[i]);
    }
    
    // Free children array
    ast_free(node->children);
    
    // Free identifier
    if (node->identifier) {
        ast_free(node->identifier);
    }
    
    // Free string literal
    if (node->kind == AST_LITERAL && node->literal.string_value) {
        ast_free(node->literal.string_value);
    }
    
    // Free the node itself
    ast_free(node);
}

void ast_destroy_tree(Ast* root) {
    ast_destroy_node(root);
}

// AST manipulation
bool ast_add_child(Ast* parent, Ast* child) {
    if (!parent || !child) return false;
    
    // Resize if necessary
    if (parent->child_count >= parent->child_capacity) {
        uint32_t new_capacity = parent->child_capacity * 2;
        Ast** new_children = ast_realloc(parent->children, sizeof(Ast*) * new_capacity);
        if (!new_children) return false;
        
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count] = child;
    parent->child_count++;
    return true;
}

Ast* ast_get_child(Ast* parent, uint32_t index) {
    if (!parent || index >= parent->child_count) return NULL;
    return parent->children[index];
}

uint32_t ast_get_child_count(Ast* parent) {
    return parent ? parent->child_count : 0;
}

// AST visitor pattern
void ast_accept(Ast* node, AstVisitor* visitor) {
    if (!node || !visitor) return;
    
    switch (node->kind) {
        case AST_COMPUNIT:
            if (visitor->visit_compunit) visitor->visit_compunit(visitor, node);
            break;
        case AST_DOMAIN:
            if (visitor->visit_domain) visitor->visit_domain(visitor, node);
            break;
        case AST_CLASS:
            if (visitor->visit_class) visitor->visit_class(visitor, node);
            break;
        case AST_METHOD:
            if (visitor->visit_method) visitor->visit_method(visitor, node);
            break;
        case AST_VAR_DECL:
            if (visitor->visit_vardecl) visitor->visit_vardecl(visitor, node);
            break;
        case AST_BINARY:
        case AST_UNARY:
        case AST_CALL:
        case AST_FIELD_ACCESS:
        case AST_INDEX_ACCESS:
        case AST_LITERAL:
        case AST_IDENTIFIER:
        case AST_LAMBDA:
            if (visitor->visit_expression) visitor->visit_expression(visitor, node);
            break;
        case AST_IF:
        case AST_WHILE:
        case AST_FOR:
        case AST_MATCH:
        case AST_RETURN:
        case AST_EXPR_STMT:
        case AST_BLOCK:
            if (visitor->visit_statement) visitor->visit_statement(visitor, node);
            break;
        case AST_TYPE:
        case AST_TYPE_ARGS:
        case AST_NULLABLE_TYPE:
        case AST_GENERIC_TYPE:
            if (visitor->visit_type) visitor->visit_type(visitor, node);
            break;
        default:
            if (visitor->visit_default) visitor->visit_default(visitor, node);
            break;
    }
    
    // Visit children
    ast_visit_children(node, visitor);
}

void ast_visit_children(Ast* node, AstVisitor* visitor) {
    if (!node || !visitor) return;
    
    for (uint32_t i = 0; i < node->child_count; i++) {
        ast_accept(node->children[i], visitor);
    }
}

// AST utilities
const char* ast_kind_to_string(AstKind kind) {
    switch (kind) {
        case AST_COMPUNIT: return "COMPUNIT";
        case AST_DOMAIN: return "DOMAIN";
        case AST_CLASS: return "CLASS";
        case AST_INTERFACE: return "INTERFACE";
        case AST_RECORD: return "RECORD";
        case AST_ENUM: return "ENUM";
        case AST_IMPORT: return "IMPORT";
        case AST_METHOD: return "METHOD";
        case AST_PROPERTY: return "PROPERTY";
        case AST_CONSTRUCTOR: return "CONSTRUCTOR";
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

void ast_print_tree(Ast* root, int depth) {
    if (!root) return;
    
    // Indent based on depth
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    printf("AST[%s]: ", ast_kind_to_string(root->kind));
    
    // Print node-specific information
    if (root->identifier) {
        printf("('%s')", root->identifier);
    }
    
    if (root->kind == AST_LITERAL) {
        switch (root->literal.token.kind) {
            case TK_INT:
                printf(" (int: %ld)", root->literal.int_value);
                break;
            case TK_FLOAT:
                printf(" (float: %f)", root->literal.float_value);
                break;
            case TK_STRING:
                printf(" (string: '%s')", root->literal.string_value);
                break;
            case TK_TRUE:
            case TK_FALSE:
                printf(" (bool: %s)", root->literal.bool_value ? "true" : "false");
                break;
            default:
                break;
        }
    }
    
    printf("\n");
    
    // Print children
    for (uint32_t i = 0; i < root->child_count; i++) {
        ast_print_tree(root->children[i], depth + 1);
    }
}

bool ast_is_expression(AstKind kind) {
    switch (kind) {
        case AST_BINARY:
        case AST_UNARY:
        case AST_CALL:
        case AST_FIELD_ACCESS:
        case AST_INDEX_ACCESS:
        case AST_LITERAL:
        case AST_IDENTIFIER:
        case AST_LAMBDA:
            return true;
        default:
            return false;
    }
}

bool ast_is_statement(AstKind kind) {
    switch (kind) {
        case AST_IF:
        case AST_WHILE:
        case AST_FOR:
        case AST_MATCH:
        case AST_RETURN:
        case AST_EXPR_STMT:
        case AST_BLOCK:
        case AST_VAR_DECL:
        case AST_ASSIGN:
            return true;
        default:
            return false;
    }
}

bool ast_is_declaration(AstKind kind) {
    switch (kind) {
        case AST_DOMAIN:
        case AST_CLASS:
        case AST_INTERFACE:
        case AST_RECORD:
        case AST_ENUM:
        case AST_METHOD:
        case AST_PROPERTY:
        case AST_CONSTRUCTOR:
        case AST_VAR_DECL:
        case AST_IMPORT:
        case AST_SECURITY_KEY:
        case AST_EVENT_DECL:
            return true;
        default:
            return false;
    }
}

// Memory management
void* ast_malloc(size_t size) {
    return malloc(size);
}

void ast_free(void* ptr) {
    free(ptr);
}

void* ast_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}
