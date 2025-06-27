
CloudLattice Node
=================


Generate a grid lattice set of points.



![img](../../images/nodes/CloudLattice_settings.png)


# Category


Geometry/Cloud
# Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|Cloud|Set of points (x, y) and elevations z.|

# Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|delta|Wavenumber|Point spacing in x and y directions.|
|jitter_ratio|Wavenumber|Point jittering (noise) in x and y directions.|
|remap|Value range|Remap the operator's output to a specified range, defaulting to [0, 1].|
|seed|Random seed number|Random seed number|
|stagger_ratio|Wavenumber|Point offset in x and y directions for every two lines or columns.|

# Example


![img](../../images/nodes/CloudLattice.png)

