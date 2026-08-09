# Helm
[Helm](https://github.com/helm/helm) is an additional commonly used tool that serves as Kubernetes Package Manager.
```sh
curl -fsSL https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-4 | bash

sudo sh -c "helm completion bash > /etc/bash_completion.d/helm-completion"
source /etc/bash_completion.d/helm-completion

helm version

export KUBECONFIG=~/.kube/config
helm list -A
```

A package or `Helm Chart` bundles templated kubernetes manifests with `values` to fill in variables, comparable to a `.env` file for a compose deployment.  
A chart is a directory with a fixed layout:
```
mychart/
  Chart.yaml          # metadata: name, version (the chart's own), appVersion (the app it installs)
  values.yaml         # default values, the user-facing configuration surface
  templates/          # the templated manifests, rendered into Kubernetes objects
    deployment.yaml
    service.yaml
    _helpers.tpl      # reusable named template snippets (filenames starting with _ are not rendered as objects)
    NOTES.txt         # usage hint printed after install
  charts/             # vendored subchart dependencies (e.g. a database the app needs)
  .helmignore         # files to exclude when packaging
```
Templates are [Go templates](https://pkg.go.dev/text/template). Values and built-in objects are referenced with `{{ ... }}`:
  - `{{ .Values.x }}`: a value from `values.yaml` (or an override), the main mechanism for configuration.
  - `{{ .Release.Name }}` / `{{ .Release.Namespace }}`: the name/namespace chosen at install time.
  - `{{ .Chart.Name }}` / `{{ .Chart.Version }}`: data from `Chart.yaml`.
  - functions and pipelines like `{{ .Values.name | default "web" | quote }}`, plus `if`/`range` for conditionals and loops.

A minimal chart can be found in [helm-files](../../helm-files).  
Its [templates/deployment.yaml](../../helm-files/templates/deployment.yaml) pulls the image, replica count and name from [values.yaml](../../helm-files/values.yaml) via `{{ .Values.* }}` and
uses `{{ .Release.Name }}` for the object name.

Render the templates locally to see what would be applied (no cluster needed), optionally overriding values:
```sh
helm template webserver ./helm-files
helm template webserver ./helm-files --set replicaCount=3 # override a single value
helm template webserver ./helm-files -f my-values.yaml    # override with a values file
```

Install, upgrade and remove a release (a release is one installed instance of a chart):
```sh
helm install webserver ./helm-files
helm list
kubectl get pods -l app=webserver

helm upgrade webserver ./helm-files --set replicaCount=3  # change values on an existing release
helm uninstall webserver
```

Inspect an installed release (Helm stores its state as a `Secret` in the cluster):
```sh
helm status webserver        # status and the rendered NOTES.txt
helm get values webserver    # the values this release was installed with
helm get manifest webserver  # the manifests Helm actually applied
helm history webserver       # revision history (helm rollback webserver <revision> to revert)
```

> [!NOTE]  
> The chart sets a `hostPort`, which in `k3d` binds the port on the node container, not on the host.  
> To reach it use `kubectl port-forward deployment/webserver 8081:80` or add a `Service`/`Ingress` as explained later.

Additionally it is possible to create `HelmChartConfig` objects (`k3s`/`rke2` specific) for providing additional overwrite values.  
An example used later can be found in [kubernetes/bootstrap/traefik-gateway-config.yaml](../../kubernetes/bootstrap/traefik-gateway-config.yaml).


### Installing charts from a repository or an OCI registry
Charts are usually not built locally but pulled from a remote source.  
Historically this was a `Helm repository` (an HTTP server with an `index.yaml`).  
Since `Helm 3.8` charts can also be stored as `OCI artifacts` in the same registries that hold container images,
which is increasingly the default way to distribute them (unified auth, signing and storage, no separate index).

As a ready-made example [podinfo](https://github.com/stefanprodan/podinfo) is used, a small web application made for Kubernetes demos.

From a classic `Helm repository`:
```sh
helm repo add podinfo https://stefanprodan.github.io/podinfo
helm repo update                       # refresh the cached index of all added repos
helm repo list                         # list the configured repositories
helm search repo podinfo               # list charts in the added repos matching "podinfo"
helm search repo podinfo/ --versions   # all available versions of the chart
helm install podinfo podinfo/podinfo --version 6.14.0
```

From an `OCI registry` (no `helm repo add` needed, the URL points straight at the chart):
```sh
helm install podinfo oci://ghcr.io/stefanprodan/charts/podinfo --version 6.14.0
```

Inspect a remote chart before installing, and check available values:
```sh
helm show chart oci://ghcr.io/stefanprodan/charts/podinfo --version 6.14.0
helm show values oci://ghcr.io/stefanprodan/charts/podinfo --version 6.14.0
```

Reach the app and remove it again:
```sh
kubectl port-forward deployment/podinfo 9898:9898
# In another shell: curl http://localhost:9898
helm uninstall podinfo
```

> [!IMPORTANT]  
> Pin the chart version with `--version` for the same reasons as [pinning image versions](../container-image-building/recommendations.md#version-pinning).  
> ArtifactHUB shows the current version: <https://artifacthub.io/packages/helm/podinfo/podinfo>

> [!TIP]  
> Charts in an `OCI registry` can be signed and verified, just like container images.  
> The podinfo charts are signed with `cosign`, see the [signing section](../container-image-building/recommendations.md#sign-the-container-images) for the concept:  
> `cosign verify ghcr.io/stefanprodan/charts/podinfo:6.14.0 --certificate-oidc-issuer=https://token.actions.githubusercontent.com --certificate-identity-regexp='^https://github.com/stefanprodan/.*$'`
