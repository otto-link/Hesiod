#/usr/bin/python3
import json
import os
import pandas as pd

HSD_DATA_PATH = "Hesiod/data"

def load_node_data():
    """Load node documentation data from a JSON file."""
    fname = os.path.join(HSD_DATA_PATH, "node_documentation.json")
    with open(fname, "r") as f:
        return json.load(f)

if __name__ == "__main__":
    json = load_node_data()
    # print(json)

    df = pd.DataFrame.from_dict(json, orient='index')
    df[['main_category', 'sub_category']] = df['category'].str.split('/', expand=True)
    df = df.reset_index()

    df = df[['label', 'main_category', 'sub_category', 'description']]
    
    with pd.ExcelWriter("nodes.ods", engine="odf") as writer:
        df.to_excel(writer)
    
    print(df)
