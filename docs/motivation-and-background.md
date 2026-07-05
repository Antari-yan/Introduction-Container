# Motivation & Background

## Traditional deployment challenges
Before containers were invented, developers and IT teams struggled with complex software deployment challenges.  
Applications were tightly coupled with specific operating system configurations and software dependencies,
which could lead to compatibility issues, dependency conflicts, and time-consuming setup processes.
Phrases like `It works on my machine` weren't always just jokes and running multiple applications on the same system could lead to clashes between libraries or
runtime environments (e.g., two applications requiring different versions of Python or Java).  
Slight differences in operating system, dependencies, or configurations could lead to inconsistent or non-reproducible results.

While virtual machines (VMs) are great to isolate environments they come with at a cost: each VM requires its own operating system, making them large and slow to boot.
Running many VMs wastes system resources and complicate scaling.


## Virtual Machines vs. Containers
- Virtual Machines (VMs):
  - Each VM runs a full operating system, including its own kernel.
  - They are heavier, consuming more CPU, RAM, and disk space.
  - Startup times are relatively slow (tens of seconds to minutes).
  - Provide strong isolation, useful for running completely different OSes.
  - While scaling is possible, setting up a whole VM usually takes at least a couple of minutes.
- Containers:
  - Share the host operating system kernel but isolate processes, filesystems, and networking.
    - The main reason why sharing the kernel can be shared, is because of it's highly stable APIs with good backwards compatibility
  - They are lightweight, often just a few megabytes or less.
  - Containers can start in seconds or less, because the host kernel is already available.
  - Simple at scaling up and down.
  - Can run basically anywhere - on a laptop, a server, the cloud or on a RaspberryPi.
  - Container don't have `systemd` (the Linux system and service manager)
    - Because often the privileges are missing
    - `systemd` expects full control of `PID 1` and `cgroups`, but containers use `PID 1` by default
    - This makes it non-trivial to run `systemd` inside of a container


## Background
### Linux Namespaces
Linux namespaces provide isolation for system resources,
meaning a process or a group of processes can operate in its own `namespace`,
unaware of other processes using the same resources in different namespaces.  
Namespaces isolate processes, allowing them to have their own view of system resources like process IDs (PIDs), file systems, network interfaces, user IDs, and more.

Each type of namespace provides isolation for a specific system resource:
  - PID namespace: Isolates the process IDs, meaning processes in one namespace can't see or affect processes in another namespace.
  - Mount namespace: Isolates the file system mount points.
  - Network namespace: Isolates network interfaces, IP addresses, routing tables, etc.
  - UTS namespace: Isolates hostname and domain name.
  - IPC namespace: Isolates inter-process communication resources such as shared memory and message queues.
  - User namespace: Isolates user and group IDs, allowing a process to have different user IDs inside and outside the namespace.
  - Cgroup namespace: Isolates control group (cgroup) resources, which are used to manage resource allocation (CPU, memory, etc.).

Namespaces are used to provide isolation for containers. Each container is placed in its own namespaces,
giving the impression of a separate machine with its own processes, network interfaces, and file system.

### Linux Containers (LXC)
- LXC (Linux Containers), introduced around 2008, was the first user-friendly toolset to manage the `namespace` kernel features.
- It allowed running multiple isolated Linux systems (lightweight VMs) on a single host.
- While powerful, LXC was considered low-level and lacked a standardized way to distribute, version, and manage applications.

### Docker
- Launched in 2013 by the `dotCloud` company (later renamed to `Docker, Inc.`), Docker popularized containers by making them easy to build, share, and run.
- Initially utilized LXC and later switched to its own runtime (libcontainer, later runc).
- Prior to Docker, Linux had features like cgroups and namespaces that enabled containers, but they were difficult to use.  
  Docker wrapped these capabilities in a simple developer-friendly interface.
- Docker's key innovation was the `Docker Image` and `Dockerfile`, which streamlined the process of defining environments and distributing them via `Docker Hub`.
- 2019 `Docker, Inc.` sold its enterprise business to `Mirantis`
- Today, Docker still exists as the developer-facing tool, but its role in orchestration has been eclipsed by Kubernetes.
- Uses the moby project as the upstream for the Docker Product: https://github.com/moby/moby
  - Moby is an open-source project created by Docker, containing:
    - container build tools
    - a container registry
    - orchestration tools
    - a runtime
    - and more

### Docker Swarm
- Docker initially pushed `Docker Swarm` (launched in 2014) as its orchestration solution.
- Swarm is simpler to use than Kubernetes, but lacked the capabilities, flexibility, and community backing of Kubernetes.
- By 2017-2018, `Kubernetes` was more widely used, and `Docker Swarm` fell into minimal maintenance mode, but it is still available.
- Because of the minimal maintenance there are many bugs and missing configuration options that are otherwise available for a standalone Docker
  (e.g., `depends_on`, `deploy.mode: replicated-job`, ...).

### Kubernetes (k8s)
- At `Google`, developers needed to manage containers at massive scale.
- Google built an internal system called `Borg` in the early 2000s to run millions of containers across thousands of machines.
- In 2014, Google released `Kubernetes` (inspired by Borg) as an open-source orchestration system.
- Kubernetes quickly became the de facto industry standard for container orchestration due to:
  - Extensibility (Custom Resources, Operators)
  - Backing by the [Cloud Native Computing Foundation (CNCF)](https://www.cncf.io/).
  - Strong community support
- Kubernetes effectively replaced Docker Swarm in most serious production environments.
- The smallest manageable unit in Kubernetes is a `pod`, which is a group of one or more container
  (checkout [Sharing namespaces between Containers](fundamentals/sharing-namespaces-between-containers.md) for how `pods` work).

### The Open Container Initiative (OCI)
- In 2015, with multiple runtimes and image formats emerging, the industry needed standardization.
- The `Open Container Initiative (OCI)` was founded by Docker, CoreOS, and others under the Linux Foundation.
- OCI maintains open standards for:
  - `Image Format` (how container images are packaged)
  - `Container Registries` (how container images are distributed)
  - `Runtime Specification` (how containers are executed on a host)
- Thanks to the OCI, tools like Docker, Kubernetes, etc. interoperate on a common foundation. This prevents fragmentation of the ecosystem.

### Podman
- Developed by `Red Hat` as an open-source daemonless and rootless alternative to Docker.
- Unlike Docker, Podman doesn't require a background service (`dockerd`) and can run entirely without root privileges, improving security.
- Podman intentionally mimics Docker's CLI, so commands like `podman run` are nearly identical to `docker run`.
- It's popular in enterprise and security-sensitive contexts and integrates tightly with `systemd`.
- Also uses the concept of `pods` like Kubernetes

### Windows and Apple
Both Windows and Apple have developed their own approaches for container and container runtime.

There is [Windows Containers](https://github.com/microsoft/Windows-Containers), which are basically Windows based container, instead of Linux based.  
So far they one work with Azure and on Windows hosts.

Apple has [container](https://github.com/apple/container), which is basically their own version of a container runtime for Mac, that uses virtual machines to run container.

This repo won't go into detail of either, because they are more like niche products with highly specific restrictions to use them.
