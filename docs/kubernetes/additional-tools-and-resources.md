# Additional Tools and Resources
Kubernetes is a small core with a large ecosystem built on top of its API.  
A few notable, production-grade tools worth knowing about (all extend the same declarative model shown above):
  - [Rancher](https://github.com/rancher/rancher): A management platform witha web UI to provision, centrally manage and secure many clusters and deployments from one place.  
    [Harvester](https://github.com/rancher/harvester-cloud) can be used as hyperconverged infrastructure counterpart, running VMs and Kubernetes on bare metal.
  - [Longhorn](https://github.com/longhorn/longhorn): Distributed, replicated block storage that solves the single-node limitation of `local-path` without an external SAN.
  - [Linkerd](https://github.com/linkerd/linkerd2) / [Istio](https://github.com/istio/istio):
    Service meshes that add mTLS, retries, traffic splitting and detailed telemetry between services, transparently e.g. via sidecars.
  - [KubeVirt](https://github.com/kubevirt/kubevirt): Runs traditional virtual machines as Kubernetes objects, side by side with containers, which can be useful for workloads that cannot be containerized.
  - [Kubespray](https://github.com/kubernetes-sigs/kubespray) / [kubeadm](https://github.com/kubernetes/kubeadm):
    Tooling for provision Kubernetes clusters on a host.
  - [Prometheus](https://github.com/prometheus/prometheus) and [Grafana](https://github.com/grafana/grafana): The common stack for metrics and dashboards.
  - [Velero](https://github.com/velero-io/velero): Backup and restore (and migration) of cluster resources and persistent volumes.
  - [External Secrets Operator](https://github.com/external-secrets/external-secrets): Syncs `Secrets` from an external store (HashiCorp Vault, AWS/GCP/Azure secret managers, ...) into a cluster.
  - [Kyverno](https://github.com/kyverno/kyverno) / [OPA Gatekeeper](https://github.com/open-policy-agent/gatekeeper):
    Policy engines to validate and enforce rules on objects (admission control), e.g. "every image must come from our registry".

> [!NOTE]  
> The [CNCF Landscape](https://landscape.cncf.io) catalogs the much larger set of cloud-native projects by category.
