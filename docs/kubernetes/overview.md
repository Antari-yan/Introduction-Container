# Kubernetes
Kubernetes (k8s) is the de facto industry standard for clustering/orchestration,
because it has much more capabilities compared to `Docker Swarm` (though more complex as the trade off).
  - Originated at Google as Borg, later open-sourced.
  - Highly extensible, modular and powerful, but more complex than Swarm.
  - Follows an [N-2 support policy](https://endoflife.date/kubernetes): the 3 most recent minor versions receive security and bug fixes.  
    With a `~4 Month` release cycle each release is supported for approximately `14 months`.
  - Supports:
    - Declarative YAML manifests
    - Self-healing workloads
    - Advanced networking, storage, and scaling
    - Operator for automated deployment and lifecycle managing
    - Huge ecosystem ([CNCF landscape](https://landscape.cncf.io))
      - [Helm](https://github.com/helm/helm) to manage Kubernetes applications
      - [ArtifactHUB](https://artifacthub.io) as central marketplace for `Helm` templates
      - [OperatorHub](https://operatorhub.io) as central marketplace for operator

Lightweight distributions variants:
  - [k3s](https://github.com/k3s-io/k3s) - Minimal single binary Kubernetes, great for edge, IoT, CI and developement.
  - [rke2](https://github.com/rancher/rke2) - Hardened k8s based single binary Kubernetes distribution.
  - [k3d](https://github.com/k3d-io/k3d) - Run k3s inside Docker/Podman, great for development and testing.


The modularity and extensiblity of Kubernetes adds a high level of complexity to maintenace and developement.  
With a 6 month lifecycle it would be foolish to do things manually.  
There are many tools like the [System Upgrade Controller](https://github.com/rancher/system-upgrade-controller)
that help automatizing things, but they also add complexity and need to be maintained.

The lightweight k8s distibutions take a way a lot of complexity,
but it still requires a lot of preparation work to get a well functioning system with minimal maintenance overhead.
