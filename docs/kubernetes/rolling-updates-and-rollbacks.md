# Rolling updates and rollbacks
A `Deployment` updates `pods` gradually, keeping the application available during the change.

Change the image and follow the rollout:
```sh
kubectl set image deployment/example-web-app example-web-app=example-web-app:v2
kubectl rollout status deployment/example-web-app
kubectl rollout history deployment/example-web-app
```

If something breaks, roll back to the previous revision:
```sh
kubectl rollout undo deployment/example-web-app
```
