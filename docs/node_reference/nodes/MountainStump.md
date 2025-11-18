
MountainStump Node
==================


Generates a mountain-like heightmap with a flattened (stump-shaped) peak. Credit: Ian Fontova.



![img](../../images/nodes/MountainStump_settings.png)


# Category


Primitive/Geological
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Heightmap|No description|
|dy|Heightmap|No description|
|envelope|Heightmap|No description|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|Heightmap|No description|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|add_deposition|Bool|No description|
|angle|Float|No description|
|base_noise_amp|Float|No description|
|center|Vec2Float|No description|
|elevation|Float|No description|
|gamma|Float|No description|
|k_smoothing|Float|No description|
|octaves|Integer|No description|
|peak_kw|Float|No description|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|ridge_amp|Float|No description|
|rugosity|Float|No description|
|scale|Float|No description|
|seed|Random seed number|No description|

# Example


No example available.