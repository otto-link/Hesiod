
DepressionFilling Node
======================


DepressionFilling is used to fill depressions or sinks in an heightmap. It ensures that there are no depressions, i.e. areas within a digital elevation model that are surrounded by higher terrain, with no outlet to lower areas.



![img](../../images/nodes/DepressionFilling_settings.png)


# Category


Erosion
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|fill map|Heightmap|Filling map.|
|output|Heightmap|Filled heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|epsilon|Float|Minimum slope tolerance.|
|iterations|Integer|Maximum number of iterations.|
|remap fill map|Bool|Remap to [0, 1] the filling map.|

# Example


No example available.