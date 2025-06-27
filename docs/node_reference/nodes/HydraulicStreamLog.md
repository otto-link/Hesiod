
HydraulicStreamLog Node
=======================


HydraulicStream is an hydraulic erosion operator using the flow stream approach and an infinite flow direction algorithm for simulating the erosion processes. Simulate water flow across the terrain using the infinite flow direction algorithm. As water flows over the terrain, flow accumulation representing the volume of water that passes through each point on the map, is computed to evaluate the erosive power of the water flow. Deposition is mimicked using convolution filters.



![img](../../images/nodes/HydraulicStreamLog_settings.png)


# Category


Erosion/Hydraulic
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|TODO|
|mask|Heightmap|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|deposition|Heightmap|Deposition map (in [0, 1]).|
|erosion|Heightmap|Erosion map (in [0, 1]).|
|flow_map|Heightmap|TODO|
|output|Heightmap|TODO|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|GPU|Bool|Toogle GPU acceleration on or off.|
|c_erosion|Float|Erosion intensity.|
|deposition_radius|Float|TODO|
|deposition_scale_ratio|Float|TODO|
|gradient_power|Float|TODO|
|gradient_prefilter_radius|Float|TODO|
|gradient_scaling_ratio|Float|TODO|
|saturation_ratio|Float|Flow accumulation clipping ratio.|
|talus_ref|Float|Reference talus, with small values of talus_ref  leading to thinner flow streams.|

# Example


![img](../../images/nodes/HydraulicStreamLog.png)

