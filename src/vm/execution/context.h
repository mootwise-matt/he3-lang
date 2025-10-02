#pragma once

#include "../vm.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Execution context management

// Create execution context
ExecutionContext* execution_context_create(void);
void execution_context_destroy(ExecutionContext* context);

// Call frame management
CallFrame* call_frame_create(uint8_t* ip, size_t local_count);
void call_frame_destroy(CallFrame* frame);

// Frame stack operations
bool execution_context_push_frame(ExecutionContext* context, CallFrame* frame);
CallFrame* execution_context_pop_frame(ExecutionContext* context);
CallFrame* execution_context_current_frame(ExecutionContext* context);

// Local variable operations
bool call_frame_set_local(CallFrame* frame, uint32_t index, Value value);
Value call_frame_get_local(CallFrame* frame, uint32_t index);
bool call_frame_has_local(CallFrame* frame, uint32_t index);

// Method call support
CallFrame* call_frame_create_for_method(uint8_t* ip, size_t local_count, size_t arg_count, Value* args, Object* this_object);
