#ifndef SYS_H
#define SYS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sys class definition - available at compile time
// This is a proxy class that the compiler can use for linking

// Sys method signatures
typedef struct {
    const char* name;
    const char* signature;
    bool is_static;
} SysMethodInfo;

// Sys class information
typedef struct {
    const char* class_name;
    const char* module_name;
    SysMethodInfo* methods;
    uint32_t method_count;
} SysClassInfo;

// Get Sys class information
const SysClassInfo* sys_get_class_info(void);

// Get method information by name
const SysMethodInfo* sys_get_method_info(const char* method_name);

#ifdef __cplusplus
}
#endif

#endif // SYS_H
