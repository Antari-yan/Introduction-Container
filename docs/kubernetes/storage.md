# Storage
A `PersistentVolumeClaim` (PVC) requests storage without caring where it comes from.  
A `StorageClass` provisions a matching `PersistentVolume` (PV) automatically, in `k3s` the [local-path](https://github.com/rancher/local-path-provisioner) provisioner is available by default.  

The valkey example runs as a `StatefulSet` ([valkey-statefulset.yaml](../../kubernetes-files/valkey-statefulset.yaml)) with a `volumeClaimTemplate`, so each replica gets its own PVC and persists its data to it:
```sh
kubectl get pvc
kubectl get pv
```

The data survives a restart of the `pod`:
```sh
# Create an entry in valkey, delete the pod than get the entry
kubectl exec valkey-0 -- sh -c 'valkey-cli -a "$VALKEY_PASSWORD" --no-auth-warning set demo persisted >/dev/null'
kubectl delete pod valkey-0
kubectl wait --for=condition=ready pod valkey-0 --timeout=60s
kubectl exec valkey-0 -- sh -c 'valkey-cli -a "$VALKEY_PASSWORD" --no-auth-warning get demo'
```

> [!IMPORTANT]  
> The `local-path` provisioner stores data on a single node, just like a `named volume` in [Docker Swarm](../docker-swarm/docker-swarm-data-persistence.md).  
> If a `pod` is rescheduled to a different node the data does not follow.  
> For multi-node persistence use network storage (`NFS`, `S3`) or a cluster-aware `StorageClass` (e.g. [Longhorn](https://longhorn.io)).

### Container Storage Interface (CSI)
The `Container Storage Interface (CSI)` is the standard that lets storage backends plug into Kubernetes.  

A CSI driver runs as `pods` in the cluster and consists of:
  - a `controller plugin` that handles volume lifecycle (create, delete, snapshot, ...)
  - a `node plugin` (a `DaemonSet`) that mounts the volumes on each node
  - it registers a provisioner name (e.g. `nfs.csi.k8s.io`) that a `StorageClass` then references

The `local-path` provisioner above is one such driver, real backends use their own, e.g.:
  - cloud block storage: `ebs.csi.aws.com`, `pd.csi.storage.gke.io`, `disk.csi.azure.com`
  - cross-node `ReadWriteMany`: [csi-driver-nfs](https://github.com/kubernetes-csi/csi-driver-nfs), [Rook/Ceph](https://rook.io), [Longhorn](https://longhorn.io)

As an example the [NFS CSI driver](https://github.com/kubernetes-csi/csi-driver-nfs) solves the multi-node limitation,
because an NFS share can be mounted by `pods` on any node (`ReadWriteMany`).  
Install the driver (pin to a current [release](https://github.com/kubernetes-csi/csi-driver-nfs/releases)):
```sh
helm repo add csi-driver-nfs https://raw.githubusercontent.com/kubernetes-csi/csi-driver-nfs/master/charts
helm repo update
helm install csi-driver-nfs csi-driver-nfs/csi-driver-nfs --namespace kube-system --version v4.11.0
kubectl wait pod --selector app.kubernetes.io/name=csi-driver-nfs --for condition=ready -n kube-system
```

Create a `StorageClass` pointing at the NFS server (`nfsvers` should match the server, see the [runtime NFS examples](../fundamentals/data-persistence.md#nfs-mount-example)):
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: nfs-csi
provisioner: nfs.csi.k8s.io
parameters:
  server: <nfs-server-ip>
  share: <path-to-nfs-share>
reclaimPolicy: Delete
volumeBindingMode: Immediate
mountOptions:
  - hard
  - nfsvers=4.1
```

A `PVC` then provisions a volume dynamically by just naming the class, no manual `PV` needed:
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: shared-data
spec:
  accessModes:
    - ReadWriteMany   # multiple pods across nodes can mount it
  storageClassName: nfs-csi
  resources:
    requests:
      storage: 1Gi
```

> [!NOTE]  
> The NFS driver only connects to an existing NFS server, it does not run one.  
> The nodes need an NFS client (e.g. `nfs-common`) installed and network access to the server.
