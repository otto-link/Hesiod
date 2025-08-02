
VorolinesFbm Node
=================


This function generates a Voronoi-like pattern based on the distance from each pixel to a set of randomly oriented lines. Each line is defined by a random point and a direction sampled from a uniform distribution around a given angle. Generates a fractal Voronoi pattern using fBm (fractal Brownian motion) based on distance fields and multiple noise layers.



![img](../../images/nodes/VorolinesFbm_settings.png)


# Category


Primitive/Coherent
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Heightmap|No description|
|dy|Heightmap|No description|
|envelope|Heightmap|No description|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|Heightmap|No description|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|No description|
|angle_span|Float|No description|
|density|Float|No description|
|exp_sigma|Float|No description|
|k_smoothing|Float|No description|
|lacunarity|Float|No description|
|octaves|Integer|No description|
|persistence|Float|No description|
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|
|return_type|Enumeration|No description|
|seed|Random seed number|No description|
|sqrt_output|Bool|No description|
|weight|Float|No description|

# Example


![img](../../images/nodes/VorolinesFbm.png)

