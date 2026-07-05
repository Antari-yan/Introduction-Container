# Container Image Specification
The `Open Container Initiative (OCI)` defined open standards for Container Images, which can be found [here](https://github.com/opencontainers/image-spec).

Key points:
  - An OCI image is essentially a `tar archive` of a root filesystem plus a `manifest` describing how to run it
  - Images usually consists of:
    - `Metadata`:
      - entrypoint
      - variables
      - work directory
      - ...
    - `Layers`:
      - Each layer represents filesystem changes (like adding a file or installing a package)
      - Layers are cached and reused between images, reducing storage and speeding up builds
    - `Manifests`:
      - Contain which layers make up the image
      - Multiple manifests can be contained within an image, each specifying e.g.:
        - For what `architecture` the image has been build for (amd64, arm64, ...)
        - For what `OS` the image has been build for (linux, windows, ...)

> [!IMPORTANT]  
> All layers are part of the final image,  
> so adding a file or package in one layer and removing it in the next doesn't reduce the final image size.  
> Only doing this in the same layer affects the final image size.
