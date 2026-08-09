# Other workload types
Besides `Deployments` and `StatefulSet`, Kubernetes has controllers for other workload shapes:
  - `Job`: Runs `pods` until a task completes successfully (batch work).
  - `CronJob`: Creates `Jobs` on a schedule.
  - `DaemonSet`: Runs one `pod` on every (or selected) node, e.g. for log or metrics agents.

[jobs.yaml](../../kubernetes/manifests/jobs.yaml) contains a one-off `Job` that tests the running app and a `CronJob` that hits it every five minutes:
```sh
kubectl get jobs
kubectl logs job/example-web-app-test

kubectl get cronjob example-web-app-heartbeat

kubectl create job --from=cronjob/example-web-app-heartbeat heartbeat-now # trigger the CronJob manually
kubectl logs jobs/heartbeat-now
```
