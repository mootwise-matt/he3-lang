#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../shared/bytecode/bytecode_format.h"
#include "../../shared/bytecode/opcodes.h"
#include "../objects/object.h"

// Forward declarations
struct BytecodeFile;
struct StringTable;
struct TypeTable;
struct MethodTable;

// Bytecode loader functions
BytecodeFile* bytecode_load_file(const char* filename);
void bytecode_destroy_file(BytecodeFile* file);

// String table functions
StringTable* string_table_create(void);
void string_table_destroy(StringTable* table);
const char* string_table_get(StringTable* table, uint32_t index);
uint32_t string_table_add(StringTable* table, const char* string);
uint32_t string_table_add_string(StringTable* table, const char* str);
uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length);

// Type table functions
TypeTable* type_table_create(void);
void type_table_destroy(TypeTable* table);
TypeEntry* type_table_get(TypeTable* table, uint32_t index);

// Method table functions
MethodTable* method_table_create(void);
void method_table_destroy(MethodTable* table);
MethodEntry* method_table_get(MethodTable* table, uint32_t index);
MethodEntry* method_table_find_by_name(MethodTable* table, const char* name);

// Utility functions
bool bytecode_validate_header(const BytecodeHeader* header);
const char* bytecode_get_domain_name(BytecodeFile* file);
void bytecode_print_info(BytecodeFile* file);