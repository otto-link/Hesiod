
Lerp Node
=========


The Lerp operator, short for linear interpolation, is a method for smoothly transitioning between two values over a specified range or interval.



![img](../../images/nodes/Lerp_settings.png)


# Category


Math/Base
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|a|VirtualArray|Start heightmap (t = 0).|
|b|VirtualArray|End heightmap (t = 1).|
|t|VirtualArray|Lerp factor, expected in [0, 1].|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|VirtualArray|Interpolated heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|t|Float|Lerp factor (in [0, 1]). Used only if the node input 't' is not set.|

# Example


No example available.