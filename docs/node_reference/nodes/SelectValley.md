
SelectValley Node
=================


Identifies and selects valley-like regions in the heightmap based on curvature analysis. The selection can be inverted, and additional parameters allow fine-tuning of the selection process. The output is a mask representing the relative width of the valley. The value is 1 at the valley center and decreases to 0 at the edges of the valley.



![img](../../images/nodes/SelectValley_settings.png)


# Category


Mask/Selector
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Heightmap data used as input for valley detection.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|A mask representing the relative width of the valley. The value is 1 at the valley center and decreases to 0 at the edges of the valley.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|GPU|Bool|Enables or disables GPU acceleration for faster processing.|
|inverse|Bool|Inverts the selection, highlighting ridges instead of valleys.|
|radius|Float|Defines the search radius for valley detection, controlling how localized or broad the selection is.|
|ridge_select|Bool|If enabled, selects ridges instead of valleys.|
|smoothing|Bool|Applies smoothing to reduce noise in curvature calculations, resulting in a cleaner selection.|
|smoothing_radius|Float|Defines the radius for the smoothing operation, determining how much curvature values are averaged over neighboring pixels.|

# Example


![img](../../images/nodes/SelectValley.png)

