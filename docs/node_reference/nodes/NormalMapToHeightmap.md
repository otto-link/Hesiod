
NormalMapToHeightmap Node
=========================


No description available



![img](../../images/nodes/NormalMapToHeightmap_settings.png)


# Category


Converter
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|normal map|HeightmapRGBA|No description|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|No description|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|No description|
|omega|Float|No description|
|poisson_solver|Bool|No description|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|

# Example


![img](../../images/nodes/NormalMapToHeightmap_hsd_example.png)

Corresponding Hesiod file: [NormalMapToHeightmap.hsd](../../examples/NormalMapToHeightmap.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

