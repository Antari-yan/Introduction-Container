# Cleanup
Remove the application, the add-ons and the cluster:
```sh
# Stop GitOps reconciliation first, so Argo CD does not recreate what is deleted next
kubectl delete -f kubernetes-bootstrap/argocd-application.yaml --ignore-not-found

# The whole application lives in one namespace, deleting it removes everything (incl. the Secret)
kubectl delete namespace example-web-app --ignore-not-found

# cluster-scoped cert-manager issuers and the Traefik Gateway override
kubectl delete -f kubernetes-bootstrap/cert-manager-issuers.yaml --ignore-not-found
kubectl delete -f kubernetes-bootstrap/traefik-gateway-config.yaml --ignore-not-found
helm uninstall cert-manager -n cert-manager
kubectl delete namespace argocd cert-manager --ignore-not-found

# reset the context namespace and delete the cluster
kubectl config set-context --current --namespace=default
k3d cluster delete introduction
```
