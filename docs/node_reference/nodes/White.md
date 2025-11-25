
White Node
==========


White noise operator generates a random signal with a flat power spectral density.



![img](../../images/nodes/White_settings.png)


# Category


Primitive/Random
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
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