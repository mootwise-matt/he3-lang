#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "lexer/lexer.h"
#include "parser/parser.h"

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
    printf("  --lexer-only            Only run lexer (tokenize)\n");
    printf("  --parser-only           Only run parser (parse to AST)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s program.he3                    # Compile program.he3 to program.bx\n", program_name);
    printf("  %s -o output.bx program.he3       # Compile to specific output file\n", program_name);
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
    ast_print_tree(node, depth);
}

// Main compilation function
int compile_file(const char* input_filename, const char* output_filename, 
                bool show_tokens, bool show_ast, bool lexer_only, bool parser_only) {
    
    // Read input file
    char* source = read_file(input_filename);
    if (!source) {
        return 1;
    }
    
    printf("Compiling: %s -> %s\n", input_filename, output_filename);
    
    // Create lexer
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    
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
    Ast* ast = parse_compilation_unit(parser);
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
    
    // TODO: Add type resolution, IR generation, bytecode emission
    printf("TODO: Type resolution, IR generation, and bytecode emission not yet implemented\n");
    
    // Cleanup
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(source);
    
    printf("Compilation completed successfully!\n");
    return 0;
}

int main(int argc, char* argv[]) {
    // Command line options
    bool show_help = false;
    bool show_version = false;
    bool show_tokens = false;
    bool show_ast = false;
    bool lexer_only = false;
    bool parser_only = false;
    bool debug = false;
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
        {"lexer-only", no_argument, 0, 1},
        {"parser-only", no_argument, 0, 2},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hvo:dta", long_options, &option_index)) != -1) {
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
                            lexer_only, parser_only);
    
    free(final_output);
    return result;
}
