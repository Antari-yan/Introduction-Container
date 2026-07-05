# Setup
All examples are provided for `Podman` and the `Docker Engine`(Docker CE).  
One of them should be installed, which can be done through their installation instructions:
- [Install Podman](https://podman.io/docs/installation)
- [Install Docker](https://docs.docker.com/engine/install/)

> [!IMPORTANT]  
> Ubuntu 22.04 uses podman version 3.4.4 which contains a bug for the network CNI plugin:  
> https://bugs.launchpad.net/ubuntu/+source/libpod/+bug/2024394  
> A workaround is to change the `cniVersion` for a network from `1.0.0` to `0.4.0`  
> The network config can be found in `~/.config/cni/net.d/<networkname>.conflist`
> Upgrading podman is possible but a bit tricky: https://github.com/containers/podman/discussions/25582  
> A general recommendation would be to use a newer distribution.

While it won't be covered, both also have a UI tool:
  - [Podman Desktop](https://podman-desktop.io/docs/installation) (No License required)
  - [Docker Desktop](https://www.docker.com/products/docker-desktop/) (Requires License for commercial use)
  - [VSCode Extension](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-containers)


## Optional Post-installation steps
### Docker - add user to the `docker` group
After installing Docker, run the following to avoid needing `sudo` for every command:
```bash
sudo usermod -aG docker $USER
newgrp docker
```
> [!NOTE]
> `newgrp docker` activates the group in the current shell without logging out.
> On the next login the group is active automatically.

### Podman - enable the user socket (for API / compose compatibility)
```bash
systemctl --user enable --now podman.socket
```
This is required for tools that speak the Docker socket API (e.g. some compose plugins or the `docker` CLI pointed at Podman).
