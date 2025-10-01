# Type System (Merged)

## Built-in Types
- `System.Object` — root of all types.
- `System.Int64` — 64-bit integer.
- `System.Float64` — 64-bit floating point.
- `System.Boolean` — true/false.
- `System.String` — UTF-8 immutable string.
- `System.Void` — for functions returning nothing.
- `System.Task<T>` — async result type.
- `System.Option<T>` — Some/None.
- `System.Result<T,E>` — Ok/Err.

## Security Types
- `System.Security.SessionToken` — authentication token for secure modules.
- `System.Security.SecurityError` — security-related error enumeration.
- `System.Security.SecurityException` — exception for security violations.
- `System.Security.SecurityContext` — security validation context.

## Event Types
- `System.Events.Event<T>` — generic event interface.
- `System.Events.EventSubscription` — event subscription handle.
- `System.Events.EventManager` — VM event management.
- `System.Events.SecureEvent<T>` — security-enabled events.

## Value vs Reference Types
- **Reference types**: classes, strings, arrays — live on heap, header includes typeId and vtable pointer.
- **Value types**: records, primitives — stored inline on stack/parent objects; can be boxed to heap.

## Nullable Types
- `T?` is sugar for `Option<T>`.
- `null` literal only valid for `T?`.
