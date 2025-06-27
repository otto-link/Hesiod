
SmoothFillSmearPeaks Node
=========================


SmoothFillSmearPeaks is a smoothing technique that restricts smoothing effect to concave regions.



![img](../../images/nodes/SmoothFillSmearPeaks_settings.png)


# Category


Filter/Smoothing
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|
|mask|Heightmap|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Filtered heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|GPU|Bool|Toogle GPU acceleration on or off.|
|radius|Float|Filter radius with respect to the domain size.|

# Example


![img](../../images/nodes/SmoothFillSmearPeaks.png)

