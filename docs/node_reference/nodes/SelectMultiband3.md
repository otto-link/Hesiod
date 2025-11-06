
SelectMultiband3 Node
=====================


TODO



![img](../../images/nodes/SelectMultiband3_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|TODO|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|high|Heightmap|TODO|
|low|Heightmap|TODO|
|mid|Heightmap|TODO|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|overlap|Float|TODO|
|post_gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|post_inverse|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|post_remap|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|post_saturate|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|post_smoothing_radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|ratio1|Float|TODO|
|ratio2|Float|TODO|

# Example


No example available.