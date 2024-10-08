
Node Reference
==============

# Some kind of introduction

# Nodes

## Abs


Apply an absolute function to every values.

![img](../images/nodes/Abs.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|vshift|Float|Reference value for the zero-equivalent value of the absolute value.|

## AbsSmooth


Apply a smooth absolute value function to every values. The smooth absolute operator computes the absolute value of an input while providing a smooth transition around zero, ensuring continuity and smoothness in the output.

![img](../images/nodes/AbsSmooth.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|mu|Float|Smoothing intensity.|
|vshift|Float|Reference value for the zero-equivalent value of the absolute value.|

## AccumulationCurvature


AccumulationCurvature is a specific type of curvature reflecting how the shape of the heightmap influences the accumulation of water. Positive accumulation curvature indicates converging flow, where water tends to accumulate and concentrate, often leading to the formation of channels or gullies. Negative accumulation curvature suggests diverging flow, where water is dispersed over a broader area, which is typical of ridges or hilltops.

![img](../images/nodes/AccumulationCurvature.png)  

### Category


Features/Landform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Accumulation curvature.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Blend


The Blend operator takes two input heightmaps.

![img](../images/nodes/Blend.png)  

### Category


Operator/Blend  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input 1|HeightMapData|Input heightmap.|
|input 2|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|blending_method|Enumeration|Blending method. Available values: add, exclusion, gradients, maximum, maximum_smooth, minimum, minimum_smooth, multiply, multiply_add, negate, overlay, soft, substract.|
|k|Float|Smoothing intensity (if any).|
|radius|Float|Filter radius with respect to the domain size (if any).|

## Bump


Bump generates a smooth transitions between zero-valued boundaries and the center of the domain.

![img](../images/nodes/Bump.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Bump heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|gain|Float|Shape control parameter.|

## Caldera


Caldera generates a volcanic caldera landscape.

![img](../images/nodes/Caldera.png)  

### Category


Primitive/Geological  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Displacement with respect to the domain size (normal direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Caldera heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|noise_r_amp|Float|Noise amplitude for the radial displacement.|
|noise_ratio_z|Float|Noise amplitude for the vertical displacement.|
|radius|Float|Crater radius.|
|sigma_inner|Float|Crater inner lip half-width.|
|sigma_outer|Float|Crater outer lip half-width.|
|z_bottom|Float|Crater bottom elevation.|

## Clamp


Clamp restrict a value within a specified range. Essentially, it ensures that a value does not exceed a defined upper limit or fall below a defined lower limit.

![img](../images/nodes/Clamp.png)  

### Category


Filter/Range  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Clamped heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|clamp|Value range|Clamping range.|
|k_max|Float|Upper bound smoothing intensity.|
|k_min|Float|Lower bound smoothing intensity.|
|smooth_max|Bool|Activate smooth clamping for the upper bound.|
|smooth_min|Bool|Activate smooth clamping for the lower bound.|

## Closing


Closing is a combination of dilation followed by erosion. It is primarily used for closing small holes or gaps in objects. Use Cases: Hole filling: Closing can fill small holes or gaps in objects, making them more solid and complete. Connecting broken objects: Closing can help connect broken segments or regions in an image, making objects more continuous.

![img](../images/nodes/Closing.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Dilated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Cloud


Set of points.

![img](../images/nodes/Cloud.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Set of points (x, y) and elevations z.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|HighMap Cloud Object|Cloud data.|

## CloudLattice


Generate a grid lattice set of points.

![img](../images/nodes/CloudLattice.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Set of points (x, y) and elevations z.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|delta|Wavenumber|Point spacing in x and y directions.|
|jitter_ratio|Wavenumber|Point jittering (noise) in x and y directions.|
|seed|Random seed|Random seed number|
|stagger_ratio|Wavenumber|Point offset in x and y directions for every two lines or columns.|

## CloudMerge


CloudMerge merges two clouds into a single one.

![img](../images/nodes/CloudMerge.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud1|CloudData|Input cloud.|
|cloud2|CloudData|Input cloud.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Merged cloud.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## CloudRandom


Random set of points.

![img](../images/nodes/CloudRandom.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Set of points (x, y) and elevations z.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|npoints|Integer|Number of points.|
|seed|Random seed|Random seed number.|

## CloudRemapValues


CloudRemapValues remap the range of the cloud point values.

![img](../images/nodes/CloudRemapValues.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Input cloud.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Output cloud.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## CloudSDF


CloudSDF evaluates the signed distance function of a set of points. It assigns a signed distance value to every point in space.

![img](../images/nodes/CloudSDF.png)  

### Category


Geometry/Cloud  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Input cloud.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|sdf|HeightMapData|Signed distance as an heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## CloudToArrayInterp


CloudToArrayInterp generates a smooth and continuous 2D elevation map from a set of scattered points using Delaunay linear interpolation.

![img](../images/nodes/CloudToArrayInterp.png)  

### Category


Primitive/Authoring  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Set of points (x, y) and elevations z.|
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|heightmap|HeightMapData|Interpolated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## CloudToPath


CloudToPath convert a Cloud to a Path.

![img](../images/nodes/CloudToPath.png)  

### Category


Converter  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Input cloud.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|closed|Bool|Decides whether the path is open and closed on itself.|
|reorder_nns|Bool|Decides whether the path points are reordered using a nearest neighbor search.|

## ColorizeCmap


ColorizeCmap generates a texture based on colormaps to assign colors to data values.

![img](../images/nodes/ColorizeCmap.png)  

### Category


Texture  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|level|HeightMapData|Data values for color selection.|
|alpha|HeightMapData|Texture alpha map.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapRGBAData|Texture (RGBA).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|clamp_alpha|Bool|Clamp to [0, 1] to input alpha map.|
|colormap|Enumeration|Color mapping selection. Available values: adrceist, aneprosi, angghore, anilyftp, anthatly, ascaired, aselllfa, aveflili, blesiblh, blywixps, boustous, byapsahi, bychpmem, cecelldf, ceitwope, cesintho, coffrrea, ctthaton, dffygutu, dfvieryo, drctheet, drfftiee, dwinthed, eafbissf, eporepop, ewntherb, feredwnt, fffadeng, ffwatsan, fleoreha, fromsesh, ftapewon, gsercrts, haviarof, hendesth, imongsir, indthsad, inosulul, intoshee, iresetyl, isungror, itteloud, ivericed, lereitth, majucent, mburmats, mearesit, meppremb, mofhewon, nalymeve, noasbian, ofasnfoi, ofoffrmp, oleonowh, onaaloni, ontomayw, outtsthe, owwheplu, pechrede, pellaine, piventon, plfgsthi, rbonblnc, rredtthe, sarermat, seclytme, sfreante, sheshesa, sinabuse, sindwirt, snotheca, ssemeool, suttlseb, taldidov, teasitis, teieebit, tharetwe, thengici, therinon, thesihas, thisathe, thoeftsw, tiamatsp, tinusprr, tiomithe, tnymeewh, tonndind, trererat, ttyaveft, tyfotinm, ucerecen, ucererds, upeesith, usoerlea, veranghh, verdeisu, wadsuthe, wbegweic, whetitat, whhorefr, wminamin, wsatavin, yierahon.|
|reverse_alpha|Bool|Reverse the input alpha map.|
|reverse_colormap|Bool|Reverse the colormap range.|

## ColorizeGradient


ColorizeGradient generates a texture based on colormaps to assign colors to data values.

![img](../images/nodes/ColorizeGradient.png)  

### Category


Texture  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|level|HeightMapData|Data values for color selection.|
|alpha|HeightMapData|Texture alpha map.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapRGBAData|Texture (RGBA).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|clamp_alpha|Bool|Clamp to [0, 1] to input alpha map.|
|gradient|Color gradient|Colormap as a manually defined color gradient.|
|reverse_alpha|Bool|Reverse the input alpha map.|
|reverse_colormap|Bool|Reverse the colormap range.|

## ColorizeSolid


ColorizeSolid generates an uniform texture based on an input color.

![img](../images/nodes/ColorizeSolid.png)  

### Category


Texture  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapRGBAData|Texture (RGBA).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|alpha|Float|Transparency as a scalar value|
|color|Color|Solid color.|

## CombineMask


CombineMask performs basic logical operations on a pair of heightmaps (assuming they are used as masks).

![img](../images/nodes/CombineMask.png)  

### Category


Mask  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input 1|HeightMapData|Input mask.|
|input 2|HeightMapData|Input mask.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Combined mask.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|method|Enumeration|Combining method. Available values: exclusion, intersection, union.|

## Comment


Comment node is a passive node holding a comment text.

![img](../images/nodes/Comment.png)  

### Category


Comment  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|comment|String|Text comment.|

## ConvolveSVD


ConvolveSVD performs convolution using a Singular Value Decomposition (SVD) of the kernel to accelerate the process.

![img](../images/nodes/ConvolveSVD.png)  

### Category


Math/Convolution  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|kernel|KernelData|Convolution kernel.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|rank|Integer|Rank of the singular value decomposition.|

## Cos


Apply a cosine function to every values.

![img](../images/nodes/Cos.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|frequency|Float|Frequency.|
|phase_shift|Float|Phase shift.|

## Crater


Crater generates a crater landscape..

![img](../images/nodes/Crater.png)  

### Category


Primitive/Geological  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Crater heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|depth|Float|Crater depth.|
|lip_decay|Float|Ejecta lip decay.|
|lip_height_ratio|Float|Controls the ejecta lip relative height.|
|radius|Float|Crater radius.|

## DataAnalysis


DataAnalysis.

![img](../images/nodes/DataAnalysis.png)  

### Category


Debug  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## DataPreview


DataPreview.

![img](../images/nodes/DataPreview.png)  

### Category


Debug  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|elevation|HeightMapData|Data values for elevation preview.|
|texture|HeightMapRGBAData|Data values for color preview.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## Dendry


Dendry is a procedural model for dendritic patterns generation.

![img](../images/nodes/Dendry.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|control|HeightMapData|Global control heightmap that defines the overall shape of the output.|
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|displacement|Float|Maximum displacement of segments.|
|eps|Float|Used to bias the area where points are generated in cells.|
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|noise_amplitude_proportion|Float|Proportion of the amplitude of the control function as noise.|
|primitives_resolution_steps|Integer|Additional resolution steps in the primitive resolution.|
|resolution|Integer|Number of resolutions in the noise function.|
|seed|Random seed|Random seed number.|
|slope_power|Float|Additional parameter to control the variation of slope on terrains.|
|subsampling|Integer|Function evaluation subsampling, use higher values for faster computation at the cost of a coarser resolution.|

## DepressionFilling


DepressionFilling is used to fill depressions or sinks in an heightmap. It ensures that there are no depressions, i.e. areas within a digital elevation model that are surrounded by higher terrain, with no outlet to lower areas.

![img](../images/nodes/DepressionFilling.png)  

### Category


Erosion  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filled heightmap.|
|fill map|HeightMapData|Filling map.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|epsilon|Float|Minimum slope tolerance.|
|iterations|Integer|Maximum number of iterations.|
|remap fill map|Bool|Remap to [0, 1] the filling map.|

## Detrend


The Detrend operator is used to adjust the elevation using a very basic (uncorrect) detrending operator.

![img](../images/nodes/Detrend.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Detrended heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## DiffusionLimitedAggregation


DiffusionLimitedAggregation creates branching, fractal-like structures that resemble the rugged, irregular contours of mountain ranges.

![img](../images/nodes/DiffusionLimitedAggregation.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|DiffusionLimitedAggregation heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|noise_ratio|Float| A parameter that controls the amount of randomness or noise introduced in the talus formation process.|
|scale|Float|A scaling factor that influences the density of the particles in the DLA pattern.|
|seed|Random seed|The seed for the random number generator.|
|seeding_outer_radius_ratio|Float|The ratio between the outer seeding radius and the initial seeding radius. It determines the outer boundary for particle seeding.|
|seeding_radius|Float|The radius within which initial seeding of particles occurs. This radius defines the area where the first particles are placed.|
|slope|Float|Slope of the talus added to the DLA pattern.|

## Dilation


Dilation expands the boundaries of objects in an image and fills in small gaps or holes in those objects. Use Cases: (+) Filling gaps: Dilation can be used to fill small gaps or holes in objects, making them more solid. (+) Merging objects: Dilation can help merge nearby objects or connect broken segments in an image.

![img](../images/nodes/Dilation.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Dilated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## DistanceTransform


DistanceTransform is a distance map or distance image where each pixel's value represents the shortest distance to the nearest non-zero value in the input heightmap.

![img](../images/nodes/DistanceTransform.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## Erosion


Erosion removes small structures or noise from an image, and it also shrinks the boundaries of objects in an image. Use Cases: (+) Noise reduction: Erosion can help remove small, unwanted pixels or noise from the image. (+) Separating objects: Erosion can be used to separate touching or overlapping objects in an image by shrinking their boundaries.

![img](../images/nodes/Erosion.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Dilated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## ExpandShrink


ExpandShrink is a maximum/minimum filter with a weighted kernel. It can be used to enhance or extract features while preserving the essential structure of the heightmap.

![img](../images/nodes/ExpandShrink.png)  

### Category


Filter/Recast  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kernel|Enumeration|Weighting kernel. Available values: biweight, cone, cone_smooth, cubic_pulse, disk, lorentzian, smooth_cosine, square, tricube.|
|radius|Float|Filter radius with respect to the domain size.|
|shrink|Bool|Shrink (if true) or expand (if false).|

## ExportAsset


ExportAsset exporting both the heightmap and texture to a single asset.

![img](../images/nodes/ExportAsset.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|elevation|HeightMapData|Data values for elevation.|
|texture|HeightMapRGBAData|Data values for color.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|elevation_scaling|Float|Elevation scaling.|
|export_format|Enumeration|Export format. Available values: Assimp Binary - *.assbin, Assxml Document - *.assxml, Autodesk 3DS (legacy) - *.3ds, Autodesk FBX (ascii) - *.fbx, Autodesk FBX (binary) - *.fbx, COLLADA - Digital Asset Exchange Schema - *.dae, Extensible 3D - *.x3d, GL Transmission Format (binary) - *.glb, GL Transmission Format - *.gltf, GL Transmission Format v. 2 (binary) - *.glb, GL Transmission Format v. 2 - *.gltf, Stanford Polygon Library (binary) - *.ply, Stanford Polygon Library - *.ply, Step Files - *.stp, Stereolithography (binary) - *.stl, Stereolithography - *.stl, The 3MF-File-Format - *.3mf, Wavefront OBJ format - *.obj, Wavefront OBJ format without material file - *.obj.|
|fname|Filename|Export file name.|
|max_error|Float|Maximum error (for optimized triangulated mesh).|
|mesh_type|Enumeration|Mesh type for the geometry. Available values: triangles, triangles (optimized).|

## ExportCloud


ExportCloud is an operator for exporting cloud data to a csv file.

![img](../images/nodes/ExportCloud.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|CloudData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|fname|Filename|Export file name.|

## ExportHeightmap


ExportHeightmap is an operator for exporting a heightmap in various file formats.

![img](../images/nodes/ExportHeightmap.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|fname|Filename|Export file name.|
|format|Enumeration|Export format. Available values: png (16 bit), png (8 bit), raw (16 bit, Unity).|

## ExportNormalMap


ExportNormalMap is an operator for exporting the normal map of an heightmap as a PNG image file.

![img](../images/nodes/ExportNormalMap.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|16bit|Bool|Whether the output file is a 16bit PNG file (instead of 8bit).|
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|fname|Filename|Export file name.|

## ExportPath


ExportPath is an operator for exporting path data to a csv file.

![img](../images/nodes/ExportPath.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|fname|Filename|Export file name.|

## ExportTexture


ExportTexture is a texture to a PNG image file.

![img](../images/nodes/ExportTexture.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapRGBAData|Input texture.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|16 bit|Bool|Decides whether the output is an 8 bit or a 16 bit PNG file.|
|auto_export|Bool|Decides whether the export is automatically performed when the node is updated.|
|fname|Filename|Export file name.|

## Falloff


Falloff is an operator that enforces values close to zero at the domain edges.

![img](../images/nodes/Falloff.png)  

### Category


Math/Boundaries  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|dr|HeightMapData|Displacement with respect to the domain size (radial direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|distance_function|Enumeration|Measure used for the distance calculation. Available values: Chebyshev, Euclidian, Euclidian/Chebyshev, Manhattan.|
|strength|Float|Falloff strength.|

## FillTalus


Fill the heightmap starting from the highest elevations using a regular downslope.

![img](../images/nodes/FillTalus.png)  

### Category


Operator/Transform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|noise_ratio|Float|Noise ratio, added to the downslope.|
|seed|Random seed|Random seed number.|
|slope|Float|Downslope used for filling.|

## Fold


Fold iteratively applies the absolute value function to the input field, effectively folding negative values to their positive counterparts.

![img](../images/nodes/Fold.png)  

### Category


Filter/Recast  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Folded heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|Number of successive foldings.|
|k|Float|Smoothing parameter of the smooth absolute value.|

## Gain


Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.

![img](../images/nodes/Gain.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gain|Float|Power law exponent.|

## GammaCorrection


Gamma correction involves applying a nonlinear transformation to the pixel values of the heightmap. This transformation is based on a power-law function, where each pixel value is raised to the power of the gamma value. The gamma value is a parameter that determines the degree and direction of the correction.

![img](../images/nodes/GammaCorrection.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gamma|Float|Gamma exponent.|

## GammaCorrectionLocal


Gamma correction involves applying a nonlinear transformation to the pixel values of the heightmap. For GammaCorrectionLocal, the transformation parameters are locally defined within a perimeter 'radius'.

![img](../images/nodes/GammaCorrectionLocal.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gamma|Float|Gamma exponent.|
|k|Float|Smoothing factor (typically in [0, 1]).|
|radius|Float|Filter radius with respect to the domain size.|

## GaussianDecay


Apply a Gaussian function to every values.

![img](../images/nodes/GaussianDecay.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|sigma|Float|Gaussian half-width, represents the distance from the peak of the Gaussian curve to the points where the amplitude drops to half of its maximum value.|

## GaussianPulse


GaussianPulse generates a Gaussian pulse.

![img](../images/nodes/GaussianPulse.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Gaussian heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|radius|Float|Pulse half-width.|

## Gradient


Gradient provides insight into the spatial distribution of a function's gradient, conveying both direction and intensity of change across the xy-plane

![img](../images/nodes/Gradient.png)  

### Category


Math/Gradient  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Gradient with respect to the x-direction.|
|dy|HeightMapData|Gradient with respect to the y-direction.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## GradientAngle


The gradient angle of a heightmap refers to the direction in which the elevation changes most rapidly at each point on the map. It represents the slope or inclination of the terrain surface relative to a horizontal plane

![img](../images/nodes/GradientAngle.png)  

### Category


Math/Gradient  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Gradient angle.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## GradientNorm


The gradient norm of a heightmap refers to the magnitude or intensity of the rate of change of elevation at each point on the map. It represents the steepness or slope of the terrain surface, irrespective of its direction.

![img](../images/nodes/GradientNorm.png)  

### Category


Math/Gradient  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Gradient norm.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## GradientTalus


Gradient talus refers to the local steepest downslope, or the direction in which the terrain descends most rapidly, at each point on the heightmap.

![img](../images/nodes/GradientTalus.png)  

### Category


Math/Gradient  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Gradient talus.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## HeightmapToKernel


Convert an heightmap to a kernel.

![img](../images/nodes/HeightmapToKernel.png)  

### Category


Converter  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|kernel|KernelData|Kernel.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|envelope|Bool|Determine if an enveloppe is applied to the kernel.|
|envelope_kernel|Enumeration|Envelope kernel. Available values: biweight, cone, cone_smooth, cubic_pulse, disk, lorentzian, smooth_cosine, square, tricube.|
|normalize|Bool|Normalize kernel so that the sum of the elements equals 1, preserving the overall intensity of an heightmap after convolution for instance.|
|radius|Float|Kernel radius with respect to the domain size.|

## HeightmapToMask


Convert an heightmap to a mask (remap values).

![img](../images/nodes/HeightmapToMask.png)  

### Category


Converter  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|mask|HeightMapData|Mask.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## HeightmapToRGBA


HeightmapToRGBA converts a series of heightmaps into an RGBA splatmap.

![img](../images/nodes/HeightmapToRGBA.png)  

### Category


Converter  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|R|HeightMapData|Red channel.|
|G|HeightMapData|Green channel.|
|B|HeightMapData|Blue channel.|
|A|HeightMapData|Alpha channel.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|RGBA|HeightMapRGBAData|RGBA heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## HydraulicBlur


Hydraulic Blur applied a cell-based hydraulic erosion using a nonlinear diffusion model.

![img](../images/nodes/HydraulicBlur.png)  

### Category


Erosion/Hydraulic  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k_smoothing|Float|Smoothing factor, if any.|
|radius|Float|Gaussian filter radius (with respect to a unit domain).|
|vmax|Float|Maximum elevation for the details.|

## HydraulicParticle


HydraulicParticle is a particle-based hydraulic erosion operator that simulates the erosion and sediment transport processes that occur due to the flow of water over a terrain represented by the input heightmap. This type of operator models erosion by tracking the movement of virtual particles (or sediment particles) as they are transported by water flow and interact with the terrain.

![img](../images/nodes/HydraulicParticle.png)  

### Category


Erosion/Hydraulic  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|bedrock|HeightMapData|Bedrock elevation, erosion process cannot carve the heightmap further down this point.|
|moisture|HeightMapData|Moisture map, influences the amount of water locally deposited.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
|erosion map|HeightMapData|Erosion map|
|depo. map|HeightMapData|Deposition map|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|c_capacity|Float|Particle capacity.|
|c_deposition|Float|Particle deposition coefficient.|
|c_erosion|Float|Particle erosion cofficient.|
|c_radius|Integer|Particle radius (c_radius = 0 for a pixel-based algorithm and c_radius > 0 for a kernel-based algorithm, which can be significantly slower).|
|drag_rate|Float|Particle drag rate.|
|evap_rate|Float|Particle water evaporation rate.|
|nparticles|Integer|Number of simulated particles.|
|seed|Random seed|Random seed number.|

## HydraulicRidge


HydraulicRidge .

![img](../images/nodes/HydraulicRidge.png)  

### Category


Erosion/Hydraulic  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|erosion_factor|Float|Erosion factor, generally in ]0, 10]. Smaller values tend to flatten the map.|
|intensity|Float|Intensity mask, expected in [0, 1] (applied as a post-processing).|
|noise_ratio|Float|Ridge talus noise ratio in [0, 1].|
|radius|Float|Pre-filter radius with respect to the domain size.|
|seed|Random seed|Random seed number.|
|smoothing_factor|Float|Smooothing factor in ]0, 1] (1 for no smoothing).|
|talus_global|Float|Ridge slope.|

## HydraulicStream


HydraulicStream is an hydraulic erosion operator using the flow stream approach and an infinite flow direction algorithm for simulating the erosion processes. Simulate water flow across the terrain using the infinite flow direction algorithm. As water flows over the terrain, flow accumulation representing the volume of water that passes through each point on the map, is computed to evaluate the erosive power of the water flow.

![img](../images/nodes/HydraulicStream.png)  

### Category


Erosion/Hydraulic  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|bedrock|HeightMapData|Bedrock elevation, erosion process cannot carve the heightmap further down this point.|
|moisture|HeightMapData|Moisture map, influences the amount of water locally deposited.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
|erosion map|HeightMapData|Erosion map|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|c_erosion|Float|Erosion intensity.|
|clipping_ratio|Float|Flow accumulation clipping ratio.|
|radius|Float|Carving kernel radius.|
|talus_ref|Float|Reference talus, with small values of talus_ref  leading to thinner flow streams.|

## HydraulicVpipes


HydraulicVpipes performs hydraulic erosion using a virtual pipe algorithm, which is a method that simulates erosion and sediment transport processes by mimicking the behavior of water flowing through a network of virtual pipes. This approach models erosion based on the principles of fluid dynamics and sediment transport, while also considering the local topography of the terrain represented by the input heightmap.

![img](../images/nodes/HydraulicVpipes.png)  

### Category


Erosion/Hydraulic  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|bedrock|HeightMapData|Bedrock elevation, erosion process cannot carve the heightmap further down this point.|
|moisture|HeightMapData|Moisture map, influences the amount of water locally deposited.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
|erosion map|HeightMapData|Erosion map|
|depo. map|HeightMapData|Deposition map|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|c_capacity|Float|Particle capacity.|
|c_deposition|Float|Particle deposition coefficient.|
|c_erosion|Float|Particle erosion cofficient.|
|evap_rate|Float|Particle water evaporation rate.|
|iterations|Integer|Number of simulation iterations.|
|rain_rate|Float|Water evaporation rate.|
|water_height|Float|Total water height..|

## ImportHeightmap


ImportHeightmap imports an heighmap from a grayscale PNG 8bit file.

![img](../images/nodes/ImportHeightmap.png)  

### Category


IO/Files  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapData|Heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|fname (8bit grayscale)|Filename|Import file name.|
|remap|Bool|Remap imported heightmap elevation to [0, 1].|

## Inverse


Inverse flips the sign of every values.

![img](../images/nodes/Inverse.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Binary heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## KernelGabor


.

![img](../images/nodes/KernelGabor.png)  

### Category


Primitive/Kernel  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|kernel|KernelData|KernelGabor generates a Gabor kernel.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Kernel angle.|
|kw|Float|Kernel wavenumber (spatial frequency).|
|normalize|Bool|Normalize kernel so that the sum of the elements equals 1, preserving the overall intensity of an heightmap after convolution for instance.|
|radius|Float|Kernel radius with respect to the domain size.|

## KernelPrim


.

![img](../images/nodes/KernelPrim.png)  

### Category


Primitive/Kernel  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|kernel|KernelData|KernelPrim generates a 'kernel', refering to a small matrix used to apply specific effects based on convolution for instance.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kernel|Enumeration|Kernel type. Available values: biweight, cone, cone_smooth, cubic_pulse, disk, lorentzian, smooth_cosine, square, tricube.|
|normalize|Bool|Normalize kernel so that the sum of the elements equals 1, preserving the overall intensity of an heightmap after convolution for instance.|
|radius|Float|Kernel radius with respect to the domain size.|

## KmeansClustering2


KmeansClustering2 node groups the data into clusters based on the values of the two input features.

![img](../images/nodes/KmeansClustering2.png)  

### Category


Features/Clustering  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|feature 1|HeightMapData|First measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
|feature 2|HeightMapData|Second measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Cluster labelling.|
|scoring|HeightMapVectorData|Score in [0, 1] of the cell to belong to a given cluster|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|compute_scoring|Bool|Determine whether scoring is computed.|
|nclusters|Integer|Number of clusters.|
|normalize_inputs|Bool|Determine whether the feature amplitudes are normalized before the clustering.|
|seed|Random seed|Random seed number.|
|weights.x|Float|Weight of the first feature.|
|weights.y|Float|Weight of the second feature.|

## KmeansClustering3


KmeansClustering2 node groups the data into clusters based on the values of the three input features.

![img](../images/nodes/KmeansClustering3.png)  

### Category


Features/Clustering  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|feature 1|HeightMapData|First measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
|feature 2|HeightMapData|First measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
|feature 3|HeightMapData|Third measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Cluster labelling.|
|scoring|HeightMapVectorData|Score in [0, 1] of the cell to belong to a given cluster|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|compute_scoring|Bool|Determine whether scoring is computed.|
|nclusters|Integer|Number of clusters.|
|normalize_inputs|Bool|Determine whether the feature amplitudes are normalized before the clustering.|
|seed|Random seed|Random seed number.|
|weights.x|Float|Weight of the first feature.|
|weights.y|Float|Weight of the third feature.|

## Laplace


Laplace smoothing filter reduces noise and smooth out variations in pixel intensity while preserving the overall structure of an image.

![img](../images/nodes/Laplace.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|Number of successive filtering iterations.|
|sigma|Float|Filtering strength.|

## Lerp


The Lerp operator, short for linear interpolation, is a method for smoothly transitioning between two values over a specified range or interval.

![img](../images/nodes/Lerp.png)  

### Category


Operator/Blend  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|a|HeightMapData|Start heightmap (t = 0).|
|b|HeightMapData|End heightmap (t = 1).|
|t|HeightMapData|Lerp factor, expected in [0, 1].|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Interpolated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|t|Float|Lerp factor (in [0, 1]). Used only if the node input 't' is not set.|

## MakeBinary


MakeBinary is a thresholding operator. It transforms an input heightmap into a binary heightmap, where each pixel is assigned either a value of 0 or 1, depending on whether its intensity value surpasses a specified threshold.

![img](../images/nodes/MakeBinary.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Binary heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|threshold|Float|Threshold value.|

## MakePeriodic


MakePeriodic makes the input heightmap tileable by building a smooth lineat transition at the boundaries.

![img](../images/nodes/MakePeriodic.png)  

### Category


Operator/Tiling  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Binary heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|overlap|Float|Overlap ratio at the boundaries.|

## MakePeriodicStitching


MakePeriodicStitching makes the input heightmap tileable by seamlessly stitching together the domain boundaries.

![img](../images/nodes/MakePeriodicStitching.png)  

### Category


Operator/Tiling  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Binary heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|overlap|Float|Overlap ratio at the boundaries.|

## Median3x3


Median3x3 filter is a median filter with a 3x3 kernel used to reduce noise while preserving edges in an image. This process removes outliers and smooths the image without noise reduction and feature preservation in digital images.

![img](../images/nodes/Median3x3.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## MixTexture


MixTexture enables the seamless integration of multiple textures by utilizing the alpha channel information to control the blending.

![img](../images/nodes/MixTexture.png)  

### Category


Texture  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture1|HeightMapRGBAData|Input texture.|
|texture2|HeightMapRGBAData|Input texture.|
|texture3|HeightMapRGBAData|Input texture.|
|texture4|HeightMapRGBAData|Input texture.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|texture|HeightMapRGBAData|Output texture.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|reset_output_alpha|Bool|Reset the output alpha channel to 1 (opaque).|
|use_sqrt_avg|Bool|Use square averaging for alpha-compositing (instead of linear averaging).|

## Mixer


The Mixer operator takes several input heightmaps and blend them according to a mixing parameter expected in [0, 1].

![img](../images/nodes/Mixer.png)  

### Category


Operator/Blend  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input 1|HeightMapData|Input heightmap.|
|input 2|HeightMapData|Input heightmap.|
|input 3|HeightMapData|Input heightmap.|
|input 4|HeightMapData|Input heightmap.|
|t|HeightMapData|Mixing parameter, expected in [0, 1].|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## MorphologicalGradient


MorphologicalGradient calculates the difference between dilation and erosion of an image, resulting in the outline of objects. Use Cases: Edge detection, boundary extraction.

![img](../images/nodes/MorphologicalGradient.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Gradient heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Noise


Noise node generates coherent and random-looking patterns.

![img](../images/nodes/Noise.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Value (delaunay), Value (linear), Worley, Worley (doube), Worley (value).|
|seed|Random seed|Random seed number.|

## NoiseFbm


Fractal noise is a mathematical algorithm used to generate complex and detailed patterns characterized by self-similarity across different scales.

![img](../images/nodes/NoiseFbm.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|weight|Float|Octave weighting.|

## NoiseIq


NoiseIq, variant of NoiseFbm.

![img](../images/nodes/NoiseIq.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gradient_scale|Float|Gradient influence scaling.|
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|weight|Float|Octave weighting.|

## NoiseJordan


NoiseJordan, variant of NoiseFbm.

![img](../images/nodes/NoiseJordan.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|damp0|Float|Damping initial value.|
|damp_scale|Float|Damping influence scaling.|
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|warp0|Float|Warping initial value.|
|warp_scale|Float|Warping influence scaling.|
|weight|Float|Octave weighting.|

## NoiseParberry


NoiseParberry, variant of NoiseFbm, is a Perlin noise based terrain generator from Ian Parberry, Tobler's First Law of Geography, Self Similarity, and Perlin Noise: A Large Scale Analysis of Gradient Distribution in Southern Utah with Application to Procedural Terrain Generation.

![img](../images/nodes/NoiseParberry.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|mu|Float|Gradient magnitude exponent.|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|weight|Float|Octave weighting.|

## NoisePingpong


NoisePingPong, variant of NoiseFbm.

![img](../images/nodes/NoisePingpong.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|weight|Float|Octave weighting.|

## NoiseRidged


NoiseRidged, variant of NoiseFbm.

![img](../images/nodes/NoiseRidged.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k_smoothing|Float|Smoothing coefficient of the aboslute value.|
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|weight|Float|Octave weighting.|

## NoiseSwiss


NoiseSwiss, variant of NoiseFbm.

![img](../images/nodes/NoiseSwiss.png)  

### Category


Primitive/Coherent Noise  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|kw|Wavenumber|Noise wavenumbers (kx, ky) for each directions.|
|lacunarity|Float|Wavenumber ratio between each octaves.|
|noise_type|Enumeration|Base primitive noise. Available values: OpenSimplex2, OpenSimplex2S, Perlin, Perlin (billow), Perlin (half), Value, Value (cubic), Worley, Worley (doube), Worley (value).|
|octaves|Integer|Number of octaves.|
|persistence|Float|Octave persistence.|
|seed|Random seed|Random seed number.|
|warp_scale|Float|Warping influence scaling.|
|weight|Float|Octave weighting.|

## NormalDisplacement


NormalDisplacement applies a displacement to the terrain along the normal direction.

![img](../images/nodes/NormalDisplacement.png)  

### Category


Filter/Recast  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|amount|Float|Displacement scaling.|
|iterations|Integer|Number of successive use of the operator.|
|radius|Float|Filter radius with respect to the domain size.|
|reverse|Bool|Reverse displacement direction.|

## Opening


Opening is a combination of erosion followed by dilation. It is primarily used for removing noise while preserving the shape and size of objects. Use Cases: Noise removal: Opening is effective in removing small, isolated noise regions while keeping the main objects intact. Smoothing object boundaries: Opening can help smooth the boundaries of objects in an image while preserving their overall shape.

![img](../images/nodes/Opening.png)  

### Category


Operator/Morphology  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Dilated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Paraboloid


.

![img](../images/nodes/Paraboloid.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Paraboloid.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|a|Float|Curvature parameter, first principal axis.|
|angle|Float|Angle.|
|b|Float|Curvature parameter, second principal axis.|
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|reverse_x|Bool|Reverse coefficient of first principal axis.|
|reverse_y|Bool|Reverse coefficient of second principal axis.|
|v0|Float|Value at the paraboloid center.|

## Path


Polyline definition.

![img](../images/nodes/Path.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Set of directed points (x, y) and elevations z.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|closed|Bool|Decides whether the path is open and closed on itself.|
|path|HighMap Path object|Path data.|
|reorder_nns|Bool|Decides whether the path points are reordered using a nearest neighbor search.|

## PathBezier


PathBezier uses Bezier interpolation to replace sharp angles and straight segments with smooth, flowing curves.

![img](../images/nodes/PathBezier.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|curvature_ratio|Float|Amount of curvature (usually in [-1, 1] and commonly > 0).|
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|

## PathBezierRound


PathBezierRound uses Bezier interpolation to replace sharp angles and straight segments with smooth, round and flowing curves. PathBezierRound is an alternative to PathBezier.

![img](../images/nodes/PathBezierRound.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|curvature_ratio|Float|Amount of curvature (usually in [-1, 1] and commonly > 0).|
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|

## PathBspline


PathBspline uses Bspline interpolation to replace sharp angles and straight segments with smooth, flowing curves.

![img](../images/nodes/PathBspline.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|

## PathDecasteljau


PathDecasteljau uses Decasteljau interpolation to replace sharp angles and straight segments with smooth, flowing curves.

![img](../images/nodes/PathDecasteljau.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|

## PathFractalize


PathFractalize fractalizes a polyline using a mid-point displacement algorithm. The procedure involves iteratively modifying the polyline's geometry to increase its complexity, mimicking fractal characteristics.

![img](../images/nodes/PathFractalize.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|Number of mid-point displacement iterations.|
|orientation|Integer|Displacement orientation (0 for random inward/outward displacement, 1 to inflate the path and -1 to deflate the path).|
|persistence|Float|Noise persistence (with iteration number).|
|seed|Random seed|Random seed number.|
|sigma|Float|Half-width of the random Gaussian displacement, normalized by the distance between points.|

## PathMeanderize


PathMeanderize uses Bezier interpolation to add menaders to the input path.

![img](../images/nodes/PathMeanderize.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|edge_divisions|Integer|Edge sub-divisions of each edge. After the operation, the path is remeshed based on this new sub-division.|
|iterations|Integer|Number of meandering iterations.|
|noise_ratio|Float|Randomness ratio.|
|ratio|Float|Meander amplitude ratio.|
|seed|Random seed|Random seed number|

## PathResample


PathResample resamples the path based to get (approximately) a given distance between points.

![img](../images/nodes/PathResample.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|PathData|Output path.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|delta|Float|Target distance between the points.|

## PathSDF


PathSDF evaluates the signed distance function of a polyline. It assigns a signed distance value to every point in space. For points outside the polyline, the distance is positive, while for points inside, it's negative. The zero level set of this function precisely defines the polyline's path Project path points to an heightmap.

![img](../images/nodes/PathSDF.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|sdf|HeightMapData|Signed distance as an heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## PathToCloud


PathToCloud convert a Path to a set of points (Cloud).

![img](../images/nodes/PathToCloud.png)  

### Category


Converter  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Output cloud.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## PathToHeightmap


PathToHeightmap Project path points to an heightmap.

![img](../images/nodes/PathToHeightmap.png)  

### Category


Geometry/Path  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Input path.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|heightmap|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|filled|Bool|Whether the resulting path contour is filled (input Path needs to be closed).|

## Plateau


Plateau node manipulates the elevation values to create flats, elevated regions surrounded by steep slopes.

![img](../images/nodes/Plateau.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|factor|Float|Influence the cliff elevation profile.|
|radius|Float|Filter radius with respect to the domain size.|

## QuiltingExpand


QuiltingExpand is an operator based on quilting that changes the feature wavenumber of a heightmap by modifying the frequency or scale of features present in the heightmap using a quilting-like technique. This operator allows for the synthesis of heightmaps with different levels of detail or spatial frequency content.

![img](../images/nodes/QuiltingExpand.png)  

### Category


Operator/Expand  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Synthetized heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|expansion_ratio|Float|Determine the features expansion ratio of the new heightmap.|
|filter_width_ratio|Float|Smooth filter width with respect the overlap length.|
|overlap|Float|Overlap ratio between patches.|
|patch_flip|Bool|Allow patch flipping.|
|patch_rotate|Bool|Allow patch 90 degree rotation.|
|patch_transpose|Bool|Allow patch tranposition.|
|patch_width|Float|Decide on the size (with respect to the domain size) of the patches that will be used to create the new heightmap. This determines the level of detail and texture in the final result.|
|seed|Random seed|Random seed number.|

## QuiltingShuffle


QuiltingShuffle performs a quilting procedure and generates a new image by seamlessly stitching together patches from an existing image.

![img](../images/nodes/QuiltingShuffle.png)  

### Category


Operator/Resynthesis  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Synthetized heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|filter_width_ratio|Float|Smooth filter width with respect the overlap length.|
|overlap|Float|Overlap ratio between patches.|
|patch_flip|Bool|Allow patch flipping.|
|patch_rotate|Bool|Allow patch 90 degree rotation.|
|patch_transpose|Bool|Allow patch tranposition.|
|patch_width|Float|Decide on the size (with respect to the domain size) of the patches that will be used to create the new heightmap. This determines the level of detail and texture in the final result.|
|seed|Random seed|Random seed number.|

## RadialDisplacementToXy


RadialDisplacementToXy interprets the input array dr as a radial displacement and convert it to a pair of displacements dx and dy in cartesian coordinates.

![img](../images/nodes/RadialDisplacementToXy.png)  

### Category


Math  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Radial displacement.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement for  the x-direction.|
|dy|HeightMapData|Displacement for  the y-direction.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|smoothing|Float|Smoothing parameter to avoid discontinuity at the origin.|

## RecastCliff


RecastCliff add cliffs in a heightmap by introducing sharp changes in elevation to simulate steep vertical terrain features such as cliffs or escarpments.

![img](../images/nodes/RecastCliff.png)  

### Category


Filter/Recast  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|amplitude|Float|Cliff amplitude.|
|gain|Float|Gain, influence the cliff elevation profile.|
|radius|Float|Filter influence radius.|
|talus_global|Float|Reference talus at which the cliff are added.|

## RecastSag


RecastSag add cliffs in a heightmap by introducing sinks, droops, or bends downward changes in elevation.

![img](../images/nodes/RecastSag.png)  

### Category


Filter/Recast  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k|Float|Smoothing parameter.|
|vref|Float|Reference elevation for the folding.|

## RecurveKura


RecurveKura is an operator based on the Kumaraswamy distribution that can be used to adjust the amplitude levels of a dataset.

![img](../images/nodes/RecurveKura.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|a|Float|Primarily controls the rightward skewness and tail behavior of the distribution.|
|b|Float|Primarily controls the leftward skewness and tail behavior.|

## RecurveS


RecurveS applied a curve adjustment filter using a smooth S-shape curve.

![img](../images/nodes/RecurveS.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## RelativeElevation


RelativeElevation identifies heightmap relative elevation based on the surrounding heightmap values.

![img](../images/nodes/RelativeElevation.png)  

### Category


Features/Landform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Relative elevation.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Remap


The Remap operator is used to adjust the elevation values across the entire dataset, effectively changing the range of elevation data to match a desired output scale or to normalize the data.

![img](../images/nodes/Remap.png)  

### Category


Filter/Range  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Remapped heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|remap|Value range|Target range: define the new minimum and maximum values to remap the elevation values to.|

## Rescale


The Rescale operator involves adjusting each data point by multiplying it with a predetermined constant.

![img](../images/nodes/Rescale.png)  

### Category


Filter/Range  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Rescaled heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|centered|Bool|Determine whether a mean offset is applied to the values to center the scaling.|
|scaling|Float|Constant by which each elevation values will be multiplied.|

## ReverseMidpoint


ReverseMidpoint generates an heightmap. It uses a polyline as a base, then recursively interpolate and displace midpoints to generate a terrain.

![img](../images/nodes/ReverseMidpoint.png)  

### Category


Primitive/Authoring  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Set of points (x, y) and elevations z.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|heightmap|HeightMapData|Interpolated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|noise_scale|Float|Added noise scaling.|
|seed|Random seed|Random seed number.|

## Ridgelines


Ridgelines generates an heightmap assuming the input path defines a ridgeline.

![img](../images/nodes/Ridgelines.png)  

### Category


Primitive/Authoring  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|path|PathData|Set of points (x, y) and elevations z.|
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|heightmap|HeightMapData|Interpolated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k_smoothing|Float|Smoothing intensity.|
|talus_global|Float|Ridge slope.|
|vmin|Float|Minimum value (lower values are clamped).|
|width|Float|Ridge edge width.|

## Rift


Rift is function used to represent a conceptualized rift.

![img](../images/nodes/Rift.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the width parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Rift heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle.|
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|sharp_bottom|Bool|Decide whether the rift bottom is sharp or not.|
|slope|Float|Rift slope.|
|width|Float|Rift width.|

## Rugosity


Rugosity identifies heightmap rugosity, i.e. the roughness or irregularity of the surface.

![img](../images/nodes/Rugosity.png)  

### Category


Features  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Rugosity.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|clamp_max|Bool|Decides whether the rugosity values are clamped.|
|radius|Float|Filter radius with respect to the domain size.|
|vc_max|Float|Rugosity clamping upper bound.|

## Saturate


Saturate limits the amplitude of a signal to a predefined range while keeping the initial input range.

![img](../images/nodes/Saturate.png)  

### Category


Filter/Recurve  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Saturated heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k_smoothing|Float|Smoothing intensity.|
|range|Value range|Saturation range: define the minimum and maximum values of the saturation process.|

## ScanMask


ScanMask adjusts the brightness and contrast of an input mask.

![img](../images/nodes/ScanMask.png)  

### Category


Mask/Adjust  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Adjusted heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|brightness|Float|Brightness adjustment.|
|contrast|Float|Contrast adjustment.|

## SelectAngle


SelectAngle is a thresholding operator. It selects angle values within a specified range defined by the shape of a Gaussian pulse.

![img](../images/nodes/SelectAngle.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Selection center value.|
|radius|Float|Pre-filter radius.|
|sigma|Float|Selection half-width.|

## SelectBlobLog


SelectBlobLog performs 'blob' detection using oa Laplacian of Gaussian (log) method. Blobs are areas in an image that are either brighter or darker than the surrounding areas.

![img](../images/nodes/SelectBlobLog.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Detection radius with respect to the domain size.|

## SelectCavities


SelectCavities analyzes the curvature of an heightmap to identify concave or convex features such as valleys, depressions, ridges, or peaks.

![img](../images/nodes/SelectCavities.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|concave|Bool|Decides whether concave or convex features are detected.|
|radius|Float|Detection radius with respect to the domain size.|

## SelectElevationSlope


SelectElevationSlope select regions based a combinaison of elevation and slope values.

![img](../images/nodes/SelectElevationSlope.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gradient_scale|Float|Gradient scaling, with respect to elevation.|

## SelectGt


SelectGt is a thresholding operator. It transforms an input heightmap into a binary heightmap, where each pixel is assigned either a value of 0 or 1, depending on whether its intensity value surpasses a specified threshold value.

![img](../images/nodes/SelectGt.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|value|Float|Selection value.|

## SelectInterval


SelectInterval is a thresholding operator. It transforms an input heightmap into a binary heightmap, where each pixel is assigned either a value of 0 or 1, depending on whether its intensity value is within an interval of values.

![img](../images/nodes/SelectInterval.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|value1|Float|Selection value, lower bound.|
|value2|Float|Selection value, upper bound.|

## SelectInwardOutward


SelectInwardOutward returns values > 0.5 if the slope is pointing to the center (slope is inward), and values < 0.5 otherwise (slope is outward).

![img](../images/nodes/SelectInwardOutward.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Reference center x coordinate.|
|center.y|Float|Reference center y coordinate.|

## SelectPulse


SelectPulse is a thresholding operator. It selects values within a specified range defined by the shape of a Gaussian pulse.

![img](../images/nodes/SelectPulse.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|sigma|Float|Selection half-width.|
|value|Float|Selection center value.|

## SelectRivers


SelectRivers is a thresholding operator. It creates a mask for river systems based on a flow accumulation threshold.

![img](../images/nodes/SelectRivers.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## SelectTransitions


SelectTransitions returns a mask filled with 1 at the blending transition between two heightmaps, and 0 elsewhere.

![img](../images/nodes/SelectTransitions.png)  

### Category


Mask/Selector  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input 1|HeightMapData|Input heightmap 1.|
|input 2|HeightMapData|Input heightmap 2.|
|blend|HeightMapData|Blended heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Mask heightmap (in [0, 1]).|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## SetAlpha


SetAlpha adjusts the transparency of a texture based on an input alpha value, which can either be a single scalar or an array. When provided with a scalar alpha value, the function uniformly adjusts the transparency of the entire texture. Alternatively, when given an array of alpha values, it enables fine-grained control over the transparency of different parts of the texture, allowing for varied opacity across the texture's surface.

![img](../images/nodes/SetAlpha.png)  

### Category


Texture  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|in|HeightMapRGBAData|Input texture.|
|alpha|HeightMapData|Transparency (expected to be in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|out|HeightMapRGBAData|Texture with new transparency.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|alpha|Float|Transparency as a scalar value (overriden if this alpha input is set).|
|clamp_alpha|Bool|Clamp to [0, 1] to input alpha map.|

## ShapeIndex


ShapeIndex is a measure used to quantify the shape complexity of landforms in an heightmap. It is calculated based on the second derivatives of the elevation surface. The surface index is greater than 0.5 for convex surface and lower than 0.5 for concave surface.

![img](../images/nodes/ShapeIndex.png)  

### Category


Features/Landform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Shape index.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## SharpenCone


SharpenCone is a cone kernel-based sharpen operator enhancing sharpness by emphasizing edges and fine details using a radial gradient shape.

![img](../images/nodes/SharpenCone.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|
|scale|Float|Sharpening intensity.|

## ShiftElevation


The ShiftElevation operator involves adjusting each data point by adding it with a predetermined constant.

![img](../images/nodes/ShiftElevation.png)  

### Category


Filter/Range  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|shift|Float|Constant by which each elevation values will be added.|

## Slope


Slope is function used to represent continuous terrain slope.

![img](../images/nodes/Slope.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Slope heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle.|
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|talus_global|Float|Slope slope...|

## SmoothCpulse


Smoothing using a cubic pulse kernel can be considered somewhat similar to Gaussian smoothing. This technique is used to reduce noise and smooth data. The cubic pulse kernel has a cubic decrease in influence with distance within a finite interval. It is zero beyond a certain radius, providing a compact support that affects only nearby points.

![img](../images/nodes/SmoothCpulse.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## SmoothFill


SmoothFill is a smoothing technique that takes the maximum between the input and the filtered field to simulate a sediment deposition effect.

![img](../images/nodes/SmoothFill.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
|depo. map|HeightMapData|Deposition map.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|k|Float|Smoothing intensity of the maximum operator.|
|normalized_map|Bool|Decides if the deposition map is normalized.|
|radius|Float|Filter radius with respect to the domain size.|

## SmoothFillHoles


SmoothFillHoles is a smoothing technique that restricts smoothing effect to convex regions.

![img](../images/nodes/SmoothFillHoles.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## SmoothFillSmearPeaks


SmoothFillSmearPeaks is a smoothing technique that restricts smoothing effect to concave regions.

![img](../images/nodes/SmoothFillSmearPeaks.png)  

### Category


Filter/Smoothing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Smoothstep3


Apply a 3rd-order smoothstep function to every values.

![img](../images/nodes/Smoothstep3.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## Smoothstep5


Apply a 5th-order smoothstep function to every values.

![img](../images/nodes/Smoothstep5.png)  

### Category


Math/Base  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## Stamping


Stamping .

![img](../images/nodes/Stamping.png)  

### Category


Primitive/Authoring  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|cloud|CloudData|Stamping locations and intensities (as a Cloud).|
|kernel|KernelData|Kernel to be stamped.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Output heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|blend_method|Enumeration|Blending method. Available values: add, maximum, minimum, multiply, substract.|
|k_smoothing|Float|Smoothing parameter (if any).|
|kernel_flip|Bool|Randomly flip, or not, the kernel before stamping (includes tranposing).|
|kernel_radius|Float|Kernel base radius, with respect a unit square domain.|
|kernel_rotate|Bool|Randomly rotate, or not, the kernel before stamping (can be any rotation angle, can also be ressource consuming).|
|kernel_scale_amplitude|Bool|Determine whether the kernel amplitude is scaled with the point values.|
|kernel_scale_radius|Bool|Determine whether the kernel radius is scaled with the point values.|
|seed|Random seed|Random seed number.|

## Step


Step is function used to represent a conceptualized escarpment, it serves as a tool for creating sharp, distinct changes in elevation.

![img](../images/nodes/Step.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
|control|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Step heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle.|
|center.x|Float|Center x coordinate.|
|center.y|Float|Center y coordinate.|
|slope|Float|Step slope.|

## Stratify


Stratify adds horizontal stratifications to the input heightmap.

![img](../images/nodes/Stratify.png)  

### Category


Erosion/Stratify  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|noise|HeightMapData|Local elevation noise, value range expected to be scaled with the one of the input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|gamma|Float|Reference value for the gamma correction applied to each strata, influence the cliff elevation profile.|
|gamma_noise|Float|Noise range for the gamma value.|
|n_strata|Integer|Numbner of strata.|
|seed|Random seed|Random seed number.|
|strata_noise|Float|Noise range for the strata elevations.|

## StratifyOblique


StratifyOblique adds oblique stratifications to the input heightmap.

![img](../images/nodes/StratifyOblique.png)  

### Category


Erosion/Stratify  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|noise|HeightMapData|Local elevation noise, value range expected to be scaled with the one of the input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|gamma|Float|Reference value for the gamma correction applied to each strata, influence the cliff elevation profile.|
|gamma_noise|Float|Noise range for the gamma value.|
|n_strata|Integer|Numbner of strata.|
|seed|Random seed|Random seed number.|
|strata_noise|Float|Noise range for the strata elevations.|
|talus_global|Float|Step slope.|

## Thermal


Thermal is an erosion operator used to simulate the process of thermal erosion, which is a type of erosion that occurs due to temperature fluctuations causing the breakdown and movement of soil and rock materials.

![img](../images/nodes/Thermal.png)  

### Category


Erosion/Thermal  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|bedrock|HeightMapData|Bedrock elevation, erosion process cannot carve the heightmap further down this point.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
|depo. map|HeightMapData|Deposition map|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|Number of iterations.|
|talus_global|Float|Repose slope.|

## ThermalRib


ThermalRib performs thermal erosion using a 'rib' algorithm (taken from Geomorph)

![img](../images/nodes/ThermalRib.png)  

### Category


Erosion/Thermal  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|bedrock|HeightMapData|Bedrock elevation, erosion process cannot carve the heightmap further down this point.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Eroded heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|iterations|Integer|Number of iterations.|

## Translate


Translates an heightmap by a specified amount along the x and y axes. This function shifts the contents of the input array by `dx` and `dy` units along the x and y axes, respectively. It supports both periodic boundary conditions, where the array wraps around, and non-periodic conditions.

![img](../images/nodes/Translate.png)  

### Category


Operator/Transform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dx|HeightMapData|Displacement with respect to the domain size (y-direction).|
|dy|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Translate heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|dx|Float|The translation distance along the x-axis. Positive values shift the array to the right.|
|dy|Float|The translation distance along the y-axis. Positive values shift the array downward.|
|periodic|Bool|If set to `true`, the translation is periodic, meaning that elements that move out of one side of the array reappear on the opposite side.|

## Unpack


KmeansClustering2 node groups the data into clusters based on the values of the two input features.

![img](../images/nodes/Unpack.png)  

### Category


Routing  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|feature 1|HeightMapData|First measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
|feature 2|HeightMapData|Second measurable property or characteristic of the data points being analyzed (e.g elevation, gradient norm, etc...|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Cluster labelling.|
|scoring|HeightMapVectorData|Score in [0, 1] of the cell to belong to a given cluster|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|compute_scoring|Bool|Determine whether scoring is computed.|
|nclusters|Integer|Number of clusters.|
|normalize_inputs|Bool|Determine whether the feature amplitudes are normalized before the clustering.|
|seed|Random seed|Random seed number.|
|weights.x|Float|Weight of the first feature.|
|weights.y|Float|Weight of the second feature.|

## Unsphericity


Unsphericity is a measure used to quantify the deviation of the heightmap shape from a perfect sphere. In other words, it indicates how much the terrain differs from being perfectly round or spherical.

![img](../images/nodes/Unsphericity.png)  

### Category


Features/Landform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Shape index.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## ValleyWidth


ValleyWidth identifies valley lines and measure the width of the valley at each cross-section.

![img](../images/nodes/ValleyWidth.png)  

### Category


Features/Landform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Valley width heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|radius|Float|Filter radius with respect to the domain size.|

## Warp


The Warp node transforms a base heightmap by warping/pushing pixels as defined by the input displacements.

![img](../images/nodes/Warp.png)  

### Category


Operator/Transform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|dx|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dy|HeightMapData|Displacement with respect to the domain size (y-direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Warped heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |

## WarpDownslope


Warp the heightmap with a direction and amount based on the local downslope.

![img](../images/nodes/WarpDownslope.png)  

### Category


Operator/Transform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|mask|HeightMapData|Mask defining the filtering intensity (expected in [0, 1]).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|amount|Float|Warp scaling with respect to the gradient.|
|radius|Float|Prefiltering radius for gradient computation.|
|reverse|Bool|Reverse warping direction (upslope if true and downslope otherwise).|

## WaveDune


WaveDune mimics using a periodic function the formation and spatial distribution of transverse sand dunes on a terrain.

![img](../images/nodes/WaveDune.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Displacement with respect to the domain size (normal direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|WaveDune heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|
|xbottom|Float|Relative position of the dune bottom, in [0, 1].|
|xtop|Float|Relative position of the dune top, in [0, 1].|

## WaveSine


WaveSine generates sine waves.

![img](../images/nodes/WaveSine.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Displacement with respect to the domain size (normal direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|WaveSine heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|

## WaveSquare


WaveSquare generates square waves.

![img](../images/nodes/WaveSquare.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Displacement with respect to the domain size (normal direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|WaveSquare heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|

## WaveTriangular


WaveTriangular generates triangular waves.

![img](../images/nodes/WaveTriangular.png)  

### Category


Primitive/Function  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|dr|HeightMapData|Displacement with respect to the domain size (normal direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|WaveTriangular heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|angle|Float|Angle in the horizontal plane.|
|kw|Float|Noise wavenumbers (kx, ky) for each directions.|
|phase_shift|Float|Phase shift.|
|slant_ratio|Float|Decides on wave asymmetry, expected in [0, 1].|

## White


White noise operator generates a random signal with a flat power spectral density.

![img](../images/nodes/White.png)  

### Category


Primitive/Random  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|seed|Random seed|Random seed number.|

## WhiteDensityMap


WhiteDensityMap noise operator generates a random signal with a flat power and a spatial density defined by an input heightmap.

![img](../images/nodes/WhiteDensityMap.png)  

### Category


Primitive/Random  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|density|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|seed|Random seed|Random seed number.|

## WhiteSparse


WhiteSparse noise operator generates a random signal with a flat power spectral density, but with a sparse spatial density.

![img](../images/nodes/WhiteSparse.png)  

### Category


Primitive/Random  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|envelope|HeightMapData|Output noise amplitude envelope.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Generated noise.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|density|Float|Noise density.|
|seed|Random seed|Random seed number.|

## ZeroedEdges


ZeroedEdges is an operator that enforces values close to zero at the domain edges.

![img](../images/nodes/ZeroedEdges.png)  

### Category


Math/Boundaries  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Input heightmap.|
|dr|HeightMapData|Displacement with respect to the domain size (radial direction).|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Filtered heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|distance_function|Enumeration|Measure used for the distance calculation. Available values: Chebyshev, Euclidian, Euclidian/Chebyshev, Manhattan.|
|sigma|Float|Shape power law.|

## Zoom


Applies a zoom effect to an heightmap with an adjustable center. This function scales the input 2D array by a specified zoom factor, effectively resizing the array's contents. The zoom operation is centered around a specified point within the array, allowing for flexible zooming behavior.

![img](../images/nodes/Zoom.png)  

### Category


Operator/Transform  

### Inputs

|Name|Type|Description|
| :--- | :--- | :--- |
|input|HeightMapData|Displacement with respect to the domain size (x-direction).|
|dx|HeightMapData|Displacement with respect to the domain size (y-direction).|
|dy|HeightMapData|Control parameter, acts as a multiplier for the weight parameter.|
  

### Outputs

|Name|Type|Description|
| :--- | :--- | :--- |
|output|HeightMapData|Zoom heightmap.|
  

### Parameters

|Name|Type|Description|
| :--- | :--- | :--- |
|center.x|Float|Center of the zoom operation.|
|center.y|Float|Center of the zoom operation.|
|periodic|Bool|If set to `true`, the zoom is periodic.|
|zoom_factor|Float|The factor by which to zoom the heightmap.|
