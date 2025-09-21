from valkey import Valkey

valkey = Valkey(host='valkey', port=6379, db=0)

def app(environ, start_response):
    if environ['PATH_INFO'] == '/':
        count = valkey.incr('hits')
        body = f"Hello World! I have been seen {count} times.\n"
        status = '200 OK'
        headers = [('Content-Type', 'text/plain')]
        start_response(status, headers)
        return [body.encode('utf-8')]
    else:
        status = '404 Not Found'
        body = "Not Found\n"
        headers = [('Content-Type', 'text/plain')]
        start_response(status, headers)
        return [body.encode('utf-8')]