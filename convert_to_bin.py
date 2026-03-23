Import("env")
import re
import os

# Путь к version.h
version_file = "Core/Inc/version.h"

# Значения по умолчанию
major = minor = patch = 0
revision = "unknown"

# Ищем нужные define'ы в version.h
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

# Читаем BOARD_REVISION из build_flags
build_flags = env.get("BUILD_FLAGS", [])
for flag in build_flags:
    # Ищем -DBOARD_REVISION=BOARD_REV_X_Y
    m = re.search(r"-DBOARD_REVISION=BOARD_REV_(\d+)_(\d+)", flag)
    if m:
        revision = f"{m.group(1)}.{m.group(2)}"
        break

# Формируем имя файла
version_str = f"{major}.{minor}.{patch}"
firmware_dir = "Firmware"
profile_name = env.get("PIOENV", "unknown")
output_bin = os.path.join(firmware_dir, f"{profile_name}_v{version_str}_rev{revision}.bin")



# Убедимся, что папка Firmware существует
os.makedirs(firmware_dir, exist_ok=True)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary",
        "$BUILD_DIR/${PROGNAME}.elf", output_bin
    ]), f"Creating {output_bin}")
)
