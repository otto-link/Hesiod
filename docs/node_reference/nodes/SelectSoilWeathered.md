
SelectSoilWeathered Node
========================


No description available



![img](../../images/nodes/SelectSoilWeathered_settings.png)


# Category


Mask/ForTexturing
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|No description|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|No description|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|curvature_clamp_mode|Enumeration|No description|
|curvature_clamping|Float|No description|
|curvature_weight|Float|No description|
|gradient_gain|Float|No description|
|gradient_weight|Float|No description|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|radius_curvature|Float|No description|
|radius_gradient|Float|No description|

# Example


![img](../../images/nodes/SelectSoilWeathered_hsd_example.png)

Corresponding Hesiod file: [SelectSoilWeathered.hsd](../../examples/SelectSoilWeathered.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

  
# Screenshots

![img](../../images/nodes/SelectSoilWeathered_wiki0.png)
