# Projeto Extensionista: Controle de Estoque

Sistema de controle de estoque e vendas desenvolvido em C++17 com a biblioteca
[FTXUI](https://github.com/ArthurSonzogni/FTXUI).

O sistema possui interface de terminal, persistência dos dados em arquivos de
texto e módulos separados para facilitar a manutenção e a evolução do projeto.

## Funcionalidades

- Cadastro de itens no estoque.
- Listagem e visualização detalhada dos itens.
- Edição e remoção de itens.
- Identificação de itens em nível crítico ou baixo.
- Registro de vendas.
- Histórico, edição e remoção de vendas.
- Ranking de itens mais vendidos.
- Tabela de clientes por quantidade comprada.
- Renomeação de clientes.
- Painel com resumo do estoque e das vendas.
- Salvamento dos dados em arquivos locais.

## Estrutura do projeto

```text
.
├── codigo/
│   ├── comum.hpp
│   ├── repositorio.hpp
│   ├── sistema_estoque.hpp
│   ├── telas_estoque.cpp
│   ├── telas_estoque.hpp
│   ├── telas_vendas.cpp
│   ├── telas_vendas.hpp
│   ├── ui.cpp
│   ├── ui.hpp
│   ├── util.cpp
│   ├── util.hpp
│   └── main.cpp
├── estoque.txt
├── vendas_semana.txt
├── CMakeLists.txt
└── README.md
```

### `comum.hpp`

Centraliza os tipos compartilhados pelo sistema, como `ItemEstoque`, `Venda`,
`ClienteRanking` e `ItemRanking`, além das configurações dos arquivos de dados e
do limite de estoque baixo.

### `util.cpp` e `util.hpp`

Contêm funções auxiliares utilizadas por vários módulos:

- remoção de espaços desnecessários;
- tratamento do separador dos arquivos;
- divisão de linhas em campos;
- validação de números inteiros.

### `repositorio.hpp`

Implementa a persistência dos dados. O módulo carrega e salva o estoque e as
vendas nos arquivos `estoque.txt` e `vendas_semana.txt`.

### `sistema_estoque.hpp`

Contém as regras de negócio e o estado do sistema. É responsável por adicionar,
editar e remover itens, registrar e editar vendas, gerar rankings e calcular os
resumos exibidos no painel.

### `ui.cpp` e `ui.hpp`

Concentram os componentes reutilizáveis da interface FTXUI, como títulos,
cartões de resumo, mensagens, confirmações e formulários.

### `telas_estoque.cpp` e `telas_estoque.hpp`

Implementam as telas e os menus relacionados ao estoque:

- cadastro;
- listagem;
- edição;
- remoção;
- itens críticos;
- menu de estoque.

### `telas_vendas.cpp` e `telas_vendas.hpp`

Implementam as telas e os menus relacionados às vendas:

- registro;
- histórico;
- edição e remoção;
- ranking de vendas;
- tabela de clientes;
- edição de clientes;
- menu de vendas.

### `main.cpp`

Inicializa o sistema, carrega os dados, exibe o menu principal e controla o
fluxo geral da aplicação.

## Como o projeto foi construído

O código original possuía todas as funções em um único arquivo. A
modularização separou as responsabilidades sem alterar os nomes das funções,
variáveis ou o comportamento do sistema:

1. Os tipos e configurações foram centralizados em `comum.hpp`.
2. As funções auxiliares foram movidas para `util.cpp`.
3. A leitura e gravação dos arquivos ficaram em `repositorio.hpp`.
4. As regras de negócio ficaram em `sistema_estoque.hpp`.
5. Os componentes reutilizáveis da interface ficaram em `ui.cpp`.
6. As telas foram divididas entre estoque e vendas.
7. O fluxo principal permaneceu em `main.cpp`.

Após a separação, o projeto foi recompilado e comparado com a implementação
original. Os arquivos de dados foram mantidos idênticos.

## Requisitos

- CMake 3.16 ou superior;
- compilador compatível com C++17;
- Git;
- conexão com a internet na primeira configuração, para baixar o FTXUI.

## Compilação e execução

Na raiz do projeto, execute:

```bash
cmake -S . -B build
cmake --build build
./build/sistema
```

O `CMakeLists.txt` usa `FetchContent` para baixar automaticamente o FTXUI
versão `v7.0.3` durante a configuração, caso a opção padrão seja mantida.
Depois do primeiro download, a dependência fica armazenada em `build/_deps`.

Para usar uma instalação local do FTXUI em vez de baixar a dependência:

```bash
cmake -S . -B build \
  -DFTXUI_USE_SYSTEM_PACKAGE=ON \
  -DCMAKE_PREFIX_PATH="/caminho/para/ftxui"
cmake --build build
./build/sistema
```

## Arquivos de dados

Os dados são salvos na mesma pasta em que o programa é executado:

- `estoque.txt`: itens, quantidades e observações;
- `vendas_semana.txt`: item vendido, quantidade e cliente.

Por isso, execute o programa a partir da raiz do projeto para que os arquivos
sejam encontrados corretamente.

## Licença

Consulte o arquivo `LICENSE` para as condições de uso deste projeto.
