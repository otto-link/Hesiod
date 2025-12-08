# Hesiod Coordinate System — Scale, Distances, Angles, and Slopes

Hesiod uses a fully normalized spatial framework to ensure that all node operations behave consistently at any resolution and in any graph configuration. This section explains how **scale**, **distances**, **horizontal angles**, and **vertical slopes** are defined across the system.

## Normalized Space

All spatial computations in Hesiod assume the heightmap lives inside a **unit square**:

```
  y
  ↑
  1 +------------------------------+
    |                              |
    |          Unit Square         |
    |     normalized domain        |
    |        [0,1] × [0,1]         |
    |                              |
    +------------------------------+ → x
            0                    1
```

Coordinates, distances, directions, and slopes all use this normalized domain.

### Benefits

* Resolution-independent node behavior
* Predictable, composable graphs
* CPU/GPU alignment
* World scale applied only at export

# Distances in Normalized Coordinates

Distances are measured in normalized units.
For points `p1 = (x1, y1)` and `p2 = (x2, y2)`:

```
distance = length(p2 - p1)
```

Typical values:

* `0.0` → same point
* `1.0` → full width or height of the heightmap

Nodes that use radii, falloff distances, or frequencies always reference normalized distance.

# Angles vs. Slopes

Hesiod separates two concepts:

* **Horizontal Angle** → defines orientation in the 2D heightmap plane
* **Vertical Slope** → defines elevation change per normalized distance

These must not be confused.

## Horizontal Angles (Directions on the Map Plane)

Horizontal angles describe **direction only**, not incline. Angles are measured in degrees. Horizontal angles are used in:

* Directional gradients
* Anisotropic noise
* Any node needing a direction vector (`cos(angle)`, `sin(angle)`)

Angles **never** represent vertical slope angles.

## Vertical Slopes (Elevation Change per Normalized Distance)

Elevation changes in Hesiod use **dimensionless slopes**, not angles.

### Definition

```
slope = Δelevation / Δdistance
```

Distance is measured in normalized space.

### Interpretation

A slope of **1.0** means:

```
Across a normalized distance of 1.0,
elevation increases by 1.0.
```

Examples:

* `slope = 0.5` → rises by 0.5 across the domain
* `slope = 2.0` → rises twice the elevation range
* `slope = -1.0` → decreases by 1.0

### Why not use slope angles?

A “30° incline” depends on world units (meters, terrain scale). Hesiod is unit-agnostic, so slope *angles* would be ambiguous. Normalized slope values remain consistent at any resolution.

## Directional Slopes (Combining Angle + Slope)

Some nodes apply slope in a particular direction.

* **Angle** → direction in the 2D plane
* **Slope** → elevation change per unit distance in that direction


# World Scale are Never Used

During graph computation:

* No meters, kilometers, or physical slopes exist
* All computations happen in normalized space

# Summary Table

| Concept                     | Meaning                        | Example / Notes                           |
| --------------------------- | ------------------------------ | ----------------------------------------- |
| **Normalized Coordinates**  | (x, y) ∈ [0,1]²                | Universal domain                          |
| **Normalized Distance**     | Euclidean metric in unit space | `1.0` = full domain width                 |
| **Horizontal Angle**        | Direction in the 2D plane      | `cos/sin` direction vector                |
| **Vertical Slope**          | Δelevation per Δdistance       | Slope `1.0` = rise of `1.0` across domain |
| **Slope Angle (Forbidden)** | Physical incline angle         | Not used (requires world units)           |
| **Directional Slope**       | Angle + slope                  | Gradient ramps, directional fields        |
| **World Scale**             | Never used         | Never affects graph logic                 |

