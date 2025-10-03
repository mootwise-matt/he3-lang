#include "ir.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// IR Builder implementation
IRBuilder* ir_builder_create(void) {
    IRBuilder* builder = malloc(sizeof(IRBuilder));
    if (!builder) return NULL;
    
    builder->current_function = NULL;
    builder->current_block = NULL;
    builder->next_temp_id = 1;
    builder->next_block_id = 1;
    builder->error_message = NULL;
    builder->has_error = false;
    
    return builder;
}

void ir_builder_destroy(IRBuilder* builder) {
    if (!builder) return;
    
    if (builder->current_function) {
        ir_builder_destroy_function(builder->current_function);
    }
    
    if (builder->error_message) {
        free(builder->error_message);
    }
    
    free(builder);
}

// Function management
IRFunction* ir_builder_create_function(IRBuilder* builder, const char* name, uint32_t signature_hash) {
    if (!builder || !name) {
        ir_builder_set_error(builder, "Invalid parameters for function creation");
        return NULL;
    }
    
    IRFunction* function = malloc(sizeof(IRFunction));
    if (!function) {
        ir_builder_set_error(builder, "Memory allocation failed");
        return NULL;
    }
    
    function->name = strdup(name);
    function->signature_hash = signature_hash;
    function->return_type = 0;
    function->param_types = NULL;
    function->param_count = 0;
    function->local_count = 0;
    function->blocks = NULL;
    function->block_count = 0;
    function->entry_block = NULL;
    function->exit_block = NULL;
    function->local_names = NULL;
    function->local_types = NULL;
    function->temp_count = 0;
    function->is_static = false;
    function->is_virtual = false;
    function->is_async = false;
    
    builder->current_function = function;
    return function;
}

void ir_builder_destroy_function(IRFunction* function) {
    if (!function) return;
    
    if (function->name) free(function->name);
    if (function->param_types) free(function->param_types);
    if (function->local_names) {
        for (uint32_t i = 0; i < function->local_count; i++) {
            if (function->local_names[i]) free(function->local_names[i]);
        }
        free(function->local_names);
    }
    if (function->local_types) free(function->local_types);
    
    if (function->blocks) {
        for (uint32_t i = 0; i < function->block_count; i++) {
            ir_builder_destroy_block(function->blocks[i]);
        }
        free(function->blocks);
    }
    
    free(function);
}

// Block management
IRBlock* ir_builder_create_block(IRBuilder* builder, const char* label) {
    if (!builder) {
        ir_builder_set_error(builder, "Invalid builder");
        return NULL;
    }
    
    IRBlock* block = malloc(sizeof(IRBlock));
    if (!block) {
        ir_builder_set_error(builder, "Memory allocation failed");
        return NULL;
    }
    
    block->id = builder->next_block_id++;
    block->label = label ? strdup(label) : NULL;
    block->instructions = NULL;
    block->instruction_count = 0;
    block->capacity = 0;
    block->predecessors = NULL;
    block->predecessor_count = 0;
    block->successors = NULL;
    block->successor_count = 0;
    block->is_entry = false;
    block->is_exit = false;
    block->is_reachable = false;
    
    // Add block to current function
    if (builder->current_function) {
        IRFunction* func = builder->current_function;
        func->blocks = realloc(func->blocks, sizeof(IRBlock*) * (func->block_count + 1));
        if (!func->blocks) {
            ir_builder_set_error(builder, "Memory allocation failed");
            free(block);
            return NULL;
        }
        func->blocks[func->block_count++] = block;
        
        // Set as entry block if it's the first one
        if (func->block_count == 1) {
            func->entry_block = block;
            block->is_entry = true;
        }
    }
    
    return block;
}

void ir_builder_destroy_block(IRBlock* block) {
    if (!block) return;
    
    if (block->label) free(block->label);
    
    if (block->instructions) {
        for (uint32_t i = 0; i < block->instruction_count; i++) {
            ir_builder_destroy_instruction(block->instructions[i]);
        }
        free(block->instructions);
    }
    
    if (block->predecessors) free(block->predecessors);
    if (block->successors) free(block->successors);
    
    free(block);
}

void ir_builder_set_current_block(IRBuilder* builder, IRBlock* block) {
    if (!builder) return;
    builder->current_block = block;
}

// Instruction creation
IRInstruction* ir_builder_create_instruction(IRBuilder* builder, IROp op) {
    if (!builder) {
        ir_builder_set_error(builder, "Invalid builder");
        return NULL;
    }
    
    IRInstruction* instruction = malloc(sizeof(IRInstruction));
    if (!instruction) {
        ir_builder_set_error(builder, "Memory allocation failed");
        return NULL;
    }
    
    instruction->op = op;
    instruction->operands = NULL;
    instruction->operand_count = 0;
    instruction->result.type = IR_VALUE_TEMP;
    instruction->result.data.temp_id = 0;
    instruction->target = 0;
    instruction->line = 0;
    instruction->col = 0;
    
    return instruction;
}

void ir_builder_destroy_instruction(IRInstruction* instruction) {
    if (!instruction) return;
    
    if (instruction->operands) {
        free(instruction->operands);
    }
    
    free(instruction);
}

void ir_builder_add_instruction(IRBuilder* builder, IRInstruction* instruction) {
    if (!builder || !instruction) return;
    
    if (!builder->current_block) {
        ir_builder_set_error(builder, "No current block");
        return;
    }
    
    IRBlock* block = builder->current_block;
    
    // Resize instruction array if needed
    if (block->instruction_count >= block->capacity) {
        block->capacity = block->capacity == 0 ? 8 : block->capacity * 2;
        block->instructions = realloc(block->instructions, sizeof(IRInstruction*) * block->capacity);
        if (!block->instructions) {
            ir_builder_set_error(builder, "Memory allocation failed");
            return;
        }
    }
    
    block->instructions[block->instruction_count++] = instruction;
}

// Value creation
IRValue ir_builder_create_i64_value(int64_t value) {
    IRValue val;
    val.type = IR_VALUE_I64;
    val.data.i64 = value;
    return val;
}

IRValue ir_builder_create_f64_value(double value) {
    IRValue val;
    val.type = IR_VALUE_F64;
    val.data.f64 = value;
    return val;
}

IRValue ir_builder_create_bool_value(bool value) {
    IRValue val;
    val.type = IR_VALUE_BOOL;
    val.data.boolean = value;
    return val;
}

IRValue ir_builder_create_object_value(void* object) {
    IRValue val;
    val.type = IR_VALUE_OBJECT;
    val.data.object = object;
    return val;
}

IRValue ir_builder_create_string_value(uint32_t string_id) {
    IRValue val;
    val.type = IR_VALUE_STRING;
    val.data.string_id = string_id;
    return val;
}

IRValue ir_builder_create_null_value(void) {
    IRValue val;
    val.type = IR_VALUE_NULL;
    val.data.object = NULL;
    return val;
}

IRValue ir_builder_create_temp_value(IRBuilder* builder, IRValueType type) {
    (void)type; // Suppress unused parameter warning
    IRValue val;
    val.type = IR_VALUE_TEMP;
    val.data.temp_id = builder->next_temp_id++;
    return val;
}

// Operand management
void ir_instruction_add_operand(IRInstruction* instruction, IRValue value) {
    if (!instruction) return;
    
    instruction->operands = realloc(instruction->operands, sizeof(IRValue) * (instruction->operand_count + 1));
    if (!instruction->operands) return;
    
    instruction->operands[instruction->operand_count++] = value;
}

void ir_instruction_set_result(IRInstruction* instruction, IRValue value) {
    if (!instruction) return;
    instruction->result = value;
}

void ir_instruction_set_target(IRInstruction* instruction, uint32_t target) {
    if (!instruction) return;
    instruction->target = target;
}

// Control flow
void ir_builder_add_jump(IRBuilder* builder, IRBlock* target) {
    if (!builder || !target) return;
    
    IRInstruction* jump = ir_builder_create_instruction(builder, IR_JMP);
    if (!jump) return;
    
    ir_instruction_set_target(jump, target->id);
    ir_builder_add_instruction(builder, jump);
}

void ir_builder_add_conditional_jump(IRBuilder* builder, IRValue condition, IRBlock* true_target, IRBlock* false_target) {
    if (!builder || !true_target || !false_target) return;
    
    IRInstruction* jump_true = ir_builder_create_instruction(builder, IR_JMPT);
    if (!jump_true) return;
    
    ir_instruction_add_operand(jump_true, condition);
    ir_instruction_set_target(jump_true, true_target->id);
    ir_builder_add_instruction(builder, jump_true);
    
    IRInstruction* jump_false = ir_builder_create_instruction(builder, IR_JMPF);
    if (!jump_false) return;
    
    ir_instruction_add_operand(jump_false, condition);
    ir_instruction_set_target(jump_false, false_target->id);
    ir_builder_add_instruction(builder, jump_false);
}

// Error handling
void ir_builder_set_error(IRBuilder* builder, const char* message) {
    if (!builder) return;
    
    if (builder->error_message) {
        free(builder->error_message);
    }
    
    builder->error_message = message ? strdup(message) : NULL;
    builder->has_error = true;
}

const char* ir_builder_get_error(IRBuilder* builder) {
    if (!builder) return NULL;
    return builder->error_message;
}

bool ir_builder_has_error(IRBuilder* builder) {
    if (!builder) return false;
    return builder->has_error;
}

// Utility functions
uint32_t ir_builder_get_next_temp_id(IRBuilder* builder) {
    if (!builder) return 0;
    return builder->next_temp_id++;
}

uint32_t ir_builder_get_next_block_id(IRBuilder* builder) {
    if (!builder) return 0;
    return builder->next_block_id++;
}

// Debug and printing
void ir_print_value(IRValue value) {
    switch (value.type) {
        case IR_VALUE_I64:
            printf("%lld", value.data.i64);
            break;
        case IR_VALUE_F64:
            printf("%.6f", value.data.f64);
            break;
        case IR_VALUE_BOOL:
            printf("%s", value.data.boolean ? "true" : "false");
            break;
        case IR_VALUE_OBJECT:
            printf("object@%p", value.data.object);
            break;
        case IR_VALUE_STRING:
            printf("string#%u", value.data.string_id);
            break;
        case IR_VALUE_NULL:
            printf("null");
            break;
        case IR_VALUE_TEMP:
            printf("t%u", value.data.temp_id);
            break;
        default:
            printf("unknown");
            break;
    }
}

void ir_print_instruction(IRInstruction* instruction) {
    if (!instruction) return;
    
    // Print operation
    printf("  %s", ir_op_to_string(instruction->op));
    
    // Print operands
    if (instruction->operand_count > 0) {
        printf(" ");
        for (uint32_t i = 0; i < instruction->operand_count; i++) {
            if (i > 0) printf(", ");
            ir_print_value(instruction->operands[i]);
        }
    }
    
    // Print result
    if (instruction->result.type != IR_VALUE_TEMP || instruction->result.data.temp_id != 0) {
        printf(" -> ");
        ir_print_value(instruction->result);
    }
    
    // Print target for jumps
    if (instruction->target != 0) {
        printf(" -> block_%u", instruction->target);
    }
    
    printf("\n");
}

void ir_print_block(IRBlock* block) {
    if (!block) return;
    
    printf("block_%u", block->id);
    if (block->label) {
        printf(" (%s)", block->label);
    }
    printf(":\n");
    
    for (uint32_t i = 0; i < block->instruction_count; i++) {
        ir_print_instruction(block->instructions[i]);
    }
}

void ir_print_function(IRFunction* function) {
    if (!function) return;
    
    printf("function %s", function->name);
    if (function->is_static) printf(" (static)");
    if (function->is_virtual) printf(" (virtual)");
    if (function->is_async) printf(" (async)");
    printf(":\n");
    
    for (uint32_t i = 0; i < function->block_count; i++) {
        ir_print_block(function->blocks[i]);
        printf("\n");
    }
}

// Helper function to convert opcode to string
const char* ir_op_to_string(IROp op) {
    switch (op) {
        case IR_LOAD_CONST: return "LOAD_CONST";
        case IR_LOAD_LOCAL: return "LOAD_LOCAL";
        case IR_LOAD_ARG: return "LOAD_ARG";
        case IR_LOAD_FIELD: return "LOAD_FIELD";
        case IR_LOAD_STATIC: return "LOAD_STATIC";
        case IR_LOAD_ARRAY: return "LOAD_ARRAY";
        case IR_STORE_LOCAL: return "STORE_LOCAL";
        case IR_STORE_FIELD: return "STORE_FIELD";
        case IR_STORE_STATIC: return "STORE_STATIC";
        case IR_STORE_ARRAY: return "STORE_ARRAY";
        case IR_ADD: return "ADD";
        case IR_SUB: return "SUB";
        case IR_MUL: return "MUL";
        case IR_DIV: return "DIV";
        case IR_MOD: return "MOD";
        case IR_NEG: return "NEG";
        case IR_INC: return "INC";
        case IR_DEC: return "DEC";
        case IR_EQ: return "EQ";
        case IR_NE: return "NE";
        case IR_LT: return "LT";
        case IR_LE: return "LE";
        case IR_GT: return "GT";
        case IR_GE: return "GE";
        case IR_AND: return "AND";
        case IR_OR: return "OR";
        case IR_NOT: return "NOT";
        case IR_JMP: return "JMP";
        case IR_JMPF: return "JMPF";
        case IR_JMPT: return "JMPT";
        case IR_JMP_EQ: return "JMP_EQ";
        case IR_JMP_NE: return "JMP_NE";
        case IR_JMP_LT: return "JMP_LT";
        case IR_JMP_LE: return "JMP_LE";
        case IR_JMP_GT: return "JMP_GT";
        case IR_JMP_GE: return "JMP_GE";
        case IR_CALL: return "CALL";
        case IR_CALLV: return "CALLV";
        case IR_CALLI: return "CALLI";
        case IR_RETURN: return "RETURN";
        case IR_RETURN_VAL: return "RETURN_VAL";
        case IR_NEW: return "NEW";
        case IR_NEW_ARRAY: return "NEW_ARRAY";
        case IR_CAST: return "CAST";
        case IR_INSTANCEOF: return "INSTANCEOF";
        case IR_IS_NULL: return "IS_NULL";
        case IR_IS_NOT_NULL: return "IS_NOT_NULL";
        case IR_BOX: return "BOX";
        case IR_UNBOX: return "UNBOX";
        case IR_GET_TYPE: return "GET_TYPE";
        case IR_SPAWN: return "SPAWN";
        case IR_AWAIT: return "AWAIT";
        case IR_ASYNC_RETURN: return "ASYNC_RETURN";
        case IR_MATCH_SOME: return "MATCH_SOME";
        case IR_MATCH_NONE: return "MATCH_NONE";
        case IR_MATCH_OK: return "MATCH_OK";
        case IR_MATCH_ERR: return "MATCH_ERR";
        case IR_PHI: return "PHI";
        case IR_COPY: return "COPY";
        case IR_NOP: return "NOP";
        default: return "UNKNOWN";
    }
}
