import re

cpp_function = """
Array watershed_ridge(
    const Array        &z,
    Array              *p_mask,
    float               amplitude = 0.2f,
    float               width = 32.f,
    float               edt_exponent = 0.5f,
    FlowDirectionMethod fd_method = FlowDirectionMethod::FDM_D8,
    const Array        *p_noise_x = nullptr,
    const Array        *p_noise_y = nullptr,
    const Array        *p_scaling = nullptr);
"""

# ---------------------------
# PARSING
# ---------------------------
# match type, pointer *, name
pattern = r"(?:const\s+)?([\w:<>]+)\s*(\*?)\s*&?\s*(\w+)(?:\s*=\s*[^,]*)?,?"
matches = re.findall(pattern, cpp_function)

arrays = []
attrs = []

for t, ptr, name in matches:
    name_clean = name.strip()
    # pointer Array* → port
    if t == "Array" and ptr == "*":
        if name_clean.startswith("p_"):
            name_clean = name_clean[2:]  # strip p_
        arrays.append(name_clean)
    # normal Array& → port
    elif t == "Array":
        arrays.append(name_clean)
    # numeric / bool → attribute
    elif t in ["float", "int", "bool"]:
        attrs.append((t, name_clean))
    else:
        pass  # ignore other types

# ---------------------------
# GENERATE CONSTEXPR
# ---------------------------
print("// --- Ports (all Array)")
for name in arrays:
    name_upper = name.upper()
    print(f'constexpr const char* P_{name_upper} = "{name}";')

print("\n// --- Attributes")
for t, name in attrs:
    name_upper = name.upper()
    print(f'constexpr const char* A_{name_upper} = "{name}";')

# ---------------------------
# GENERATE node.add_port / add_attr
# ---------------------------
print("\n// --- Node setup")
for name in arrays:
    print(
        f'node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_{name.upper()});'
    )

print()
for t, name in attrs:
    if t == "bool":
        print(
            f'node.add_attr<BoolAttribute>(A_{name.upper()}, "{name}", false);'
        )
    elif t == "int":
        print(
            f'node.add_attr<IntAttribute>(A_{name.upper()}, "{name}", 0, 0, 100);'
        )
    else:
        print(
            f'node.add_attr<FloatAttribute>(A_{name.upper()}, "{name}", 0.f, 0.f, 1.f);'
        )

# ---------------------------
# GENERATE params lambda
# ---------------------------
print("\n// --- Params lambda")
print("const auto params = [&node]()")
print("{")
print("    struct P")
print("    {")
for t, name in attrs:
    cpp_type = "bool" if t == "bool" else ("int" if t == "int" else "float")
    print(f"        {cpp_type} {name};")
print("    };")
print("    return P{")
for t, name in attrs:
    cpp_attr = "BoolAttribute" if t == "bool" else (
        "IntAttribute" if t == "int" else "FloatAttribute")
    print(f"        .{name} = node.get_attr<{cpp_attr}>(A_{name.upper()}),")
print("    };")
print("}();")
