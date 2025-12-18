
ValleyFill Node
===============


Fill valleys using thermal scree deposition and height-based blending.



![img](../../images/nodes/ValleyFill_settings.png)


# Category


Erosion/Thermal
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|
|mask|Heightmap|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Eroded heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Duration|Float|Controls the number of erosion iterations or the simulated time span during which thermal erosion is applied. Higher values increase slope relaxation and material transport.|
|Deposition Gamma|Float|Gamma applied to the height-based mixing mask.|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Mix Factor|Float|Mixing factor for blending input and output values. A value of 0 uses only the input, 1 uses only the output, and intermediate values perform a linear interpolation.|
|Mix Method|Enumeration|Method used to combine input and output values. Options include linear interpolation (default), min, max, smooth min, smooth max, add, and subtract.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|Deposition Ratio|Float|Blend ratio controlling valley influence.|
|Scale with Elevation|Bool|Defines the critical talus angle (repose slope) above which material becomes unstable and moves downslope during thermal erosion.|
|Slope|Float|Selects the thermal erosion variant or behavior model, controlling how material is redistributed when slopes exceed the talus angle.|

# Example


![img](../../images/nodes/ValleyFill_hsd_example.png)

Corresponding Hesiod file: [ValleyFill.hsd](../../examples/ValleyFill.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

