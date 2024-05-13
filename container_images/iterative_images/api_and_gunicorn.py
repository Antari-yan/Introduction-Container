import os
import time
import random
import falcon
from gunicorn.app.base import BaseApplication

waitTime = int(os.environ.get('WAIT_TIME', '2'))

class RandomGenerator(object):
  def on_get(self, request, response):
    time.sleep(waitTime)
    number = random.randint(0, 100)
    result = {'Min': 0, 'Max': 100, 'Num': number}
    response.media = result

class StandaloneRandomNumberAPI(BaseApplication):
  def __init__(self, app, options=None):
    self.options = options or {}
    self.application = app
    super().__init__()
  def load_config(self):
    config = {key: value for key, value in self.options.items()
              if key in self.cfg.settings and value is not None}
    for key, value in config.items():
        self.cfg.set(key.lower(), value)
  def load(self):
    return self.application

api = falcon.App()
api.add_route('/number', RandomGenerator())


if __name__ == "__main__":
  options = {
    'bind': '%s:%s' % ('0.0.0.0', '8080'),
    'workers': 4,
    }
  StandaloneRandomNumberAPI(api, options).run()