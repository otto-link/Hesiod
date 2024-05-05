import json
from mdutils.mdutils import MdUtils
from mdutils.tools import Html
from mdutils.tools import Image
import os
import sys

BUILD_PATH = "build"
NODE_SNAPSHOT_PATH = "docs/images/nodes/"
NODE_MARKDOWN_PATH = "docs/node_reference/"

if __name__ == "__main__":

    img_relative_path = os.path.relpath(NODE_SNAPSHOT_PATH, NODE_MARKDOWN_PATH)

    print(img_relative_path)
    
    # --- generate snapshots and data

    print("generating snapshots...")

    os.system("cd {} ; bin/./hesiod --snapshot".format(BUILD_PATH))
    
    # --- retrieve data from json file

    fname = os.path.join(BUILD_PATH, "nodes_description.json")

    with open(fname, "r") as f:
        data = json.load(f)
    
    # --- categories

    md_file = MdUtils(file_name=os.path.join(NODE_MARKDOWN_PATH, "categories"),
                      title="Node Categories")

    md_file.new_header(level=1, title="Categories")

    cat = {}
    node_per_cat = {}
    for node_type, node_data in data.items():

        if (node_data["category"]) not in node_per_cat.keys():
            node_per_cat[node_data["category"]] = []
        node_per_cat[node_data["category"]] += [node_type]
        
        node_cat = node_data["category"].split("/")
        if node_cat[0] not in cat.keys():
            cat[node_cat[0]] = []
            
        if (len(node_cat) > 1):
            for k in range(1, len(node_cat)):
                cat[node_cat[0]] += [node_cat[k]]

    cat_table = ["Primary", "Secondary", "Nodes"]
    for c in sorted(cat.keys()):       
        if (len(cat[c])):
            for sc in sorted(set(cat[c])):
                node_list = []
                for s in node_per_cat[c + "/" + sc]:
                    node_list += [s]
                
                cat_table.extend([c, sc, ", ".join(sorted(node_list))])
        else:
            node_list = []
            for s in node_per_cat[c]:
                node_list += [s]

            cat_table.extend([c, "", ", ".join(sorted(node_list))])
                
    md_file.new_line()
    md_file.new_table(columns=3,
                      rows=int(len(cat_table) / 3),
                      text=cat_table,
                      text_align="left")
    
    md_file.create_md_file()
    
    # --- all nodes
    
    md_file = MdUtils(file_name=os.path.join(NODE_MARKDOWN_PATH, "nodes"),
                      title="Node Reference")

    md_file.new_header(level=1, title="Some kind of introduction")
    
    md_file.new_header(level=1, title="Nodes")
    
    for node_type in data.keys():
        # print(node_type)

        md_file.new_header(level=2, title=node_type)
                
        md_file.new_paragraph(data[node_type]["description"])

        img_fname = os.path.join(img_relative_path, data[node_type]["snapshot"])
        
        md_file.new_paragraph(Image.Image.new_inline_image(text="img",
                                                           path=img_fname))

        md_file.new_line()
        md_file.new_header(level=3, title="Category")
        md_file.new_paragraph(data[node_type]["category"])
        
        # inputs
        
        inputs_table = ["Name", "Type", "Description"]
        for data_inputs in data[node_type]["inputs"]:
            inputs_table.extend([data_inputs["caption"],
                                  data_inputs["type"],
                                  data_inputs["description"]])

        md_file.new_line()
        md_file.new_header(level=3, title="Inputs")
        md_file.new_table(columns=3,
                          rows=len(data[node_type]["inputs"]) + 1,
                          text=inputs_table,
                          text_align="left")

        # outputs
        
        outputs_table = ["Name", "Type", "Description"]
        for data_outputs in data[node_type]["outputs"]:
            outputs_table.extend([data_outputs["caption"],
                                  data_outputs["type"],
                                  data_outputs["description"]])

        md_file.new_line()
        md_file.new_header(level=3, title="Outputs")
        md_file.new_table(columns=3,
                          rows=len(data[node_type]["outputs"]) + 1,
                          text=outputs_table,
                          text_align="left")

        # parameters

        attr_table = ["Name", "Type", "Description"]
        for data_attr in data[node_type]["parameters"]:
            attr_table.extend([data_attr["key"],
                                  data_attr["type"],
                                  data_attr["description"]])

        md_file.new_line()
        md_file.new_header(level=3, title="Parameters")
        md_file.new_table(columns=3,
                          rows=len(data[node_type]["parameters"]) + 1,
                          text=attr_table,
                          text_align="left")
            
    # eventually dump markdown content to disk
    md_file.create_md_file()
    
