# Inspecting a Container Image
Inspect detailed information about a `Container Image`, like the digest, the OS and architecture it is build for,
it's default configuration (env vars, start command, etc.), Labels, Annotations and more.
```sh
$CR image inspect alpine:3
```

Show the layer history of a `Container Image`, which essentially includes the steps done to build it.
```sh
$CR image history alpine:3
```

Inspect the manifests of a `Container Image`:
```sh
$CR manifest inspect alpine:3
```
Like this Docker doesn't show the annotations and other details for each manifest.
It only does so when either using `-v` or by specifically inspecting a manifest by using the `digest` instead of a `tag`.
