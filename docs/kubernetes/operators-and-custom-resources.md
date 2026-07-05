# Operators and Custom Resources
A `CustomResourceDefinition (CRD)` adds a new object type to the Kubernetes API (the [Gateway API](services-and-networking.md#gateway-api) above is an example).  
An `Operator` is a custom controller watching such objects.  
It encodes the operational knowledge a human operator would otherwise apply (deploy, configure, back up, upgrade, fail over)
into the same reconcile-to-desired-state loop the built-in controllers use.

This is how complex and stateful applications are run on Kubernetes, e.g.:
  - [cert-manager](https://github.com/cert-manager/cert-manager): Issues and renews TLS certificates from a `Certificate` object.
  - [Prometheus Operator](https://github.com/prometheus-operator/prometheus-operator): Runs monitoring from `Prometheus` and `ServiceMonitor` objects.
  - [CloudNativePG](https://github.com/cloudnative-pg/cloudnative-pg): Runs and manages `PostgreSQL cluster`.

Operators are usually installed with Helm or the [Operator Lifecycle Manager (OLM)](https://github.com/operator-framework/operator-lifecycle-manager), [OperatorHub](https://operatorhub.io) lists available ones.  
The workflow is always the same: install the operator, then declare a high-level object and let it reconcile reality to match.  

### Self-signed certificates with cert-manager
[cert-manager](https://github.com/cert-manager/cert-manager) is an operator that issues and automatically renews TLS certificates from `Certificate` objects.  
It supports public certificates via [ACME](https://cert-manager.io/docs/configuration/acme/) (e.g. Let's Encrypt) and internal ones via a self-signed CA.

Install cert-manager with its CRDs (also available via `OCI` at `oci://quay.io/jetstack/charts/cert-manager`):
```sh
helm repo add jetstack https://charts.jetstack.io
helm repo update
helm install cert-manager jetstack/cert-manager \
  --namespace cert-manager --create-namespace \
  --set crds.enabled=true
kubectl wait --for=condition=available deployment --all -n cert-manager --timeout=120s
```

Create a self-signed CA and Cluster Issuer from [kubernetes-bootstrap/cert-manager-issuers.yaml](../../kubernetes-bootstrap/cert-manager-issuers.yaml):
```sh
kubectl apply -f kubernetes-bootstrap/cert-manager-issuers.yaml
kubectl get clusterissuer
```

The web app's `Ingress` already requests its certificate by annotation, t.  
The most common way to use cert-manager is by using the `cert-manager.io/cluster-issuer` annotation, which lets cert-manager's `ingress-shim` create and
renew the `Certificate` automatically from the `Ingress` `tls` block:
```yaml
metadata:
  annotations:
    cert-manager.io/cluster-issuer: ca  # ingress-shim issues the cert for the tls hosts
spec:
  tls:
    - hosts:
        - example-web-app.k3d.localhost
      secretName: example-web-app-tls   # cert-manager fills this Secret
```

With cert-manager now installed, the certificate the `Ingress` requested is issued on its own:
```sh
kubectl get certificate
kubectl get secret example-web-app-tls

# Reach the app over HTTPS through the Ingress, verifying against the CA
kubectl get secret root-ca -n cert-manager -o jsonpath='{.data.tls\.crt}' | base64 -d > data/root-ca.crt
curl --cacert data/root-ca.crt --resolve example-web-app.k3d.localhost:8443:127.0.0.1 https://example-web-app.k3d.localhost:8443
```
