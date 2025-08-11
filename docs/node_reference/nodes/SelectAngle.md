
SelectAngle Node
================


SelectAngle is a thresholding operator. It selects angle values within a specified range defined by the shape of a Gaussian pulse.



![img](../../images/nodes/SelectAngle_settings.png)


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
|angle|Float|Selection center value.|
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|
|radius|Float|Pre-filter radius.|
|sigma|Float|Selection half-width.|

# Example


![img](../../images/nodes/SelectAngle.png)

