
SelectTransitions Node
======================


SelectTransitions returns a mask filled with 1 at the blending transition between two heightmaps, and 0 elsewhere.



![img](../../images/nodes/SelectTransitions_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|blend|Heightmap|Blended heightmap.|
|input 1|Heightmap|Input heightmap 1.|
|input 2|Heightmap|Input heightmap 2.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Mask heightmap (in [0, 1]).|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|inverse|Bool|Toggle inversion of the output values.|
|smoothing|Bool|Enable or disable smoothing to reduce noise in the curvature computation.|
|smoothing_radius|Float|Specifies the radius for smoothing, determining how much the curvature is averaged over neighboring pixels.|

# Example


![img](../../images/nodes/SelectTransitions.png)

