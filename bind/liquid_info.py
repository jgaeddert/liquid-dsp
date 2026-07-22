#!/usr/bin/env python3
'''print information about package'''
import liquid as dsp
import json

print(json.dumps(dsp.build_info(),indent=2))

