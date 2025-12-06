import glob
import os
import requests
import shutil

# ----------------------------
# Config
# ----------------------------
GITHUB_BASE_URL = "https://raw.githubusercontent.com/marella/material-design-icons/main/svg/outlined"
ICONS = ["bookmark", "check", "file_open", "hdr_strong", "help", "home", "info", "landscape", "palette",
         "push_pin", "refresh", "restore", "save", "scatter_plot", "settings_backup_restore",
         "u_turn_left", "visibility", "visibility_off", "waves"]
ICONS_ACCENT = ["push_pin"]
COLORS = ["#D9D9D9", "#5E81AC"]
SUFFIX = ["", "_ACCENT"]
OUT_DIR = "Hesiod/data/icons/"

# ----------------------------
# Functions
# ----------------------------

def recolor_svg_file(file_path, color):
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()
        
        # Replace <path with <path fill="COLOR"
        colored_content = content.replace('<path ', f'<path fill="{color}" ')
        
        with open(file_path, "w", encoding="utf-8") as f:
            f.write(colored_content)
        
        print(f"File recolored successfully: {file_path}")
    
    except Exception as e:
        print(f"Error recoloring {file_path}: {e}")

# ----------------------------
# Ensure output directory exists
# ----------------------------




# # ----------------------------
# # Download & recolor SVGs
# # ----------------------------

# for icon in ICONS:
#     svg_url = f"{GITHUB_BASE_URL}/{icon}.svg"  # default size 24px
#     response = requests.get(svg_url)
#     if response.status_code != 200:
#         print(f"Failed to download {icon}: {response.status_code}")
#         continue
#     original_svg = response.text

#     for color, suffix in zip(COLORS, SUFFIX):
#         print(f"exporting to color {color}")
        
#         safe_color = color.lstrip("#")
#         filename = f"{icon}.svg"
#         out_path = os.path.join(OUT_DIR, filename)

#         # with open(out_path, "w", encoding="utf-8") as f:
#         #     f.write(original_svg)
        
#         # # Replace 'currentColor' with desired hex color
#         # svg_colored = original_svg.replace("currentColor", color)

#         colored_svg = original_svg.replace('<path ', f'<path fill="{color}" ')

#         filename_base = os.path.splitext(os.path.basename(filename))[0]
#         filename_colored = f"{filename_base}{suffix}.svg"
        
#         with open(os.path.join(OUT_DIR, filename_colored), "w", encoding="utf-8") as f:
#             f.write(colored_svg)

#         print(filename_colored)
            
#         # recolor_svg(out_path, f"{os.path.basename(out_path)}_{color}.svg", color)

# print("Done. Check the folder:", OUT_DIR)

COLOR = '#D9D9D9'
COLOR_ACCENT = '#5E81AC'

if __name__ == "__main__":
    os.makedirs(OUT_DIR, exist_ok=True)

    # --- starts with non-material icons

    path = '/home/glop/Documents/dev/Hesiod/scripts/generate_icons/icon_others/'
    fnames = glob.glob(os.path.join(path, '*.svg'))

    for fname in fnames:
        fname_dst = os.path.join(OUT_DIR, os.path.basename(fname))
        shutil.copy(fname, fname_dst)

        recolor_svg_file(fname_dst, COLOR)
        
    # --- download non-material icons
    
    for icon in ICONS:
        svg_url = f"{GITHUB_BASE_URL}/{icon}.svg"  # default size 24px
        response = requests.get(svg_url)
        if response.status_code != 200:
            print(f"Failed to download {icon}: {response.status_code}")
            continue
        svg_content = response.text

        fname = os.path.join(OUT_DIR, f"{icon}.svg")
      
        with open(fname, "w", encoding="utf-8") as f:
            f.write(svg_content)

        if icon in ICONS_ACCENT:
            fname_accent = os.path.join(OUT_DIR, f"{icon}_accent.svg")
            shutil.copy(fname, fname_accent)
            recolor_svg_file(fname_accent, COLOR_ACCENT)
            
        recolor_svg_file(fname, COLOR)


