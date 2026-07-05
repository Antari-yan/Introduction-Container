# Kubernetes Objects
Everything in Kubernetes is a declarative object stored in the `etcd`, described in `YAML` Manifests and applied to the cluster.  
The most common ones are:
  - `Pod`: The smallest deployable unit, a group of one or more container sharing network and storage
    (see [Sharing namespaces between Containers](../fundamentals/sharing-namespaces-between-containers.md)).
  - `ReplicaSet`: Keeps a defined number of identical `pods` running.
  - `Deployment`: Manages `ReplicaSets` and enables rolling updates and rollbacks (the usual way to run stateless applications).
  - `StatefulSet`: Like a `Deployment` but with stable network identities and storage (e.g. for databases).
  - `DaemonSet`: Runs one `pod` on every (or selected) node (e.g. for log collectors).
  - `Job`/`CronJob`: Run a `pod` to completion, once or on a schedule.
  - `Service`: A stable virtual IP and DNS name load balancing across a set of `pods`.
  - `Ingress`: Routes external HTTP(S) traffic to `Services`.
  - `ConfigMap`/`Secret`: Provide configuration and credentials to `pods`.
  - `Namespace`: Mechanism for isolating groups of resources within a single cluster (e.g. Deployments, Services, etc.).
    Names of resources need to be unique within a namespace, but not across namespaces.
    There are some exceptions of objects that are available cluster-wide (e.g. StorageClass, Nodes, PersistentVolumes, etc.).
  - `PersistentVolume`(PV)/`PersistentVolumeClaim`(PVC): Storage (`volume`) definitions (like amount and access mode) for statefull applications.

> [!NOTE]  
> Beyond the built-in objects, Kubernetes can be extended with `Custom Resource Definitions (CRDs)`.  
> `CRDs` are often used together with so called `Operator` (a controller acting on a CRD) which enable complex applications like databases to be managed automatically (from deployment management to upgrading to backup/restore).
