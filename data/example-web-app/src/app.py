import json
import os
import ssl
import urllib.request

from valkey import Valkey

# Allow configuration overwrite via environment variables
GREETING = os.environ.get('GREETING', 'Hello from Docker')
VALKEY_HOST = os.environ.get('VALKEY_HOST', 'valkey')
VALKEY_PORT = int(os.environ.get('VALKEY_PORT', '6379'))
VALKEY_PASSWORD = os.environ.get('VALKEY_PASSWORD') or None

valkey = Valkey(host=VALKEY_HOST, port=VALKEY_PORT, password=VALKEY_PASSWORD, db=0)

SERVICE_ACCOUNT = '/var/run/secrets/kubernetes.io/serviceaccount'


def respond(start_response, status, body, content_type='text/plain'):
    data = body.encode('utf-8')
    start_response(status, [('Content-Type', content_type), ('Content-Length', str(len(data)))])
    return [data]


def list_pods():
    # Calls the Kubernetes API with the pod's ServiceAccount token (requires RBAC permission to list pods).
    with open(f'{SERVICE_ACCOUNT}/namespace') as file:
        namespace = file.read().strip()
    with open(f'{SERVICE_ACCOUNT}/token') as file:
        token = file.read().strip()
    context = ssl.create_default_context(cafile=f'{SERVICE_ACCOUNT}/ca.crt')
    url = f'https://kubernetes.default.svc/api/v1/namespaces/{namespace}/pods'
    request = urllib.request.Request(url, headers={'Authorization': f'Bearer {token}'})
    with urllib.request.urlopen(request, context=context, timeout=5) as response:
        payload = json.load(response)
    return [item['metadata']['name'] for item in payload['items']]


def app(environ, start_response):
    path = environ.get('PATH_INFO', '/')

    # Liveness: only reports that the process itself is up.
    if path == '/healthz':
        return respond(start_response, '200 OK', 'ok\n')

    # Readiness: only ready when valkey can be reached.
    if path == '/readyz':
        try:
            valkey.ping()
        except Exception:
            return respond(start_response, '503 Service Unavailable', 'valkey unavailable\n')
        return respond(start_response, '200 OK', 'ready\n')

    # Lists the pods in the namespace, demonstrating RBAC via the ServiceAccount.
    if path == '/pods':
        try:
            pods = list_pods()
        except Exception as error:
            return respond(start_response, '500 Internal Server Error', f'could not list pods: {error}\n')
        return respond(start_response, '200 OK', ''.join(f'{name}\n' for name in pods))

    # Main page: counts visits in valkey and shows the configed greeting.
    if path == '/':
        try:
            count = valkey.incr('hits')
        except Exception:
            return respond(start_response, '503 Service Unavailable', 'valkey unavailable\n')
        body = (
            '<!doctype html>\n'
            f'<title>{GREETING}</title>\n'
            f'<h1>{GREETING}</h1>\n'
            f'<p>This page has been viewed {count} times.</p>\n'
        )
        return respond(start_response, '200 OK', body, 'text/html')

    return respond(start_response, '404 Not Found', 'Not Found\n')
