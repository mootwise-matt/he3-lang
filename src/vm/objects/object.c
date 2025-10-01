#include "object.h"
#include "../vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Global class registry
static Class* class_registry = NULL;
static uint32_t next_type_id = 1;

// Object creation and destruction
Object* object_create(struct Heap* heap, Class* class_info) {
    if (!heap || !class_info) {
        return NULL;
    }
    
    // Calculate total object size
    size_t object_size = sizeof(ObjectHeader) + class_info->size;
    
    // Allocate object memory
    Object* object = (Object*)heap_allocate(heap, object_size);
    if (!object) {
        return NULL;
    }
    
    // Initialize object header
    object->header.type_id = class_info->type_id;
    object->header.ref_count = 1;
    object->header.size = object_size;
    object->header.flags = 0;
    object->header.class_info = class_info;
    
    // Initialize object data to zero
    memset(object->data, 0, class_info->size);
    
    return object;
}

void object_destroy(Object* object) {
    if (!object) return;
    
    // Decrement reference count
    object_remove_reference(object);
}

Object* object_clone(Object* object) {
    if (!object) return NULL;
    
    // Create new object of same class
    // Note: This is a simplified clone - we need the heap from somewhere
    // For now, return NULL as we don't have access to the heap here
    Object* clone = NULL;
    if (!clone) return NULL;
    
    // Copy object data
    memcpy(clone->data, object->data, object->header.class_info->size);
    
    return clone;
}

// Reference counting
void object_add_reference(Object* object) {
    if (object) {
        object->header.ref_count++;
    }
}

void object_remove_reference(Object* object) {
    if (!object) return;
    
    object->header.ref_count--;
    if (object->header.ref_count == 0) {
        // Object can be garbage collected
        // TODO: Add to GC queue
    }
}

uint32_t object_get_ref_count(Object* object) {
    return object ? object->header.ref_count : 0;
}

// Object properties
Class* object_get_class(Object* object) {
    return object ? object->header.class_info : NULL;
}

uint32_t object_get_type_id(Object* object) {
    return object ? object->header.type_id : 0;
}

size_t object_get_size(Object* object) {
    return object ? object->header.size : 0;
}

bool object_is_instance_of(Object* object, Class* class_info) {
    if (!object || !class_info) return false;
    
    Class* current = object->header.class_info;
    while (current) {
        if (current == class_info) {
            return true;
        }
        current = current->superclass;
    }
    return false;
}

bool object_is_instance_of_interface(Object* object, Interface* interface) {
    if (!object || !interface) return false;
    
    Class* class_info = object->header.class_info;
    for (uint32_t i = 0; i < class_info->interface_count; i++) {
        if (class_info->interfaces[i] == interface) {
            return true;
        }
    }
    
    // Check superclasses
    Class* current = class_info->superclass;
    while (current) {
        for (uint32_t i = 0; i < current->interface_count; i++) {
            if (current->interfaces[i] == interface) {
                return true;
            }
        }
        current = current->superclass;
    }
    
    return false;
}

// Field access
struct Value object_get_field(Object* object, const char* field_name) {
    if (!object || !field_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    Field* field = class_find_field(object->header.class_info, field_name);
    if (!field || field->is_static) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // Calculate field address
    void* field_addr = (char*)object->data + field->offset;
    
    // TODO: Read field value based on type
    // For now, return null
    return (struct Value){VALUE_NULL, {0}};
}

void object_set_field(Object* object, const char* field_name, struct Value value) {
    if (!object || !field_name) return;
    
    Field* field = class_find_field(object->header.class_info, field_name);
    if (!field || field->is_static) return;
    
    // Calculate field address
    void* field_addr = (char*)object->data + field->offset;
    
    // TODO: Write field value based on type
}

struct Value object_get_static_field(Class* class_info, const char* field_name) {
    if (!class_info || !field_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    Field* field = class_find_field(class_info, field_name);
    if (!field || !field->is_static) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // TODO: Read static field value
    return (struct Value){VALUE_NULL, {0}};
}

void object_set_static_field(Class* class_info, const char* field_name, struct Value value) {
    if (!class_info || !field_name) return;
    
    Field* field = class_find_field(class_info, field_name);
    if (!field || !field->is_static) return;
    
    // TODO: Write static field value
}

// Method calls
struct Value object_call_method(Object* object, const char* method_name, struct Value* args, size_t arg_count) {
    if (!object || !method_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    Method* method = class_find_method(object->header.class_info, method_name);
    if (!method) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    if (method->is_virtual) {
        return method_dispatch_virtual(object, method, args, arg_count);
    } else {
        return method_dispatch_static(method, args, arg_count);
    }
}

struct Value object_call_static_method(Class* class_info, const char* method_name, struct Value* args, size_t arg_count) {
    if (!class_info || !method_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    Method* method = class_find_method(class_info, method_name);
    if (!method || !method->is_static) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    return method_dispatch_static(method, args, arg_count);
}

struct Value object_call_virtual_method(Object* object, const char* method_name, struct Value* args, size_t arg_count) {
    if (!object || !method_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // Find method in class hierarchy
    Class* current = object->header.class_info;
    while (current) {
        Method* method = class_find_method(current, method_name);
        if (method && method->is_virtual) {
            return method_dispatch_virtual(object, method, args, arg_count);
        }
        current = current->superclass;
    }
    
    return (struct Value){VALUE_NULL, {0}};
}

// Class management
Class* class_create(const char* name, uint32_t type_id, uint32_t size) {
    if (!name) return NULL;
    
    Class* class_info = malloc(sizeof(Class));
    if (!class_info) return NULL;
    
    class_info->type_id = type_id;
    class_info->name = strdup(name);
    class_info->size = size;
    class_info->superclass = NULL;
    class_info->methods = NULL;
    class_info->fields = NULL;
    class_info->interfaces = NULL;
    class_info->interface_count = 0;
    class_info->field_count = 0;
    class_info->method_count = 0;
    class_info->is_abstract = false;
    class_info->is_final = false;
    class_info->is_interface = false;
    class_info->next = NULL;
    
    return class_info;
}

void class_destroy(Class* class_info) {
    if (!class_info) return;
    
    // Free name
    if (class_info->name) {
        free(class_info->name);
    }
    
    // Free methods
    Method* method = class_info->methods;
    while (method) {
        Method* next = method->next;
        method_destroy(method);
        method = next;
    }
    
    // Free fields
    Field* field = class_info->fields;
    while (field) {
        Field* next = field->next;
        field_destroy(field);
        field = next;
    }
    
    // Free interfaces
    if (class_info->interfaces) {
        free(class_info->interfaces);
    }
    
    free(class_info);
}

Class* class_find(const char* name) {
    if (!name) return NULL;
    
    Class* current = class_registry;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void class_register(Class* class_info) {
    if (!class_info) return;
    
    class_info->next = class_registry;
    class_registry = class_info;
}

void class_set_superclass(Class* class_info, Class* superclass) {
    if (class_info) {
        class_info->superclass = superclass;
    }
}

void class_add_interface(Class* class_info, Interface* interface) {
    if (!class_info || !interface) return;
    
    // Resize interfaces array
    class_info->interfaces = realloc(class_info->interfaces, 
                                    sizeof(Interface*) * (class_info->interface_count + 1));
    if (!class_info->interfaces) return;
    
    class_info->interfaces[class_info->interface_count] = interface;
    class_info->interface_count++;
}

// Method management
Method* method_create(const char* name, const char* signature, uint8_t* bytecode, uint32_t bytecode_size) {
    if (!name || !signature) return NULL;
    
    Method* method = malloc(sizeof(Method));
    if (!method) return NULL;
    
    method->name = strdup(name);
    method->signature = strdup(signature);
    method->bytecode = bytecode;
    method->bytecode_size = bytecode_size;
    method->local_count = 0;
    method->param_count = 0;
    method->is_static = false;
    method->is_virtual = false;
    method->is_abstract = false;
    method->is_private = false;
    method->is_protected = false;
    method->is_public = true;
    method->next = NULL;
    
    return method;
}

void method_destroy(Method* method) {
    if (!method) return;
    
    if (method->name) free(method->name);
    if (method->signature) free(method->signature);
    if (method->bytecode) free(method->bytecode);
    free(method);
}

void class_add_method(Class* class_info, Method* method) {
    if (!class_info || !method) return;
    
    method->next = class_info->methods;
    class_info->methods = method;
    class_info->method_count++;
}

Method* class_find_method(Class* class_info, const char* name) {
    if (!class_info || !name) return NULL;
    
    Method* method = class_info->methods;
    while (method) {
        if (strcmp(method->name, name) == 0) {
            return method;
        }
        method = method->next;
    }
    return NULL;
}

Method* class_find_method_by_signature(Class* class_info, const char* name, const char* signature) {
    if (!class_info || !name || !signature) return NULL;
    
    Method* method = class_info->methods;
    while (method) {
        if (strcmp(method->name, name) == 0 && strcmp(method->signature, signature) == 0) {
            return method;
        }
        method = method->next;
    }
    return NULL;
}

// Field management
Field* field_create(const char* name, uint32_t type_id, uint32_t offset, uint32_t size) {
    if (!name) return NULL;
    
    Field* field = malloc(sizeof(Field));
    if (!field) return NULL;
    
    field->name = strdup(name);
    field->type_id = type_id;
    field->offset = offset;
    field->size = size;
    field->is_static = false;
    field->is_private = false;
    field->is_protected = false;
    field->is_public = true;
    field->next = NULL;
    
    return field;
}

void field_destroy(Field* field) {
    if (!field) return;
    
    if (field->name) free(field->name);
    free(field);
}

void class_add_field(Class* class_info, Field* field) {
    if (!class_info || !field) return;
    
    field->next = class_info->fields;
    class_info->fields = field;
    class_info->field_count++;
}

Field* class_find_field(Class* class_info, const char* name) {
    if (!class_info || !name) return NULL;
    
    Field* field = class_info->fields;
    while (field) {
        if (strcmp(field->name, name) == 0) {
            return field;
        }
        field = field->next;
    }
    return NULL;
}

// Interface management
Interface* interface_create(const char* name, uint32_t interface_id) {
    if (!name) return NULL;
    
    Interface* interface = malloc(sizeof(Interface));
    if (!interface) return NULL;
    
    interface->name = strdup(name);
    interface->interface_id = interface_id;
    interface->methods = NULL;
    interface->method_count = 0;
    interface->next = NULL;
    
    return interface;
}

void interface_destroy(Interface* interface) {
    if (!interface) return;
    
    if (interface->name) free(interface->name);
    
    Method* method = interface->methods;
    while (method) {
        Method* next = method->next;
        method_destroy(method);
        method = next;
    }
    
    free(interface);
}

void interface_add_method(Interface* interface, Method* method) {
    if (!interface || !method) return;
    
    method->next = interface->methods;
    interface->methods = method;
    interface->method_count++;
}

Method* interface_find_method(Interface* interface, const char* name) {
    if (!interface || !name) return NULL;
    
    Method* method = interface->methods;
    while (method) {
        if (strcmp(method->name, name) == 0) {
            return method;
        }
        method = method->next;
    }
    return NULL;
}

// Method dispatch
struct Value method_dispatch_static(Method* method, struct Value* args, size_t arg_count) {
    if (!method) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // TODO: Implement static method dispatch
    // This would involve setting up a call frame and executing bytecode
    return (struct Value){VALUE_NULL, {0}};
}

struct Value method_dispatch_virtual(Object* object, Method* method, struct Value* args, size_t arg_count) {
    if (!object || !method) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // TODO: Implement virtual method dispatch
    // This would involve setting up a call frame with 'this' pointer and executing bytecode
    return (struct Value){VALUE_NULL, {0}};
}

struct Value method_dispatch_interface(Object* object, Interface* interface, const char* method_name, struct Value* args, size_t arg_count) {
    if (!object || !interface || !method_name) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    Method* method = interface_find_method(interface, method_name);
    if (!method) {
        return (struct Value){VALUE_NULL, {0}};
    }
    
    // TODO: Implement interface method dispatch
    return (struct Value){VALUE_NULL, {0}};
}

// Object utilities
void object_print(Object* object) {
    if (!object) {
        printf("NULL object\n");
        return;
    }
    
    printf("Object: %s (type_id=%u, ref_count=%u, size=%zu)\n",
           object->header.class_info->name,
           object->header.type_id,
           object->header.ref_count,
           object->header.size);
}

void object_print_class_hierarchy(Class* class_info) {
    if (!class_info) return;
    
    printf("Class hierarchy for %s:\n", class_info->name);
    Class* current = class_info;
    int level = 0;
    
    while (current) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("- %s (type_id=%u)\n", current->name, current->type_id);
        current = current->superclass;
        level++;
    }
}

void object_print_method_table(Class* class_info) {
    if (!class_info) return;
    
    printf("Method table for %s:\n", class_info->name);
    Method* method = class_info->methods;
    while (method) {
        printf("  %s%s%s%s%s%s\n",
               method->name,
               method->is_static ? " [static]" : "",
               method->is_virtual ? " [virtual]" : "",
               method->is_abstract ? " [abstract]" : "",
               method->is_private ? " [private]" : "",
               method->is_protected ? " [protected]" : "");
        method = method->next;
    }
}

void object_print_field_table(Class* class_info) {
    if (!class_info) return;
    
    printf("Field table for %s:\n", class_info->name);
    Field* field = class_info->fields;
    while (field) {
        printf("  %s (type_id=%u, offset=%u, size=%u)%s%s%s%s\n",
               field->name,
               field->type_id,
               field->offset,
               field->size,
               field->is_static ? " [static]" : "",
               field->is_private ? " [private]" : "",
               field->is_protected ? " [protected]" : "",
               field->is_public ? " [public]" : "");
        field = field->next;
    }
}

// Type checking
bool type_is_assignable(Class* from, Class* to) {
    if (!from || !to) return false;
    if (from == to) return true;
    
    return type_is_subtype(from, to);
}

bool type_is_subtype(Class* subtype, Class* supertype) {
    if (!subtype || !supertype) return false;
    
    Class* current = subtype;
    while (current) {
        if (current == supertype) {
            return true;
        }
        current = current->superclass;
    }
    return false;
}

bool type_implements_interface(Class* class_info, Interface* interface) {
    if (!class_info || !interface) return false;
    
    for (uint32_t i = 0; i < class_info->interface_count; i++) {
        if (class_info->interfaces[i] == interface) {
            return true;
        }
    }
    
    // Check superclasses
    Class* current = class_info->superclass;
    while (current) {
        for (uint32_t i = 0; i < current->interface_count; i++) {
            if (current->interfaces[i] == interface) {
                return true;
            }
        }
        current = current->superclass;
    }
    
    return false;
}

// Object registry
void object_registry_init(void) {
    class_registry = NULL;
    next_type_id = 1;
}

void object_registry_cleanup(void) {
    Class* current = class_registry;
    while (current) {
        Class* next = current->next;
        // Only destroy if the class is still valid (not already destroyed)
        if (current->name) {
            class_destroy(current);
        }
        current = next;
    }
    class_registry = NULL;
}

void object_registry_clear(void) {
    class_registry = NULL;
}

void object_registry_register_class(Class* class_info) {
    class_register(class_info);
}

Class* object_registry_find_class(const char* name) {
    return class_find(name);
}

Class* object_registry_find_class_by_id(uint32_t type_id) {
    Class* current = class_registry;
    while (current) {
        if (current->type_id == type_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
