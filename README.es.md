[English](README.md) | **Español** | [Português](README.pt.md)

# ShuraFechditor

Editor de perfiles de color para [fastfetch](https://github.com/fastfetch-cli/fastfetch) en terminal.

Permite elegir una imagen PNG y un color para cada uno de los 4 grupos de módulos de fastfetch, previsualizar el resultado en truecolor y guardar el perfil en `colores.conf`.

> **Estado:** proyecto en desarrollo. La interfaz, la carga de la paleta y el preview ya funcionan. El guardado de perfiles todavía no está implementado.

## Características

- [x] Flujo interactivo por terminal
- [x] Carga de la paleta desde `color.options.colors`
- [x] Preview en truecolor (ANSI 24-bit)
- [x] Validación de entradas
- [ ] Guardado de perfiles en `colores.conf`
- [ ] Copia/renombrado de la imagen a `logos/`
- [ ] Verificación de consistencia entre perfiles e imágenes
- [ ] Validación real de PNG
- [ ] Tests
- [ ] Script de instalación

## Requisitos

- Linux (probado en CachyOS/Arch)
- Compilador con C++17 (`g++` o `clang++`)
- CMake >= 3.16
- Terminal con soporte de color truecolor
- `fastfetch` (para usar los perfiles generados)

## Compilación

```bash
git clone https://github.com/Sura578/ShuraFechditor.git
cd ShuraFechditor/code

cmake -S . -B build
cmake --build build
```

El ejecutable queda en `code/build/shurafechditor`.

CMake copia `color.options.colors`, `colores.conf` y `config.jsonc` a la carpeta `build/` para poder ejecutar desde ahí.

## Uso

```bash
cd code
./build/shurafechditor
```

Flujo:

1. Indica la ruta de una imagen `.png`.
2. Elige un color (0-8) para cada grupo:
   - **Grupo 1:** OS, kernel, packages, shell
   - **Grupo 2:** WM, lm, icons, terminal
   - **Grupo 3:** host, CPU, GPU, disk, memory, uptime, display
   - **Grupo 4:** sound, player, media
3. Revisa el preview.
4. Confirma con `y` o cancela con `n`.

> ⚠️ Actualmente el paso 4 muestra "New profile documents created", pero el guardado real todavía es un stub. Está en el roadmap.

## Formatos de archivo

### `color.options.colors`

Paleta disponible. Cada línea tiene el formato `índice NOMBRE #RRGGBB`.

```
0 RED #F4A6A6
1 ORANGE #F8C49A
2 YELLOW #F9E7A5
3 GREEN #B9DDBB
4 BLUE #A9D5E8
5 INDIGO #B5B8E8
6 VIOLET #D8B5E5
7 WHITE #FFFFFF
8 BLACK #333333
```

### `colores.conf`

Un perfil por línea: número de imagen + 4 colores hexadecimales (uno por grupo).

```
# imagen | grupo1 | grupo2 | grupo3 | grupo4
1  #8dbfca  #f9a2c5  #eec2d6  #96cdd9
```

`config.jsonc` usa `%%C1%%`, `%%C2%%`, `%%C3%%` y `%%C4%%` para asignar cada módulo a su grupo de color.

## Estructura del proyecto

```
code/
├── CMakeLists.txt
├── main.cpp
├── ui.h / ui.cpp              # interfaz y flujo
├── palette.h / palette.cpp    # carga de color.options.colors
├── profilemaker.h / .cpp      # lógica de perfiles (pendiente)
├── color.options.colors       # paleta
├── colores.conf               # perfiles
└── config.jsonc               # configuración de fastfetch
```

## Roadmap

- [x] Interfaz por terminal
- [x] Carga de paleta y preview
- [ ] `profilesaver()`: escribir `colores.conf` y copiar la imagen
- [ ] `profilechecker()`: detectar perfiles e imágenes faltantes
- [ ] `imagechecker()`: validar PNG real (firma y dimensiones)
- [ ] Manejo de errores y casos borde
- [ ] Tests
- [ ] Documentación de instalación

## Contribuir

Las issues y pull requests son bienvenidas. Si encuentras un bug, abre una issue con los pasos para reproducirlo.

## Licencia

Este proyecto todavía no tiene una licencia definida.

## Autor

- **Sura578** ([@Sura578](https://github.com/Sura578))
