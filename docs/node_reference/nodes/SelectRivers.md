
SelectRivers Node
=================


SelectRivers is a thresholding operator. It creates a mask for river systems based on a flow accumulation threshold.



![img](../../images/nodes/SelectRivers_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Mask heightmap (in [0, 1]).|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|clipping_ratio|Float|TODO|
|post_gain|Float|No description|
|post_inverse|Bool|No description|
|post_remap|Value range|No description|
|post_smoothing_radius|Float|No description|
|talus_ref|Float|TODO|

# Example


![img](../../images/nodes/SelectRivers.png)

