
Translate Node
==============


Translates an heightmap by a specified amount along the x and y axes. This function shifts the contents of the input array by `dx` and `dy` units along the x and y axes, respectively. It supports both periodic boundary conditions, where the array wraps around, and non-periodic conditions.



![img](../../images/nodes/Translate_settings.png)


# Category


Operator/Transform
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Heightmap|Displacement with respect to the domain size (y-direction).|
|dy|Heightmap|Control parameter, acts as a multiplier for the weight parameter.|
|input|Heightmap|Displacement with respect to the domain size (x-direction).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Translate heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Float|The translation distance along the x-axis. Positive values shift the array to the right.|
|dy|Float|The translation distance along the y-axis. Positive values shift the array downward.|
|periodic|Bool|If set to `true`, the translation is periodic, meaning that elements that move out of one side of the array reappear on the opposite side.|

# Example


No example available.