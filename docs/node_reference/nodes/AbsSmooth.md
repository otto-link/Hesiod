
AbsSmooth Node
==============


Apply a smooth absolute value function to every values. The smooth absolute operator computes the absolute value of an input while providing a smooth transition around zero, ensuring continuity and smoothness in the output.



![img](../../images/nodes/AbsSmooth_settings.png)


# Category


Math/Base
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Output heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|inverse|Bool|Toggle inversion of the output values.|
|mu|Float|Smoothing intensity.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|vshift|Float|Reference value for the zero-equivalent value of the absolute value.|

# Example


No example available.  
# Note

Automatically included.
