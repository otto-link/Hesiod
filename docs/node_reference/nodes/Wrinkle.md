
Wrinkle Node
============


TODO



![img](../../images/nodes/Wrinkle_settings.png)


# Category


WIP
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|TODO|
|mask|Heightmap|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|TODO|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|displacement_amplitude|Float|TODO|
|kw|Float|Base spatial frequencies in the X and Y directions. The frequencies are defined with respect to the entire domain: for example, kw = 2 produces two full oscillations across the domain width (and similarly for the Y direction).|
|octaves|Integer|The number of octaves for fractal noise generation. More octaves add finer details to the terrain.|
|radius|Float|TODO|
|Seed|Random seed number|Random seed number. The random seed is an offset to the randomized process. A different seed will produce a new result.|
|weight|Float|Controls how much higher FBM octaves contribute to the noise based on local elevation. A higher weight suppresses high-frequency octaves at low elevations and increases their influence at higher elevations, producing terrain where fine details appear mainly near peaks while lower areas remain smoother.|
|wrinkle_amplitude|Float|TODO|
|wrinkle_angle|Float|TODO|

# Example


No example available.