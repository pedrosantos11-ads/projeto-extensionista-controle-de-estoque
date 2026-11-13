#pragma once

#include "util.hpp"

class Repositorio {
public:
    bool carregarEstoque(vector<ItemEstoque>& estoque) const {
        estoque.clear();
        ifstream arquivo(config::ARQ_ESTOQUE);
        if (!arquivo.is_open()) return true;

        int proximoId = 1;
        string linha;
        while (getline(arquivo, linha)) {
            if (aparar(linha).empty()) continue;
            const auto campos = separar(linha, '|');
            if (campos.size() < 2) continue;

            int id = 0;
            int quantidade = 0;
            string nome;
            string categoria;
            string observacao;

            if (campos.size() >= 5) {
                if (!inteiroValido(campos[0], id) || id <= 0 ||
                    !inteiroValido(campos[3], quantidade)) {
                    continue;
                }
                nome = aparar(campos[1]);
                categoria = aparar(campos[2]);
                observacao = aparar(campos[4]);
            } else {
                if (!inteiroValido(campos[1], quantidade)) continue;
                nome = aparar(campos[0]);
                observacao = campos.size() >= 3 ? aparar(campos[2]) : "";
                do {
                    id = proximoId++;
                } while (any_of(estoque.begin(), estoque.end(), [id](const auto& item) {
                    return item.id == id;
                }));
            }

            if (quantidade < 0) continue;
            if (nome.empty()) continue;
            if (any_of(estoque.begin(), estoque.end(), [id](const auto& item) {
                return item.id == id;
            })) {
                continue;
            }

            ItemEstoque item{id, nome, categoria, quantidade, observacao};
            estoque.push_back(item);
            proximoId = max(proximoId, id + 1);
        }
        sort(estoque.begin(), estoque.end(), [](const auto& a, const auto& b) {
            return a.id < b.id;
        });
        return !arquivo.bad();
    }

    bool salvarEstoque(const vector<ItemEstoque>& estoque) const {
        return salvar(config::ARQ_ESTOQUE, [&](ofstream& arquivo) {
            for (const auto& item : estoque) {
                arquivo << item.id << '|'
                        << limparSeparador(item.nome) << '|'
                        << limparSeparador(item.categoria) << '|'
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
