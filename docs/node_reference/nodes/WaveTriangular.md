
WaveTriangular Node
===================


WaveTriangular generates triangular waves.



![img](../../images/nodes/WaveTriangular_settings.png)


# Category


Primitive/Function
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|Heightmap|Displacement with respect to the domain size (normal direction).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|WaveTriangular heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|inverse|Bool|Toggle inversion of the output values.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|slant_ratio|Float|Decides on wave asymmetry, expected in [0, 1].|

# Example


![img](../../images/nodes/WaveTriangular.png)

