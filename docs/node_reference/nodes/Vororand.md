
Vororand Node
=============


This function computes a Voronoi diagram or derived metric (such as F1, F2, or edge distances) on a grid of given shape. A set of random points is generated within an extended bounding box, based on the desired density and variability, to reduce edge artifacts. Optionally, per-pixel displacement can be applied through noise fields. Optionnaly, a user-defined set of points can be used instead of automatically generated random points.



![img](../../images/nodes/Vororand_settings.png)


# Category


Primitive/Coherent
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|Cloud|No description|
|dx|Heightmap|Displacement with respect to the domain size (x-direction).|
|dy|Heightmap|Displacement with respect to the domain size (y-direction).|
|envelope|Heightmap|Heightmap used as a post-process amplitude multiplier for the generated noise.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|Heightmap|No description|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|density|Float|No description|
|exp_sigma|Float|No description|
|k_smoothing|Float|No description|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|return_type|Enumeration|No description|
|Seed|Random seed number|Random seed number. The random seed is an offset to the randomized process. A different seed will produce a new result.|
|sqrt_output|Bool|No description|
|variability|Float|No description|

# Example


No example available.