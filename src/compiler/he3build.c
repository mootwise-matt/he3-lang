#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "packager/packager.h"
#include "../shared/build_info.h"

// Print usage information
void print_usage(const char* program_name) {
    printf("He³ Build System v%s\n", HE3_VERSION_STRING);
    printf("Usage: %s [options] <project_file>\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -o, --output <file>     Specify output file\n");
    printf("  -d, --debug             Enable debug output\n");
    printf("  -c, --compile-only      Only compile, don't link\n");
    printf("  -l, --link-only         Only link (assume already compiled)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s project/he3project.json           # Build project\n", program_name);
    printf("  %s -o output.helium3 project/       # Build to specific output\n", program_name);
    printf("  %s -c project/                      # Only compile source files\n", program_name);
}

// Print version information
void print_version() {
    printf("He³ Build System v%s\n", HE3_FULL_VERSION_STRING);
    printf("A 64-bit, UTF-8, object-oriented programming language\n");
    printf("with Pascal roots and Java/C#-style syntax\n");
    printf("%s\n", HE3_BUILD_INFO_STRING);
}

int main(int argc, char* argv[]) {
    fflush(stdout);
    
    bool show_help = false;
    bool show_version = false;
    bool debug = false;
    bool compile_only = false;
    bool link_only = false;
    char* output_filename = NULL;
    char* project_file = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            show_version = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--compile-only") == 0) {
            compile_only = true;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--link-only") == 0) {
            link_only = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_filename = argv[++i];
            } else {
                fprintf(stderr, "Error: -o requires a filename\n");
                return 1;
            }
        } else if (argv[i][0] != '-') {
            project_file = argv[i];
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
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
    
    // Check for project file
    if (!project_file) {
        fprintf(stderr, "Error: No project file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Load project
    fflush(stdout);
    He3Project* project = he3_project_load(project_file);
    fflush(stdout);
    if (!project) {
        fprintf(stderr, "Error: Failed to load project\n");
        return 1;
    }
    
    // Validate project
    fflush(stdout);
    if (!he3_project_validate(project)) {
        fprintf(stderr, "Error: Invalid project configuration\n");
        he3_project_destroy(project);
        return 1;
    }
    fflush(stdout);
    
    printf("Project: %s v%s (%s)\n", project->name, project->version, project->type);
    printf("Source files: %u\n", project->source_count);
    
    // Create packager
    fflush(stdout);
    ProjectPackager* packager = project_packager_create(project);
    fflush(stdout);
    if (!packager) {
        fprintf(stderr, "Error: Failed to create project packager\n");
        he3_project_destroy(project);
        return 1;
    }
    
    // Compile all source files
    if (!link_only) {
        if (!project_packager_compile_all(packager)) {
            fprintf(stderr, "Error: Compilation failed\n");
            project_packager_destroy(packager);
            he3_project_destroy(project);
            return 1;
        }
    }
    
    // Link if not compile-only
    if (!compile_only) {
        fflush(stdout);
        if (!project_packager_link(packager)) {
            fprintf(stderr, "Error: Linking failed\n");
            project_packager_destroy(packager);
            he3_project_destroy(project);
            return 1;
        }
        fflush(stdout);
        
        // Save final module
        fflush(stdout);
        if (!project_packager_save(packager, output_filename)) {
            fprintf(stderr, "Error: Failed to save final module\n");
            project_packager_destroy(packager);
            he3_project_destroy(project);
            return 1;
        }
        fflush(stdout);
    }
    
    // Cleanup
    fflush(stdout);
    project_packager_destroy(packager);
    fflush(stdout);
    
    fflush(stdout);
    he3_project_destroy(project);
    fflush(stdout);
    
    printf("Build completed successfully!\n");
    return 0;
}
