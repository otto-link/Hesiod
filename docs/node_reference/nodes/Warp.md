
Warp Node
=========


The Warp node transforms a base heightmap by warping/pushing pixels as defined by the input displacements.



![img](../../images/nodes/Warp_settings.png)


# Category


Operator/Transform
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Heightmap|Displacement with respect to the domain size (x-direction).|
|dy|Heightmap|Displacement with respect to the domain size (y-direction).|
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Warped heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_mix|Float|No description|
|post_mix_method|Enumeration|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|
|scaling.x|Float|No description|
|scaling.y|Float|No description|

# Example


![img](../../images/nodes/Warp.png)

