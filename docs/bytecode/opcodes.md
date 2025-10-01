# Opcode Reference

## Stack Operations
- `PUSH_I64 <i64>` — push integer.
- `PUSH_BOOL <u8>` — push boolean.
- `PUSH_STR <id>` — push string ref.

## Locals & Args
- `LOAD_LOCAL <idx>`
- `STORE_LOCAL <idx>`
- `LOAD_ARG <idx>`

## Arithmetic
- `ADD`, `SUB`, `MUL`, `DIV`

## Control Flow
- `JMP <offset>`
- `JMPF <offset>`

## Calls
- `CALL <methodId>` — static call.
- `CALLV <slot>` — virtual dispatch.

## Object Ops
- `NEW <typeId>`
- `BOX <typeId>`
- `UNBOX <typeId>`

## Async
- `SPAWN <methodId>`
- `AWAIT`

## Security
- `SECURE_CALL <methodId>` — call method with security validation
- `HANDSHAKE <keyId>` — initiate secure module handshake
- `VALIDATE_TOKEN <tokenId>` — validate session token
- `REVOKE_TOKEN <tokenId>` — revoke session token

## Events
- `PUBLISH_EVENT <eventId>` — publish event data
- `SUBSCRIBE_EVENT <eventId>` — subscribe to event
- `UNSUBSCRIBE_EVENT <subscriptionId>` — unsubscribe from event
- `CREATE_EVENT <eventName>` — create new event
