
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

Corresponding Hesiod file: [ZeroedEdges.hsd](../../examples/ZeroedEdges.hsd). Use [Ctrl+I] in the node editor to import a hsd file within your current project. 

> **Note:** Example files are kept up-to-date with the latest version of [Hesiod](https://github.com/otto-link/Hesiod).
> If you find an error, please [open an issue](https://github.com/otto-link/Hesiod/issues).

