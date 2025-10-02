#include "../../shared/bytecode/bytecode_format.h"
#include "../../shared/bytecode/opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ============================================================================
// BYTECODE FILE OPERATIONS
// ============================================================================

BytecodeFile* bytecode_file_create(void) {
    BytecodeFile* file = malloc(sizeof(BytecodeFile));
    if (!file) return NULL;
    
    // Initialize header
    memcpy(file->header.magic, BYTECODE_MAGIC, BYTECODE_MAGIC_SIZE);
    file->header.version_major = BYTECODE_VERSION_MAJOR;
    file->header.version_minor = BYTECODE_VERSION_MINOR;
    file->header.flags = 0;
    
    // Initialize offsets to 0
    file->header.domain_name_offset = 0;
    file->header.module_name_offset = 0;
    file->header.module_version_offset = 0;
    file->header.string_table_offset = 0;
    file->header.string_table_size = 0;
    file->header.constant_table_offset = 0;
    file->header.constant_table_size = 0;
    file->header.type_table_offset = 0;
    file->header.type_table_size = 0;
    file->header.method_table_offset = 0;
    file->header.method_table_size = 0;
    // Field table not implemented yet
    file->header.field_table_offset = 0;
    file->header.field_table_size = 0;
    file->header.bytecode_offset = 0;
    file->header.bytecode_size = 0;
    file->header.entry_point_method_id = 0;
    file->header.main_class_id = 0;
    file->header.dependency_count = 0;
    file->header.dependency_table_offset = 0;
    
    // Initialize tables
    file->string_table = NULL;
    file->constant_table = NULL;
    file->type_table = NULL;
    file->method_table = NULL;
    // Field table not implemented yet
    file->field_table = NULL;
    file->dependency_table = NULL;
    file->bytecode = NULL;
    file->bytecode_size = 0;
    
    return file;
}

void bytecode_file_destroy(BytecodeFile* file) {
    if (!file) return;
    
    // Destroy tables
    if (file->string_table) {
        string_table_destroy(file->string_table);
    }
    if (file->constant_table) {
        constant_table_destroy(file->constant_table);
    }
    if (file->type_table) {
        type_table_destroy(file->type_table);
    }
    if (file->method_table) {
        method_table_destroy(file->method_table);
    }
    // Field table not implemented yet
    // if (file->field_table) {
    //     field_table_destroy(file->field_table);
    // }
    if (file->dependency_table) {
        free(file->dependency_table);
    }
    
    // Free bytecode
    if (file->bytecode) {
        free(file->bytecode);
    }
    
    free(file);
}

BytecodeFile* bytecode_file_load(const char* filename) {
    if (!filename) return NULL;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    // Read header
    BytecodeHeader header;
    if (fread(&header, sizeof(BytecodeHeader), 1, file) != 1) {
        fclose(file);
        return NULL;
    }
    
    // Validate magic number
    if (!validate_magic(header.magic)) {
        fclose(file);
        return NULL;
    }
    
    // Create bytecode file
    BytecodeFile* bytecode_file = bytecode_file_create();
    if (!bytecode_file) {
        fclose(file);
        return NULL;
    }
    
    // Copy header
    bytecode_file->header = header;
    
    // Load string table
    if (header.string_table_size > 0) {
        fseek(file, header.string_table_offset, SEEK_SET);
        bytecode_file->string_table = string_table_create();
        if (bytecode_file->string_table) {
            // Read string table header
            uint32_t count, total_size;
            if (fread(&count, sizeof(uint32_t), 1, file) == 1 &&
                fread(&total_size, sizeof(uint32_t), 1, file) == 1) {
                
                bytecode_file->string_table->count = count;
                bytecode_file->string_table->total_size = total_size;
                
                // Allocate and read entries
                if (count > 0) {
                    bytecode_file->string_table->entries = malloc(count * sizeof(StringEntry));
                    if (bytecode_file->string_table->entries) {
                        fread(bytecode_file->string_table->entries, sizeof(StringEntry), count, file);
                    }
                }
                
                // Allocate and read string data
                if (total_size > 0) {
                    bytecode_file->string_table->data = malloc(total_size);
                    if (bytecode_file->string_table->data) {
                        fread(bytecode_file->string_table->data, 1, total_size, file);
                    }
                }
            }
        }
    }
    
    // Load constant table
    if (header.constant_table_size > 0) {
        fseek(file, header.constant_table_offset, SEEK_SET);
        bytecode_file->constant_table = constant_table_create();
        if (bytecode_file->constant_table) {
            // Read constant table header
            uint32_t count;
            if (fread(&count, sizeof(uint32_t), 1, file) == 1) {
                bytecode_file->constant_table->count = count;
                
                // Allocate and read entries
                if (count > 0) {
                    bytecode_file->constant_table->entries = malloc(count * sizeof(ConstantEntry));
                    if (bytecode_file->constant_table->entries) {
                        fread(bytecode_file->constant_table->entries, sizeof(ConstantEntry), count, file);
                    }
                }
            }
        }
    }
    
    // Load type table
    if (header.type_table_size > 0) {
        fseek(file, header.type_table_offset, SEEK_SET);
        bytecode_file->type_table = type_table_create();
        if (bytecode_file->type_table) {
            // Read type table header
            uint32_t count;
            if (fread(&count, sizeof(uint32_t), 1, file) == 1) {
                bytecode_file->type_table->count = count;
                
                // Allocate and read entries
                if (count > 0) {
                    bytecode_file->type_table->entries = malloc(count * sizeof(TypeEntry));
                    if (bytecode_file->type_table->entries) {
                        fread(bytecode_file->type_table->entries, sizeof(TypeEntry), count, file);
                    }
                }
            }
        }
    }
    
    // Load method table
    if (header.method_table_size > 0) {
        fseek(file, header.method_table_offset, SEEK_SET);
        bytecode_file->method_table = method_table_create();
        if (bytecode_file->method_table) {
            // Read method table header
            uint32_t count;
            if (fread(&count, sizeof(uint32_t), 1, file) == 1) {
                bytecode_file->method_table->count = count;
                
                // Allocate and read entries
                if (count > 0) {
                    bytecode_file->method_table->entries = malloc(count * sizeof(MethodEntry));
                    if (bytecode_file->method_table->entries) {
                        fread(bytecode_file->method_table->entries, sizeof(MethodEntry), count, file);
                    }
                }
            }
        }
    }
    
    // Load bytecode
    if (header.bytecode_size > 0) {
        fseek(file, header.bytecode_offset, SEEK_SET);
        bytecode_file->bytecode = malloc(header.bytecode_size);
        if (bytecode_file->bytecode) {
            fread(bytecode_file->bytecode, 1, header.bytecode_size, file);
            bytecode_file->bytecode_size = header.bytecode_size;
        }
    }
    
    fclose(file);
    return bytecode_file;
}

bool bytecode_file_save(BytecodeFile* file, const char* filename) {
    if (!file || !filename) return false;
    
    FILE* f = fopen(filename, "wb");
    if (!f) return false;
    
    // Write header
    if (fwrite(&file->header, sizeof(BytecodeHeader), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    // Calculate offsets
    size_t current_offset = sizeof(BytecodeHeader);
    
    // Write string table
    if (file->string_table && file->string_table->count > 0) {
        file->header.string_table_offset = current_offset;
        
        // Write string table header
        if (fwrite(&file->string_table->count, sizeof(uint32_t), 1, f) != 1 ||
            fwrite(&file->string_table->total_size, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        current_offset += 2 * sizeof(uint32_t);
        
        // Write string entries
        if (file->string_table->count > 0) {
            if (fwrite(file->string_table->entries, sizeof(StringEntry), file->string_table->count, f) != file->string_table->count) {
                fclose(f);
                return false;
            }
            current_offset += file->string_table->count * sizeof(StringEntry);
        }
        
        // Write string data
        if (file->string_table->total_size > 0) {
            if (fwrite(file->string_table->data, 1, file->string_table->total_size, f) != file->string_table->total_size) {
                fclose(f);
                return false;
            }
            current_offset += file->string_table->total_size;
        }
        
        file->header.string_table_size = current_offset - file->header.string_table_offset;
    }
    
    // Write constant table
    if (file->constant_table && file->constant_table->count > 0) {
        file->header.constant_table_offset = current_offset;
        
        // Write constant table header
        if (fwrite(&file->constant_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        current_offset += sizeof(uint32_t);
        
        // Write constant entries
        if (file->constant_table->count > 0) {
            if (fwrite(file->constant_table->entries, sizeof(ConstantEntry), file->constant_table->count, f) != file->constant_table->count) {
                fclose(f);
                return false;
            }
            current_offset += file->constant_table->count * sizeof(ConstantEntry);
        }
        
        file->header.constant_table_size = current_offset - file->header.constant_table_offset;
    }
    
    // Write type table
    if (file->type_table && file->type_table->count > 0) {
        file->header.type_table_offset = current_offset;
        
        // Write type table header
        if (fwrite(&file->type_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        current_offset += sizeof(uint32_t);
        
        // Write type entries
        if (file->type_table->count > 0) {
            if (fwrite(file->type_table->entries, sizeof(TypeEntry), file->type_table->count, f) != file->type_table->count) {
                fclose(f);
                return false;
            }
            current_offset += file->type_table->count * sizeof(TypeEntry);
        }
        
        file->header.type_table_size = current_offset - file->header.type_table_offset;
    }
    
    // Write method table
    if (file->method_table && file->method_table->count > 0) {
        file->header.method_table_offset = current_offset;
        
        // Write method table header
        if (fwrite(&file->method_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        current_offset += sizeof(uint32_t);
        
        // Write method entries
        if (file->method_table->count > 0) {
            if (fwrite(file->method_table->entries, sizeof(MethodEntry), file->method_table->count, f) != file->method_table->count) {
                fclose(f);
                return false;
            }
            current_offset += file->method_table->count * sizeof(MethodEntry);
        }
        
        file->header.method_table_size = current_offset - file->header.method_table_offset;
    }
    
    // Write bytecode
    if (file->bytecode && file->bytecode_size > 0) {
        file->header.bytecode_offset = current_offset;
        
        if (fwrite(file->bytecode, 1, file->bytecode_size, f) != file->bytecode_size) {
            fclose(f);
            return false;
        }
        current_offset += file->bytecode_size;
        
        file->header.bytecode_size = file->bytecode_size;
    }
    
    // Update header with final offsets
    fseek(f, 0, SEEK_SET);
    if (fwrite(&file->header, sizeof(BytecodeHeader), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    fclose(f);
    return true;
}

bool bytecode_file_validate(BytecodeFile* file) {
    if (!file) return false;
    
    // Validate magic number
    if (!validate_magic(file->header.magic)) {
        return false;
    }
    
    // Validate version
    if (file->header.version_major != BYTECODE_VERSION_MAJOR ||
        file->header.version_minor != BYTECODE_VERSION_MINOR) {
        return false;
    }
    
    return true;
}

// ============================================================================
// STRING TABLE OPERATIONS
// ============================================================================

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
    
    if (table->entries) {
        free(table->entries);
    }
    if (table->data) {
        free(table->data);
    }
    
    free(table);
}

uint32_t string_table_add_string(StringTable* table, const char* str) {
    if (!table || !str) {
        printf("DEBUG: string_table_add_string failed - table=%p, str=%p\n", table, str);
        return 0;
    }
    
    printf("DEBUG: string_table_add_string - adding '%s'\n", str);
    
    // Check if string already exists
    for (uint32_t i = 0; i < table->count; i++) {
        if (strcmp(table->data + table->entries[i].offset, str) == 0) {
            printf("DEBUG: string_table_add_string - found existing string at index %u\n", i);
            return i;
        }
    }
    
    // Add new string
    size_t len = strlen(str) + 1; // Include null terminator
    
    // Resize data buffer
    char* new_data = realloc(table->data, table->total_size + len);
    if (!new_data) {
        printf("DEBUG: string_table_add_string failed - realloc data failed\n");
        return 0;
    }
    
    table->data = new_data;
    
    // Resize entries array
    StringEntry* new_entries = realloc(table->entries, (table->count + 1) * sizeof(StringEntry));
    if (!new_entries) {
        printf("DEBUG: string_table_add_string failed - realloc entries failed\n");
        return 0;
    }
    
    table->entries = new_entries;
    
    // Add string data
    strcpy(table->data + table->total_size, str);
    
    // Add entry
    table->entries[table->count].offset = table->total_size;
    table->entries[table->count].length = len - 1; // Exclude null terminator
    table->entries[table->count].hash = string_hash(str);
    
    uint32_t index = table->count;
    table->count++;
    table->total_size += len;
    
    printf("DEBUG: string_table_add_string success - index=%u, offset=%u\n", index, table->entries[index].offset);
    return index;
}

uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length) {
    if (!table || !str) return 0;
    
    // Check if string already exists
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].length == length &&
            strncmp(table->data + table->entries[i].offset, str, length) == 0) {
            return i;
        }
    }
    
    // Add new string
    size_t len = length + 1; // Include null terminator
    
    // Resize data buffer
    char* new_data = realloc(table->data, table->total_size + len);
    if (!new_data) return 0;
    
    table->data = new_data;
    
    // Resize entries array
    StringEntry* new_entries = realloc(table->entries, (table->count + 1) * sizeof(StringEntry));
    if (!new_entries) return 0;
    
    table->entries = new_entries;
    
    // Add string data
    memcpy(table->data + table->total_size, str, length);
    table->data[table->total_size + length] = '\0'; // Null terminate
    
    // Add entry
    table->entries[table->count].offset = table->total_size;
    table->entries[table->count].length = length;
    table->entries[table->count].hash = string_hash(str);
    
    uint32_t index = table->count;
    table->count++;
    table->total_size += len;
    
    return index;
}

const char* string_table_get_string(StringTable* table, uint32_t index) {
    if (!table || !table->data || !table->entries || index >= table->count) {
        return NULL;
    }
    
    return table->data + table->entries[index].offset;
}

int32_t string_table_find_string(StringTable* table, const char* str) {
    if (!table || !str) return -1;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (strcmp(table->data + table->entries[i].offset, str) == 0) {
            return i;
        }
    }
    
    return -1;
}

// ============================================================================
// TYPE TABLE OPERATIONS
// ============================================================================

TypeTable* type_table_create(void) {
    TypeTable* table = malloc(sizeof(TypeTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->entries = NULL;
    
    return table;
}

void type_table_destroy(TypeTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

bool type_table_add_type(TypeTable* table, const TypeEntry* entry) {
    if (!table || !entry) return false;
    
    // Resize entries array
    TypeEntry* new_entries = realloc(table->entries, (table->count + 1) * sizeof(TypeEntry));
    if (!new_entries) return false;
    
    table->entries = new_entries;
    
    // Add entry
    table->entries[table->count] = *entry;
    table->count++;
    
    return true;
}

const TypeEntry* type_table_get_type(TypeTable* table, uint32_t type_id) {
    if (!table || !table->entries) return NULL;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].type_id == type_id) {
            return &table->entries[i];
        }
    }
    
    return NULL;
}

int32_t type_table_find_type_by_name(TypeTable* table, const char* name) {
    if (!table || !name) return -1;
    
    for (uint32_t i = 0; i < table->count; i++) {
        // This would need string table lookup in real implementation
        // For now, just return -1
        (void)table->entries[i];
    }
    
    return -1;
}

// ============================================================================
// METHOD TABLE OPERATIONS
// ============================================================================

MethodTable* method_table_create(void) {
    MethodTable* table = malloc(sizeof(MethodTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->entries = NULL;
    
    return table;
}

void method_table_destroy(MethodTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

bool method_table_add_method(MethodTable* table, const MethodEntry* entry) {
    if (!table || !entry) return false;
    
    // Resize entries array
    MethodEntry* new_entries = realloc(table->entries, (table->count + 1) * sizeof(MethodEntry));
    if (!new_entries) return false;
    
    table->entries = new_entries;
    
    // Add entry
    table->entries[table->count] = *entry;
    table->count++;
    
    return true;
}

const MethodEntry* method_table_get_method(MethodTable* table, uint32_t method_id) {
    if (!table || !table->entries) return NULL;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].method_id == method_id) {
            return &table->entries[i];
        }
    }
    
    return NULL;
}

int32_t method_table_find_method(MethodTable* table, uint32_t type_id, const char* name) {
    if (!table || !name) return -1;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].type_id == type_id) {
            // This would need string table lookup in real implementation
            // For now, just return -1
            (void)name;
        }
    }
    
    return -1;
}

// ============================================================================
// FIELD TABLE OPERATIONS
// ============================================================================

FieldTable* field_table_create(void) {
    FieldTable* table = malloc(sizeof(FieldTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->entries = NULL;
    
    return table;
}

void field_table_destroy(FieldTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

bool field_table_add_field(FieldTable* table, const FieldEntry* entry) {
    if (!table || !entry) return false;
    
    // Resize entries array
    FieldEntry* new_entries = realloc(table->entries, (table->count + 1) * sizeof(FieldEntry));
    if (!new_entries) return false;
    
    table->entries = new_entries;
    
    // Add entry
    table->entries[table->count] = *entry;
    table->count++;
    
    return true;
}

const FieldEntry* field_table_get_field(FieldTable* table, uint32_t field_id) {
    if (!table || !table->entries) return NULL;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].field_id == field_id) {
            return &table->entries[i];
        }
    }
    
    return NULL;
}

int32_t field_table_find_field(FieldTable* table, uint32_t type_id, const char* name) {
    if (!table || !name) return -1;
    
    for (uint32_t i = 0; i < table->count; i++) {
        if (table->entries[i].type_id == type_id) {
            // This would need string table lookup in real implementation
            // For now, just return -1
            (void)name;
        }
    }
    
    return -1;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

uint32_t string_hash(const char* str) {
    if (!str) return 0;
    
    uint32_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

size_t align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

bool validate_magic(const char* magic) {
    return memcmp(magic, BYTECODE_MAGIC, BYTECODE_MAGIC_SIZE) == 0;
}

const char* bytecode_get_string(BytecodeFile* file, uint32_t offset) {
    if (!file || !file->string_table) return NULL;
    
    return string_table_get_string(file->string_table, offset);
}

const ConstantEntry* bytecode_get_constant(BytecodeFile* file, uint32_t index) {
    if (!file || !file->constant_table) return NULL;
    
    return constant_table_get_constant(file->constant_table, index);
}

const TypeEntry* bytecode_get_type(BytecodeFile* file, uint32_t type_id) {
    if (!file || !file->type_table) return NULL;
    
    return type_table_get_type(file->type_table, type_id);
}

const MethodEntry* bytecode_get_method(BytecodeFile* file, uint32_t method_id) {
    if (!file || !file->method_table) return NULL;
    
    return method_table_get_method(file->method_table, method_id);
}

const FieldEntry* bytecode_get_field(BytecodeFile* file, uint32_t field_id) {
    if (!file || !file->field_table) return NULL;
    
    return field_table_get_field(file->field_table, field_id);
}

// ============================================================================
// DEBUGGING AND DISASSEMBLY
// ============================================================================

void bytecode_file_print_info(BytecodeFile* file) {
    if (!file) {
        printf("BytecodeFile: NULL\n");
        return;
    }
    
    printf("BytecodeFile:\n");
    printf("  Magic: %.4s\n", file->header.magic);
    printf("  Version: %u.%u\n", file->header.version_major, file->header.version_minor);
    printf("  Flags: 0x%08X\n", file->header.flags);
    printf("  String Table: %u entries, %u bytes\n", 
           file->string_table ? file->string_table->count : 0,
           file->header.string_table_size);
    printf("  Constant Table: %u entries, %u bytes\n", 
           file->constant_table ? file->constant_table->count : 0,
           file->header.constant_table_size);
    printf("  Type Table: %u entries, %u bytes\n", 
           file->type_table ? file->type_table->count : 0,
           file->header.type_table_size);
    printf("  Method Table: %u entries, %u bytes\n", 
           file->method_table ? file->method_table->count : 0,
           file->header.method_table_size);
    printf("  Bytecode: %zu bytes\n", file->bytecode_size);
}

void string_table_print(StringTable* table) {
    if (!table) {
        printf("StringTable: NULL\n");
        return;
    }
    
    printf("StringTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        printf("  [%u] \"%s\"\n", i, table->data + table->entries[i].offset);
    }
}

void type_table_print(TypeTable* table) {
    if (!table) {
        printf("TypeTable: NULL\n");
        return;
    }
    
    printf("TypeTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        printf("  [%u] Type ID: %u\n", i, table->entries[i].type_id);
    }
}

void method_table_print(MethodTable* table) {
    if (!table) {
        printf("MethodTable: NULL\n");
        return;
    }
    
    printf("MethodTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        printf("  [%u] Method ID: %u, Type ID: %u\n", i, table->entries[i].method_id, table->entries[i].type_id);
    }
}

void field_table_print(FieldTable* table) {
    if (!table) {
        printf("FieldTable: NULL\n");
        return;
    }
    
    printf("FieldTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        printf("  [%u] Field ID: %u, Type ID: %u\n", i, table->entries[i].field_id, table->entries[i].type_id);
    }
}

void bytecode_disassemble(BytecodeFile* file, uint32_t method_id) {
    if (!file || !file->bytecode) {
        printf("No bytecode to disassemble\n");
        return;
    }
    
    // Find method
    const MethodEntry* method = bytecode_get_method(file, method_id);
    if (!method) {
        printf("Method %u not found\n", method_id);
        return;
    }
    
    printf("Disassembling method %u:\n", method_id);
    printf("  Bytecode offset: %u\n", method->bytecode_offset);
    printf("  Bytecode size: %u\n", method->bytecode_size);
    
    // Simple disassembly - just print raw bytes for now
    uint8_t* bytecode = file->bytecode + method->bytecode_offset;
    for (uint32_t i = 0; i < method->bytecode_size; i++) {
        if (i % 16 == 0) {
            printf("\n  %04X: ", i);
        }
        printf("%02X ", bytecode[i]);
    }
    printf("\n");
}

// ============================================================================
// MISSING FUNCTIONS FOR VM INTEGRATION
// ============================================================================

const char* bytecode_get_domain_name(BytecodeFile* file) {
    if (!file || !file->string_table) {
        return "unknown";
    }
    
    // For now, return a default domain name
    // TODO: Implement proper domain name lookup from string table
    return "default";
}

MethodEntry* method_table_find_by_name(MethodTable* table, const char* name) {
    if (!table || !name) {
        return NULL;
    }
    
    // For now, return the first method if it exists
    // TODO: Implement proper method name lookup
    if (table->count > 0) {
        return &table->entries[0];
    }
    
    return NULL;
}