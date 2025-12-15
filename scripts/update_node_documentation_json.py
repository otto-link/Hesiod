import json
import os

HSD_DATA_PATH = "Hesiod/data"

STD_PARAM = {
    "seed":
    "Random seed number. The random seed is an offset to the randomized process. A different seed will produce a new result.",
    "kw":
    "Base spatial frequencies in the X and Y directions. The frequencies are defined with respect to the entire domain: for example, kw = 2 produces two full oscillations across the domain width (and similarly for the Y direction).",
    "lacunarity":
    "The frequency scaling factor for successive noise octaves. Higher values increase the frequency of each successive octave.",
    "octaves":
    "The number of octaves for fractal noise generation. More octaves add finer details to the terrain.",
    "persistence":
    "The amplitude scaling factor for subsequent noise octaves. Lower values reduce the contribution of higher octaves.",
    "weight":
    "Controls how much higher FBM octaves contribute to the noise based on local elevation. A higher weight suppresses high-frequency octaves at low elevations and increases their influence at higher elevations, producing terrain where fine details appear mainly near peaks while lower areas remain smoother.",
    #
    "post_gain":
    "Set the gain. Gain is a power law transformation altering the distribution of signal values, compressing or expanding certain regions of the signal depending on the exponent of the power law.",
    "post_inverse":
    "Inverts the output values after processing, flipping low and high values across the midrange.",
    "post_mix":
    "Mixing factor for blending input and output values. A value of 0 uses only the input, 1 uses only the output, and intermediate values perform a linear interpolation.",
    "post_mix_method":
    "Method used to combine input and output values. Options include linear interpolation (default), min, max, smooth min, smooth max, add, and subtract.",
    "post_remap":
    "Linearly remaps the output values to a specified target range (default is [0, 1]).",
    "post_saturate":
    "Modifies the amplitude of elevations by first clamping them to a given interval and then scaling them so that the restricted interval matches the original input range. This enhances contrast in elevation variations while maintaining overall structure.",
    "post_smoothing_radius":
    "Defines the radius for post-processing smoothing, determining the size of the neighborhood used to average local values and reduce high-frequency detail. A radius of 0 disables smoothing.",
    #
    "mask_activate":
    "Enables or disables the internal mask. If the node's 'mask' input is connected, this setting is bypassed and the input mask is used instead.",
    "mask_gain":
    "Controls the intensity or influence of the internal mask. Bypassed if the 'mask' input is connected.",
    "mask_inverse":
    "Inverts the internal mask, applying the operator where the mask is low. Ignored if a 'mask' input is provided.",
    "mask_radius":
    "Defines the smoothing radius for the internal mask. A value of 0 disables smoothing. This is bypassed if the 'mask' input is used.",
    "mask_type":
    "Specifies how the internal mask is computed: 'Elevation' uses height, 'Gradient Norm' uses slope, and 'Elevation mid-range' selects the middle portion of the height range. This parameter is ignored when a 'mask' input is connected."
}

STD_PORTS = {
    "water_depth":
    "Output water depth map representing flooded areas.",
    "dx":
    "Displacement with respect to the domain size (x-direction).",
    "dy":
    "Displacement with respect to the domain size (y-direction).",
    "envelope":
    "Heightmap used as a post-process amplitude multiplier for the generated noise."
}


def load_node_data():
    """Load node documentation data from a JSON file."""
    fname = os.path.join(HSD_DATA_PATH, "node_documentation.json")
    with open(fname, "r") as f:
        return json.load(f)


if __name__ == "__main__":
    data = load_node_data()

    for k, v in data.items():
        print(k)

        # make sure "standard" descriptions are set
        if v["parameters"] is not None:
            for kp, vp in v["parameters"].items():
                if kp in STD_PARAM.keys():
                    data[k]["parameters"][kp]["description"] = STD_PARAM[kp]

        if v["ports"] is not None:
            for kp, vp in v["ports"].items():
                if kp in STD_PORTS.keys():
                    data[k]["ports"][kp]["description"] = STD_PORTS[kp]
                    print(kp)

    with open(os.path.join(HSD_DATA_PATH, "node_documentation.json"),
              'w') as f:
        json.dump(data, f)
