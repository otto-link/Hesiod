
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
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|

# Example


![img](../../images/nodes/SelectTransitions.png)

