
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
|GPU|Bool|Toogle GPU acceleration on or off.|

# Example


![img](../../images/nodes/Warp.png)

