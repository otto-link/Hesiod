
WaterMask Node
==============


Computes a gradient-based water mask using an extended water-depth model. This produces a smooth mask indicating regions that become flooded when water depth is artificially increased by a specified amount. It can also be used to generate a transition mask between water and ground, useful for effects such as texturing a sand beach.



![img](../../images/nodes/WaterMask_settings.png)


# Category


Hydrology
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|elevation|Heightmap|Elevation map corresponding to the water depth grid.|
|water_depth|Heightmap|Output water depth map representing flooded areas.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|mask|Heightmap|Output normalized water-extension mask, ranging from 0 to 1.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Additional Water Depth|Float|Amount of additional water depth used to simulate how water would further spread over the terrain.|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|

# Example


![img](../../images/nodes/WaterMask_hsd_example.png)

Corresponding Hesiod file: [WaterMask.hsd](../../examples/WaterMask.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

