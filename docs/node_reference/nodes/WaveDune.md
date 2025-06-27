
WaveDune Node
=============


WaveDune mimics using a periodic function the formation and spatial distribution of transverse sand dunes on a terrain.



![img](../../images/nodes/WaveDune_settings.png)


# Category


Primitive/Function
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|Heightmap|Displacement with respect to the domain size (normal direction).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|WaveDune heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|inverse|Bool|Toggle inversion of the output values.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|xbottom|Float|Relative position of the dune bottom, in [0, 1].|
|xtop|Float|Relative position of the dune top, in [0, 1].|

# Example


![img](../../images/nodes/WaveDune.png)

