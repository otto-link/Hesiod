
NormalDisplacement Node
=======================


NormalDisplacement applies a displacement to the terrain along the normal direction.



![img](../../images/nodes/NormalDisplacement_settings.png)


# Category


Filter/Recast
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|VirtualArray|Input heightmap.|
|mask|VirtualArray|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|VirtualArray|Output heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|amount|Float|Displacement scaling.|
|iterations|Integer|Number of successive use of the operator.|
|radius|Float|Filter radius with respect to the domain size.|
|reverse|Bool|Reverse displacement direction.|

# Example


No example available.