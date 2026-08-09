# Managing many clusters and GitOps
A major strength of Kubernetes over plain Docker is that its declarative API scales to whole fleets of clusters and applications, managed the same way as any other object.

Cluster lifecycle (provisioning and managing the clusters themselves):
  - [Cluster API (CAPI)](https://github.com/kubernetes-sigs/cluster-api): Manages clusters as Kubernetes objects (`Cluster`, `Machine`, ...) from a central management cluster,
    provisioning downstream clusters on many infrastructures.  
    It is itself a set of operators.
    And through [`ClusterResourceSet`](https://cluster-api.sigs.k8s.io/tasks/experimental-features/cluster-resource-set) objects it is possible to
    automatically apply a set of manifests (selected by cluster labels) to newly created clusters, e.g. to install a CNI or base add-ons so a fresh cluster comes up ready to use.

GitOps (Git as the single source of truth, an agent continuously reconciles the cluster to match the repository):
  - [Fleet](https://fleet.rancher.io)
  - [Argo CD](https://github.com/argoproj/argo-cd)
  - [Flux CD](https://github.com/fluxcd/flux2)

> [!NOTE]  
> It is possible to also manage Cluster via GitOps for a fully automated infrastructure management.


### GitOps with Argo CD
Install Argo CD (`--server-side` is required because the CRDs are large):
```sh
kubectl create namespace argocd
kubectl apply -n argocd --server-side --force-conflicts \
  -f https://raw.githubusercontent.com/argoproj/argo-cd/v3.4.4/manifests/install.yaml
kubectl wait --for=condition=available deployment --all -n argocd --timeout=180s
```

Point it at this repository with [kubernetes/bootstrap/argocd-application.yaml](../../kubernetes/bootstrap/argocd-application.yaml).  
It syncs the top level of the `kubernetes/manifest` directory from the `main` branch into the `example-web-app` namespace,
so Argo CD deploys the entire application and keeps it in sync with Git on its own:
```sh
kubectl apply -f kubernetes/bootstrap/argocd-application.yaml
kubectl get application -n argocd example-web-app   # SYNC STATUS becomes Synced, HEALTH Healthy
kubectl get pods -n example-web-app                 # example-web-app and valkey, now reconciled by Argo CD
```

Because the sync policy is automated with `selfHeal`, manual drift is reverted to the Git state:
```sh
kubectl delete service example-web-app -n example-web-app
# Argo CD notices the drift and recreates the Service from Git
kubectl get service example-web-app -n example-web-app --watch
```

Open the UI (the initial admin password is stored in a `Secret`):
```sh
kubectl -n argocd get secret argocd-initial-admin-secret -o jsonpath='{.data.password}' | base64 -d
kubectl port-forward svc/argocd-server -n argocd 8444:443
# Open https://localhost:8444 and log in as "admin"
```

> [!NOTE]  
> Argo CD only manages what is in Git, e.g. the `valkey-credentials` `Secret` is created outside Git,
> so a different approach like using the [External Secrets Operator](https://github.com/external-secrets/external-secrets would be recommended.
