#include "bytecode_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// CONSTANT TABLE OPERATIONS
// ============================================================================

ConstantTable* constant_table_create(void) {
    ConstantTable* table = malloc(sizeof(ConstantTable));
    if (!table) return NULL;
    
    table->count = 0;
    table->entries = NULL;
    
    return table;
}

void constant_table_destroy(ConstantTable* table) {
    if (!table) return;
    
    if (table->entries) {
        free(table->entries);
    }
    
    free(table);
}

uint32_t constant_table_add_constant(ConstantTable* table, const ConstantEntry* entry) {
    if (!table || !entry) {
        printf("DEBUG: constant_table_add_constant failed - table=%p, entry=%p\n", table, entry);
        return 0;
    }
    
    // Resize entries array if needed
    ConstantEntry* new_entries = realloc(table->entries, 
                                       (table->count + 1) * sizeof(ConstantEntry));
    if (!new_entries) {
        printf("DEBUG: constant_table_add_constant failed - realloc failed\n");
        return 0;
    }
    
    table->entries = new_entries;
    
    // Add the new entry
    table->entries[table->count] = *entry;
    uint32_t index = table->count;
    table->count++;
    
    printf("DEBUG: constant_table_add_constant success - index=%u, type=%d\n", index, entry->type);
    return index;
}

const ConstantEntry* constant_table_get_constant(ConstantTable* table, uint32_t index) {
    if (!table || !table->entries || index >= table->count) {
        return NULL;
    }
    
    return &table->entries[index];
}

uint32_t constant_table_add_int64(ConstantTable* table, int64_t value) {
    if (!table) return 0;
    
    ConstantEntry entry;
    entry.type = CONSTANT_TYPE_INT64;
    entry.value.int_value = value;
    
    return constant_table_add_constant(table, &entry);
}

uint32_t constant_table_add_float64(ConstantTable* table, double value) {
    if (!table) return 0;
    
    ConstantEntry entry;
    entry.type = CONSTANT_TYPE_FLOAT64;
    entry.value.float_value = value;
    
    return constant_table_add_constant(table, &entry);
}

uint32_t constant_table_add_boolean(ConstantTable* table, bool value) {
    if (!table) return 0;
    
    ConstantEntry entry;
    entry.type = CONSTANT_TYPE_BOOLEAN;
    entry.value.bool_value = value;
    
    return constant_table_add_constant(table, &entry);
}

uint32_t constant_table_add_string(ConstantTable* table, uint32_t string_offset) {
    if (!table) return 0;
    
    ConstantEntry entry;
    entry.type = CONSTANT_TYPE_STRING;
    entry.value.string_offset = string_offset;
    
    return constant_table_add_constant(table, &entry);
}

uint32_t constant_table_add_null(ConstantTable* table) {
    if (!table) return 0;
    
    ConstantEntry entry;
    entry.type = CONSTANT_TYPE_NULL;
    entry.value.string_offset = 0; // Not used for null
    
    return constant_table_add_constant(table, &entry);
}

void constant_table_print(ConstantTable* table) {
    if (!table) {
        printf("ConstantTable: NULL\n");
        return;
    }
    
    printf("ConstantTable: %u entries\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        const ConstantEntry* entry = &table->entries[i];
        printf("  [%u] ", i);
        
        switch (entry->type) {
            case CONSTANT_TYPE_INT64:
                printf("INT64: %lld\n", (long long)entry->value.int_value);
                break;
            case CONSTANT_TYPE_FLOAT64:
                printf("FLOAT64: %g\n", entry->value.float_value);
                break;
            case CONSTANT_TYPE_BOOLEAN:
                printf("BOOLEAN: %s\n", entry->value.bool_value ? "true" : "false");
                break;
            case CONSTANT_TYPE_STRING:
                printf("STRING: offset %u\n", entry->value.string_offset);
                break;
            case CONSTANT_TYPE_NULL:
                printf("NULL\n");
                break;
            default:
                printf("UNKNOWN: type %d\n", entry->type);
                break;
        }
    }
}
