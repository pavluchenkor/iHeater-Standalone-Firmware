Import("env")
import re
import os

# Путь к version.h
version_file = "Core/Inc/version.h"
revision_file = "Core/Inc/config.h"

# Значения по умолчанию
major = minor = patch = 0

# Ищем нужные define'ы
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


# version_str = f"{major}.{minor}.{patch}"
# firmware_dir = "Firmware"
# output_bin = os.path.join(firmware_dir, f"iHeater_v{version_str}.bin")

# Читаем ревизию платы
try:
    with open(revision_file, "r") as f:
        for line in f:
            # Находим строку с BOARD_REVISION
            m = re.search(r"#define\s+BOARD_REVISION\s+BOARD_REV_(\d+)_(\d+)", line)
            if m:
                revision = f"{m.group(1)}.{m.group(2)}"
                break
except FileNotFoundError:
    print(f"WARNING: {revision_file} not found, BOARD_REVISION unknown")

# Формируем имя файла
version_str = f"{major}.{minor}.{patch}"
firmware_dir = "Firmware"
output_bin = os.path.join(firmware_dir, f"iHeater_v{version_str}_rev{revision}.bin")



# Убедимся, что папка Firmware существует
os.makedirs(firmware_dir, exist_ok=True)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary",
        "$BUILD_DIR/${PROGNAME}.elf", output_bin
    ]), f"Creating {output_bin}")
)