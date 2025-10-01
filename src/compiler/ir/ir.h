#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward declarations
struct IRInstruction;
struct IRBuilder;
struct IRBlock;
struct IRFunction;

// IR Operation types
typedef enum {
    // Load operations
    IR_LOAD_CONST,      // Load constant value
    IR_LOAD_LOCAL,      // Load local variable
    IR_LOAD_ARG,        // Load argument
    IR_LOAD_FIELD,      // Load object field
    IR_LOAD_STATIC,     // Load static field
    IR_LOAD_ARRAY,      // Load array element
    
    // Store operations
    IR_STORE_LOCAL,     // Store local variable
    IR_STORE_FIELD,     // Store object field
    IR_STORE_STATIC,    // Store static field
    IR_STORE_ARRAY,     // Store array element
    
    // Arithmetic operations
    IR_ADD,             // Addition
    IR_SUB,             // Subtraction
    IR_MUL,             // Multiplication
    IR_DIV,             // Division
    IR_MOD,             // Modulo
    IR_NEG,             // Negation
    IR_INC,             // Increment
    IR_DEC,             // Decrement
    
    // Comparison operations
    IR_EQ,              // Equal
    IR_NE,              // Not equal
    IR_LT,              // Less than
    IR_LE,              // Less than or equal
    IR_GT,              // Greater than
    IR_GE,              // Greater than or equal
    
    // Logical operations
    IR_AND,             // Logical AND
    IR_OR,              // Logical OR
    IR_NOT,             // Logical NOT
    
    // Control flow
    IR_JMP,             // Unconditional jump
    IR_JMPF,            // Jump if false
    IR_JMPT,            // Jump if true
    IR_JMP_EQ,          // Jump if equal
    IR_JMP_NE,          // Jump if not equal
    IR_JMP_LT,          // Jump if less than
    IR_JMP_LE,          // Jump if less than or equal
    IR_JMP_GT,          // Jump if greater than
    IR_JMP_GE,          // Jump if greater than or equal
    
    // Method calls
    IR_CALL,            // Call method
    IR_CALLV,           // Virtual method call
    IR_CALLI,           // Interface method call
    IR_RETURN,          // Return from method
    IR_RETURN_VAL,      // Return with value
    
    // Object operations
    IR_NEW,             // Create new object
    IR_NEW_ARRAY,       // Create new array
    IR_CAST,            // Type cast
    IR_INSTANCEOF,      // Instance of check
    IR_IS_NULL,         // Null check
    IR_IS_NOT_NULL,     // Not null check
    
    // Type operations
    IR_BOX,             // Box value type
    IR_UNBOX,           // Unbox value type
    IR_GET_TYPE,        // Get object type
    
    // Async operations
    IR_SPAWN,           // Spawn async task
    IR_AWAIT,           // Await async result
    IR_ASYNC_RETURN,    // Async return
    
    // Pattern matching
    IR_MATCH_SOME,      // Match Some variant
    IR_MATCH_NONE,      // Match None variant
    IR_MATCH_OK,        // Match Ok variant
    IR_MATCH_ERR,       // Match Err variant
    
    // Special operations
    IR_PHI,             // Phi function (for SSA)
    IR_COPY,            // Copy value
    IR_NOP,             // No operation
} IROp;

// Value types for IR
typedef enum {
    IR_VALUE_I64,       // 64-bit integer
    IR_VALUE_F64,       // 64-bit float
    IR_VALUE_BOOL,      // Boolean
    IR_VALUE_OBJECT,    // Object reference
    IR_VALUE_STRING,    // String reference
    IR_VALUE_NULL,      // Null reference
    IR_VALUE_TEMP,      // Temporary variable
} IRValueType;

// IR Value representation
typedef struct IRValue {
    IRValueType type;
    union {
        int64_t i64;
        double f64;
        bool boolean;
        void* object;
        uint32_t string_id;
        uint32_t temp_id;
    } data;
} IRValue;

// IR Instruction
typedef struct IRInstruction {
    IROp op;                // Operation
    IRValue* operands;      // Operand values
    uint32_t operand_count; // Number of operands
    IRValue result;         // Result value
    uint32_t target;        // Jump target (for control flow)
    uint32_t line;          // Source line number
    uint32_t col;           // Source column number
} IRInstruction;

// IR Basic Block
typedef struct IRBlock {
    uint32_t id;                    // Block ID
    char* label;                    // Block label
    IRInstruction** instructions;   // Instructions in block
    uint32_t instruction_count;     // Number of instructions
    uint32_t capacity;              // Instruction capacity
    
    // Control flow
    struct IRBlock** predecessors;         // Predecessor blocks
    uint32_t predecessor_count;     // Number of predecessors
    struct IRBlock** successors;           // Successor blocks
    uint32_t successor_count;       // Number of successors
    
    // Block flags
    bool is_entry;                  // Entry block
    bool is_exit;                   // Exit block
    bool is_reachable;              // Reachable from entry
} IRBlock;

// IR Function
typedef struct IRFunction {
    char* name;                     // Function name
    uint32_t signature_hash;        // Function signature hash
    uint32_t return_type;           // Return type ID
    uint32_t* param_types;          // Parameter type IDs
    uint32_t param_count;           // Number of parameters
    uint32_t local_count;           // Number of local variables
    
    // Control flow graph
    struct IRBlock** blocks;               // Basic blocks
    uint32_t block_count;           // Number of blocks
    struct IRBlock* entry_block;           // Entry block
    struct IRBlock* exit_block;            // Exit block
    
    // Local variables
    char** local_names;             // Local variable names
    uint32_t* local_types;          // Local variable types
    uint32_t temp_count;            // Number of temporaries
    
    // Flags
    bool is_static;                 // Static function
    bool is_virtual;                // Virtual function
    bool is_async;                  // Async function
} IRFunction;

// IR Builder
typedef struct IRBuilder {
    struct IRFunction* current_function;   // Current function being built
    struct IRBlock* current_block;         // Current block being built
    uint32_t next_temp_id;          // Next temporary ID
    uint32_t next_block_id;         // Next block ID
    
    // Error handling
    char* error_message;            // Last error message
    bool has_error;                 // Has error flag
} IRBuilder;

// IR Builder functions
IRBuilder* ir_builder_create(void);
void ir_builder_destroy(IRBuilder* builder);

// Function management
IRFunction* ir_builder_create_function(IRBuilder* builder, const char* name, uint32_t signature_hash);
void ir_builder_destroy_function(IRFunction* function);

// Block management
IRBlock* ir_builder_create_block(IRBuilder* builder, const char* label);
void ir_builder_destroy_block(IRBlock* block);
void ir_builder_set_current_block(IRBuilder* builder, IRBlock* block);

// Instruction creation
IRInstruction* ir_builder_create_instruction(IRBuilder* builder, IROp op);
void ir_builder_destroy_instruction(IRInstruction* instruction);
void ir_builder_add_instruction(IRBuilder* builder, IRInstruction* instruction);

// Value creation
IRValue ir_builder_create_i64_value(int64_t value);
IRValue ir_builder_create_f64_value(double value);
IRValue ir_builder_create_bool_value(bool value);
IRValue ir_builder_create_object_value(void* object);
IRValue ir_builder_create_string_value(uint32_t string_id);
IRValue ir_builder_create_null_value(void);
IRValue ir_builder_create_temp_value(IRBuilder* builder, IRValueType type);

// Operand management
void ir_instruction_add_operand(IRInstruction* instruction, IRValue value);
void ir_instruction_set_result(IRInstruction* instruction, IRValue value);
void ir_instruction_set_target(IRInstruction* instruction, uint32_t target);

// Control flow
void ir_builder_add_jump(IRBuilder* builder, IRBlock* target);
void ir_builder_add_conditional_jump(IRBuilder* builder, IRValue condition, IRBlock* true_target, IRBlock* false_target);

// Error handling
void ir_builder_set_error(IRBuilder* builder, const char* message);
const char* ir_builder_get_error(IRBuilder* builder);
bool ir_builder_has_error(IRBuilder* builder);

// Utility functions
uint32_t ir_builder_get_next_temp_id(IRBuilder* builder);
uint32_t ir_builder_get_next_block_id(IRBuilder* builder);

// Debug and printing
void ir_print_function(IRFunction* function);
void ir_print_block(IRBlock* block);
void ir_print_instruction(IRInstruction* instruction);
void ir_print_value(IRValue value);
const char* ir_op_to_string(IROp op);
