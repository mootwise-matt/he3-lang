#include "bytecode_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Magic number is now defined in bytecode_format.h

// Bytecode loader functions
BytecodeFile* bytecode_load_file(const char* filename) {
    if (!filename) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }
    
    BytecodeFile* bytecode_file = malloc(sizeof(BytecodeFile));
    if (!bytecode_file) {
        fclose(file);
        return NULL;
    }
    
    // Initialize bytecode file
    memset(bytecode_file, 0, sizeof(BytecodeFile));
    
    // Read header
    if (fread(&bytecode_file->header, sizeof(BytecodeHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to read bytecode header\n");
        bytecode_destroy_file(bytecode_file);
        fclose(file);
        return NULL;
    }
    
    // Validate header
    if (!validate_magic(bytecode_file->header.magic)) {
        fprintf(stderr, "Invalid bytecode header\n");
        bytecode_destroy_file(bytecode_file);
        fclose(file);
        return NULL;
    }
    
    // Read string table
    if (bytecode_file->header.string_table_size > 0) {
        fseek(file, bytecode_file->header.string_table_offset, SEEK_SET);
        bytecode_file->string_table = string_table_create();
        if (!bytecode_file->string_table) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read string table data
        bytecode_file->string_table->data = malloc(bytecode_file->header.string_table_size);
        if (!bytecode_file->string_table->data) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->string_table->data, 1, bytecode_file->header.string_table_size, file) != bytecode_file->header.string_table_size) {
            fprintf(stderr, "Failed to read string table\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        bytecode_file->string_table->total_size = bytecode_file->header.string_table_size;
    }
    
    // Read type table
    if (bytecode_file->header.type_table_size > 0) {
        fseek(file, bytecode_file->header.type_table_offset, SEEK_SET);
        bytecode_file->type_table = type_table_create();
        if (!bytecode_file->type_table) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read type table header
        if (fread(&bytecode_file->type_table->count, sizeof(uint32_t), 1, file) != 1) {
            fprintf(stderr, "Failed to read type table header\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        bytecode_file->type_table->entries = malloc(sizeof(TypeEntry) * bytecode_file->type_table->count);
        if (!bytecode_file->type_table->entries) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->type_table->entries, sizeof(TypeEntry), bytecode_file->type_table->count, file) != bytecode_file->type_table->count) {
            fprintf(stderr, "Failed to read type table\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
    }
    
    // Read method table
    if (bytecode_file->header.method_table_size > 0) {
        fseek(file, bytecode_file->header.method_table_offset, SEEK_SET);
        bytecode_file->method_table = method_table_create();
        if (!bytecode_file->method_table) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        // Read method table header
        if (fread(&bytecode_file->method_table->count, sizeof(uint32_t), 1, file) != 1) {
            fprintf(stderr, "Failed to read method table header\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        bytecode_file->method_table->entries = malloc(sizeof(MethodEntry) * bytecode_file->method_table->count);
        if (!bytecode_file->method_table->entries) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->method_table->entries, sizeof(MethodEntry), bytecode_file->method_table->count, file) != bytecode_file->method_table->count) {
            fprintf(stderr, "Failed to read method table\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
    }
    
    // Read bytecode
    if (bytecode_file->header.bytecode_size > 0) {
        fseek(file, bytecode_file->header.bytecode_offset, SEEK_SET);
        bytecode_file->bytecode = malloc(bytecode_file->header.bytecode_size);
        if (!bytecode_file->bytecode) {
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        if (fread(bytecode_file->bytecode, 1, bytecode_file->header.bytecode_size, file) != bytecode_file->header.bytecode_size) {
            fprintf(stderr, "Failed to read bytecode\n");
            bytecode_destroy_file(bytecode_file);
            fclose(file);
            return NULL;
        }
        
        bytecode_file->bytecode_size = bytecode_file->header.bytecode_size;
    }
    
    fclose(file);
    return bytecode_file;
}

void bytecode_destroy_file(BytecodeFile* file) {
    if (!file) return;
    
    if (file->string_table) {
        string_table_destroy(file->string_table);
    }
    
    if (file->type_table) {
        type_table_destroy(file->type_table);
    }
    
    if (file->method_table) {
        method_table_destroy(file->method_table);
    }
    
    if (file->bytecode) {
        free(file->bytecode);
    }
    
    // filename field removed from shared format
    
    free(file);
}

// String table functions
StringTable* string_table_create(void) {
    StringTable* table = malloc(sizeof(StringTable));
    if (!table) {
        return NULL;
    }
    
    table->data = NULL;
    table->total_size = 0;
    table->entries = NULL;
    table->count = 0;
    
    return table;
}

void string_table_destroy(StringTable* table) {
    if (!table) return;
    
    if (table->data) {
        free(table->data);
    }
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

const char* string_table_get(StringTable* table, uint32_t index) {
    if (!table || !table->data || index >= table->count) {
        return NULL;
    }
    
    return table->data + index;
}

uint32_t string_table_add_string(StringTable* table, const char* str) {
    if (!table || !str) return 0;
    return string_table_add_string_with_length(table, str, strlen(str));
}

uint32_t string_table_add_string_with_length(StringTable* table, const char* str, size_t length) {
    if (!table || !str) return 0;
    
    // For now, just return a dummy ID
    // TODO: Implement proper string table management
    return 1;
}

// Type table functions
TypeTable* type_table_create(void) {
    TypeTable* table = malloc(sizeof(TypeTable));
    if (!table) {
        return NULL;
    }
    
    table->entries = NULL;
    table->count = 0;
    
    return table;
}

void type_table_destroy(TypeTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

TypeEntry* type_table_get(TypeTable* table, uint32_t index) {
    if (!table || !table->entries || index >= table->count) {
        return NULL;
    }
    
    return &table->entries[index];
}

// Method table functions
MethodTable* method_table_create(void) {
    MethodTable* table = malloc(sizeof(MethodTable));
    if (!table) {
        return NULL;
    }
    
    table->entries = NULL;
    table->count = 0;
    
    return table;
}

void method_table_destroy(MethodTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

MethodEntry* method_table_get(MethodTable* table, uint32_t index) {
    if (!table || !table->entries || index >= table->count) {
        return NULL;
    }
    
    return &table->entries[index];
}

MethodEntry* method_table_find_by_name(MethodTable* table, const char* name) {
    if (!table || !table->entries || !name) {
        return NULL;
    }
    
    for (size_t i = 0; i < table->count; i++) {
        // TODO: Compare with string table lookup
        // For now, just return the first method
        if (i == 0) {
            return &table->entries[i];
        }
    }
    
    return NULL;
}

// Utility functions
bool bytecode_validate_header(const BytecodeHeader* header) {
    if (!header) {
        return false;
    }
    
    if (!validate_magic(header->magic)) {
        fprintf(stderr, "Invalid magic number: %.4s\n", header->magic);
        return false;
    }
    
    if (header->version_major != BYTECODE_VERSION_MAJOR) {
        fprintf(stderr, "Unsupported version: %u.%u\n", header->version_major, header->version_minor);
        return false;
    }
    
    return true;
}

const char* bytecode_get_domain_name(BytecodeFile* file) {
    if (!file || !file->string_table || file->header.domain_name_offset >= file->string_table->total_size) {
        return "unknown";
    }
    
    return string_table_get(file->string_table, file->header.domain_name_offset);
}

void bytecode_print_info(BytecodeFile* file) {
    if (!file) {
        printf("Bytecode file: NULL\n");
        return;
    }
    
    printf("Bytecode file: unknown\n");
    printf("  Magic: %.4s\n", file->header.magic);
    printf("  Version: %u.%u\n", file->header.version_major, file->header.version_minor);
    printf("  Domain: %s\n", bytecode_get_domain_name(file));
    printf("  Types: %u\n", file->type_table ? file->type_table->count : 0);
    printf("  Methods: %u\n", file->method_table ? file->method_table->count : 0);
    printf("  String table size: %u\n", file->header.string_table_size);
    printf("  Bytecode size: %u\n", file->header.bytecode_size);
}
