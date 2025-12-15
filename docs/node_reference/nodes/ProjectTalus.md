
ProjectTalus Node
=================


Projects heightmap values along a discrete grid direction using talus-based attenuation. Values propagate outward following the D8 convention and decay with distance according to a talus coefficient. The operation is typically used for slope-dependent diffusion, debris flow approximation, or directional spreading effects.



![img](../../images/nodes/ProjectTalus_settings.png)


# Category


Filter
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap whose values are projected along the selected direction.|
|mask|Heightmap|Optional mask controlling where the projection effect is applied.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Resulting heightmap after directional projection and talus-based attenuation.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Propagation Direction (D8)|Integer|Propagation direction following the D8 grid convention. Values range from 0 to 7, each corresponding to one of the eight neighboring directions on a 2D grid.|
|mask_activate|Bool|Enables or disables the internal mask. If the node's 'mask' input is connected, this setting is bypassed and the input mask is used instead.|
|mask_gain|Float|Controls the intensity or influence of the internal mask. Bypassed if the 'mask' input is connected.|
|mask_inverse|Bool|Inverts the internal mask, applying the operator where the mask is low. Ignored if a 'mask' input is provided.|
|mask_radius|Float|Defines the smoothing radius for the internal mask. A value of 0 disables smoothing. This is bypassed if the 'mask' input is used.|
|mask_type|Choice|Specifies how the internal mask is computed: 'Elevation' uses height, 'Gradient Norm' uses slope, and 'Elevation mid-range' selects the middle portion of the height range. This parameter is ignored when a 'mask' input is connected.|
|Gain|Float|Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.|
|Invert Output|Bool|Inverts the output values after processing, flipping low and high values across the midrange.|
|Mix Factor|Float|Mixing factor for blending input and output values. A value of 0 uses only the input, 1 uses only the output, and intermediate values perform a linear interpolation.|
|Mix Method|Enumeration|Method used to combine input and output values. Options include linear interpolation (default), min, max, smooth min, smooth max, add, and subtract.|
|Remap Range|Value range|Linearly remaps the output values to a specified target range (default is [0, 1]).|
|Saturation Range|Value range|Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.|
|Smoothing Radius|Float|Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.|
|Slope|Float|Talus attenuation coefficient controlling how quickly projected values decay with distance along the propagation direction. Higher values result in shorter propagation ranges.|

# Example


No example available.