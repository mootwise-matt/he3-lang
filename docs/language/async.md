# Async & Concurrency

## Async Functions
- Declared with `async`.
- Compiled to return `Task<T>`.

```he3
async function fetch(): string { return "data"; }
```

## Await
- Suspends execution until Task completes.
- Lowers to VM opcodes: `SPAWN`, `AWAIT`.

```he3
async function main(): integer {
  let v := await fetch();
  print(v);
  return 0;
}
```

## Scheduler
- Phase 1: single-threaded event loop.
- Phase 2+: thread pool + futures.
