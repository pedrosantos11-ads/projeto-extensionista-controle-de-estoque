#pragma once

#include "util.hpp"

class Repositorio {
public:
    bool carregarEstoque(vector<ItemEstoque>& estoque) const {
        estoque.clear();
        ifstream arquivo(config::ARQ_ESTOQUE);
        if (!arquivo.is_open()) return true;

        string linha;
        while (getline(arquivo, linha)) {
            if (aparar(linha).empty()) continue;
            const auto campos = separar(linha, '|');
            if (campos.size() < 2) continue;

            int quantidade = 0;
            if (!inteiroValido(campos[1], quantidade) || quantidade < 0) continue;

            const string nome = aparar(campos[0]);
            if (nome.empty()) continue;

            ItemEstoque item{nome, quantidade, campos.size() >= 3 ? aparar(campos[2]) : ""};
            estoque.push_back(item);
        }
        return !arquivo.bad();
    }

    bool salvarEstoque(const vector<ItemEstoque>& estoque) const {
        return salvar(config::ARQ_ESTOQUE, [&](ofstream& arquivo) {
            for (const auto& item : estoque) {
                arquivo << limparSeparador(item.nome) << '|'
                        << item.quantidade << '|'
                        << limparSeparador(item.observacao) << '\n';
            }
        });
    }

    bool carregarVendas(vector<Venda>& vendas) const {
        vendas.clear();
        ifstream arquivo(config::ARQ_VENDAS);
        if (!arquivo.is_open()) return true;

        string linha;
        while (getline(arquivo, linha)) {
            if (aparar(linha).empty()) continue;
            const auto campos = separar(linha, '|');
            if (campos.size() < 3) continue;

            int quantidade = 0;
            if (!inteiroValido(campos[1], quantidade) || quantidade <= 0) continue;

            const string item = aparar(campos[0]);
            const string cliente = aparar(campos[2]);
            if (item.empty() || cliente.empty()) continue;

            vendas.push_back({item, quantidade, cliente});
        }
        return !arquivo.bad();
    }

    bool salvarVendas(const vector<Venda>& vendas) const {
        return salvar(config::ARQ_VENDAS, [&](ofstream& arquivo) {
            for (const auto& venda : vendas) {
                arquivo << limparSeparador(venda.item) << '|'
                        << venda.quantidade << '|'
                        << limparSeparador(venda.cliente) << '\n';
            }
        });
    }

private:
    template <typename Funcao>
    bool salvar(const string& caminho, Funcao escrever) const {
        ofstream arquivo(caminho, ios::out | ios::trunc);
        if (!arquivo.is_open()) return false;
        escrever(arquivo);
        return arquivo.good();
    }
};
