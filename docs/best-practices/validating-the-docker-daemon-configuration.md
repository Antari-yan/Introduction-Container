# Validating the Docker daemon configuration
Before restarting `dockerd` after editing `/etc/docker/daemon.json`, validate the config:
```bash
dockerd --validate
```
