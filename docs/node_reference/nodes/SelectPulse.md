
SelectPulse Node
================


SelectPulse is a thresholding operator. It selects values within a specified range defined by the shape of a Gaussian pulse.



![img](../../images/nodes/SelectPulse_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Mask heightmap (in [0, 1]).|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|
|sigma|Float|Selection half-width.|
|value|Float|Selection center value.|

# Example


![img](../../images/nodes/SelectPulse.png)

