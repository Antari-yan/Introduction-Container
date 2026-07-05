# What is a Container?
At its core, a container is a way to `package software` together with everything it needs to run and then execute it in an `isolated environment` on a host system.


## Definition
A container is:
  - A `lightweight`, `standalone`, `executable unit` of software.
  - It contains:
    - the application code
    - libraries and dependencies (optional)
    - system tools (optional)
    - runtime (e.g., Python interpreter, Java runtime) (optional)
  - Runs in `isolation` from other containers and from the host system, while still sharing the host's operating system kernel.

This means that if your software runs inside a container on your laptop, the same container will run the same way on a server, a colleague's workstation, or in the cloud.


## Analogy: Shipping Containers
The best way to think about containers is to compare them with shipping containers in logistics:
  - Before shipping containers, transporting goods was chaotic - each product needed special handling.
  - Shipping containers standardized the process: goods are packed into a box that can be moved by any ship, train, or truck.
  - Similarly, a software container “packs” code and dependencies into a standard format that can run anywhere a container engine exists.
  - Just like physical containers can be stacked and shipped efficiently, software containers can be deployed, scaled, and moved around easily.


## Key Concepts
To understand containers, we need to clarify some essential building blocks:
  - Image
      - A container image is a blueprint or template.
      - It is read-only and describes the filesystem and environment your application needs.
      - Example: An image might include Ubuntu, Python 3.11, and your machine learning code.
  - Container
      - A container is a running instance of an image.
      - Multiple containers can be created from the same image.
      - Containers can be short-lived (e.g., a data processing task) or long-running (e.g., a web server).
  - Registry
      - A registry is a storage and distribution system for container images.
      - Public: Docker Hub, GitHub Container Registry.
      - Private: self-hosted registries for labs or enterprises.
