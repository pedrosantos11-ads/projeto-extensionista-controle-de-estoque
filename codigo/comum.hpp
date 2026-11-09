#pragma once

#include <ftxui/ftxui.hpp>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ftxui;
using namespace std;

namespace config {
constexpr int LIMITE_ESTOQUE_BAIXO = 10;
const string ARQ_ESTOQUE = "estoque.txt";
const string ARQ_VENDAS = "vendas_semana.txt";
}

struct ItemEstoque {
    string nome;
    int quantidade = 0;
    string observacao;
};

struct Venda {
    string item;
    int quantidade = 0;
    string cliente;
};

struct ClienteRanking {
    string nome;
    int totalComprado = 0;
};

struct ItemRanking {
    string nome;
    int totalVendido = 0;
};

struct ResultadoFormulario {
    bool confirmado = false;
    vector<string> valores;
};
