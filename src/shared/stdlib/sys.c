#include <string.h>
#include "sys.h"

// Sys method definitions
static const SysMethodInfo sys_methods[] = {
    {"print", "(string)void", false},
    {"println", "(string)void", false},
    {"readLine", "()string", false},
    {"fileExists", "(string)boolean", false},
    {"readFile", "(string)string", false},
    {"writeFile", "(string,string)void", false},
    {"listDir", "(string)string[]", false},
    {"createDir", "(string)void", false},
    {"deleteFile", "(string)void", false},
    {"deleteDir", "(string)void", false},
    {"currentTimeMillis", "()integer", false},
    {"exit", "(integer)void", false},
    {"argc", "()integer", false},
    {"argv", "(integer)string", false},
    {"workingDir", "()string", false},
    {"changeDir", "(string)void", false},
    {"envGet", "(string)string", false},
    {"envSet", "(string,string)void", false}
};

static const SysClassInfo sys_class_info = {
    .class_name = "Sys",
    .module_name = "System",
    .methods = (SysMethodInfo*)sys_methods,
    .method_count = sizeof(sys_methods) / sizeof(sys_methods[0])
};

const SysClassInfo* sys_get_class_info(void) {
    return &sys_class_info;
}

const SysMethodInfo* sys_get_method_info(const char* method_name) {
    if (!method_name) return NULL;
    
    for (uint32_t i = 0; i < sys_class_info.method_count; i++) {
        if (strcmp(sys_methods[i].name, method_name) == 0) {
            return &sys_methods[i];
        }
    }
    return NULL;
}
