# RBAC and ServiceAccounts
`Role Based Access Control (RBAC)` decides who may do what.  
Permissions are additive (deny by default) and granted by binding a `Role` to an identity:
  - `Role` / `ClusterRole`: A set of permissions (verbs on resources). `Role` is namespaced, `ClusterRole` is cluster-wide.
  - `RoleBinding` / `ClusterRoleBinding`: Grants a `Role`/`ClusterRole` to a subject (a user, group or `ServiceAccount`).
  - `ServiceAccount`: The identity a `pod` runs as. If none is set, the namespace's `default` `ServiceAccount` is used.

The web app runs under a dedicated `ServiceAccount` with read-only access to `pods` ([example-web-app-rbac.yaml](../../kubernetes/manifests/example-web-app-rbac.yaml)),
which its `/pods` endpoint uses to call the Kubernetes API:
```sh
# The app lists its own namespace's pods through the API, using its ServiceAccount
kubectl exec deploy/example-web-app -- wget -qO- http://localhost:8080/pods

# The same permissions checked directly with "auth can-i"
kubectl auth can-i list pods   --as=system:serviceaccount:example-web-app:example-web-app # yes
kubectl auth can-i delete pods --as=system:serviceaccount:example-web-app:example-web-app # no
```

> [!IMPORTANT]  
> Give each workload its own `ServiceAccount` with the least privileges it needs, never the `default` one with broad rights.  
> Set `automountServiceAccountToken: false` on `pods` that do not talk to the Kubernetes API, so a compromised container cannot use the token.
