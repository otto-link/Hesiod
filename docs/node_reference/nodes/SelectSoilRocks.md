
SelectSoilRocks Node
====================


Computes a multi-scale soil/rock mask using curvature analysis across progressively larger smoothing radii.



![img](../../images/nodes/SelectSoilRocks_settings.png)


# Category


Mask/ForTexturing
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap or scalar field used to compute curvature across scales.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Computed multi-scale soil/rock selection mask.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Clamp Mode|Enumeration|Defines how curvature values are clamped before accumulation across scales.|
|Clamp Limit|Float|Threshold used to clamp curvature values at each scale.|
|Saturation Smoothing|Float|Controls the smoothness of the saturation response applied after curvature aggregation.|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|Max Radius|Float|Maximum smoothing radius used when evaluating curvature across logarithmic scales.|
|Min Radius|Float|Minimum smoothing radius.|
|Saturation Limit|Float|Limits the maximum saturation effect applied during the output shaping stage.|
|Smaller-Scale Influence|Float|Weight applied when descending to smaller smoothing radii, allowing fine-scale features to contribute more or less strongly.|
|Sampling Steps|Integer|Number of logarithmic scales evaluated between rmin and rmax.|

# Example


![img](../../images/nodes/SelectSoilRocks_hsd_example.png)

Corresponding Hesiod file: [SelectSoilRocks.hsd](../../examples/SelectSoilRocks.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

