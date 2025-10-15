import glob
import hashlib
import json
import os
import shutil
import struct

def list_hash(floats, id_length=4):
    def truncate(f, p):
        factor = 10 ** p
        # truncation, not rounding
        return int(f * factor) / factor

    precision = 6
    data = b''.join(struct.pack('>d', truncate(f, precision)) for f in floats)
    digest = hashlib.sha256(data).digest().hex()
    return digest[:id_length]
    
def retrieve_gradient_data(fname):
    with open(fname) as f:
        d = json.load(f)

    colors = []
    for v in d["value"]:
        colors += [v["position"]]
        colors += [c for c in v["color"]]
    
    return colors
        
if __name__ == "__main__":
    path = "Hesiod/data/color_gradients"
    id_length = 6

    for fname in glob.glob(path + "/*.json"):
        raw = retrieve_gradient_data(fname)
        new_fname = os.path.join(path, list_hash(raw, id_length) + ".json")
        print(fname, " -> ", new_fname)
        shutil.move(fname, new_fname)
