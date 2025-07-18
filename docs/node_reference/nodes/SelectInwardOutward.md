
SelectInwardOutward Node
========================


SelectInwardOutward returns values > 0.5 if the slope is pointing to the center (slope is inward), and values < 0.5 otherwise (slope is outward).



![img](../../images/nodes/SelectInwardOutward_settings.png)


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
|center|Vec2Float|Reference center within the heightmap.|
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|

# Example


![img](../../images/nodes/SelectInwardOutward.png)

