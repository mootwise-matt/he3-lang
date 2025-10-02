#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create execution context
ExecutionContext* execution_context_create(void) {
    ExecutionContext* context = malloc(sizeof(ExecutionContext));
    if (!context) return NULL;
    
    context->frames = NULL;
    context->frame_count = 0;
    context->frame_capacity = 0;
    context->current_frame = NULL;
    
    return context;
}

// Destroy execution context
void execution_context_destroy(ExecutionContext* context) {
    if (!context) return;
    
    // Destroy all frames
    while (context->frame_count > 0) {
        CallFrame* frame = execution_context_pop_frame(context);
        if (frame) {
            call_frame_destroy(frame);
        }
    }
    
    if (context->frames) {
        free(context->frames);
    }
    
    free(context);
}

// Create call frame
CallFrame* call_frame_create(uint8_t* ip, size_t local_count) {
    CallFrame* frame = malloc(sizeof(CallFrame));
    if (!frame) return NULL;
    
    frame->ip = ip;
    frame->local_count = local_count;
    frame->caller = NULL;
    frame->this_object = NULL;
    
    // Allocate local variables
    if (local_count > 0) {
        frame->locals = malloc(sizeof(Value) * local_count);
        if (!frame->locals) {
            free(frame);
            return NULL;
        }
        
        // Initialize all locals to null
        for (size_t i = 0; i < local_count; i++) {
            frame->locals[i] = value_create_null();
        }
    } else {
        frame->locals = NULL;
    }
    
    return frame;
}

// Destroy call frame
void call_frame_destroy(CallFrame* frame) {
    if (!frame) return;
    
    // Destroy local variables
    if (frame->locals) {
        for (size_t i = 0; i < frame->local_count; i++) {
            value_destroy(&frame->locals[i]);
        }
        free(frame->locals);
    }
    
    free(frame);
}

// Push frame onto execution context
bool execution_context_push_frame(ExecutionContext* context, CallFrame* frame) {
    if (!context || !frame) return false;
    
    // Grow frame array if needed
    if (context->frame_count >= context->frame_capacity) {
        size_t new_capacity = context->frame_capacity == 0 ? 8 : context->frame_capacity * 2;
        CallFrame** new_frames = realloc(context->frames, sizeof(CallFrame*) * new_capacity);
        if (!new_frames) return false;
        
        context->frames = new_frames;
        context->frame_capacity = new_capacity;
    }
    
    // Set caller relationship
    frame->caller = context->current_frame;
    
    // Add frame
    context->frames[context->frame_count] = frame;
    context->frame_count++;
    context->current_frame = frame;
    
    return true;
}

// Pop frame from execution context
CallFrame* execution_context_pop_frame(ExecutionContext* context) {
    if (!context || context->frame_count == 0) return NULL;
    
    CallFrame* frame = context->frames[context->frame_count - 1];
    context->frame_count--;
    
    // Update current frame
    if (context->frame_count > 0) {
        context->current_frame = context->frames[context->frame_count - 1];
    } else {
        context->current_frame = NULL;
    }
    
    return frame;
}

// Get current frame
CallFrame* execution_context_current_frame(ExecutionContext* context) {
    if (!context) return NULL;
    return context->current_frame;
}

// Set local variable
bool call_frame_set_local(CallFrame* frame, uint32_t index, Value value) {
    if (!frame || !frame->locals || index >= frame->local_count) {
        return false;
    }
    
    // Destroy old value
    value_destroy(&frame->locals[index]);
    
    // Set new value
    frame->locals[index] = value;
    
    return true;
}

// Get local variable
Value call_frame_get_local(CallFrame* frame, uint32_t index) {
    if (!frame || !frame->locals || index >= frame->local_count) {
        return value_create_null();
    }
    
    return frame->locals[index];
}

// Check if local variable exists
bool call_frame_has_local(CallFrame* frame, uint32_t index) {
    if (!frame || !frame->locals || index >= frame->local_count) {
        return false;
    }
    
    return true;
}

// Create call frame for method call
CallFrame* call_frame_create_for_method(uint8_t* ip, size_t local_count, size_t arg_count, Value* args, Object* this_object) {
    CallFrame* frame = call_frame_create(ip, local_count);
    if (!frame) return NULL;
    
    frame->this_object = this_object;
    
    // Copy arguments to local variables
    if (args && arg_count > 0) {
        size_t copy_count = arg_count < local_count ? arg_count : local_count;
        for (size_t i = 0; i < copy_count; i++) {
            call_frame_set_local(frame, i, args[i]);
        }
    }
    
    return frame;
}
