#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "emitter/ast_to_ir.h"
#include "emitter/ir_to_bytecode.h"
#include "../shared/bytecode/bytecode_format.h"
#include "../shared/bytecode/helium_format.h"

// Compiler version
#define HE3_VERSION "0.1.0"

// Print usage information
void print_usage(const char* program_name) {
    printf("He³ Compiler v%s\n", HE3_VERSION);
    printf("Usage: %s [options] <input_file>\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -o, --output <file>     Specify output file (default: input.bx)\n");
    printf("  -d, --debug             Enable debug output\n");
    printf("  -t, --tokens            Show tokenized output\n");
    printf("  -a, --ast               Show AST output\n");
    printf("  -m, --module            Generate .helium3 module file\n");
    printf("  --lexer-only            Only run lexer (tokenize)\n");
    printf("  --parser-only           Only run parser (parse to AST)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s program.he3                    # Compile program.he3 to program.bx\n", program_name);
    printf("  %s -o output.bx program.he3       # Compile to specific output file\n", program_name);
    printf("  %s -m program.he3                 # Generate .helium3 module file\n", program_name);
    printf("  %s -t program.he3                 # Show tokens only\n", program_name);
    printf("  %s -a program.he3                 # Show AST only\n", program_name);
}

// Print version information
void print_version() {
    printf("He³ Compiler v%s\n", HE3_VERSION);
    printf("A 64-bit, UTF-8, object-oriented programming language\n");
    printf("with Pascal roots and Java/C#-style syntax\n");
}

// Read file contents
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Out of memory\n");
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t bytes_read = fread(buffer, 1, size, file);
    buffer[bytes_read] = '\0';
    
    fclose(file);
    return buffer;
}

// Get output filename from input filename
char* get_output_filename(const char* input_filename, const char* custom_output) {
    if (custom_output) {
        return strdup(custom_output);
    }
    
    // Find last dot in filename
    const char* last_dot = strrchr(input_filename, '.');
    if (last_dot && strcmp(last_dot, ".he3") == 0) {
        // Replace .he3 with .bx
        size_t len = last_dot - input_filename;
        char* output = malloc(len + 4);
        strncpy(output, input_filename, len);
        strcpy(output + len, ".bx");
        return output;
    } else {
        // Just append .bx
        char* output = malloc(strlen(input_filename) + 4);
        strcpy(output, input_filename);
        strcat(output, ".bx");
        return output;
    }
}

// Print tokens (for debug)
void print_tokens(Lexer* lexer) {
    printf("=== TOKENS ===\n");
    Token token;
    int count = 0;
    
    do {
        token = lexer_next_token(lexer);
        printf("Token %d: %d (", count++, token.kind);
        
        // Print token text
        if (token.len > 0) {
            printf("'%.*s'", (int)token.len, token.start);
        } else {
            printf("EOF");
        }
        
        printf(") at line %d, col %d\n", token.line, token.col);
    } while (token.kind != TK_EOF);
    
    printf("=== END TOKENS ===\n\n");
}

// Print AST (for debug)
void print_ast(Ast* node, int depth) {
    ast_print(node, depth);
}

// Main compilation function
int compile_file(const char* input_filename, const char* output_filename, 
                bool show_tokens, bool show_ast, bool lexer_only, bool parser_only, bool generate_module) {
    
    // Read input file
    char* source = read_file(input_filename);
    if (!source) {
        return 1;
    }
    
    printf("Compiling: %s -> %s\n", input_filename, output_filename);
    
    // Create lexer
    printf("DEBUG: About to create lexer\n");
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    printf("DEBUG: Lexer created successfully\n");
    
    // Show tokens if requested
    if (show_tokens || lexer_only) {
        print_tokens(lexer);
    }
    
    if (lexer_only) {
        lexer_destroy(lexer);
        free(source);
        return 0;
    }
    
    // Create parser
    Parser* parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Parse to AST
    printf("DEBUG: About to call parse_compilation_unit\n");
    Ast* ast = parse_compilation_unit(parser);
    printf("DEBUG: parse_compilation_unit returned\n");
    if (!ast) {
        fprintf(stderr, "Error: Failed to parse file\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Show AST if requested
    if (show_ast || parser_only) {
        printf("=== AST ===\n");
        print_ast(ast, 0);
        printf("=== END AST ===\n\n");
    }
    
    if (parser_only) {
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 0;
    }
    
    // Generate IR from AST
    printf("Generating IR...\n");
    AstToIRTranslator* ir_translator = ast_to_ir_translator_create();
    if (!ir_translator) {
        fprintf(stderr, "Error: Failed to create IR translator\n");
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    IRFunction* ir_function = ast_to_ir_translate_compilation_unit(ir_translator, ast);
    if (!ir_function) {
        fprintf(stderr, "Error: Failed to generate IR: %s\n", ast_to_ir_translator_get_error(ir_translator));
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("IR generated successfully (function: %s, blocks: %u)\n", ir_function->name, ir_function->block_count);
    
    // Generate bytecode from IR
    printf("Generating bytecode...\n");
    IRToBytecodeTranslator* bytecode_translator = ir_to_bytecode_translator_create();
    if (!bytecode_translator) {
        fprintf(stderr, "Error: Failed to create bytecode translator\n");
        // Note: ir_function_destroy not implemented yet
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Set current function in translator before adding method
    bytecode_translator->current_function = ir_function;
    
    // Add method to method table before translating
    uint32_t method_id = ir_to_bytecode_add_method(bytecode_translator, "main", "()I", 1);
    if (method_id == 0) {
        fprintf(stderr, "Error: Failed to add method to bytecode translator\n");
        ir_to_bytecode_translator_destroy(bytecode_translator);
        // Note: ir_function_destroy not implemented yet
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Translate the function
    if (!ir_to_bytecode_translate_function(bytecode_translator, ir_function)) {
        fprintf(stderr, "Error: Failed to translate function: %s\n", ir_to_bytecode_translator_get_error(bytecode_translator));
        ir_to_bytecode_translator_destroy(bytecode_translator);
        // Note: ir_function_destroy not implemented yet
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Generate final bytecode file
    BytecodeFile* bytecode_file = ir_to_bytecode_generate_file(bytecode_translator);
    if (!bytecode_file) {
        fprintf(stderr, "Error: Failed to generate bytecode file: %s\n", ir_to_bytecode_translator_get_error(bytecode_translator));
        ir_to_bytecode_translator_destroy(bytecode_translator);
        // Note: ir_function_destroy not implemented yet
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("Bytecode generated successfully\n");
    
    // Save bytecode to file
    printf("Saving bytecode to %s...\n", output_filename);
    if (!bytecode_file_save(bytecode_file, output_filename)) {
        fprintf(stderr, "Error: Failed to save bytecode file\n");
        bytecode_file_destroy(bytecode_file);
        ir_to_bytecode_translator_destroy(bytecode_translator);
        // Note: ir_function_destroy not implemented yet
        ast_to_ir_translator_destroy(ir_translator);
        // Note: ast_destroy not implemented yet
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    printf("Bytecode saved successfully\n");
    
    // Generate .helium3 module if requested
    if (generate_module) {
        printf("Generating .helium3 module...\n");
        
        // Create helium module
        HeliumModule* helium_module = helium_module_create();
        if (!helium_module) {
            fprintf(stderr, "Error: Failed to create helium module\n");
            bytecode_file_destroy(bytecode_file);
            ir_to_bytecode_translator_destroy(bytecode_translator);
            ast_to_ir_translator_destroy(ir_translator);
            parser_destroy(parser);
            lexer_destroy(lexer);
            free(source);
            return 1;
        }
        
        // Copy data from bytecode file to helium module
        helium_module->string_table_obj = bytecode_file->string_table;
        helium_module->constant_table = bytecode_file->constant_table;
        helium_module->type_table = bytecode_file->type_table;
        helium_module->method_table = bytecode_file->method_table;
        helium_module->field_table = bytecode_file->field_table;
        helium_module->bytecode = bytecode_file->bytecode;
        helium_module->bytecode_size = bytecode_file->header.bytecode_size;
        helium_module->header.entry_point_method_id = bytecode_file->header.entry_point_method_id;
        
        // Add Sys class to module manifest (as first entry)
        if (!helium_module_add_sys_class(helium_module)) {
            fprintf(stderr, "Error: Failed to add Sys class to module manifest\n");
            helium_module_destroy(helium_module);
            bytecode_file_destroy(bytecode_file);
            ir_to_bytecode_translator_destroy(bytecode_translator);
            ast_to_ir_translator_destroy(ir_translator);
            parser_destroy(parser);
            lexer_destroy(lexer);
            return 1;
        }
        
        // Set module name and version
        helium_module->header.module_name_offset = helium_module_add_string(helium_module, "example");
        helium_module->header.module_version_offset = helium_module_add_string(helium_module, "1.0.0");
        
        // Generate .helium3 filename
        char helium_filename[256];
        const char* last_dot = strrchr(input_filename, '.');
        if (last_dot && strcmp(last_dot, ".he3") == 0) {
            size_t len = last_dot - input_filename;
            strncpy(helium_filename, input_filename, len);
            helium_filename[len] = '\0';
            strcat(helium_filename, ".helium3");
        } else {
            strcpy(helium_filename, input_filename);
            strcat(helium_filename, ".helium3");
        }
        
        // Save helium module
        if (!helium_module_save(helium_module, helium_filename)) {
            fprintf(stderr, "Error: Failed to save helium module\n");
            helium_module_destroy(helium_module);
            bytecode_file_destroy(bytecode_file);
            ir_to_bytecode_translator_destroy(bytecode_translator);
            ast_to_ir_translator_destroy(ir_translator);
            parser_destroy(parser);
            lexer_destroy(lexer);
            free(source);
            return 1;
        }
        
        printf("Helium module saved to %s\n", helium_filename);
        helium_module_destroy(helium_module);
    }
    
    // Cleanup
    // Note: Temporarily commenting out cleanup to avoid segfault
    // bytecode_file_destroy(bytecode_file);
    // ir_to_bytecode_translator_destroy(bytecode_translator);
    // Note: ir_function_destroy not implemented yet
    // ast_to_ir_translator_destroy(ir_translator);
    // Note: ast_destroy not implemented yet
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(source);
    
    printf("Compilation completed successfully!\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("DEBUG: main() started with %d arguments\n", argc);
    
    // Command line options
    bool show_help = false;
    bool show_version = false;
    bool show_tokens = false;
    bool show_ast = false;
    bool lexer_only = false;
    bool parser_only = false;
    bool debug = false;
    bool generate_module = false;
    char* output_filename = NULL;
    char* input_filename = NULL;
    
    // Parse command line arguments
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"output", required_argument, 0, 'o'},
        {"debug", no_argument, 0, 'd'},
        {"tokens", no_argument, 0, 't'},
        {"ast", no_argument, 0, 'a'},
        {"module", no_argument, 0, 'm'},
        {"lexer-only", no_argument, 0, 1},
        {"parser-only", no_argument, 0, 2},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hvo:dtam", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                show_help = true;
                break;
            case 'v':
                show_version = true;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'd':
                debug = true;
                break;
            case 't':
                show_tokens = true;
                break;
            case 'a':
                show_ast = true;
                break;
            case 'm':
                generate_module = true;
                break;
            case 1: // --lexer-only
                lexer_only = true;
                break;
            case 2: // --parser-only
                parser_only = true;
                break;
            case '?':
                return 1;
            default:
                break;
        }
    }
    
    // Handle help and version
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (show_version) {
        print_version();
        return 0;
    }
    
    // Get input filename
    if (optind < argc) {
        input_filename = argv[optind];
    } else {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Get output filename
    char* final_output = get_output_filename(input_filename, output_filename);
    
    // Enable debug output
    if (debug) {
        printf("Debug mode enabled\n");
    }
    
    // Compile the file
    int result = compile_file(input_filename, final_output, show_tokens, show_ast, 
                            lexer_only, parser_only, generate_module);
    
    free(final_output);
    return result;
}
