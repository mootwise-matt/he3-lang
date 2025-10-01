#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../lexer/lexer.h"

// Forward declarations
typedef struct Ast Ast;
typedef struct AstVisitor AstVisitor;

// AST node kinds for He³ parser
typedef enum {
    // Compilation unit
    AST_COMPUNIT,
    
    // Declarations
    AST_DOMAIN,
    AST_CLASS,
    AST_INTERFACE,
    AST_RECORD,
    AST_ENUM,
    AST_IMPORT,
    
    // Members
    AST_METHOD,
    AST_PROPERTY,
    AST_CONSTRUCTOR,
    
    // Statements
    AST_BLOCK,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_MATCH,
    AST_RETURN,
    AST_EXPR_STMT,
    AST_CASE,
    
    // Expressions
    AST_BINARY,
    AST_UNARY,
    AST_CALL,
    AST_FIELD_ACCESS,
    AST_INDEX_ACCESS,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_LAMBDA,
    AST_ARGUMENTS,
    
    // Types
    AST_TYPE,
    AST_TYPE_ARGS,
    AST_NULLABLE_TYPE,
    AST_GENERIC_TYPE,
    
    // Patterns
    AST_PATTERN,
    AST_LITERAL_PATTERN,
    AST_IDENTIFIER_PATTERN,
    AST_CONSTRUCTOR_PATTERN,
    
    // Security and Events
    AST_SECURITY_KEY,
    AST_SECURE_DOMAIN,
    AST_HANDSHAKE,
    AST_EVENT_DECL,
    AST_PUBLISH,
    AST_SUBSCRIBE,
    AST_UNSUBSCRIBE,
    
    // Qualified names
    AST_QUALIFIED_NAME
} AstKind;

// AST node structure
struct Ast {
    AstKind kind;
    Ast** children;
    uint32_t child_count;
    uint32_t child_capacity;
    
    // Node-specific data
    char* identifier;        // For identifiers, method names, etc.
    uint32_t type_index;     // Type information
    bool is_resolved;        // Whether type has been resolved
    bool is_mutable;         // Whether variable is mutable
    
    // Source location
    uint32_t line;
    uint32_t col;
    
    // Literal data
    union {
        int64_t int_value;
        double float_value;
        bool bool_value;
        char* string_value;
        Token token;         // For literal patterns
    } literal;
};

// AST visitor interface
struct AstVisitor {
    void (*visit_compunit)(AstVisitor* visitor, Ast* node);
    void (*visit_domain)(AstVisitor* visitor, Ast* node);
    void (*visit_class)(AstVisitor* visitor, Ast* node);
    void (*visit_method)(AstVisitor* visitor, Ast* node);
    void (*visit_vardecl)(AstVisitor* visitor, Ast* node);
    void (*visit_expression)(AstVisitor* visitor, Ast* node);
    void (*visit_statement)(AstVisitor* visitor, Ast* node);
    void (*visit_type)(AstVisitor* visitor, Ast* node);
    void (*visit_default)(AstVisitor* visitor, Ast* node);
};

// AST creation and destruction
Ast* ast_create_node(AstKind kind);
void ast_destroy_node(Ast* node);
void ast_destroy_tree(Ast* root);

// AST manipulation
bool ast_add_child(Ast* parent, Ast* child);
Ast* ast_get_child(Ast* parent, uint32_t index);
uint32_t ast_get_child_count(Ast* parent);

// AST visitor pattern
void ast_accept(Ast* node, AstVisitor* visitor);
void ast_visit_children(Ast* node, AstVisitor* visitor);

// AST utilities
const char* ast_kind_to_string(AstKind kind);
void ast_print_tree(Ast* root, int depth);
bool ast_is_expression(AstKind kind);
bool ast_is_statement(AstKind kind);
bool ast_is_declaration(AstKind kind);

// Memory management
void* ast_malloc(size_t size);
void ast_free(void* ptr);
void* ast_realloc(void* ptr, size_t size);
