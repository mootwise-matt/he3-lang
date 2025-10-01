#include "opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// OPCODE UTILITY FUNCTIONS
// ============================================================================

const char* opcode_get_name(uint8_t opcode) {
    switch (opcode) {
        case OP_PUSH_NULL: return "PUSH_NULL";
        case OP_PUSH_TRUE: return "PUSH_TRUE";
        case OP_PUSH_FALSE: return "PUSH_FALSE";
        case OP_PUSH_INT8: return "PUSH_INT8";
        case OP_PUSH_INT16: return "PUSH_INT16";
        case OP_PUSH_INT32: return "PUSH_INT32";
        case OP_PUSH_INT64: return "PUSH_INT64";
        case OP_PUSH_UINT8: return "PUSH_UINT8";
        case OP_PUSH_UINT16: return "PUSH_UINT16";
        case OP_PUSH_UINT32: return "PUSH_UINT32";
        case OP_PUSH_UINT64: return "PUSH_UINT64";
        case OP_PUSH_FLOAT32: return "PUSH_FLOAT32";
        case OP_PUSH_FLOAT64: return "PUSH_FLOAT64";
        case OP_PUSH_STRING: return "PUSH_STRING";
        case OP_PUSH_CONSTANT: return "PUSH_CONSTANT";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_SWAP: return "SWAP";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_NEG: return "NEG";
        case OP_INC: return "INC";
        case OP_DEC: return "DEC";
        case OP_EQ: return "EQ";
        case OP_NE: return "NE";
        case OP_LT: return "LT";
        case OP_LE: return "LE";
        case OP_GT: return "GT";
        case OP_GE: return "GE";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_JUMP_IF_NULL: return "JUMP_IF_NULL";
        case OP_JUMP_IF_NOT_NULL: return "JUMP_IF_NOT_NULL";
        case OP_CALL: return "CALL";
        case OP_CALL_VIRTUAL: return "CALL_VIRTUAL";
        case OP_CALL_STATIC: return "CALL_STATIC";
        case OP_CALL_INTERFACE: return "CALL_INTERFACE";
        case OP_RETURN: return "RETURN";
        case OP_RETURN_VALUE: return "RETURN_VALUE";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_ARG: return "LOAD_ARG";
        case OP_STORE_ARG: return "STORE_ARG";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_NEW_OBJECT: return "NEW_OBJECT";
        case OP_NEW_ARRAY: return "NEW_ARRAY";
        case OP_LOAD_FIELD: return "LOAD_FIELD";
        case OP_STORE_FIELD: return "STORE_FIELD";
        case OP_LOAD_STATIC_FIELD: return "LOAD_STATIC_FIELD";
        case OP_STORE_STATIC_FIELD: return "STORE_STATIC_FIELD";
        case OP_LOAD_ARRAY: return "LOAD_ARRAY";
        case OP_STORE_ARRAY: return "STORE_ARRAY";
        case OP_ARRAY_LENGTH: return "ARRAY_LENGTH";
        case OP_ARRAY_RESIZE: return "ARRAY_RESIZE";
        case OP_TYPE_OF: return "TYPE_OF";
        case OP_IS_INSTANCE_OF: return "IS_INSTANCE_OF";
        case OP_CAST: return "CAST";
        case OP_STRING_LENGTH: return "STRING_LENGTH";
        case OP_STRING_CONCAT: return "STRING_CONCAT";
        case OP_STRING_SUBSTRING: return "STRING_SUBSTRING";
        case OP_STRING_INDEX_OF: return "STRING_INDEX_OF";
        case OP_PRINT: return "PRINT";
        case OP_PRINTLN: return "PRINTLN";
        case OP_READ_LINE: return "READ_LINE";
        case OP_READ_INT: return "READ_INT";
        case OP_READ_FLOAT: return "READ_FLOAT";
        case OP_AWAIT: return "AWAIT";
        case OP_YIELD: return "YIELD";
        case OP_ASYNC_CALL: return "ASYNC_CALL";
        case OP_MATCH: return "MATCH";
        case OP_MATCH_CASE: return "MATCH_CASE";
        case OP_MATCH_WHEN: return "MATCH_WHEN";
        case OP_SECURE_CALL: return "SECURE_CALL";
        case OP_CHECK_PERMISSION: return "CHECK_PERMISSION";
        case OP_DOMAIN_CALL: return "DOMAIN_CALL";
        case OP_PUBLISH_EVENT: return "PUBLISH_EVENT";
        case OP_SUBSCRIBE_EVENT: return "SUBSCRIBE_EVENT";
        case OP_UNSUBSCRIBE_EVENT: return "UNSUBSCRIBE_EVENT";
        case OP_BREAKPOINT: return "BREAKPOINT";
        case OP_TRACE: return "TRACE";
        default: return "UNKNOWN";
    }
}

uint8_t opcode_get_operand_count(uint8_t opcode) {
    switch (opcode) {
        // No operands
        case OP_PUSH_NULL:
        case OP_PUSH_TRUE:
        case OP_PUSH_FALSE:
        case OP_POP:
        case OP_DUP:
        case OP_SWAP:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_NEG:
        case OP_INC:
        case OP_DEC:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_AND:
        case OP_OR:
        case OP_NOT:
        case OP_RETURN:
        case OP_ARRAY_LENGTH:
        case OP_TYPE_OF:
        case OP_STRING_LENGTH:
        case OP_STRING_CONCAT:
        case OP_STRING_SUBSTRING:
        case OP_STRING_INDEX_OF:
        case OP_PRINT:
        case OP_PRINTLN:
        case OP_READ_LINE:
        case OP_READ_INT:
        case OP_READ_FLOAT:
        case OP_AWAIT:
        case OP_YIELD:
        case OP_BREAKPOINT:
        case OP_TRACE:
            return 0;
            
        // 1 byte operand
        case OP_PUSH_INT8:
        case OP_PUSH_UINT8:
            return 1;
            
        // 2 byte operand
        case OP_PUSH_INT16:
        case OP_PUSH_UINT16:
            return 2;
            
        // 4 byte operand
        case OP_PUSH_INT32:
        case OP_PUSH_UINT32:
        case OP_PUSH_FLOAT32:
        case OP_PUSH_STRING:
        case OP_PUSH_CONSTANT:
        case OP_JUMP:
        case OP_JUMP_IF_TRUE:
        case OP_JUMP_IF_FALSE:
        case OP_JUMP_IF_NULL:
        case OP_JUMP_IF_NOT_NULL:
        case OP_CALL:
        case OP_CALL_VIRTUAL:
        case OP_CALL_STATIC:
        case OP_CALL_INTERFACE:
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
        case OP_LOAD_ARG:
        case OP_STORE_ARG:
        case OP_LOAD_GLOBAL:
        case OP_STORE_GLOBAL:
        case OP_NEW_OBJECT:
        case OP_NEW_ARRAY:
        case OP_LOAD_FIELD:
        case OP_STORE_FIELD:
        case OP_LOAD_STATIC_FIELD:
        case OP_STORE_STATIC_FIELD:
        case OP_LOAD_ARRAY:
        case OP_STORE_ARRAY:
        case OP_ARRAY_RESIZE:
        case OP_IS_INSTANCE_OF:
        case OP_CAST:
        case OP_ASYNC_CALL:
        case OP_MATCH:
        case OP_MATCH_CASE:
        case OP_MATCH_WHEN:
        case OP_SECURE_CALL:
        case OP_CHECK_PERMISSION:
        case OP_DOMAIN_CALL:
        case OP_PUBLISH_EVENT:
        case OP_SUBSCRIBE_EVENT:
        case OP_UNSUBSCRIBE_EVENT:
            return 4;
            
        // 8 byte operand
        case OP_PUSH_INT64:
        case OP_PUSH_UINT64:
        case OP_PUSH_FLOAT64:
            return 8;
            
        default:
            return 0;
    }
}

uint8_t opcode_get_operand_size(uint8_t opcode) {
    return opcode_get_operand_count(opcode);
}

bool opcode_is_valid(uint8_t opcode) {
    return opcode_get_name(opcode) != "UNKNOWN";
}

bool opcode_has_operands(uint8_t opcode) {
    return opcode_get_operand_count(opcode) > 0;
}

const char* opcode_get_category(uint8_t opcode) {
    if (opcode >= OP_PUSH_NULL && opcode <= OP_SWAP) {
        return OPCODE_CATEGORY_STACK;
    } else if (opcode >= OP_ADD && opcode <= OP_DEC) {
        return OPCODE_CATEGORY_ARITHMETIC;
    } else if (opcode >= OP_EQ && opcode <= OP_GE) {
        return OPCODE_CATEGORY_COMPARISON;
    } else if (opcode >= OP_AND && opcode <= OP_NOT) {
        return OPCODE_CATEGORY_LOGICAL;
    } else if (opcode >= OP_JUMP && opcode <= OP_JUMP_IF_NOT_NULL) {
        return OPCODE_CATEGORY_CONTROL;
    } else if (opcode >= OP_CALL && opcode <= OP_RETURN_VALUE) {
        return OPCODE_CATEGORY_CALL;
    } else if (opcode >= OP_LOAD_LOCAL && opcode <= OP_STORE_ARG) {
        return OPCODE_CATEGORY_LOCAL;
    } else if (opcode >= OP_LOAD_GLOBAL && opcode <= OP_STORE_GLOBAL) {
        return OPCODE_CATEGORY_GLOBAL;
    } else if (opcode >= OP_NEW_OBJECT && opcode <= OP_STORE_STATIC_FIELD) {
        return OPCODE_CATEGORY_OBJECT;
    } else if (opcode >= OP_LOAD_ARRAY && opcode <= OP_ARRAY_RESIZE) {
        return OPCODE_CATEGORY_ARRAY;
    } else if (opcode >= OP_TYPE_OF && opcode <= OP_CAST) {
        return OPCODE_CATEGORY_TYPE;
    } else if (opcode >= OP_STRING_LENGTH && opcode <= OP_STRING_INDEX_OF) {
        return OPCODE_CATEGORY_STRING;
    } else if (opcode >= OP_PRINT && opcode <= OP_READ_FLOAT) {
        return OPCODE_CATEGORY_BUILTIN;
    } else if (opcode >= OP_AWAIT && opcode <= OP_ASYNC_CALL) {
        return OPCODE_CATEGORY_ASYNC;
    } else if (opcode >= OP_MATCH && opcode <= OP_MATCH_WHEN) {
        return OPCODE_CATEGORY_PATTERN;
    } else if (opcode >= OP_SECURE_CALL && opcode <= OP_DOMAIN_CALL) {
        return OPCODE_CATEGORY_SECURITY;
    } else if (opcode >= OP_PUBLISH_EVENT && opcode <= OP_UNSUBSCRIBE_EVENT) {
        return OPCODE_CATEGORY_EVENT;
    } else if (opcode >= OP_BREAKPOINT && opcode <= OP_TRACE) {
        return OPCODE_CATEGORY_DEBUG;
    } else {
        return "UNKNOWN";
    }
}

void opcode_disassemble(uint8_t* bytecode, size_t offset, char* buffer, size_t buffer_size) {
    if (!bytecode || !buffer || buffer_size == 0) return;
    
    uint8_t opcode = bytecode[offset];
    const char* name = opcode_get_name(opcode);
    
    snprintf(buffer, buffer_size, "%s", name);
}

void opcode_disassemble_with_operands(uint8_t* bytecode, size_t offset, char* buffer, size_t buffer_size) {
    if (!bytecode || !buffer || buffer_size == 0) return;
    
    uint8_t opcode = bytecode[offset];
    const char* name = opcode_get_name(opcode);
    uint8_t operand_count = opcode_get_operand_count(opcode);
    
    if (operand_count == 0) {
        snprintf(buffer, buffer_size, "%s", name);
    } else {
        snprintf(buffer, buffer_size, "%s ", name);
        size_t name_len = strlen(name);
        size_t remaining = buffer_size - name_len - 1;
        
        if (remaining > 0) {
            char* operand_start = buffer + name_len + 1;
            for (uint8_t i = 0; i < operand_count && remaining > 0; i++) {
                int written = snprintf(operand_start, remaining, "%02X ", bytecode[offset + 1 + i]);
                if (written > 0) {
                    operand_start += written;
                    remaining -= written;
                }
            }
        }
    }
}

size_t opcode_get_size(uint8_t* bytecode, size_t offset) {
    if (!bytecode) return 0;
    
    uint8_t opcode = bytecode[offset];
    return 1 + opcode_get_operand_count(opcode);
}

bool opcode_validate(uint8_t* bytecode, size_t offset, size_t bytecode_size) {
    if (!bytecode || offset >= bytecode_size) return false;
    
    uint8_t opcode = bytecode[offset];
    if (!opcode_is_valid(opcode)) return false;
    
    uint8_t operand_count = opcode_get_operand_count(opcode);
    return (offset + 1 + operand_count) <= bytecode_size;
}

bool opcode_validate_all(uint8_t* bytecode, size_t bytecode_size) {
    if (!bytecode) return false;
    
    size_t offset = 0;
    while (offset < bytecode_size) {
        if (!opcode_validate(bytecode, offset, bytecode_size)) {
            return false;
        }
        offset += opcode_get_size(bytecode, offset);
    }
    
    return true;
}

uint64_t opcode_read_operand(uint8_t* bytecode, size_t offset, uint8_t operand_size) {
    if (!bytecode || operand_size == 0) return 0;
    
    uint64_t value = 0;
    for (uint8_t i = 0; i < operand_size; i++) {
        value |= ((uint64_t)bytecode[offset + i]) << (i * 8);
    }
    
    return value;
}

void opcode_write_operand(uint8_t* bytecode, size_t offset, uint8_t operand_size, uint64_t value) {
    if (!bytecode || operand_size == 0) return;
    
    for (uint8_t i = 0; i < operand_size; i++) {
        bytecode[offset + i] = (uint8_t)((value >> (i * 8)) & 0xFF);
    }
}

size_t opcode_get_next_offset(uint8_t* bytecode, size_t offset) {
    if (!bytecode) return offset;
    
    uint8_t opcode = bytecode[offset];
    return offset + 1 + opcode_get_operand_count(opcode);
}

void opcode_count_by_category(uint8_t* bytecode, size_t bytecode_size, uint32_t* counts) {
    if (!bytecode || !counts) return;
    
    // Initialize counts
    memset(counts, 0, 16 * sizeof(uint32_t)); // 16 categories max
    
    size_t offset = 0;
    while (offset < bytecode_size) {
        uint8_t opcode = bytecode[offset];
        const char* category = opcode_get_category(opcode);
        
        // Simple category counting - just increment based on opcode range
        if (opcode >= OP_PUSH_NULL && opcode <= OP_SWAP) counts[0]++;
        else if (opcode >= OP_ADD && opcode <= OP_DEC) counts[1]++;
        else if (opcode >= OP_EQ && opcode <= OP_GE) counts[2]++;
        else if (opcode >= OP_AND && opcode <= OP_NOT) counts[3]++;
        else if (opcode >= OP_JUMP && opcode <= OP_JUMP_IF_NOT_NULL) counts[4]++;
        else if (opcode >= OP_CALL && opcode <= OP_RETURN_VALUE) counts[5]++;
        else if (opcode >= OP_LOAD_LOCAL && opcode <= OP_STORE_ARG) counts[6]++;
        else if (opcode >= OP_LOAD_GLOBAL && opcode <= OP_STORE_GLOBAL) counts[7]++;
        else if (opcode >= OP_NEW_OBJECT && opcode <= OP_STORE_STATIC_FIELD) counts[8]++;
        else if (opcode >= OP_LOAD_ARRAY && opcode <= OP_ARRAY_RESIZE) counts[9]++;
        else if (opcode >= OP_TYPE_OF && opcode <= OP_CAST) counts[10]++;
        else if (opcode >= OP_STRING_LENGTH && opcode <= OP_STRING_INDEX_OF) counts[11]++;
        else if (opcode >= OP_PRINT && opcode <= OP_READ_FLOAT) counts[12]++;
        else if (opcode >= OP_AWAIT && opcode <= OP_ASYNC_CALL) counts[13]++;
        else if (opcode >= OP_MATCH && opcode <= OP_MATCH_WHEN) counts[14]++;
        else counts[15]++; // Other/Unknown
        
        offset = opcode_get_next_offset(bytecode, offset);
    }
}

void opcode_get_most_common(uint8_t* bytecode, size_t bytecode_size, uint8_t* opcodes, uint32_t* counts, size_t max_count) {
    if (!bytecode || !opcodes || !counts || max_count == 0) return;
    
    // Simple implementation - just return the first few opcodes found
    size_t offset = 0;
    size_t found = 0;
    
    while (offset < bytecode_size && found < max_count) {
        uint8_t opcode = bytecode[offset];
        opcodes[found] = opcode;
        counts[found] = 1;
        found++;
        offset = opcode_get_next_offset(bytecode, offset);
    }
}

float opcode_calculate_density(uint8_t* bytecode, size_t bytecode_size) {
    if (!bytecode || bytecode_size == 0) return 0.0f;
    
    size_t instruction_count = 0;
    size_t offset = 0;
    
    while (offset < bytecode_size) {
        instruction_count++;
        offset = opcode_get_next_offset(bytecode, offset);
    }
    
    return (float)instruction_count / (float)bytecode_size;
}
