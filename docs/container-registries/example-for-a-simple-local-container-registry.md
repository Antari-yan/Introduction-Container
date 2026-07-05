# Example for a simple local Container Registry
https://distribution.github.io/distribution/

Setup requirements:
```sh
# Create directories
mkdir -p data/registry/{auth,data,certs}

# Create certificate for localhost
openssl req -x509 -new -nodes \
        -newkey rsa:4096 \
        -keyout data/registry/certs/server.key \
        -sha512 \
        -days 90 \
        -subj "/C=''/ST=''/L=''/O=''/OU=''/CN=localhost/emailAddress=''" \
        -out data/registry/certs/server.pem \
        -reqexts SAN \
        -config <(cat /etc/ssl/openssl.cnf <(printf "[SAN]\nsubjectAltName= DNS:localhost"))

# Create basic authentication credentials
$CR run \
  --entrypoint htpasswd \
  docker.io/httpd:2-alpine -Bbn testuser testpassword > data/registry/auth/htpasswd
```

> [!NOTE]  
> While `self-signed certificates` are fine for internal and development setups,
> in production setups proper valid certificates should be used.  
> One of the most well-known certificate provider would be [Let's Encrypt](https://letsencrypt.org/).

Add the certificate to trust store:
  - Podman:
    ```sh
    sudo mkdir -p /etc/containers/certs.d/localhost:5000
    sudo cp data/registry/certs/server.pem /etc/containers/certs.d/localhost:5000/ca.crt
    ```
  - Docker:
    ```sh
    sudo mkdir -p /etc/docker/certs.d/localhost:5000
    sudo cp data/registry/certs/server.pem /etc/docker/certs.d/localhost:5000/ca.crt
    ```

Running a simple registry locally:
```sh
$CR run -d \
  -p 5000:5000 \
  --restart=always \
  --name registry \
  -v "$(pwd)"/data/registry/certs:/certs:ro \
  -e REGISTRY_HTTP_TLS_CERTIFICATE=/certs/server.pem \
  -e REGISTRY_HTTP_TLS_KEY=/certs/server.key \
  -v "$(pwd)"/data/registry/auth:/auth:ro \
  -e "REGISTRY_AUTH=htpasswd" \
  -e "REGISTRY_AUTH_HTPASSWD_REALM=Registry Realm" \
  -e REGISTRY_AUTH_HTPASSWD_PATH=/auth/htpasswd \
  -v "$(pwd)"/data/registry/data:/var/lib/registry \
  registry:3

# $(pwd) -> current directory
```

Log in to the registry:
```sh
$CR login localhost:5000

# User: testuser
# Pass: testpassword
```

Pull an image:
```sh
$CR image pull alpine:3
```

Tag existing image to point to the local registry:
```sh
$CR image tag alpine:3 localhost:5000/myfirstimage:3
```

Upload (push) the image:
```sh
$CR image push localhost:5000/myfirstimage:3
```

Remove the locally stored image:
```sh
$CR image rm alpine:3
$CR image rm localhost:5000/myfirstimage:3
```

Download (pull) the images:
```sh
$CR image pull localhost:5000/myfirstimage:3
```

Stop the Registry:
```sh
$CR logout localhost:5000
$CR container stop registry
$CR container rm registry
```
