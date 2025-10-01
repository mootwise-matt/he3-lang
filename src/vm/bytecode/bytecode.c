#include "bytecode.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Bytecode writer implementation
BytecodeWriter* bytecode_writer_create(size_t initial_capacity) {
    BytecodeWriter* writer = malloc(sizeof(BytecodeWriter));
    if (!writer) return NULL;
    
    writer->buffer = malloc(initial_capacity);
    if (!writer->buffer) {
        free(writer);
        return NULL;
    }
    
    writer->capacity = initial_capacity;
    writer->position = 0;
    writer->size = 0;
    writer->strings = string_table_create();
    writer->relocations = relocation_table_create();
    writer->error_message = NULL;
    writer->has_error = false;
    
    if (!writer->strings || !writer->relocations) {
        bytecode_writer_destroy(writer);
        return NULL;
    }
    
    return writer;
}

void bytecode_writer_destroy(BytecodeWriter* writer) {
    if (!writer) return;
    
    if (writer->buffer) free(writer->buffer);
    if (writer->strings) string_table_destroy(writer->strings);
    if (writer->relocations) relocation_table_destroy(writer->relocations);
    if (writer->error_message) free(writer->error_message);
    
    free(writer);
}

// Buffer management
bool bytecode_writer_ensure_capacity(BytecodeWriter* writer, size_t additional_bytes) {
    if (!writer) return false;
    
    if (writer->position + additional_bytes > writer->capacity) {
        size_t new_capacity = writer->capacity * 2;
        while (new_capacity < writer->position + additional_bytes) {
            new_capacity *= 2;
        }
        
        uint8_t* new_buffer = realloc(writer->buffer, new_capacity);
        if (!new_buffer) {
            bytecode_writer_set_error(writer, "Memory allocation failed");
            return false;
        }
        
        writer->buffer = new_buffer;
        writer->capacity = new_capacity;
    }
    
    return true;
}

uint8_t* bytecode_writer_get_current_position(BytecodeWriter* writer) {
    if (!writer) return NULL;
    return writer->buffer + writer->position;
}

size_t bytecode_writer_get_size(BytecodeWriter* writer) {
    if (!writer) return 0;
    return writer->size;
}

const uint8_t* bytecode_writer_get_buffer(BytecodeWriter* writer) {
    if (!writer) return NULL;
    return writer->buffer;
}

// Basic emission functions
bool bytecode_writer_emit_u8(BytecodeWriter* writer, uint8_t value) {
    if (!bytecode_writer_ensure_capacity(writer, 1)) return false;
    
    writer->buffer[writer->position++] = value;
    if (writer->position > writer->size) {
        writer->size = writer->position;
    }
    
    return true;
}

bool bytecode_writer_emit_u16(BytecodeWriter* writer, uint16_t value) {
    if (!bytecode_writer_ensure_capacity(writer, 2)) return false;
    
    writer->buffer[writer->position++] = (uint8_t)(value & 0xFF);
    writer->buffer[writer->position++] = (uint8_t)((value >> 8) & 0xFF);
    
    if (writer->position > writer->size) {
        writer->size = writer->position;
    }
    
    return true;
}

bool bytecode_writer_emit_u32(BytecodeWriter* writer, uint32_t value) {
    if (!bytecode_writer_ensure_capacity(writer, 4)) return false;
    
    writer->buffer[writer->position++] = (uint8_t)(value & 0xFF);
    writer->buffer[writer->position++] = (uint8_t)((value >> 8) & 0xFF);
    writer->buffer[writer->position++] = (uint8_t)((value >> 16) & 0xFF);
    writer->buffer[writer->position++] = (uint8_t)((value >> 24) & 0xFF);
    
    if (writer->position > writer->size) {
        writer->size = writer->position;
    }
    
    return true;
}

bool bytecode_writer_emit_u64(BytecodeWriter* writer, uint64_t value) {
    if (!bytecode_writer_ensure_capacity(writer, 8)) return false;
    
    for (int i = 0; i < 8; i++) {
        writer->buffer[writer->position++] = (uint8_t)(value & 0xFF);
        value >>= 8;
    }
    
    if (writer->position > writer->size) {
        writer->size = writer->position;
    }
    
    return true;
}

bool bytecode_writer_emit_i8(BytecodeWriter* writer, int8_t value) {
    return bytecode_writer_emit_u8(writer, (uint8_t)value);
}

bool bytecode_writer_emit_i16(BytecodeWriter* writer, int16_t value) {
    return bytecode_writer_emit_u16(writer, (uint16_t)value);
}

bool bytecode_writer_emit_i32(BytecodeWriter* writer, int32_t value) {
    return bytecode_writer_emit_u32(writer, (uint32_t)value);
}

bool bytecode_writer_emit_i64(BytecodeWriter* writer, int64_t value) {
    return bytecode_writer_emit_u64(writer, (uint64_t)value);
}

bool bytecode_writer_emit_f64(BytecodeWriter* writer, double value) {
    // Convert double to uint64_t for emission
    union { double f; uint64_t i; } converter;
    converter.f = value;
    return bytecode_writer_emit_u64(writer, converter.i);
}

bool bytecode_writer_emit_bool(BytecodeWriter* writer, bool value) {
    return bytecode_writer_emit_u8(writer, value ? 1 : 0);
}

// String emission
uint32_t bytecode_writer_emit_string(BytecodeWriter* writer, const char* str) {
    if (!writer || !str) return 0;
    return string_table_add_string(writer->strings, str);
}

uint32_t bytecode_writer_emit_string_with_length(BytecodeWriter* writer, const char* str, size_t length) {
    if (!writer || !str) return 0;
    return string_table_add_string_with_length(writer->strings, str, length);
}

// Instruction emission
bool bytecode_writer_emit_opcode(BytecodeWriter* writer, OpCode opcode) {
    return bytecode_writer_emit_u8(writer, (uint8_t)opcode);
}

bool bytecode_writer_emit_operand(BytecodeWriter* writer, uint32_t operand) {
    return bytecode_writer_emit_u32(writer, operand);
}

bool bytecode_writer_emit_instruction(BytecodeWriter* writer, OpCode opcode, uint32_t* operands, uint16_t operand_count) {
    return bytecode_writer_emit_instruction_with_flags(writer, opcode, 0, operands, operand_count);
}

bool bytecode_writer_emit_instruction_with_flags(BytecodeWriter* writer, OpCode opcode, uint8_t flags, uint32_t* operands, uint16_t operand_count) {
    if (!writer) return false;
    
    // Emit opcode
    if (!bytecode_writer_emit_u8(writer, (uint8_t)opcode)) return false;
    
    // Emit flags
    if (!bytecode_writer_emit_u8(writer, flags)) return false;
    
    // Emit operand count
    if (!bytecode_writer_emit_u16(writer, operand_count)) return false;
    
    // Emit operands
    for (uint16_t i = 0; i < operand_count; i++) {
        if (!bytecode_writer_emit_u32(writer, operands[i])) return false;
    }
    
    return true;
}

// Relocation support
bool bytecode_writer_add_relocation(BytecodeWriter* writer, uint32_t offset, uint32_t type, uint32_t symbol_id, int32_t addend) {
    if (!writer) return false;
    return relocation_table_add_entry(writer->relocations, offset, type, symbol_id, addend);
}

bool bytecode_writer_resolve_relocations(BytecodeWriter* writer) {
    if (!writer || !writer->relocations) return false;
    
    // For now, just return true - actual relocation resolution would be implemented later
    // when we have symbol tables and linking
    return true;
}

// String table functions
StringTable* string_table_create(void) {
    StringTable* table = malloc(sizeof(StringTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->total_size = 0;
    table->entries = NULL;
    table->data = NULL;
    
    return table;
}

void string_table_destroy(StringTable* table) {
    if (!table) return;
    
    if (table->entries) free(table->entries);
    if (table->data) free(table->data);
    
    free(table);
}

uint32_t string_table_add_string(StringTable* table, const char* str) {
    if (!table || !str) return 0;
    return string_table_add_string_with_length(table, str, strlen(str));
}

uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length) {
    if (!table || !str) return 0;
    
    // Check if string already exists
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].length == length &&
            memcmp(table->data + table->entries[i].offset, str, length) == 0) {
            return i + 1; // Return 1-based ID
        }
    }
    
    // Add new string
    uint32_t new_offset = table->total_size;
    size_t new_total_size = table->total_size + length + 1; // +1 for null terminator
    
    // Reallocate data buffer
    char* new_data = realloc(table->data, new_total_size);
    if (!new_data) return 0;
    
    // Reallocate entries array
    StringEntry* new_entries = realloc(table->entries, sizeof(StringEntry) * (table->count + 1));
    if (!new_entries) {
        free(new_data);
        return 0;
    }
    
    table->data = new_data;
    table->entries = new_entries;
    
    // Copy string data
    memcpy(table->data + new_offset, str, length);
    table->data[new_offset + length] = '\0';
    
    // Add entry
    table->entries[table->count].offset = new_offset;
    table->entries[table->count].length = length;
    table->entries[table->count].hash = 0; // TODO: Calculate hash
    
    table->total_size = new_total_size;
    return ++table->count; // Return 1-based ID
}

const char* string_table_get_string(StringTable* table, uint32_t id) {
    if (!table || id == 0 || id > table->count) return NULL;
    return table->data + table->entries[id - 1].offset;
}

uint32_t string_table_get_count(StringTable* table) {
    if (!table) return 0;
    return table->count;
}

uint32_t string_table_get_total_size(StringTable* table) {
    if (!table) return 0;
    return table->total_size;
}

// Relocation table functions
RelocationTable* relocation_table_create(void) {
    RelocationTable* table = malloc(sizeof(RelocationTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->entries = NULL;
    
    return table;
}

void relocation_table_destroy(RelocationTable* table) {
    if (!table) return;
    
    if (table->entries) free(table->entries);
    free(table);
}

bool relocation_table_add_entry(RelocationTable* table, uint32_t offset, uint32_t type, uint32_t symbol_id, int32_t addend) {
    if (!table) return false;
    
    RelocationEntry* new_entries = realloc(table->entries, sizeof(RelocationEntry) * (table->count + 1));
    if (!new_entries) return false;
    
    table->entries = new_entries;
    table->entries[table->count].offset = offset;
    table->entries[table->count].type = type;
    table->entries[table->count].symbol_id = symbol_id;
    table->entries[table->count].addend = addend;
    
    table->count++;
    return true;
}

uint32_t relocation_table_get_count(RelocationTable* table) {
    if (!table) return 0;
    return table->count;
}

// Error handling
void bytecode_writer_set_error(BytecodeWriter* writer, const char* message) {
    if (!writer) return;
    
    if (writer->error_message) {
        free(writer->error_message);
    }
    
    writer->error_message = message ? strdup(message) : NULL;
    writer->has_error = true;
}

const char* bytecode_writer_get_error(BytecodeWriter* writer) {
    if (!writer) return NULL;
    return writer->error_message;
}

bool bytecode_writer_has_error(BytecodeWriter* writer) {
    if (!writer) return false;
    return writer->has_error;
}

// Utility functions
void bytecode_writer_align(BytecodeWriter* writer, size_t alignment) {
    if (!writer) return;
    
    size_t aligned_position = (writer->position + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned_position - writer->position;
    
    if (padding > 0) {
        bytecode_writer_pad(writer, padding);
    }
}

void bytecode_writer_pad(BytecodeWriter* writer, size_t bytes) {
    if (!writer) return;
    
    if (!bytecode_writer_ensure_capacity(writer, bytes)) return;
    
    memset(writer->buffer + writer->position, 0, bytes);
    writer->position += bytes;
    
    if (writer->position > writer->size) {
        writer->size = writer->position;
    }
}

size_t bytecode_writer_get_aligned_size(BytecodeWriter* writer, size_t alignment) {
    if (!writer) return 0;
    return (writer->size + alignment - 1) & ~(alignment - 1);
}

// Debug functions
void bytecode_writer_print_hex(BytecodeWriter* writer) {
    if (!writer) return;
    
    printf("Bytecode (%zu bytes):\n", writer->size);
    for (size_t i = 0; i < writer->size; i++) {
        if (i % 16 == 0) {
            printf("%08zx: ", i);
        }
        printf("%02x ", writer->buffer[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    if (writer->size % 16 != 0) {
        printf("\n");
    }
}

void bytecode_writer_print_disassembly(BytecodeWriter* writer) {
    if (!writer) return;
    
    printf("Disassembly:\n");
    size_t pos = 0;
    
    while (pos < writer->size) {
        printf("%08zx: ", pos);
        
        if (pos >= writer->size) break;
        
        uint8_t opcode = writer->buffer[pos++];
        printf("%s", opcode_to_string((OpCode)opcode));
        
        if (pos >= writer->size) break;
        
        uint8_t flags = writer->buffer[pos++];
        if (flags != 0) {
            printf(" [flags=0x%02x]", flags);
        }
        
        if (pos + 2 > writer->size) break;
        
        uint16_t operand_count = writer->buffer[pos] | (writer->buffer[pos + 1] << 8);
        pos += 2;
        
        if (operand_count > 0) {
            printf(" ");
            for (uint16_t i = 0; i < operand_count && pos + 4 <= writer->size; i++) {
                if (i > 0) printf(", ");
                uint32_t operand = writer->buffer[pos] | 
                                  (writer->buffer[pos + 1] << 8) |
                                  (writer->buffer[pos + 2] << 16) |
                                  (writer->buffer[pos + 3] << 24);
                pos += 4;
                printf("%u", operand);
            }
        }
        
        printf("\n");
    }
}

const char* opcode_to_string(OpCode opcode) {
    switch (opcode) {
        case OP_PUSH_I64: return "PUSH_I64";
        case OP_PUSH_F64: return "PUSH_F64";
        case OP_PUSH_BOOL: return "PUSH_BOOL";
        case OP_PUSH_STR: return "PUSH_STR";
        case OP_PUSH_NULL: return "PUSH_NULL";
        case OP_POP: return "POP";
        case OP_DUP: return "DUP";
        case OP_SWAP: return "SWAP";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_LOCAL_0: return "LOAD_LOCAL_0";
        case OP_LOAD_LOCAL_1: return "LOAD_LOCAL_1";
        case OP_LOAD_LOCAL_2: return "LOAD_LOCAL_2";
        case OP_LOAD_LOCAL_3: return "LOAD_LOCAL_3";
        case OP_STORE_LOCAL_0: return "STORE_LOCAL_0";
        case OP_STORE_LOCAL_1: return "STORE_LOCAL_1";
        case OP_STORE_LOCAL_2: return "STORE_LOCAL_2";
        case OP_STORE_LOCAL_3: return "STORE_LOCAL_3";
        case OP_LOAD_ARG: return "LOAD_ARG";
        case OP_LOAD_ARG_0: return "LOAD_ARG_0";
        case OP_LOAD_ARG_1: return "LOAD_ARG_1";
        case OP_LOAD_ARG_2: return "LOAD_ARG_2";
        case OP_LOAD_ARG_3: return "LOAD_ARG_3";
        case OP_LOAD_FIELD: return "LOAD_FIELD";
        case OP_STORE_FIELD: return "STORE_FIELD";
        case OP_LOAD_STATIC: return "LOAD_STATIC";
        case OP_STORE_STATIC: return "STORE_STATIC";
        case OP_LOAD_ARRAY: return "LOAD_ARRAY";
        case OP_STORE_ARRAY: return "STORE_ARRAY";
        case OP_ARRAY_LENGTH: return "ARRAY_LENGTH";
        case OP_NEW_ARRAY: return "NEW_ARRAY";
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
        case OP_XOR: return "XOR";
        case OP_BIT_AND: return "BIT_AND";
        case OP_BIT_OR: return "BIT_OR";
        case OP_BIT_XOR: return "BIT_XOR";
        case OP_BIT_NOT: return "BIT_NOT";
        case OP_SHL: return "SHL";
        case OP_SHR: return "SHR";
        case OP_USHR: return "USHR";
        case OP_JMP: return "JMP";
        case OP_JMPF: return "JMPF";
        case OP_JMPT: return "JMPT";
        case OP_JMP_EQ: return "JMP_EQ";
        case OP_JMP_NE: return "JMP_NE";
        case OP_JMP_LT: return "JMP_LT";
        case OP_JMP_LE: return "JMP_LE";
        case OP_JMP_GT: return "JMP_GT";
        case OP_JMP_GE: return "JMP_GE";
        case OP_CALL: return "CALL";
        case OP_CALLV: return "CALLV";
        case OP_CALLI: return "CALLI";
        case OP_RETURN: return "RETURN";
        case OP_RETURN_VAL: return "RETURN_VAL";
        case OP_NEW: return "NEW";
        case OP_CAST: return "CAST";
        case OP_INSTANCEOF: return "INSTANCEOF";
        case OP_IS_NULL: return "IS_NULL";
        case OP_IS_NOT_NULL: return "IS_NOT_NULL";
        case OP_BOX: return "BOX";
        case OP_UNBOX: return "UNBOX";
        case OP_GET_TYPE: return "GET_TYPE";
        case OP_GET_TYPE_NAME: return "GET_TYPE_NAME";
        case OP_SPAWN: return "SPAWN";
        case OP_AWAIT: return "AWAIT";
        case OP_ASYNC_RETURN: return "ASYNC_RETURN";
        case OP_YIELD: return "YIELD";
        case OP_MATCH: return "MATCH";
        case OP_MATCH_SOME: return "MATCH_SOME";
        case OP_MATCH_NONE: return "MATCH_NONE";
        case OP_MATCH_OK: return "MATCH_OK";
        case OP_MATCH_ERR: return "MATCH_ERR";
        case OP_SECURITY_CHECK: return "SECURITY_CHECK";
        case OP_DOMAIN_ENTER: return "DOMAIN_ENTER";
        case OP_DOMAIN_EXIT: return "DOMAIN_EXIT";
        case OP_CROWD_VERIFY: return "CROWD_VERIFY";
        case OP_HALT: return "HALT";
        default: return "UNKNOWN";
    }
}
