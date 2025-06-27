
Dendry Node
===========


Dendry is a procedural model for dendritic patterns generation.



![img](../../images/nodes/Dendry_settings.png)


# Category


Primitive/Coherent
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|control|Heightmap|Global control heightmap that defines the overall shape of the output.|
|dx|Heightmap|Displacement with respect to the domain size (x-direction).|
|dy|Heightmap|Displacement with respect to the domain size (y-direction).|
|envelope|Heightmap|Output noise amplitude envelope.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|Heightmap|TODO|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|displacement|Float|Maximum displacement of segments.|
|eps|Float|Used to bias the area where points are generated in cells.|
|inverse|Bool|Toggle inversion of the output values.|
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|noise_amplitude_proportion|Float|Proportion of the amplitude of the control function as noise.|
|primitives_resolution_steps|Integer|Additional resolution steps in the primitive resolution.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|resolution|Integer|Number of resolutions in the noise function.|
|seed|Random seed number|Random seed number.|
|slope_power|Float|Additional parameter to control the variation of slope on terrains.|
|subsampling|Integer|Function evaluation subsampling, use higher values for faster computation at the cost of a coarser resolution.|

# Example


![img](../../images/nodes/Dendry.png)

