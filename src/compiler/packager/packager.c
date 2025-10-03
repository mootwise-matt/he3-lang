#include "packager.h"
#include "../parser/parser.h"
#include "../emitter/ast_to_ir.h"
#include "../emitter/ir_to_bytecode.h"
#include "../../shared/bytecode/bytecode_format.h"
#include "../../shared/bytecode/helium_format.h"
#include "../../shared/stdlib/sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Load project from JSON file
He3Project* he3_project_load(const char* project_file) {
    FILE* file = fopen(project_file, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open project file '%s'\n", project_file);
        return NULL;
    }
    
    // For now, create a simple project structure
    // TODO: Implement proper JSON parsing
    He3Project* project = malloc(sizeof(He3Project));
    if (!project) return NULL;
    
    // Initialize with defaults
    project->name = strdup("untitled");
    project->version = strdup("1.0.0");
    project->type = strdup("executable");
    project->entry_point = NULL;
    project->source_files = NULL;
    project->source_count = 0;
    project->dependencies = NULL;
    project->dependency_count = 0;
    project->output_path = strdup("build/output.helium3");
    project->optimize = false;
    project->debug = true;
    
    // Simple heuristic: find all .he3 files in the project directory
    char project_dir[512];
    strncpy(project_dir, project_file, sizeof(project_dir) - 1);
    project_dir[sizeof(project_dir) - 1] = '\0';
    
    // Find last slash and truncate to get directory
    char* last_slash = strrchr(project_dir, '/');
    if (last_slash) {
        *last_slash = '\0';
    }
    
    // Look for source files in src/ directory
    char src_dir[512];
    snprintf(src_dir, sizeof(src_dir), "%s/src", project_dir);
    
    
    // Count .he3 files
    project->source_count = 0;
    char** temp_files = malloc(64 * sizeof(char*));
    if (!temp_files) {
        fclose(file);
        he3_project_destroy(project);
        return NULL;
    }
    
    // Simple directory scanning (in a real implementation, use opendir/readdir)
    // For now, assume common patterns
    const char* common_files[] = {"main.he3", "utils.he3", "lib.he3", "app.he3"};
    for (int i = 0; i < 4; i++) {
        char test_path[512];
        snprintf(test_path, sizeof(test_path), "%s/%s", src_dir, common_files[i]);
        
        struct stat st;
        if (stat(test_path, &st) == 0) {
            temp_files[project->source_count] = strdup(test_path);
            if (!temp_files[project->source_count]) {
                // Cleanup on error
                for (uint32_t j = 0; j < project->source_count; j++) {
                    free(temp_files[j]);
                }
                free(temp_files);
                fclose(file);
                he3_project_destroy(project);
                return NULL;
            }
            project->source_count++;
        }
    }
    
    // Allocate final array
    project->source_files = malloc(project->source_count * sizeof(char*));
    if (!project->source_files) {
        for (uint32_t i = 0; i < project->source_count; i++) {
            free(temp_files[i]);
        }
        free(temp_files);
        fclose(file);
        he3_project_destroy(project);
        return NULL;
    }
    
    // Copy file paths
    for (uint32_t i = 0; i < project->source_count; i++) {
        project->source_files[i] = temp_files[i];
    }
    free(temp_files);
    
    fclose(file);
    return project;
}

// Destroy project
void he3_project_destroy(He3Project* project) {
    if (!project) return;
    
    free(project->name);
    free(project->version);
    free(project->type);
    free(project->entry_point);
    free(project->output_path);
    
    for (uint32_t i = 0; i < project->source_count; i++) {
        free(project->source_files[i]);
    }
    free(project->source_files);
    
    for (uint32_t i = 0; i < project->dependency_count; i++) {
        free(project->dependencies[i]);
    }
    free(project->dependencies);
    
    free(project);
}

// Validate project
bool he3_project_validate(He3Project* project) {
    if (!project) return false;
    
    if (!project->name || !project->version || !project->type) {
        fprintf(stderr, "Error: Project missing required fields\n");
        return false;
    }
    
    if (project->source_count == 0) {
        fprintf(stderr, "Error: No source files found\n");
        return false;
    }
    
    return true;
}

// Create project packager
ProjectPackager* project_packager_create(He3Project* project) {
    if (!project) return NULL;
    
    ProjectPackager* packager = malloc(sizeof(ProjectPackager));
    if (!packager) return NULL;
    
    packager->project = project;
    packager->unit_count = project->source_count;
    packager->units = calloc(packager->unit_count, sizeof(CompilationUnit));
    packager->final_module = NULL;
    packager->has_errors = false;
    
    if (!packager->units) {
        free(packager);
        return NULL;
    }
    
    // Initialize compilation units
    for (uint32_t i = 0; i < packager->unit_count; i++) {
        fflush(stdout);
        
        packager->units[i].filename = strdup(project->source_files[i]);
        packager->units[i].source_code = NULL;
        packager->units[i].ast = NULL;
        packager->units[i].bytecode = NULL;
        packager->units[i].compiled = false;
        packager->units[i].error_message = NULL;
        packager->units[i].bytecode_translator = NULL;  // Initialize to NULL
        
        fflush(stdout);
    }
    
    fflush(stdout);
    
    return packager;
}

// Destroy project packager
void project_packager_destroy(ProjectPackager* packager) {
    if (!packager) return;
    
    for (uint32_t i = 0; i < packager->unit_count; i++) {
        compilation_unit_destroy(&packager->units[i]);
    }
    free(packager->units);
    
    if (packager->final_module) {
        helium_module_destroy(packager->final_module);
    }
    
    free(packager);
}

// Compile all source files
bool project_packager_compile_all(ProjectPackager* packager) {
    fflush(stdout);
    
    if (!packager) {
        fflush(stdout);
        return false;
    }
    
    fflush(stdout);
    
    fflush(stdout);
    
    for (uint32_t i = 0; i < packager->unit_count; i++) {
        fflush(stdout);
        
        if (packager->units[i].filename) {
            // Print filename carefully to avoid crash
            for (int j = 0; j < 100 && packager->units[i].filename[j] != '\0'; j++) {
                putchar(packager->units[i].filename[j]);
            }
            printf("\n");
            fflush(stdout);
        } else {
            printf("ERROR: filename is NULL for unit %u\n", i);
        }
        
        if (!compile_single_file(packager->units[i].filename, &packager->units[i])) {
            packager->has_errors = true;
            printf("  Failed: %s\n", packager->units[i].error_message ? packager->units[i].error_message : "Unknown error");
        } else {
            printf("  Success\n");
        }
    }
    
    return !packager->has_errors;
}


// Link all compilation units into final module
bool project_packager_link(ProjectPackager* packager) {
    fflush(stdout);
    
    if (!packager || packager->has_errors) return false;
    
    printf("Linking %u compilation units...\n", packager->unit_count);
    
    // Create final helium module
    fflush(stdout);
    packager->final_module = helium_module_create();
    fflush(stdout);
    if (!packager->final_module) {
        fprintf(stderr, "Error: Failed to create final helium module\n");
        return false;
    }
    
    // Merge all bytecode files first
    fflush(stdout);
    if (!merge_bytecode_files(packager)) {
        fprintf(stderr, "Error: Failed to merge bytecode files\n");
        helium_module_destroy(packager->final_module);
        packager->final_module = NULL;
        return false;
    }
    fflush(stdout);
    
    // Add Sys class to the module AFTER merging bytecode
    printf("  Adding Sys class...\n");
    if (!helium_module_add_sys_class(packager->final_module)) {
        printf("  Warning: Failed to add Sys class (continuing without it)\n");
    } else {
        printf("  Sys class added successfully\n");
    }
    
    // Set module metadata
    packager->final_module->header.module_name_offset = helium_module_add_string(packager->final_module, packager->project->name);
    packager->final_module->header.module_version_offset = helium_module_add_string(packager->final_module, packager->project->version);
    
    printf("Linking completed successfully\n");
    return true;
}

// Save final module
bool project_packager_save(ProjectPackager* packager, const char* output_path) {
    fflush(stdout);
    
    if (!packager || !packager->final_module) return false;
    
    const char* path = output_path ? output_path : packager->project->output_path;
    fflush(stdout);
    
    fflush(stdout);
    if (!helium_module_save(packager->final_module, path)) {
        fprintf(stderr, "Error: Failed to save final module to %s\n", path);
        return false;
    }
    
    fflush(stdout);
    printf("Final module saved to %s\n", path);
    return true;
}

// Compile single file
bool compile_single_file(const char* filename, CompilationUnit* unit) {
    fflush(stdout);
    
    if (!unit) {
        fflush(stdout);
        return false;
    }
    
    fflush(stdout);
    
    // Read source file
    fflush(stdout);
    
    FILE* file = fopen(filename, "r");
    
    fflush(stdout);
    
    if (!file) {
        fflush(stdout);
        unit->error_message = strdup("Could not open file");
        return false;
    }
    
    fflush(stdout);
    
    // Get file size
    fflush(stdout);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fflush(stdout);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    fflush(stdout);
    unit->source_code = malloc(size + 1);
    if (!unit->source_code) {
        unit->error_message = strdup("Out of memory");
        fclose(file);
        return false;
    }
    fflush(stdout);
    
    // Read file
    fflush(stdout);
    size_t bytes_read = fread(unit->source_code, 1, size, file);
    unit->source_code[bytes_read] = '\0';
    fflush(stdout);
    fclose(file);
    fflush(stdout);
    
    // Create lexer
    fflush(stdout);
    Lexer* lexer = lexer_create(unit->source_code);
    fflush(stdout);
    if (!lexer) {
        unit->error_message = strdup("Failed to create lexer");
        return false;
    }
    
    // Create parser
    fflush(stdout);
    Parser* parser = parser_create(lexer);
    fflush(stdout);
    if (!parser) {
        unit->error_message = strdup("Failed to create parser");
        lexer_destroy(lexer);
        return false;
    }
    
    // Parse to AST
    fflush(stdout);
    unit->ast = parse_compilation_unit(parser);
    fflush(stdout);
    if (!unit->ast) {
        unit->error_message = strdup("Failed to parse file");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    // Generate IR
    fflush(stdout);
    AstToIRTranslator* ir_translator = ast_to_ir_translator_create();
    fflush(stdout);
    if (!ir_translator) {
        unit->error_message = strdup("Failed to create IR translator");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    fflush(stdout);
    IRFunction* ir_function = ast_to_ir_translate_compilation_unit(ir_translator, unit->ast);
    fflush(stdout);
    if (!ir_function) {
        unit->error_message = strdup("Failed to generate IR");
        ast_to_ir_translator_destroy(ir_translator);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    // Generate bytecode
    fflush(stdout);
    IRToBytecodeTranslator* bytecode_translator = ir_to_bytecode_translator_create();
    fflush(stdout);
    if (!bytecode_translator) {
        unit->error_message = strdup("Failed to create bytecode translator");
        ast_to_ir_translator_destroy(ir_translator);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    fflush(stdout);
    bytecode_translator->current_function = ir_function;
    
    // Add method to bytecode translator
    fflush(stdout);
    uint32_t method_id = ir_to_bytecode_add_method(bytecode_translator, "main", "()I", 1);
    fflush(stdout);
    if (method_id == 0) {
        unit->error_message = strdup("Failed to add method to bytecode translator");
        ir_to_bytecode_translator_destroy(bytecode_translator);
        ast_to_ir_translator_destroy(ir_translator);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    // Translate IR to bytecode
    fflush(stdout);
    if (!ir_to_bytecode_translate_function(bytecode_translator, ir_function)) {
        unit->error_message = strdup("Failed to translate IR to bytecode");
        ir_to_bytecode_translator_destroy(bytecode_translator);
        ast_to_ir_translator_destroy(ir_translator);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    fflush(stdout);
    
    // Get bytecode file
    fflush(stdout);
    unit->bytecode = ir_to_bytecode_generate_file(bytecode_translator);
    fflush(stdout);
    if (!unit->bytecode) {
        unit->error_message = strdup("Failed to get bytecode file");
        ir_to_bytecode_translator_destroy(bytecode_translator);
        ast_to_ir_translator_destroy(ir_translator);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return false;
    }
    
    fflush(stdout);
    unit->compiled = true;
    
    // Cleanup (but preserve bytecode file)
    // Don't destroy bytecode_translator yet - we need the bytecode file
    fflush(stdout);
    ast_to_ir_translator_destroy(ir_translator);
    fflush(stdout);
    
    fflush(stdout);
    parser_destroy(parser);
    fflush(stdout);
    
    fflush(stdout);
    lexer_destroy(lexer);
    fflush(stdout);
    
    // Store the translator for later cleanup
    fflush(stdout);
    unit->bytecode_translator = bytecode_translator;
    
    fflush(stdout);
    
    return true;
}

// Destroy compilation unit
void compilation_unit_destroy(CompilationUnit* unit) {
    if (!unit) return;
    
    free(unit->filename);
    free(unit->source_code);
    free(unit->error_message);
    
    // Clean up bytecode translator if it exists
    if (unit->bytecode_translator) {
        ir_to_bytecode_translator_destroy((IRToBytecodeTranslator*)unit->bytecode_translator);
    }
    
    // Note: ast_destroy and bytecode_file_destroy not implemented yet
    // In a real implementation, these would be called here
}

// Merge bytecode files into final module
bool merge_bytecode_files(ProjectPackager* packager) {
    if (!packager || !packager->final_module) return false;
    
    // For now, just copy the first bytecode file's data
    // In a real implementation, this would merge all bytecode files
    // and resolve cross-references
    
    if (packager->unit_count == 0) return false;
    
    CompilationUnit* first_unit = &packager->units[0];
    if (!first_unit->compiled || !first_unit->bytecode) return false;
    
    // Copy bytecode data
    fflush(stdout);
    
    packager->final_module->bytecode = malloc(first_unit->bytecode->bytecode_size);
    if (!packager->final_module->bytecode) return false;
    
    fflush(stdout);
    
    memcpy(packager->final_module->bytecode, 
           first_unit->bytecode->bytecode, 
           first_unit->bytecode->bytecode_size);
    
    fflush(stdout);
    
    packager->final_module->bytecode_size = first_unit->bytecode->bytecode_size;
    
    // Set entry point method ID (for now, use 1 as the main method)
    packager->final_module->header.entry_point_method_id = 1;
    
    // Set bytecode size in header (this is what the save function uses)
    packager->final_module->header.bytecode_size = first_unit->bytecode->bytecode_size;
    
    // Merge all bytecode files into the helium module
    // This involves collecting metadata from all .bx files and merging bytecode
    
    // For now, just transfer ownership of the first unit's data
    // TODO: Implement proper merging of multiple .bx files
    
    // Transfer ownership of type table
    if (first_unit->bytecode->type_table) {
        packager->final_module->type_table = first_unit->bytecode->type_table;
        first_unit->bytecode->type_table = NULL;  // Transfer ownership
    }
    
    // Transfer ownership of method table
    if (first_unit->bytecode->method_table) {
        packager->final_module->method_table = first_unit->bytecode->method_table;
        first_unit->bytecode->method_table = NULL;  // Transfer ownership
    }
    
    // Transfer ownership of field table
    if (first_unit->bytecode->field_table) {
        packager->final_module->field_table = first_unit->bytecode->field_table;
        first_unit->bytecode->field_table = NULL;  // Transfer ownership
    }
    
    // Transfer ownership of string table
    if (first_unit->bytecode->string_table) {
        packager->final_module->string_table_obj = first_unit->bytecode->string_table;
        first_unit->bytecode->string_table = NULL;  // Transfer ownership
    }
    
    // Transfer ownership of constant table
    fflush(stdout);
    if (first_unit->bytecode->constant_table) {
        fflush(stdout);
        packager->final_module->constant_table = first_unit->bytecode->constant_table;
        first_unit->bytecode->constant_table = NULL;  // Transfer ownership
        fflush(stdout);
    } else {
        fflush(stdout);
    }
    
    // Clear the bytecode translator's table pointers since we transferred ownership
    if (first_unit->bytecode_translator) {
        IRToBytecodeTranslator* translator = (IRToBytecodeTranslator*)first_unit->bytecode_translator;
        translator->string_table = NULL;
        translator->constant_table = NULL;
        translator->type_table = NULL;
        translator->method_table = NULL;
    }
    
    
    return true;
}
