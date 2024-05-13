import os
import time
import random
import falcon

waitTime = int(os.environ.get('WAIT_TIME', '2'))

class RandomGenerator(object):
    def on_get(self, request, response):
        time.sleep(waitTime)
        number = random.randint(0, 100)
        result = {'Min': 0, 'Max': 100, 'Num': number}
        response.media = result


api = falcon.App()
api.add_route('/number', RandomGenerator())