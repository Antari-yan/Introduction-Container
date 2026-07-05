# Get DockerHub pull rate limit
This is how to retreive the poull rate limit for DockerHub (requires curl, grep and optionally jq):

Get a token without authentication:
```sh
# Without jq:
TOKEN=$(curl -s "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```
```sh
# With jq:
TOKEN=$(curl -s "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | jq -r .token)
```
```sh
# With wget:
TOKEN=$(wget -qO- "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```

Get token with authentication:
```sh
# Without jq:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```
```sh
# With jq:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | jq -r .token)
```
```sh
# With wget:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```

Get rate limit:
```sh
curl -s --head -H "Authorization: Bearer $TOKEN" https://registry-1.docker.io/v2/ratelimitpreview/test/manifests/latest
```
```sh
# With wget:
wget --server-response --spider --header="Authorization: Bearer $TOKEN" https://registry-1.docker.io/v2/ratelimitpreview/test/manifests/latest
```
