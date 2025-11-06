
SelectTransitions Node
======================


SelectTransitions returns a mask filled with 1 at the blending transition between two heightmaps, and 0 elsewhere.



![img](../../images/nodes/SelectTransitions_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|blend|Heightmap|Blended heightmap.|
|input 1|Heightmap|Input heightmap 1.|
|input 2|Heightmap|Input heightmap 2.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Mask heightmap (in [0, 1]).|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|post_gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|post_inverse|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|post_remap|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|post_saturate|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|post_smoothing_radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|

# Example


No example available.