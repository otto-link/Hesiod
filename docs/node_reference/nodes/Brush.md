
Brush Node
==========


A manual brush tool for directly painting heightmaps, allowing interactive terrain editing.



![img](../../images/nodes/Brush_settings.png)


# Category


Primitive/Authoring
# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|Heightmap|The generated heightmap from brush strokes.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|Heightmap|Array|The heightmap data representing the painted terrain.|
|inverse|Bool|Invert the drawn values, swapping elevations between high and low.|
|remap|Value range|Remap the brush output to a specified range, defaulting to [0, 1].|

# Example


![img](../../images/nodes/Brush.png)

