
WaterDepthFromMask Node
=======================


No description available



![img](../../images/nodes/WaterDepthFromMask_settings.png)


# Category


Hydrology
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|elevation|VirtualArray|No description|
|water_mask|VirtualArray|No description|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|water_depth|VirtualArray|Output water depth map representing flooded areas.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Max Iterations|Integer|No description|
|Mask Activation Threshold|Float|No description|
|Relaxation Factor|Float|No description|
|Convergence Tolerance|Float|No description|

# Example


![img](../../images/nodes/WaterDepthFromMask_hsd_example.png)

Corresponding Hesiod file: [WaterDepthFromMask.hsd](../../examples/WaterDepthFromMask.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

