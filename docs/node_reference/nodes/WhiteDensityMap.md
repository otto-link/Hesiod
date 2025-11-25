
WhiteDensityMap Node
====================


WhiteDensityMap noise operator generates a random signal with a flat power and a spatial density defined by an input heightmap.



![img](../../images/nodes/WhiteDensityMap_settings.png)


# Category


Primitive/Random
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|density|Heightmap|Output noise amplitude envelope.|
|envelope|Heightmap|Heightmap used as a post-process amplitude multiplier for the generated noise.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Generated noise.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|inverse|Bool|Toggle inversion of the output values.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|Seed|Random seed number|Random seed number. The random seed is an offset to the randomized process. A different seed will produce a new result.|

# Example


No example available.