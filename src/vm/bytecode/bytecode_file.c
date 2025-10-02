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
    
    bytecode_file->header = header;
    
    // Load string table
    if (header.string_table_size > 0) {
        fseek(file, header.string_table_offset, SEEK_SET);
        bytecode_file->string_table = string_table_create();
        if (!bytecode_file->string_table) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read string table header
        if (fread(&bytecode_file->string_table->count, sizeof(uint32_t), 1, file) != 1) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(&bytecode_file->string_table->total_size, sizeof(uint32_t), 1, file) != 1) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Allocate and read entries
        bytecode_file->string_table->entries = malloc(sizeof(StringEntry) * bytecode_file->string_table->count);
        if (!bytecode_file->string_table->entries) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->string_table->entries, sizeof(StringEntry), bytecode_file->string_table->count, file) != bytecode_file->string_table->count) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Allocate and read string data
        bytecode_file->string_table->data = malloc(bytecode_file->string_table->total_size);
        if (!bytecode_file->string_table->data) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->string_table->data, 1, bytecode_file->string_table->total_size, file) != bytecode_file->string_table->total_size) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
    }
    
    // Load type table
    if (header.type_table_size > 0) {
        fseek(file, header.type_table_offset, SEEK_SET);
        bytecode_file->type_table = type_table_create();
        if (!bytecode_file->type_table) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read type table header
        if (fread(&bytecode_file->type_table->count, sizeof(uint32_t), 1, file) != 1) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Allocate and read entries
        bytecode_file->type_table->entries = malloc(sizeof(TypeEntry) * bytecode_file->type_table->count);
        if (!bytecode_file->type_table->entries) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->type_table->entries, sizeof(TypeEntry), bytecode_file->type_table->count, file) != bytecode_file->type_table->count) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
    }
    
    // Load method table
    if (header.method_table_size > 0) {
        fseek(file, header.method_table_offset, SEEK_SET);
        bytecode_file->method_table = method_table_create();
        if (!bytecode_file->method_table) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read method table header
        if (fread(&bytecode_file->method_table->count, sizeof(uint32_t), 1, file) != 1) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Allocate and read entries
        bytecode_file->method_table->entries = malloc(sizeof(MethodEntry) * bytecode_file->method_table->count);
        if (!bytecode_file->method_table->entries) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->method_table->entries, sizeof(MethodEntry), bytecode_file->method_table->count, file) != bytecode_file->method_table->count) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
    }
    
    // Field table loading not implemented yet
    
    // Load bytecode
    if (header.bytecode_size > 0) {
        fseek(file, header.bytecode_offset, SEEK_SET);
        bytecode_file->bytecode = malloc(header.bytecode_size);
        if (!bytecode_file->bytecode) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->bytecode, 1, header.bytecode_size, file) != header.bytecode_size) {
            bytecode_file_destroy(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        bytecode_file->bytecode_size = header.bytecode_size;
    }
    
    fclose(file);
    return bytecode_file;
}

bool bytecode_file_save(BytecodeFile* file, const char* filename) {
    if (!file || !filename) return false;
    
    FILE* f = fopen(filename, "wb");
    if (!f) return false;
    
    // Calculate offsets
    size_t current_offset = sizeof(BytecodeHeader);
    
    // String table offset
    file->header.string_table_offset = current_offset;
    if (file->string_table && file->header.string_table_size > 0) {
        current_offset += file->header.string_table_size;
    }
    
    // Type table offset
    file->header.type_table_offset = current_offset;
    if (file->type_table && file->header.type_table_size > 0) {
        current_offset += file->header.type_table_size;
    }
    
    // Method table offset
    file->header.method_table_offset = current_offset;
    if (file->method_table && file->header.method_table_size > 0) {
        current_offset += file->header.method_table_size;
    }
    
    // Field table offset (not implemented yet)
    file->header.field_table_offset = current_offset;
    
    // Bytecode offset
    file->header.bytecode_offset = current_offset;
    
    // Write header
    if (fwrite(&file->header, sizeof(BytecodeHeader), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    // Write string table
    if (file->string_table && file->header.string_table_size > 0) {
        if (fwrite(&file->string_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        
        if (fwrite(&file->string_table->total_size, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        
        if (fwrite(file->string_table->entries, sizeof(StringEntry), file->string_table->count, f) != file->string_table->count) {
            fclose(f);
            return false;
        }
        
        if (fwrite(file->string_table->data, 1, file->string_table->total_size, f) != file->string_table->total_size) {
            fclose(f);
            return false;
        }
    }
    
    // Write type table
    if (file->type_table && file->header.type_table_size > 0) {
        if (fwrite(&file->type_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        
        if (fwrite(file->type_table->entries, sizeof(TypeEntry), file->type_table->count, f) != file->type_table->count) {
            fclose(f);
            return false;
        }
    }
    
    // Write method table
    if (file->method_table && file->header.method_table_size > 0) {
        if (fwrite(&file->method_table->count, sizeof(uint32_t), 1, f) != 1) {
            fclose(f);
            return false;
        }
        
        if (fwrite(file->method_table->entries, sizeof(MethodEntry), file->method_table->count, f) != file->method_table->count) {
            fclose(f);
            return false;
        }
    }
    
    // Field table saving not implemented yet
    
    // Write bytecode
    if (file->bytecode && file->header.bytecode_size > 0) {
        if (fwrite(file->bytecode, 1, file->header.bytecode_size, f) != file->header.bytecode_size) {
            fclose(f);
            return false;
        }
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
    if (file->header.version_major > BYTECODE_VERSION_MAJOR) {
        return false;
    }
    
    // Validate flags
    if (!(file->header.flags & BYTECODE_FLAG_EXECUTABLE) && 
        !(file->header.flags & BYTECODE_FLAG_LIBRARY)) {
        return false;
    }
    
    // Validate string table
    if (file->string_table) {
        if (file->string_table->count == 0 && file->header.string_table_size > 0) {
            return false;
        }
        if (file->string_table->total_size == 0 && file->header.string_table_size > 0) {
            return false;
        }
    }
    
    // Validate type table
    if (file->type_table) {
        if (file->type_table->count == 0 && file->header.type_table_size > 0) {
            return false;
        }
    }
    
    // Validate method table
    if (file->method_table) {
        if (file->method_table->count == 0 && file->header.method_table_size > 0) {
            return false;
        }
    }
    
    // Field table validation not implemented yet
    
    return true;
}

// String, Type, Method, and Field table operations are implemented in bytecode_loader.c

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

// These functions are implemented in bytecode_loader.c

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
    printf("  Type Table: %u entries, %u bytes\n", 
           file->type_table ? file->type_table->count : 0,
           file->header.type_table_size);
    printf("  Method Table: %u entries, %u bytes\n", 
           file->method_table ? file->method_table->count : 0,
           file->header.method_table_size);
    // Field table printing not implemented yet
    printf("  Bytecode: %u bytes\n", file->header.bytecode_size);
    printf("  Entry Point: %u\n", file->header.entry_point_method_id);
    printf("  Main Class: %u\n", file->header.main_class_id);
}

void string_table_print(StringTable* table) {
    if (!table) {
        printf("StringTable: NULL\n");
        return;
    }
    
    printf("StringTable: %u entries, %u bytes\n", table->count, table->total_size);
    for (uint32_t i = 0; i < table->count; i++) {
        StringEntry* entry = &table->entries[i];
        const char* str = table->data + entry->offset;
        printf("  [%u] \"%s\" (hash: 0x%08X)\n", i, str, entry->hash);
    }
}

void type_table_print(TypeTable* table) {
    if (!table) {
        printf("TypeTable: NULL\n");
        return;
    }
    
    printf("TypeTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        TypeEntry* entry = &table->entries[i];
        printf("  [%u] Type ID: %u, Name Offset: %u, Size: %u\n", 
               i, entry->type_id, entry->name_offset, entry->size);
    }
}

void method_table_print(MethodTable* table) {
    if (!table) {
        printf("MethodTable: NULL\n");
        return;
    }
    
    printf("MethodTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        MethodEntry* entry = &table->entries[i];
        printf("  [%u] Method ID: %u, Type ID: %u, Bytecode: %u bytes\n", 
               i, entry->method_id, entry->type_id, entry->bytecode_size);
    }
}

// Field table printing not implemented yet

// Disassembly function removed - would need to be implemented in bytecode_loader.c
