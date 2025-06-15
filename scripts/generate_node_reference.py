import json
import os
import shutil
import sys
from mdutils.mdutils import MdUtils
from mdutils.tools import Image

# Paths for build, data, and documentation directories
BUILD_PATH = "build"
HSD_DATA_PATH = "Hesiod/data"
NODE_SNAPSHOT_PATH = "docs/images/nodes/"
NODE_MARKDOWN_PATH = "docs/node_reference/"

def generate_snapshots():
    """Generate node snapshots using the Hesiod executable."""
    print("Generating snapshots...")
    # os.system(f"cd {BUILD_PATH} ; bin/./hesiod --snapshot")

def load_node_data():
    """Load node documentation data from a JSON file."""
    fname = os.path.join(HSD_DATA_PATH, "node_documentation.json")
    with open(fname, "r") as f:
        return json.load(f)

def generate_categories_markdown(data):
    """Generate markdown documentation for node categories."""
    md_file = MdUtils(file_name=os.path.join(NODE_MARKDOWN_PATH, "categories"),
                      title="Node Categories")
    md_file.new_header(level=1, title="Categories")
    
    # Dictionary to store primary categories and their subcategories
    # and dictionary to store nodes grouped by category
    cat = {}
    node_per_cat = {}
    
    # Organizing node data into categories
    for node_type, node_data in data.items():
        category = node_data["category"]
        node_per_cat.setdefault(category, []).append(node_type)
        
        node_cat = category.split("/")
        cat.setdefault(node_cat[0], []).extend(node_cat[1:])
    
    cat_table = ["Primary", "Secondary", "Nodes"]
    
    # Constructing category markdown table
    for primary in sorted(cat.keys()):
        secondary_list = sorted(set(cat[primary]))
        
        if secondary_list:
            for secondary in secondary_list:
                node_list = sorted(node_per_cat.get(f"{primary}/{secondary}", []))
                cat_table.extend([primary, secondary, ", ".join(node_list)])
        else:
            node_list = sorted(node_per_cat.get(primary, []))
            cat_table.extend([primary, "", ", ".join(node_list)])
    
    md_file.new_table(columns=3, rows=len(cat_table) // 3, text=cat_table, text_align="left")
    md_file.create_md_file()

def generate_node_markdown(data):
    """Generate markdown documentation for all nodes."""
    img_relative_path = os.path.relpath(NODE_SNAPSHOT_PATH, NODE_MARKDOWN_PATH)
    md_file = MdUtils(file_name=os.path.join(NODE_MARKDOWN_PATH, "nodes"),
                      title="Node Reference")
    
    md_file.new_header(level=1, title="Some kind of introduction")
    md_file.new_header(level=1, title="Nodes")
    
    for node_type, node_info in data.items():
        print(node_info['label'])
        md_file.new_header(level=2, title=node_type)
        md_file.new_paragraph(node_info["description"])
        
        # Handle node snapshot images
        img_fname = node_type + ".png"
        src_path = os.path.join(BUILD_PATH, img_fname)
        dst_path = os.path.join(NODE_SNAPSHOT_PATH, img_fname)
        if os.path.isfile(src_path):
            shutil.copy2(src_path, dst_path)
        
        img_path_rel = os.path.join(img_relative_path, img_fname)
        md_file.new_paragraph(Image.Image.new_inline_image(text="img", path=img_path_rel))
        
        # Category
        md_file.new_header(level=3, title="Category")
        md_file.new_paragraph(node_info["category"])
        
        # Generate tables for inputs, outputs, and parameters
        generate_ports_table(md_file, node_info, "input", "Inputs")
        generate_ports_table(md_file, node_info, "output", "Outputs")
        generate_parameters_table(md_file, node_info)
    
    md_file.create_md_file()

def generate_ports_table(md_file, node_info, port_type, title):
    """Generate markdown table for input or output ports."""
    table = ["Name", "Type", "Description"]
    entries = [
        [p["caption"], p["data_type"], p["description"]]
        for p in node_info["ports"].values()
        if p["type"] == port_type
    ]
    
    if entries:
        for entry in entries:
            table.extend(entry)
        
        md_file.new_header(level=3, title=title)
        md_file.new_table(columns=3, rows=len(entries) + 1, text=table, text_align="left")

def generate_parameters_table(md_file, node_info):
    """Generate markdown table for node parameters."""
    table = ["Name", "Type", "Description"]
    parameters = node_info.get("parameters", {})

    if parameters is None:
        return
    
    entries = [[p["label"], p["type"], p["description"]] for p in parameters.values()]
    
    if entries:
        for entry in entries:
            table.extend(entry)
        
        md_file.new_header(level=3, title="Parameters")
        md_file.new_table(columns=3, rows=len(entries) + 1, text=table, text_align="left")

def main():
    """Main function to generate markdown documentation."""
    generate_snapshots()  # Generate node snapshots

    print("Updating markdown documentation...")

    node_data = load_node_data()
    generate_categories_markdown(node_data)
    generate_node_markdown(node_data)

if __name__ == "__main__":
    main()
