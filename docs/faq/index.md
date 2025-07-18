# Frequently Asked Questions

## Q: What is the development state of this project?
A: The application is still under active development.

---

## Q: Is the application stable?
A: To be transparent — the application is still under active development and **may be unstable** at times. We recommend saving your work frequently.

Use **"Ctrl+S"** to save, and **"Ctrl+Alt+S"** to save a timestamped backup copy. This helps protect your work in case of crashes or unexpected behavior.

---

## Q: Why isn’t the application production-ready?
A: This project is **Free and Open Source Software (FOSS)**, and it’s currently developed and maintained by just **one person**. As a result, things may be rough around the edges, features might be incomplete, and bugs may occur.

I'm actively looking for **feedback, contributors, and help with everything** — development, testing, documentation, or just sharing your experience using the app. Any support is appreciated!

---

## Q: What’s the license, and can I use the results commercially?
A: The source code is released under the **GNU General Public License v3.0 (GPL-3.0)**. This means you're free to:
- **Use**, **study**, **modify**, and **share** the software,
- But if you distribute a modified version, you must also release it under the same GPL-3.0 license.

As for what you create **with** the app (e.g., terrain, heightmaps, meshes), you’re free to use those **without any restriction**, including in **personal or commercial projects** — no crediting required (though it's always appreciated!).

The only exception: **you’re not allowed to sell the app itself**, or distribute/sell generated **assets or presets** as standalone commercial products.

---

## Q: Are there any tutorials for Hesiod?
A: Not really — at least not yet. However, you can find a few **basic video guides** here:  
📺 [YouTube Playlist](https://m.youtube.com/playlist?list=PLvla2FXp5tDxbPypf_Mp66gWzX_Lga3DK)

Also, check the **[Releases page on GitHub](https://github.com/otto-link/Hesiod/releases/)**. Most releases include example `.hsd` files at the bottom — these are often **commented and self-explanatory**, and can help you understand how things work.

More detailed tutorials and documentation are planned — and if you’d like to help with that, please reach out!

---

## Q: How big can I make the terrain in the app?
A: In the desktop application, terrain size is limited to **4k × 4k cells** to maintain responsive performance. Beyond that, computation becomes slow and can degrade the user experience.

To work around this, you can edit the terrain at a lower resolution (e.g., **1k × 1k**) in the GUI, then use **Hesiod's batch mode** (headless, no GUI) to generate higher-resolution output. In batch mode, the actual limit depends on your hardware—primarily your **RAM and GPU memory**. As a rough estimate, resolutions up to **32k × 32k** may be possible on powerful systems.
