#include "../../shared/bytecode/helium_format.h"
#include "../../shared/stdlib/sys.h"
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
    module->constant_table = constant_table_create();
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
    
    if (module->constant_table) {
        constant_table_destroy(module->constant_table);
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
    
    // Constant table offset
    module->header.constant_table_offset = current_offset;
    if (module->constant_table) {
        module->header.constant_table_size = 
            sizeof(uint32_t) + 
            sizeof(ConstantEntry) * module->constant_table->count;
        current_offset += module->header.constant_table_size;
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
    
    // Write constant table
    if (module->constant_table && module->header.constant_table_size > 0) {
        if (fwrite(&module->constant_table->count, sizeof(uint32_t), 1, file) != 1) {
            fclose(file);
            return false;
        }
        
        if (fwrite(module->constant_table->entries, sizeof(ConstantEntry), module->constant_table->count, file) != module->constant_table->count) {
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
    
    // Load constant table
    if (module->header.constant_table_size > 0) {
        fseek(file, module->header.constant_table_offset, SEEK_SET);
        module->constant_table = constant_table_create();
        if (!module->constant_table) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(&module->constant_table->count, sizeof(uint32_t), 1, file) != 1) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        module->constant_table->entries = malloc(sizeof(ConstantEntry) * module->constant_table->count);
        if (!module->constant_table->entries) {
            helium_module_destroy(module);
            fclose(file);
            return NULL;
        }
        
        if (fread(module->constant_table->entries, sizeof(ConstantEntry), module->constant_table->count, file) != module->constant_table->count) {
            helium_module_destroy(module);
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
    
    const char* result = module->string_table_obj->data + offset;
    return result;
}

// Add string to module
uint32_t helium_module_add_string(HeliumModule* module, const char* str) {
    if (!module || !str) return 0;
    
    uint32_t index = string_table_add_string(module->string_table_obj, str);
    if (index >= module->string_table_obj->count) {
        return 0; // Error adding string
    }
    
    // Convert index to offset
    return module->string_table_obj->entries[index].offset;
}

// Add type to module
uint32_t helium_module_add_type(HeliumModule* module, const char* name, const char* module_name) {
    if (!module || !name || !module->type_table) return 0;
    
    TypeEntry type_entry;
    type_entry.type_id = module->type_table->count + 1; // Assign next available ID
    type_entry.name_offset = helium_module_add_string(module, name);
    type_entry.parent_type_id = 0; // Default to System.Object
    type_entry.size = sizeof(void*); // Default size
    type_entry.field_count = 0;
    type_entry.method_count = 0; // Will be updated when methods are added
    type_entry.interface_count = 0;
    type_entry.flags = TYPE_FLAG_CLASS;
    type_entry.vtable_offset = 0;
    
    if (!type_table_add_type(module->type_table, &type_entry)) {
        return 0;
    }
    
    return type_entry.type_id;
}

// Add method to module
uint32_t helium_module_add_method(HeliumModule* module, const char* name, const char* signature, uint32_t type_id, bool is_static) {
    if (!module || !name || !signature || !module->method_table) return 0;
    
    MethodEntry method_entry;
    method_entry.method_id = module->method_table->count + 1; // Assign next available ID
    method_entry.type_id = type_id;
    method_entry.name_offset = helium_module_add_string(module, name);
    method_entry.signature_offset = helium_module_add_string(module, signature);
    method_entry.bytecode_offset = 0; // Will be set by VM
    method_entry.bytecode_size = 0; // Will be set by VM
    method_entry.local_count = 0;
    method_entry.param_count = 0; // Will be parsed from signature
    method_entry.return_type_id = 0; // Will be parsed from signature
    method_entry.flags = is_static ? METHOD_FLAG_STATIC : 0;
    method_entry.line_number = 0;
    method_entry.column_number = 0;
    
    if (!method_table_add_method(module->method_table, &method_entry)) {
        return 0;
    }
    
    return method_entry.method_id;
}

// Add Sys class to module using Sys class information
bool helium_module_add_sys_class_from_info(HeliumModule* module) {
    if (!module || !module->type_table || !module->method_table) {
        return false;
    }
    
    // Get Sys class information
    const SysClassInfo* sys_info = sys_get_class_info();
    if (!sys_info) {
        return false;
    }
    
    // Add Sys class to type table
    uint32_t sys_type_id = helium_module_add_type(module, sys_info->class_name, sys_info->module_name);
    if (sys_type_id == 0) {
        return false;
    }
    
    // Add Sys methods to method table
    for (uint32_t i = 0; i < sys_info->method_count; i++) {
        const SysMethodInfo* method_info = &sys_info->methods[i];
        
        // Add method to module
        uint32_t method_id = helium_module_add_method(module, 
            method_info->name, 
            method_info->signature, 
            sys_type_id, 
            method_info->is_static);
        
        if (method_id == 0) {
            return false;
        }
        
    }
    return true;
}

// Add Sys class to module manifest (as first entry)
bool helium_module_add_sys_class(HeliumModule* module) {
    if (!module || !module->type_table || !module->method_table) {
        return false;
    }
    
    // Use the new function that uses Sys class information
    return helium_module_add_sys_class_from_info(module);
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
