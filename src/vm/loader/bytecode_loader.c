#include "bytecode_loader.h"
#include "../shared/bytecode/bytecode_format.h"
#include "../shared/bytecode/opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// BYTECODE LOADER IMPLEMENTATION
// ============================================================================

BytecodeFile* bytecode_load_file(const char* filename) {
    if (!filename) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open bytecode file: %s\n", filename);
        return NULL;
    }
    
    // Read header
    BytecodeHeader header;
    if (fread(&header, sizeof(BytecodeHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to read bytecode header\n");
        fclose(file);
        return NULL;
    }
    
    // Validate magic number
    printf("DEBUG: Read magic: %.4s, expected: %s\n", header.magic, BYTECODE_MAGIC);
    if (memcmp(header.magic, BYTECODE_MAGIC, BYTECODE_MAGIC_SIZE) != 0) {
        fprintf(stderr, "Invalid bytecode magic number\n");
        fclose(file);
        return NULL;
    }
    
    // Validate version
    printf("DEBUG: Read version: %u.%u, expected: %u.%u\n", 
           header.version_major, header.version_minor, 
           BYTECODE_VERSION_MAJOR, BYTECODE_VERSION_MINOR);
    if (header.version_major != BYTECODE_VERSION_MAJOR || 
        header.version_minor != BYTECODE_VERSION_MINOR) {
        fprintf(stderr, "Unsupported bytecode version: %u.%u\n", 
                header.version_major, header.version_minor);
        fclose(file);
        return NULL;
    }
    
    // Create bytecode file
    BytecodeFile* bytecode_file = malloc(sizeof(BytecodeFile));
    if (!bytecode_file) {
        fprintf(stderr, "Failed to allocate memory for bytecode file\n");
        fclose(file);
        return NULL;
    }
    
    // Initialize bytecode file
    bytecode_file->header = header;
    bytecode_file->string_table = NULL;
    bytecode_file->constant_table = NULL;
    bytecode_file->type_table = NULL;
    bytecode_file->method_table = NULL;
    bytecode_file->field_table = NULL;
    bytecode_file->bytecode = NULL;
    bytecode_file->bytecode_size = 0;
    
    // Load string table
    printf("DEBUG: Loading string table (size=%u, offset=%u)\n", 
           header.string_table_size, header.string_table_offset);
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
    printf("DEBUG: Loading constant table (size=%u, offset=%u)\n", 
           header.constant_table_size, header.constant_table_offset);
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
    printf("DEBUG: Loading type table (size=%u, offset=%u)\n", 
           header.type_table_size, header.type_table_offset);
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
    printf("DEBUG: Loading method table (size=%u, offset=%u)\n", 
           header.method_table_size, header.method_table_offset);
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
    printf("DEBUG: Loading bytecode (size=%u, offset=%u)\n", 
           header.bytecode_size, header.bytecode_offset);
    if (header.bytecode_size > 0) {
        fseek(file, header.bytecode_offset, SEEK_SET);
        bytecode_file->bytecode = malloc(header.bytecode_size);
        if (bytecode_file->bytecode) {
            if (fread(bytecode_file->bytecode, 1, header.bytecode_size, file) != header.bytecode_size) {
                fprintf(stderr, "Failed to read bytecode\n");
                bytecode_destroy_file(bytecode_file);
                fclose(file);
                return NULL;
            }
            
            bytecode_file->bytecode_size = bytecode_file->header.bytecode_size;
            printf("DEBUG: Bytecode loaded successfully\n");
        }
    }
    
    fclose(file);
    return bytecode_file;
}

void bytecode_destroy_file(BytecodeFile* file) {
    if (!file) return;
    
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
    
    if (file->field_table) {
        field_table_destroy(file->field_table);
    }
    
    if (file->bytecode) {
        free(file->bytecode);
    }
    
    free(file);
}

// Note: String table, type table, method table, and field table functions
// are implemented in bytecode_file.c to avoid duplication