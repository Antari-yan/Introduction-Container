# ConfigMaps and Secrets
A ConfigMap` holds non-confidential configuration, a `Secret` holds sensitive data (only `base64` encoded, not encrypted).  
Both can be injected as environment variables or mounted as files.

The web app reads its `GREETING` from [example-web-app-configmap.yaml](../../kubernetes/manifests/example-web-app-configmap.yaml) (injected with `envFrom`) and
its `VALKEY_PASSWORD` from the `valkey-credentials` `Secret` (injected with `secretKeyRef`).

Change the greeting and roll it out:
```sh
kubectl edit configmap example-web-app-config       # change the GREETING value
kubectl rollout restart deployment/example-web-app  # pods pick up the new value on restart
curl http://localhost:8080                          # (with the port-forward running)
```

A `Secret` is created the same way the `valkey-credentials` one was during the application deployment:
```sh
# From files
echo "$(openssl rand -base64 24)" > data/valkey-password
kubectl create secret generic valkey-credentials --from-file=password=data/valkey-password

# Or from literals
kubectl create secret generic valkey-credentials --from-literal=password="$(openssl rand -base64 24)"
```

> [!IMPORTANT]  
> `--from-file` keeps the file content verbatim, including a trailing newline, which sometimes can lead to failing authentication.  

> [!NOTE]  
> A `Secret` is stored `base64` encoded, not encrypted, so some additional steps can be useful:
>   - Enable [encryption at rest](https://kubernetes.io/docs/tasks/administer-cluster/encrypt-data/) on the cluster (a one-time cluster setting).
>   - Use the [External Secrets Operator](https://github.com/external-secrets/external-secrets) to pull secrets from an external store like [HashiCorp Vault](https://github.com/hashicorp/vault).
