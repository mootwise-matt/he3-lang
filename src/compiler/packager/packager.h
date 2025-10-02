#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "../../shared/bytecode/helium_format.h"
#include "../../shared/ast/ast.h"
#include "../../shared/bytecode/bytecode_format.h"

// Project file structure
typedef struct {
    char* name;
    char* version;
    char* type; // "executable" or "library"
    char* entry_point;
    char** source_files;
    uint32_t source_count;
    char** dependencies;
    uint32_t dependency_count;
    char* output_path;
    bool optimize;
    bool debug;
} He3Project;

// Compilation unit (single .he3 file)
typedef struct {
    char* filename;
    char* source_code;
    Ast* ast;
    BytecodeFile* bytecode;
    bool compiled;
    char* error_message;
} CompilationUnit;

// Project packager
typedef struct {
    He3Project* project;
    CompilationUnit* units;
    uint32_t unit_count;
    HeliumModule* final_module;
    bool has_errors;
} ProjectPackager;

// Function declarations
He3Project* he3_project_load(const char* project_file);
void he3_project_destroy(He3Project* project);
bool he3_project_validate(He3Project* project);

ProjectPackager* project_packager_create(He3Project* project);
void project_packager_destroy(ProjectPackager* packager);
bool project_packager_compile_all(ProjectPackager* packager);
bool project_packager_link(ProjectPackager* packager);
bool project_packager_save(ProjectPackager* packager, const char* output_path);

// Individual compilation
bool compile_single_file(const char* filename, CompilationUnit* unit);
void compilation_unit_destroy(CompilationUnit* unit);

// Linking functions
bool link_compilation_units(ProjectPackager* packager);
bool resolve_cross_references(ProjectPackager* packager);
bool merge_bytecode_files(ProjectPackager* packager);
