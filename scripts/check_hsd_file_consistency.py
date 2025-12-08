import json
import os
import sys
from deepdiff import DeepDiff

if __name__ == "__main__":
    fnames = sys.argv[1:]
    # print(fnames)

    for fname in fnames:
        # load data
        with open(fname, "r") as f:
            data = json.load(f)

        print(fname)

        # --- rebuild a dict for model

        dic_model = {}

        for k, v in data["graph_manager"]["graph_nodes"].items():
            dic_model["links"] = v["links"]

            dic_model["nodes"] = []
            for vn in v["nodes"]:
                dic_model["nodes"].append({
                    "id": vn["id"],
                    "label": vn["label"]
                })

        # --- rebuild a dict for UI

        dic_ui = {}

        for k, v in data["graph_tabs_widget"]["graph_node_widgets"].items():

            dic_ui["links"] = []
            for vl in v["links"]:
                dic_ui["links"].append({
                    "node_id_from": vl["node_out_id"],
                    "node_id_to": vl["node_in_id"],
                    "port_id_from": vl["port_out_id"],
                    "port_id_to": vl["port_in_id"]
                })

            dic_ui["nodes"] = []
            for vn in v["nodes"]:
                dic_ui["nodes"].append({
                    "id": vn["id"],
                    "label": vn["caption"]
                })

        # --- diff

        diff = DeepDiff(dic_model, dic_ui, ignore_order=True)

        if not diff:
            print(" - {}: ok".format(fname))
        else:
            print(" - {}: NOK".format(fname))
            print("   DIFF: {}".format(diff))
