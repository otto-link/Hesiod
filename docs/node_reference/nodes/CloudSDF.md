
CloudSDF Node
=============


CloudSDF evaluates the signed distance function of a set of points. It assigns a signed distance value to every point in space.



![img](../../images/nodes/CloudSDF_settings.png)


# Category


Geometry/Cloud
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|Cloud|Input cloud.|
|dx|Heightmap|Displacement with respect to the domain size (x-direction).|
|dy|Heightmap|Displacement with respect to the domain size (y-direction).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|sdf|Heightmap|Signed distance as an heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|

# Example


![img](../../images/nodes/CloudSDF.png)

