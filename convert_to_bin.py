Import("env")
import re
import os

# Path to version.h
version_file = "Core/Inc/version.h"

# Default values
major = minor = patch = 0

# We look for the necessary defines in version.h
try:
    with open(version_file, "r") as f:
        for line in f:
            if "#define VERSION_MAJOR" in line:
                major = int(re.search(r"\d+", line).group())
            elif "#define VERSION_MINOR" in line:
                minor = int(re.search(r"\d+", line).group())
            elif "#define VERSION_PATCH" in line:
                patch = int(re.search(r"\d+", line).group())
except FileNotFoundError:
    print(f"WARNING: {version_file} not found, using version 0.0.0")

# Build the output file name
version_str = f"{major}.{minor}.{patch}"
firmware_dir = "Firmware"
profile_name = env.get("PIOENV", "unknown")
output_bin = os.path.join(firmware_dir, f"{profile_name}_v{version_str}.bin")



# Make sure the Firmware folder exists.
os.makedirs(firmware_dir, exist_ok=True)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary",
        "$BUILD_DIR/${PROGNAME}.elf", output_bin
    ]), f"Creating {output_bin}")
)
