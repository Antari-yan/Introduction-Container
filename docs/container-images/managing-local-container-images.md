# Managing local Container Images
List all locally available `Container Images`:
```sh
$CR image ls
```

Remove a `Container Image`:
```sh
$CR image rm alpine:3
```

Remove all unused (dangling) `Container Images`:
```sh
$CR image prune
# $CR image prune -a -f
```

Export a `Container Image` to an archive:
```sh
$CR image save alpine:3 -o alpine3.tar.gz
```

Import a `Container Image` to an archive:
```sh
$CR load < alpine3.tar.gz
```
```sh
$CR load --input alpine3.tar.gz
```
