
PathBspline Node
================


PathBspline uses Bspline interpolation to replace sharp angles and straight segments with smooth, flowing curves.



![img](../../images/nodes/PathBspline_settings.png)


# Category


Geometry/Path
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Path|Input path.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Path|Output path.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|

# Example


![img](../../images/nodes/PathBspline.png)

