---
layout: page
title: errors
---
# Error Handling

## Option<T>
```he3
function head(xs: List<Integer>): Option<Integer> {
  if (xs.length == 0) return None;
  return Some(xs[0]);
}
```

## Result<T,E>
```he3
function parseInt(s: string): Result<Integer, string> {
  if (s == "42") return Ok(42);
  else return Err("bad number");
}
```

## Match
```he3
match (parseInt("42")) {
  Ok(n) => print("ok " + n.toString()),
  Err(e) => print("error " + e)
}
```
