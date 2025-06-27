
WhiteSparse Node
================


WhiteSparse noise operator generates a random signal with a flat power spectral density, but with a sparse spatial density.



![img](../../images/nodes/WhiteSparse_settings.png)


# Category


Primitive/Random
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|envelope|Heightmap|Output noise amplitude envelope.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Generated noise.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|density|Float|Noise density.|
|inverse|Bool|Toggle inversion of the output values.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|seed|Random seed number|Random seed number.|

# Example


![img](../../images/nodes/WhiteSparse.png)

