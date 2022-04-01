import sys
import json
import collections

# Change this it True if you want to format it to compare versions.
pretty = False

with open(sys.argv[1], "r") as f:
    config = json.load(f)

print(config.keys())

config["middlewares"].sort(key=lambda x: x["identifier"])
config["drivers"].sort(key=lambda x: x["identifier"])

new_root = collections.OrderedDict()
for key in ['jsonForm', 'formatVersion', 'board', 'identifier', 'name', 'details', 'application', 'middlewares', 'drivers', 'pads']:
    new_root[key] = config[key]

with open(sys.argv[1], "w") as f:
    indent = 2
    separators = (',', ': ')
    sort_keys = True
    if not pretty:
        indent = None
        separators = (',', ':')
        sort_keys = False
    json.dump(new_root, f,
              indent=indent, separators=separators, sort_keys=sort_keys)
