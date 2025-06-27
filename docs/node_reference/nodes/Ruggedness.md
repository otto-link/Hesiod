
Ruggedness Node
===============


Measures the terrain roughness by computing the square root of the sum of squared elevation differences within a specified radius. Indicates how rough or smooth the terrain is by analyzing how much the elevation changes between neighboring areas. Higher values mean a more rugged, uneven surface, while lower values indicate a flatter, smoother landscape.



![img](../../images/nodes/Ruggedness_settings.png)


# Category


Features
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Heightmap input representing terrain elevations, used to calculate ruggedness.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Resulting heightmap where each pixel represents the calculated ruggedness value.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|GPU|Bool|Toogle GPU acceleration on or off.|
|inverse|Bool|Toggle inversion of the output values.|
|radius|Float|Defines the neighborhood radius used for computing ruggedness. Larger values consider a wider area, capturing broader terrain variations.|
|smoothing|Bool|Enable or disable smoothing to reduce noise in the curvature computation.|
|smoothing_radius|Float|Specifies the radius for smoothing, determining how much the curvature is averaged over neighboring pixels.|

# Example


![img](../../images/nodes/Ruggedness.png)

