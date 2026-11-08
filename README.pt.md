[English](README.md) | [Español](README.es.md) | **Português**

# ShuraFechditor

Editor de perfis de cor para o [fastfetch](https://github.com/fastfetch-cli/fastfetch) no terminal.

Permite escolher uma imagem PNG e uma cor para cada um dos 4 grupos de módulos do fastfetch, visualizar o resultado em truecolor e salvar o perfil em `colores.conf`.

> **Status:** projeto em desenvolvimento. A interface, o carregamento da paleta e a pré-visualização já funcionam. O salvamento de perfis ainda não está implementado.

## Recursos

- [x] Fluxo interativo pelo terminal
- [x] Carregamento da paleta a partir de `color.options.colors`
- [x] Pré-visualização em truecolor (ANSI 24-bit)
- [x] Validação de entradas
- [ ] Salvamento de perfis em `colores.conf`
- [ ] Cópia/renomeação da imagem para `logos/`
- [ ] Verificação de consistência entre perfis e imagens
- [ ] Validação real de PNG
- [ ] Testes
- [ ] Script de instalação

## Requisitos

- Linux (testado no CachyOS/Arch)
- Compilador com C++17 (`g++` ou `clang++`)
- CMake >= 3.16
- Terminal com suporte a cor truecolor
- `fastfetch` (para usar os perfis gerados)

## Compilação

```bash
git clone https://github.com/Sura578/ShuraFechditor.git
cd ShuraFechditor/code

cmake -S . -B build
cmake --build build
```

O executável fica em `code/build/shurafechditor`.

O CMake copia `color.options.colors`, `colores.conf` e `config.jsonc` para a pasta `build/` para poder executar a partir dela.

## Uso

```bash
cd code
./build/shurafechditor
```

Fluxo:

1. Informe o caminho de uma imagem `.png`.
2. Escolha uma cor (0-8) para cada grupo:
   - **Grupo 1:** OS, kernel, packages, shell
   - **Grupo 2:** WM, lm, icons, terminal
   - **Grupo 3:** host, CPU, GPU, disk, memory, uptime, display
   - **Grupo 4:** sound, player, media
3. Confira a pré-visualização.
4. Confirme com `y` ou cancele com `n`.

> ⚠️ Atualmente o passo 4 mostra "New profile documents created", mas o salvamento real ainda é um stub. Está no roadmap.

## Formatos de arquivo

### `color.options.colors`

Paleta disponível. Cada linha tem o formato `índice NOME #RRGGBB`.

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

Um perfil por linha: número da imagem + 4 cores hexadecimais (uma por grupo).

```
# imagem | grupo1 | grupo2 | grupo3 | grupo4
1  #8dbfca  #f9a2c5  #eec2d6  #96cdd9
```

O `config.jsonc` usa `%%C1%%`, `%%C2%%`, `%%C3%%` e `%%C4%%` para atribuir cada módulo ao seu grupo de cor.

## Estrutura do projeto

```
code/
├── CMakeLists.txt
├── main.cpp
├── ui.h / ui.cpp              # interface e fluxo
├── palette.h / palette.cpp    # carregamento de color.options.colors
├── profilemaker.h / .cpp      # lógica de perfis (pendente)
├── color.options.colors       # paleta
├── colores.conf               # perfis
└── config.jsonc               # configuração do fastfetch
```

## Roadmap

- [x] Interface pelo terminal
- [x] Carregamento da paleta e pré-visualização
- [ ] `profilesaver()`: escrever `colores.conf` e copiar a imagem
- [ ] `profilechecker()`: detectar perfis e imagens ausentes
- [ ] `imagechecker()`: validar PNG real (assinatura e dimensões)
- [ ] Tratamento de erros e casos limite
- [ ] Testes
- [ ] Documentação de instalação

## Contribuição

Issues e pull requests são bem-vindos. Se encontrar um bug, abra uma issue com os passos para reproduzi-lo.

## Licença

Este projeto ainda não tem uma licença definida.

## Autor

- **Sura578** ([@Sura578](https://github.com/Sura578))
