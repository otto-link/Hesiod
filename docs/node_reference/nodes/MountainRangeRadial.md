
MountainRangeRadial Node
========================


Generates a heightmap representing a radial mountain range emanating from a specified center.



![img](../../images/nodes/MountainRangeRadial_settings.png)


# Category


Primitive/Geological
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|control|Heightmap|Optional input that modulates the terrain shape and noise distribution.|
|dx|Heightmap|Displacement with respect to the domain size (x-direction).|
|dy|Heightmap|Displacement with respect to the domain size (y-direction).|
|envelope|Heightmap|Heightmap used as a post-process amplitude multiplier for the generated noise.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Heightmap|Optional output providing the computed angle values for terrain features.|
|output|Heightmap|The final heightmap representing the radial mountain range.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle_spread_ratio|Float|Controls the angular spread of the mountain range. Lower values create more concentrated ridges, while higher values distribute them more evenly.|
|center|Vec2Float|The center point of the radial mountain range as normalized coordinates within [0, 1].|
|core_size_ratio|Float|Determines the relative size of the core mountain region before tapering begins.|
|half_width|Float|Defines the half-width of the radial mountain range, controlling its spread.|
|inverse|Bool|Toggle inversion of the output values, flipping elevations.|
|kw|Wavenumber|Base spatial frequencies in the X and Y directions. The frequencies are defined with respect to the entire domain: for example, kw = 2 produces two full oscillations across the domain width (and similarly for the Y direction).|
|lacunarity|Float|The frequency scaling factor for successive noise octaves. Higher values increase the frequency of each successive octave.|
|octaves|Integer|The number of octaves for fractal noise generation. More octaves add finer details to the terrain.|
|persistence|Float|The amplitude scaling factor for subsequent noise octaves. Lower values reduce the contribution of higher octaves.|
|remap|Value range|Remaps the operator's output values to a specified range, defaulting to [0, 1].|
|Seed|Random seed number|Random seed number. The random seed is an offset to the randomized process. A different seed will produce a new result.|
|weight|Float|Controls how much higher FBM octaves contribute to the noise based on local elevation. A higher weight suppresses high-frequency octaves at low elevations and increases their influence at higher elevations, producing terrain where fine details appear mainly near peaks while lower areas remain smoother.|

# Example


No example available.