import os
import shutil
from pathlib import Path

Import("env") # type: ignore

def move_firmware_bin(source, target, env):
    # Определяем путь к исходному файлу firmware.bin
    firmware_bin = Path(env.subst("$BUILD_DIR")) / "firmware.bin"
    
    # Проверяем, существует ли файл firmware.bin
    if not firmware_bin.exists():
        print(f"File {firmware_bin} not found!")
        return
    
    # Создаем целевую директорию /bin, если она не существует
    output_dir = Path("bin")
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Перемещаем файл firmware.bin в директорию /bin
    destination = output_dir / firmware_bin.name
    shutil.move(firmware_bin, destination)
    print(f"Moved {firmware_bin} to {destination}")

# Регистрируем функцию для выполнения после сборки
env.AddPostAction("$BUILD_DIR/firmware.bin", move_firmware_bin) # type: ignore