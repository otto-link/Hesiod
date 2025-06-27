
DistanceTransform Node
======================


Generates a distance map where each pixel's value represents the shortest distance to the nearest non-zero value in the input heightmap.



![img](../../images/nodes/DistanceTransform_settings.png)


# Category


Operator/Morphology
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap used to compute the distance transform.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Generated distance map based on the input heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|remap|Bool|Remap the operator's output to a specified range, defaulting to [0, 1].|
|reverse_input|Bool|Invert the input values before processing.|
|reverse_output|Bool|Invert the output values after processing.|
|threshold|Float|Defines the cutoff value for considering non-zero input pixels.|
|transform_type|Enumeration|Specifies the distance metric used for transformation (e.g., Euclidean, Manhattan, or Chessboard distance).|

# Example


![img](../../images/nodes/DistanceTransform.png)

