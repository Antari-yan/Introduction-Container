# Architecture
A Kubernetes cluster consists of one or more `control plane` nodes and one or more `worker` nodes.  
The `control plane` makes the global decisions, the `worker` nodes run the actual workloads.

Control plane components:
  - `kube-apiserver`: The front end of the cluster, every interaction (e.g. `kubectl`) goes through this API.
  - `etcd`: A key-value store holding the complete cluster state and.
  - `kube-scheduler`: Decides on which node a new `pod` should run.
  - `kube-controller-manager`: Runs controllers that reconcile the actual state towards the desired state.

Worker node components:
  - `kubelet`: The agent on each node that starts and supervises `pods`.
  - `kube-proxy`: Maintains the network rules for `Service` communication.
  - `container runtime`: The OCI runtime (e.g. `containerd`) that actually runs the container.

> [!NOTE]  
> Kubernetes only talks to a `container runtime` via the [Container Runtime Interface (CRI)](https://kubernetes.io/docs/concepts/architecture/cri/).  
> Docker itself is not CRI compliant, which is why support for it (`dockershim`) was removed in `v1.24`.  
> `k3s` (and therefore `k3d`) uses `containerd` internally regardless of whether the host uses Docker or Podman.

While it is technically possible to run a `control plane` only cluster
(by removing/ignoring the default `NoSchedule` taint so that workloads run on the control plane nodes), it is not recommended.  
A misbehaving workload can starve control plane components like `etcd` and `kube-apiserver` and
degrade the whole cluster, and it runs right next to the most sensitive data (`etcd`, certificates, credentials).  
`k3s` is an exception that schedules on its server nodes by default, which suits its lightweight and local use case.  
Valid exceptions are local or test clusters and dedicated management clusters
(see [Managing many clusters and GitOps](managing-many-clusters-and-gitops.md)) that only run trusted tooling, never tenant workloads.
