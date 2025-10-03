#include <string.h>
#include "sys.h"

// Sys method definitions - all methods are static
static const SysMethodInfo sys_methods[] = {
    {"print", "(string)void", true},
    {"println", "(string)void", true},
    {"readLine", "()string", true},
    {"fileExists", "(string)boolean", true},
    {"readFile", "(string)string", true},
    {"writeFile", "(string,string)void", true},
    {"listDir", "(string)string[]", true},
    {"createDir", "(string)void", true},
    {"deleteFile", "(string)void", true},
    {"deleteDir", "(string)void", true},
    {"currentTimeMillis", "()integer", true},
    {"exit", "(integer)void", true},
    {"argc", "()integer", true},
    {"argv", "(integer)string", true},
    {"workingDir", "()string", true},
    {"changeDir", "(string)void", true},
    {"envGet", "(string)string", true},
    {"envSet", "(string,string)void", true}
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
