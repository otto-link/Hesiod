
Gain Node
=========


Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.



![img](../../images/nodes/Gain_settings.png)


# Category


Filter/Recurve
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|VirtualArray|Input heightmap.|
|mask|VirtualArray|Mask defining the filtering intensity (expected in [0, 1]).|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|VirtualArray|Filtered heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gain|Float|Power law exponent.|

# Example


No example available.