
ConeSigmoid Node
================


Generates a smooth conical heightmap using a sigmoid-based profile.



![img](../../images/nodes/ConeSigmoid_settings.png)


# Category


Primitive/Function
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Heightmap|Optional X-displacement noise field for coordinate perturbation.|
|dy|Heightmap|Optional Y-displacement noise field for coordinate perturbation.|
|envelope|Heightmap|Optional mask controlling the effective region of the generated cone.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Generated conical heightfield.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|alpha|Float|Exponent controlling the steepness of the cone slope.|
|center|Vec2Float|Center position of the cone in world coordinate space.|
|post_gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|post_inverse|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|post_remap|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|post_saturate|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|post_smoothing_radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|radius|Float|Effective radius of the cone in coordinate space units.|

# Example


No example available.