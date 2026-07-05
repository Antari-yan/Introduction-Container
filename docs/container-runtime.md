# Container Runtime
Under the hood, containers are made possible by `container runtimes`, which is specified by the OCI [here](https://github.com/opencontainers/runtime-spec).
These are the low-level components responsible for creating and running containers on the host operating system.

Most commonly used are:
  - [runc](https://github.com/opencontainers/runc)
    - A low-level container runtime that actually starts and runs containers, by directly interacting with the Linux kernel.
      - handling the low-level operations like namespace isolation, cgroups management, and file system mounts.
        - Namespaces: Provide isolation for processes, networking, and file systems.
          runc uses Linux namespaces (such as PID, UTS, IPC, MNT, and NET) to ensure that containers are isolated from each other and from the host system.
        - Cgroups (Control Groups): Manage resource allocation and limitation.
          runc leverages cgroups to control the amount of CPU, memory, I/O, and other resources that a container can use.
        - Capabilities: Allow fine-grained control over what processes inside the container can do.
          runc sets Linux capabilities to restrict the privileges of containerized processes, enhancing security.
        - Seccomp (Secure Computing Mode): Limits the system calls that a containerized process can execute, reducing the attack surface.
          runc applies seccomp profiles to containers, allowing only a subset of safe system calls.
        - Filesystem Mounts: runc handles setting up the container's filesystem, including root filesystem and
          additional mounts (such as volumes or bind mounts) as defined in the container's configuration.
      - doesn't include higher-level container management features like image distribution or networking orchestration,
        which are instead handled by higher-level tools like Docker or Kubernetes.
    - Originally developed in Go as part of Docker, later donated to the Open Container Initiative (OCI).
    - Implements the OCI runtime specification, ensuring interoperability across tools.
  - [containerd](https://github.com/containerd/containerd)
    - A higher-level runtime that manages the complete lifecycle of containers (pulling images, starting/stopping containers, managing storage and networking).
    - Used internally by Docker and (sometimes) Kubernetes.
  - [CRI-O](https://github.com/cri-o/cri-o)
    - A lightweight container runtime built specifically for Kubernetes.
    - Implements the Container Runtime Interface (CRI).

How it fits together:
  - Docker -> calls `containerd` -> which then uses `runc` to create and manage the actual container process.
  - Podman is using the `libpod` library to directly interact with `runc`, `crun`, or other OCI compliant container runtimes
  - This layered design means different container tools can share the same underlying runtime, thanks to OCI standards.


## Docker
- Released in 2013, Docker was the first tool to make containers accessible to developers.
- Provides a CLI (docker) and relies on a daemon process (`dockerd`) that runs in the background to manage containers.
- Widely adopted in industry; most tutorials and documentation still use Docker commands.
- Strong ecosystem: Docker Hub (public image registry), Docker Compose (multi-container applications).
- The Docker Engine, is a client-server application that includes:
  - Docker Daemon (`dockerd`): Runs in the background and manages Docker objects like containers, images, networks, and volumes.
    It listens for Docker API requests and is responsible for building, running, and distributing Docker containers.
  - Docker Client (`docker`): A command-line interface (CLI) tool that allows users to interact with the Docker Daemon.
    When you type commands like docker run or docker build, the client sends these commands to the Docker Daemon, which then carries them out.
  - `Containerd`
  - `runc`
  - Networking Components: Docker installs components to set up and manage container networks.
    This includes the Docker network bridge (docker0) and tools for setting up networking options like overlay networks for multi-host container networking.
  - Docker CLI Plugins: Additional command-line tools that extend the Docker CLI capabilities 

Pros
  - Most popular and widely supported container tool.
  - Large ecosystem of documentation, images, and community resources.
  - Easy to get started.

Cons
  - Requires a privileged background daemon (dockerd).
  - Containers often run as root by default, which can raise security concerns.
  - Orchestration tool (Swarm) is minimally maintained compared to Kubernetes.

> [!IMPORTANT]  
> Anything like `Images`, `Container`, `Volumes`, etc. are stored locally in `/var/lib/docker`.


## Podman
- Introduced by Red Hat, Podman is a container engine designed as a drop-in replacement for Docker.
- Daemonless architecture: Podman does not rely on a background service.
- Supports rootless mode, allowing containers to run without administrator privileges - a significant security improvement.
- Integrates tightly with systemd, the init system used by most modern Linux distributions, making it suitable for managing long-running services.
- Uses the concept of `pods` like kubernetes
  - A `pod` is a group of one or more containers, with shared storage and network resources, and a specification for how to run the containers.

Pros
  - Secure by design (rootless, daemonless).
  - Docker-compatible CLI (podman run ≈ docker run).
  - Supported by enterprise Linux distributions (RHEL, Fedora).
  - Can run Kubernetes files (Helm charts need to be converted first with the `helm template` command)

Cons
  - Less common in tutorials and examples compared to Docker.

> [!IMPORTANT]  
> Anything like `Images`, `Container`, `Volumes`, etc. are stored locally in `~/.local/share/containers`.  
> Configuration can also be located in `/etc/containers/`.
