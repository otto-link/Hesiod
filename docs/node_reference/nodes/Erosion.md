
Erosion Node
============


Erosion removes small structures or noise from an image, and it also shrinks the boundaries of objects in an image. Use Cases: (+) Noise reduction: Erosion can help remove small, unwanted pixels or noise from the image. (+) Separating objects: Erosion can be used to separate touching or overlapping objects in an image by shrinking their boundaries.



![img](../../images/nodes/Erosion_settings.png)


# Category


Operator/Morphology
# Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|Heightmap|Input heightmap.|

# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|Heightmap|Dilated heightmap.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|GPU|Bool|Toogle GPU acceleration on or off.|
|radius|Float|Filter radius with respect to the domain size.|

# Example


![img](../../images/nodes/Erosion.png)

