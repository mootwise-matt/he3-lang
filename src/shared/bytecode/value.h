#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// HE³ VALUE SYSTEM
// ============================================================================
// This header defines the unified value system used throughout the He³
// virtual machine. All values are represented as Value structures that
// can hold different types of data.

// ============================================================================
// VALUE TYPES
// ============================================================================

typedef enum {
    VALUE_NULL = 0,         // Null value
    VALUE_BOOLEAN,          // Boolean value (true/false)
    VALUE_INT8,             // 8-bit signed integer
    VALUE_INT16,            // 16-bit signed integer
    VALUE_INT32,            // 32-bit signed integer
    VALUE_INT64,            // 64-bit signed integer
    VALUE_UINT8,            // 8-bit unsigned integer
    VALUE_UINT16,           // 16-bit unsigned integer
    VALUE_UINT32,           // 32-bit unsigned integer
    VALUE_UINT64,           // 64-bit unsigned integer
    VALUE_FLOAT32,          // 32-bit floating point
    VALUE_FLOAT64,          // 64-bit floating point
    VALUE_STRING,           // String value
    VALUE_OBJECT,           // Object reference
    VALUE_ARRAY,            // Array reference
    VALUE_FUNCTION,         // Function reference
    VALUE_TYPE,             // Type reference
    VALUE_OPTION,           // Option<T> value
    VALUE_RESULT,           // Result<T,E> value
    VALUE_TASK,             // Task<T> value
    VALUE_EVENT,            // Event reference
    VALUE_INTERFACE         // Interface reference
} ValueType;

// ============================================================================
// VALUE STRUCTURE
// ============================================================================

// Forward declaration for Object
typedef struct Object Object;

// Main value structure
typedef struct {
    ValueType type;         // Value type
    union {
        // Primitive values
        bool boolean_value;
        int8_t int8_value;
        int16_t int16_value;
        int32_t int32_value;
        int64_t int64_value;
        uint8_t uint8_value;
        uint16_t uint16_value;
        uint32_t uint32_value;
        uint64_t uint64_value;
        float float32_value;
        double float64_value;
        
        // Reference values
        char* string_value;
        Object* object_value;
        struct Value* array_value;
        void* function_value;
        void* type_value;
        struct Value* option_value;
        struct Value* result_value;
        void* task_value;
        void* event_value;
        void* interface_value;
        
        // Raw data for complex types
        uint8_t raw_data[8];
    };
} Value;

// ============================================================================
// VALUE CREATION
// ============================================================================

// Create null value
Value value_create_null(void);

// Create boolean value
Value value_create_boolean(bool value);

// Create integer values
Value value_create_int8(int8_t value);
Value value_create_int16(int16_t value);
Value value_create_int32(int32_t value);
Value value_create_int64(int64_t value);
Value value_create_uint8(uint8_t value);
Value value_create_uint16(uint16_t value);
Value value_create_uint32(uint32_t value);
Value value_create_uint64(uint64_t value);

// Create floating point values
Value value_create_float32(float value);
Value value_create_float64(double value);

// Create string value
Value value_create_string(const char* value);
Value value_create_string_copy(const char* value);

// Create object value
Value value_create_object(Object* value);

// Create array value
Value value_create_array(struct Value* array, size_t length);

// Create function value
Value value_create_function(void* function);

// Create type value
Value value_create_type(void* type);

// Create Option values
Value value_create_option_some(const Value* value);
Value value_create_option_none(void);

// Create Result values
Value value_create_result_ok(const Value* value);
Value value_create_result_err(const Value* error);

// ============================================================================
// VALUE DESTRUCTION
// ============================================================================

// Destroy value (free any allocated memory)
void value_destroy(Value* value);

// Destroy value array
void value_destroy_array(Value* values, size_t count);

// ============================================================================
// VALUE ACCESS
// ============================================================================

// Get value type
ValueType value_get_type(const Value* value);

// Check if value is null
bool value_is_null(const Value* value);

// Check if value is truthy
bool value_is_truthy(const Value* value);

// Check if value is falsy
bool value_is_falsy(const Value* value);

// ============================================================================
// VALUE CONVERSION
// ============================================================================

// Convert value to boolean
bool value_to_boolean(const Value* value);

// Convert value to integer
int64_t value_to_int64(const Value* value);
int32_t value_to_int32(const Value* value);
int16_t value_to_int16(const Value* value);
int8_t value_to_int8(const Value* value);

// Convert value to unsigned integer
uint64_t value_to_uint64(const Value* value);
uint32_t value_to_uint32(const Value* value);
uint16_t value_to_uint16(const Value* value);
uint8_t value_to_uint8(const Value* value);

// Convert value to floating point
double value_to_float64(const Value* value);
float value_to_float32(const Value* value);

// Convert value to string
const char* value_to_string(const Value* value);
char* value_to_string_copy(const Value* value);

// Convert value to object
Object* value_to_object(const Value* value);

// ============================================================================
// VALUE COMPARISON
// ============================================================================

// Compare two values for equality
bool value_equals(const Value* a, const Value* b);

// Compare two values for ordering
int value_compare(const Value* a, const Value* b);

// Check if values are identical (same reference)
bool value_identical(const Value* a, const Value* b);

// ============================================================================
// VALUE ARITHMETIC
// ============================================================================

// Add two values
Value value_add(const Value* a, const Value* b);

// Subtract two values
Value value_subtract(const Value* a, const Value* b);

// Multiply two values
Value value_multiply(const Value* a, const Value* b);

// Divide two values
Value value_divide(const Value* a, const Value* b);

// Modulo two values
Value value_modulo(const Value* a, const Value* b);

// Negate value
Value value_negate(const Value* value);

// ============================================================================
// VALUE LOGICAL OPERATIONS
// ============================================================================

// Logical AND
Value value_and(const Value* a, const Value* b);

// Logical OR
Value value_or(const Value* a, const Value* b);

// Logical NOT
Value value_not(const Value* value);

// ============================================================================
// VALUE STRING OPERATIONS
// ============================================================================

// Concatenate strings
Value value_string_concat(const Value* a, const Value* b);

// Get string length
size_t value_string_length(const Value* value);

// Get string substring
Value value_string_substring(const Value* value, size_t start, size_t length);

// Find string index
int value_string_index_of(const Value* value, const Value* substring);

// ============================================================================
// VALUE ARRAY OPERATIONS
// ============================================================================

// Get array length
size_t value_array_length(const Value* value);

// Get array element
Value value_array_get(const Value* value, size_t index);

// Set array element
void value_array_set(Value* value, size_t index, const Value* element);

// Append to array
void value_array_append(Value* value, const Value* element);

// Remove from array
void value_array_remove(Value* value, size_t index);

// ============================================================================
// VALUE TYPE CHECKING
// ============================================================================

// Check if value is of specific type
bool value_is_type(const Value* value, ValueType type);

// Check if value is numeric
bool value_is_numeric(const Value* value);

// Check if value is integer
bool value_is_integer(const Value* value);

// Check if value is floating point
bool value_is_float(const Value* value);

// Check if value is string
bool value_is_string(const Value* value);

// Check if value is object
bool value_is_object(const Value* value);

// Check if value is array
bool value_is_array(const Value* value);

// Check if value is function
bool value_is_function(const Value* value);

// Check if value is Option
bool value_is_option(const Value* value);

// Check if value is Result
bool value_is_result(const Value* value);

// Option operations
bool value_option_is_some(const Value* value);
bool value_option_is_none(const Value* value);
Value value_option_unwrap(const Value* value);
Value value_option_unwrap_or(const Value* value, const Value* default_value);

// Result operations
bool value_result_is_ok(const Value* value);
bool value_result_is_err(const Value* value);
Value value_result_unwrap(const Value* value);
Value value_result_unwrap_or(const Value* value, const Value* default_value);
Value value_result_unwrap_err(const Value* value);

// ============================================================================
// VALUE COPYING
// ============================================================================

// Copy value (shallow copy)
Value value_copy(const Value* value);

// Deep copy value
Value value_deep_copy(const Value* value);

// Copy value array
Value* value_copy_array(const Value* values, size_t count);

// ============================================================================
// VALUE PRINTING
// ============================================================================

// Print value to string
char* value_to_string_debug(const Value* value);

// Print value to stdout
void value_print(const Value* value);

// Print value with newline
void value_println(const Value* value);

// ============================================================================
// VALUE HASHING
// ============================================================================

// Calculate hash of value
uint32_t value_hash(const Value* value);

// Calculate hash of value array
uint32_t value_hash_array(const Value* values, size_t count);

// ============================================================================
// VALUE VALIDATION
// ============================================================================

// Validate value
bool value_validate(const Value* value);

// Validate value array
bool value_validate_array(const Value* values, size_t count);

// ============================================================================
// VALUE CONSTANTS
// ============================================================================

// Predefined constant values
extern const Value VALUE_NULL_CONST;
extern const Value VALUE_TRUE_CONST;
extern const Value VALUE_FALSE_CONST;
extern const Value VALUE_ZERO_CONST;
extern const Value VALUE_ONE_CONST;
extern const Value VALUE_EMPTY_STRING_CONST;

// ============================================================================
// VALUE UTILITIES
// ============================================================================

// Get value size in bytes
size_t value_get_size(const Value* value);

// Get value alignment
size_t value_get_alignment(const Value* value);

// Check if value is constant
bool value_is_constant(const Value* value);

// Check if value is mutable
bool value_is_mutable(const Value* value);

// ============================================================================
// VALUE STACK OPERATIONS
// ============================================================================

// Push value onto stack
void value_stack_push(Value* stack, size_t* stack_top, const Value* value);

// Pop value from stack
Value value_stack_pop(Value* stack, size_t* stack_top);

// Peek at top of stack
Value value_stack_peek(const Value* stack, size_t stack_top);

// Check if stack is empty
bool value_stack_is_empty(size_t stack_top);

// Get stack size
size_t value_stack_size(size_t stack_top);

// ============================================================================
// VALUE MEMORY MANAGEMENT
// ============================================================================

// Allocate value array
Value* value_allocate_array(size_t count);

// Reallocate value array
Value* value_reallocate_array(Value* array, size_t old_count, size_t new_count);

// Free value array
void value_free_array(Value* array, size_t count);

// ============================================================================
// VALUE SERIALIZATION
// ============================================================================

// Serialize value to bytes
uint8_t* value_serialize(const Value* value, size_t* size);

// Deserialize value from bytes
Value value_deserialize(const uint8_t* data, size_t size);

// ============================================================================
// VALUE DEBUGGING
// ============================================================================

// Get value debug information
void value_get_debug_info(const Value* value, char* buffer, size_t buffer_size);

// Print value debug information
void value_print_debug_info(const Value* value);

// ============================================================================
// VALUE TYPE CONVERSION HELPERS
// ============================================================================

// Convert between integer types
Value value_convert_int(const Value* value, ValueType target_type);

// Convert between float types
Value value_convert_float(const Value* value, ValueType target_type);

// Convert to string representation
Value value_convert_to_string(const Value* value);

// Convert from string representation
Value value_convert_from_string(const char* string, ValueType target_type);
