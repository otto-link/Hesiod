
ZeroedEdges Node
================


An operator that enforces values close to zero at the domain edges.



![img](../../images/nodes/ZeroedEdges_settings.png)


# Category


Math/Boundaries
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|Heightmap|Displacement relative to the domain size (radial direction).|
|input|Heightmap|The input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|The filtered heightmap result.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|distance_function|Enumeration|Determines the method used for distance calculation. Options: Chebyshev, Euclidean, Euclidean/Chebyshev, Manhattan.|
|remap|Value range|Specifies the output range for the operator. Defaults to [0, 1].|
|sigma|Float|Controls the shape power law.|

# Example


![img](../../images/nodes/ZeroedEdges_hsd_example.png)

[ZeroedEdges.hsd](../images/nodes/ZeroedEdges.hsd)

