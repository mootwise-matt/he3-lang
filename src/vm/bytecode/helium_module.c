#include "../../shared/bytecode/helium_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new helium module
HeliumModule* helium_module_create(void) {
    HeliumModule* module = malloc(sizeof(HeliumModule));
    if (!module) return NULL;
    
    // Initialize header
    memset(&module->header, 0, sizeof(HeliumHeader));
    strncpy(module->header.magic, HELIUM_MAGIC, 7);
    module->header.magic[7] = '\0';
    module->header.version_major = HELIUM_VERSION_MAJOR;
    module->header.version_minor = HELIUM_VERSION_MINOR;
    module->header.flags = HELIUM_FLAG_EXECUTABLE;
    
    // Initialize tables
    module->string_table_obj = string_table_create();
    module->type_table = type_table_create();
    module->method_table = method_table_create();
    module->field_table = field_table_create();
    
    // Initialize other fields
    module->string_table = NULL;
    module->string_table_size = 0;
    module->bytecode = NULL;
    module->bytecode_size = 0;
    
    return module;
}

// Destroy helium module
void helium_module_destroy(HeliumModule* module) {
    if (!module) return;
    
    if (module->string_table) {
        free(module->string_table);
    }
    
    if (module->bytecode) {
        free(module->bytecode);
    }
    
    if (module->string_table_obj) {
        string_table_destroy(module->string_table_obj);
    }
    
    if (module->type_table) {
        type_table_destroy(module->type_table);
    }
    
    if (module->method_table) {
        method_table_destroy(module->method_table);
    }
    
    if (module->field_table) {
        field_table_destroy(module->field_table);
    }
    
    free(module);
}

// Save helium module to file
bool helium_module_save(HeliumModule* module, const char* filename) {
    if (!module || !filename) return false;
    
    FILE* file = fopen(filename, "wb");
    if (!file) return false;
    
    // Calculate offsets
    size_t current_offset = sizeof(HeliumHeader);
    
    // String table offset
    module->header.string_table_offset = current_offset;
    if (module->string_table_obj) {
        module->header.string_table_size = 
            sizeof(uint32_t) * 2 + 
            sizeof(StringEntry) * module->string_table_obj->count + 
            module->string_table_obj->total_size;
        current_offset += module->header.string_table_size;
    }
    
    // Type table offset
    module->header.type_table_offset = current_offset;
    if (module->type_table) {
        module->header.type_table_size = 
            sizeof(uint32_t) + 
            sizeof(TypeEntry) * module->type_table->count;
        current_offset += module->header.type_table_size;
    }
    
    // Method table offset
    module->header.method_table_offset = current_offset;
    if (module->method_table) {
        module->header.method_table_size = 
            sizeof(uint32_t) + 
            sizeof(MethodEntry) * module->method_table->count;
        current_offset += module->header.method_table_size;
    }
    
    // Field table offset
    module->header.field_table_offset = current_offset;
    if (module->field_table) {
        module->header.field_table_size = 
            sizeof(uint32_t) + 
            sizeof(FieldEntry) * module->field_table->count;
        current_offset += module->header.field_table_size;
    }
    
    // Bytecode offset
    module->header.bytecode_offset = current_offset;
    module->header.bytecode_size = module->bytecode_size;
    
    // Write header
    if (fwrite(&module->header, sizeof(HeliumHeader), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Write string table
    if (module->string_table_obj && module->header.string_table_size > 0) {
        if (fwrite(&module->string_table_obj->count, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(&module->string_table_obj->total_size, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->string_table_obj->entries, sizeof(StringEntry), module->string_table_obj->count, file) != module->string_table_obj->count) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->string_table_obj->data, 1, module->string_table_obj->total_size, file) != module->string_table_obj->total_size) {
            fclose(file);
            return false;
        }
    }
    
    // Write type table
    if (module->type_table && module->header.type_table_size > 0) {
        if (fwrite(&module->type_table->count, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->type_table->entries, sizeof(TypeEntry), module->type_table->count, file) != module->type_table->count) {
            fclose(file);
            return false;
        }
    }
    
    // Write method table
    if (module->method_table && module->header.method_table_size > 0) {
        if (fwrite(&module->method_table->count, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->method_table->entries, sizeof(MethodEntry), module->method_table->count, file) != module->method_table->count) {
            fclose(file);
            return false;
        }
    }
    
    // Write field table
    if (module->field_table && module->header.field_table_size > 0) {
        if (fwrite(&module->field_table->count, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->field_table->entries, sizeof(FieldEntry), module->field_table->count, file) != module->field_table->count) {
            fclose(file);
            return false;
        }
    }
    
    // Write bytecode
    if (module->bytecode && module->header.bytecode_size > 0) {
        if (fwrite(module->bytecode, 1, module->header.bytecode_size, file) != module->header.bytecode_size) {
            fclose(file);
            return false;
        }
    }
    
    fclose(file);
    return true;
}

// Load helium module from file
HeliumModule* helium_module_load(const char* filename) {
    if (!filename) return NULL;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    HeliumModule* module = malloc(sizeof(HeliumModule));
    if (!module) {
        fclose(file);
        return NULL;
    }
    
    // Read header
    if (fread(&module->header, sizeof(HeliumHeader), 1, file) != 1) {
        free(module);
        fclose(file);
        return NULL;
    }
    
    // Validate magic number
    if (strncmp(module->header.magic, HELIUM_MAGIC, 7) != 0) {
        free(module);
        fclose(file);
        return NULL;
    }
    
    // Load string table
    if (module->header.string_table_size > 0) {
        fseek(file, module->header.string_table_offset, SEEK_SET);
        module->string_table_obj = string_table_create();
        if (!module->string_table_obj) {
            free(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->string_table_obj->count, sizeof(uint32_t), 1, file) != 1) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->string_table_obj->total_size, sizeof(uint32_t), 1, file) != 1) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
        
        module->string_table_obj->entries = malloc(sizeof(StringEntry) * module->string_table_obj->count);
        if (!module->string_table_obj->entries) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->string_table_obj->entries, sizeof(StringEntry), module->string_table_obj->count, file) != module->string_table_obj->count) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
        
        module->string_table_obj->data = malloc(module->string_table_obj->total_size);
        if (!module->string_table_obj->data) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->string_table_obj->data, 1, module->string_table_obj->total_size, file) != module->string_table_obj->total_size) {
            string_table_destroy(module->string_table_obj);
            free(module);
            fclose(file);
            return NULL;
        }
    }
    
    // Load type table
    if (module->header.type_table_size > 0) {
        fseek(file, module->header.type_table_offset, SEEK_SET);
        module->type_table = type_table_create();
        if (!module->type_table) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->type_table->count, sizeof(uint32_t), 1, file) != 1) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        module->type_table->entries = malloc(sizeof(TypeEntry) * module->type_table->count);
        if (!module->type_table->entries) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->type_table->entries, sizeof(TypeEntry), module->type_table->count, file) != module->type_table->count) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
    }
    
    // Load method table
    if (module->header.method_table_size > 0) {
        fseek(file, module->header.method_table_offset, SEEK_SET);
        module->method_table = method_table_create();
        if (!module->method_table) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->method_table->count, sizeof(uint32_t), 1, file) != 1) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        module->method_table->entries = malloc(sizeof(MethodEntry) * module->method_table->count);
        if (!module->method_table->entries) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->method_table->entries, sizeof(MethodEntry), module->method_table->count, file) != module->method_table->count) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
    }
    
    // Load field table
    if (module->header.field_table_size > 0) {
        fseek(file, module->header.field_table_offset, SEEK_SET);
        module->field_table = field_table_create();
        if (!module->field_table) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->field_table->count, sizeof(uint32_t), 1, file) != 1) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        module->field_table->entries = malloc(sizeof(FieldEntry) * module->field_table->count);
        if (!module->field_table->entries) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->field_table->entries, sizeof(FieldEntry), module->field_table->count, file) != module->field_table->count) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
    }
    
    // Load bytecode
    if (module->header.bytecode_size > 0) {
        fseek(file, module->header.bytecode_offset, SEEK_SET);
        module->bytecode = malloc(module->header.bytecode_size);
        if (!module->bytecode) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->bytecode, 1, module->header.bytecode_size, file) != module->header.bytecode_size) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        module->bytecode_size = module->header.bytecode_size;
    }
    
    fclose(file);
    return module;
}

// Validate helium module
bool helium_module_validate(HeliumModule* module) {
    if (!module) return false;
    
    // Validate magic number
    if (strncmp(module->header.magic, HELIUM_MAGIC, 7) != 0) {
        return false;
    }
    
    // Validate version
    if (module->header.version_major > HELIUM_VERSION_MAJOR) {
        return false;
    }
    
    // Validate flags
    if (!(module->header.flags & HELIUM_FLAG_EXECUTABLE) && 
        !(module->header.flags & HELIUM_FLAG_LIBRARY)) {
        return false;
    }
    
    return true;
}

// Get string from module
const char* helium_module_get_string(HeliumModule* module, uint32_t offset) {
    if (!module || !module->string_table_obj || !module->string_table_obj->data) {
        return NULL;
    }
    
    if (offset >= module->string_table_obj->total_size) {
        return NULL;
    }
    
    return module->string_table_obj->data + offset;
}

// Add string to module
uint32_t helium_module_add_string(HeliumModule* module, const char* str) {
    if (!module || !str) return 0;
    
    return string_table_add_string(module->string_table_obj, str);
}

// Print module information
void helium_module_print_info(HeliumModule* module) {
    if (!module) {
        printf("Module: NULL\n");
        return;
    }
    
    printf("Helium3 Module:\n");
    printf("  Magic: %.7s\n", module->header.magic);
    printf("  Version: %u.%u\n", module->header.version_major, module->header.version_minor);
    printf("  Flags: 0x%08X\n", module->header.flags);
    printf("  Module Name: %s\n", helium_module_get_string(module, module->header.module_name_offset));
    printf("  Module Version: %s\n", helium_module_get_string(module, module->header.module_version_offset));
    printf("  Types: %u\n", module->type_table ? module->type_table->count : 0);
    printf("  Methods: %u\n", module->method_table ? module->method_table->count : 0);
    printf("  Fields: %u\n", module->field_table ? module->field_table->count : 0);
    printf("  Bytecode Size: %u bytes\n", module->bytecode_size);
    printf("  Entry Point: Method %u\n", module->header.entry_point_method_id);
}
