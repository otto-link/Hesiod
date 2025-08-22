import json
import matplotlib.pyplot as plt
import networkx as nx
import subprocess
import sys

font_size = 8

def format_value(val, digits=6):
    if isinstance(val, float):
        return f'{val:.{digits}f}'
    elif isinstance(val, list):
        return [format_value(v, digits) for v in val]
    
    return str(val)

if __name__ == '__main__':
    fname = sys.argv[1]
    print('exporting: {}'.format(fname))

    # parse file
    with open(fname, 'r') as f:
        data = json.load(f)

    # keep only model data
    data = data['graph_manager']['graph_nodes']

    for gid, gdata in data.items():
        print('exporting graph: {}'.format(gid))

        # create visualization
        G = nx.DiGraph()

        # Apply graph-level attributes for dark theme
        G.graph['graph'] = {
            'bgcolor': '#1e1e1e',   # Dark background
            'fontcolor': '#f0f0f0', # Default font color
            'rankdir': 'TB',        # Top to Bottom layout
        }

        G.graph['node'] = {
            'shape': 'box',
            'style': 'filled,rounded',
            'fontcolor': '#f0f0f0',
            'fillcolor': '#333333', # Node background
            'color': '#888888',     # Node border
            'labeljust': 'l',
            'labelloc': 'c',
            'fontname': 'Arial',
        }

        G.graph['edge'] = {
            'color': '#aaaaaa',     # Light gray edges
            'fontcolor': '#f0f0f0', # Edge label color
            'fontname': 'Arial',
        }
        
        labels = {}

        for node in gdata['nodes']:
            n = node['id']

            label = node['label'] + '\n'

            # add attributes
            if (node['label'] != 'Brush'):
                for k, v in node.items():
                    if isinstance(v, dict):
                        if 'label' in v:
                            if 'choice' in v:
                                val = v['choice'].replace(':', '')
                            else:
                                val = v['value']
                            label += '+ ' + k + '= ' + str(format_value(val)) + '\\l'
            
            G.add_node(n, size=1)
            G.nodes[n]['shape'] = 'box'
            G.nodes[n]['label'] = label
            G.nodes[n]['labeljust'] = 'left' # align text left
            G.nodes[n]['style'] = 'filled'
    
        # edges
        for link in gdata['links']:
            txt = link['port_id_from'] + ' -> ' +  link['port_id_to']
            G.add_edge(link['node_id_from'], link['node_id_to'], label=txt)

        edge_labels = {(u, v): d['label'] for u, v, d in G.edges(data=True)}
            
        # pos = nx.nx_agraph.graphviz_layout(G, prog='dot')

        nx.drawing.nx_pydot.write_dot(G, 'graph.dot')

        # Render to SVG using Graphviz directly
        subprocess.run(['dot', '-Tsvg', 'graph.dot', '-o', gid + '.svg'], check=True)
        
plt.show()
