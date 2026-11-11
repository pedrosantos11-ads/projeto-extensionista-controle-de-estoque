#pragma once

#include "repositorio.hpp"

class SistemaEstoque {
public:
    bool carregarTudo() {
        return repositorio.carregarEstoque(estoque) &&
               repositorio.carregarVendas(vendas);
    }

    bool salvarTudo() const {
        const bool estoqueOk = repositorio.salvarEstoque(estoque);
        const bool vendasOk = repositorio.salvarVendas(vendas);
        return estoqueOk && vendasOk;
    }

    bool salvarApenasEstoque() const {
        return repositorio.salvarEstoque(estoque);
    }

    int encontrarItem(const string& nome) const {
        const string alvo = aparar(nome);
        for (size_t i = 0; i < estoque.size(); ++i) {
            if (estoque[i].nome == alvo) return static_cast<int>(i);
        }
        return -1;
    }

    int encontrarItemPorId(int id) const {
        for (size_t i = 0; i < estoque.size(); ++i) {
            if (estoque[i].id == id) return static_cast<int>(i);
        }
        return -1;
    }

    bool adicionarItem(int id, const string& nome, const string& categoria,
                       int quantidade, const string& observacao, string& erro) {
        if (id <= 0) { erro = "O ID deve ser maior que zero."; return false; }
        const string nomeLimpo = aparar(nome);
        if (nomeLimpo.empty()) { erro = "Informe o nome do item."; return false; }
        if (aparar(categoria).empty()) { erro = "Informe a categoria do item."; return false; }
        if (quantidade < 0) { erro = "A quantidade não pode ser negativa."; return false; }
        if (encontrarItemPorId(id) >= 0) { erro = "Já existe um item com esse ID."; return false; }

        estoque.push_back({id, nomeLimpo, aparar(categoria), quantidade, aparar(observacao)});
        return repositorio.salvarEstoque(estoque) ? true : (erro = "Não foi possível gravar o estoque.", false);
    }

    bool editarItem(size_t indice, const string& categoria, int quantidade,
                    const string& observacao, string& erro) {
        if (indice >= estoque.size()) { erro = "Item inválido."; return false; }
        if (aparar(categoria).empty()) { erro = "Informe a categoria do item."; return false; }
        if (quantidade < 0) { erro = "A quantidade não pode ser negativa."; return false; }
        estoque[indice].quantidade = quantidade;
        estoque[indice].categoria = aparar(categoria);
        estoque[indice].observacao = aparar(observacao);
        return repositorio.salvarEstoque(estoque) ? true : (erro = "Não foi possível gravar o estoque.", false);
    }

    bool removerItem(size_t indice, string& nomeRemovido, string& erro) {
        if (indice >= estoque.size()) {
            erro = "Item inválido.";
            return false;
        }

        const ItemEstoque itemRemovido = estoque[indice];
        nomeRemovido = itemRemovido.nome;

        estoque.erase(estoque.begin() + static_cast<ptrdiff_t>(indice));

        if (!repositorio.salvarEstoque(estoque)) {
            estoque.insert(estoque.begin() + static_cast<ptrdiff_t>(indice), itemRemovido);
            erro = "Não foi possível gravar o estoque. O item não foi removido.";
            return false;
        }

        return true;
    }

    bool registrarVenda(size_t indice, int quantidade, const string& cliente, string& erro) {
        if (indice >= estoque.size()) { erro = "Item não encontrado no estoque."; return false; }
        if (quantidade <= 0) { erro = "A quantidade vendida deve ser maior que zero."; return false; }
        const string clienteLimpo = aparar(cliente);
        if (clienteLimpo.empty()) { erro = "Informe o nome do cliente."; return false; }

        const int vendida = min(quantidade, estoque[indice].quantidade);
        estoque[indice].quantidade -= vendida;
        vendas.push_back({estoque[indice].nome, quantidade, clienteLimpo});

        if (!repositorio.salvarEstoque(estoque) || !repositorio.salvarVendas(vendas)) {
            erro = "Não foi possível salvar a venda e o estoque.";
            return false;
        }
        return true;
    }

    bool editarVenda(size_t indice, int quantidade, const string& cliente, string& erro) {
        if (indice >= vendas.size()) { erro = "Venda inválida."; return false; }
        if (quantidade <= 0 || aparar(cliente).empty()) { erro = "Preencha os dados corretamente."; return false; }
        vendas[indice].quantidade = quantidade;
        vendas[indice].cliente = aparar(cliente);
        return repositorio.salvarVendas(vendas) ? true : (erro = "Não foi possível gravar as vendas.", false);
    }

    bool removerVenda(size_t indice, string& erro) {
        if (indice >= vendas.size()) { erro = "Venda inválida."; return false; }
        vendas.erase(vendas.begin() + static_cast<ptrdiff_t>(indice));
        return repositorio.salvarVendas(vendas) ? true : (erro = "Não foi possível gravar as vendas.", false);
    }

    bool renomearCliente(const string& antigo, const string& novo, string& erro) {
        const string novoLimpo = aparar(novo);
        if (novoLimpo.empty()) { erro = "Informe um nome válido."; return false; }
        for (auto& venda : vendas) if (venda.cliente == antigo) venda.cliente = novoLimpo;
        return repositorio.salvarVendas(vendas) ? true : (erro = "Não foi possível gravar as vendas.", false);
    }

    vector<ItemRanking> rankingItens() const {
        unordered_map<string, int> totais;
        for (const auto& venda : vendas) totais[venda.item] += venda.quantidade;
        vector<ItemRanking> ranking;
        ranking.reserve(totais.size());
        for (const auto& [nome, total] : totais) ranking.push_back({nome, total});
        sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
            return a.totalVendido > b.totalVendido;
        });
        return ranking;
    }

    vector<ClienteRanking> rankingClientes() const {
        unordered_map<string, int> totais;
        for (const auto& venda : vendas) totais[venda.cliente] += venda.quantidade;
        vector<ClienteRanking> ranking;
        ranking.reserve(totais.size());
        for (const auto& [nome, total] : totais) ranking.push_back({nome, total});
        sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
            return a.totalComprado > b.totalComprado;
        });
        return ranking;
    }

    int quantidadeItensBaixos() const {
        return static_cast<int>(count_if(estoque.begin(), estoque.end(), [](const auto& item) {
            return item.quantidade < config::LIMITE_ESTOQUE_BAIXO;
        }));
    }

    int totalUnidades() const {
        int total = 0;
        for (const auto& item : estoque) total += item.quantidade;
        return total;
    }

    const vector<ItemEstoque>& getEstoque() const { return estoque; }
    const vector<Venda>& getVendas() const { return vendas; }

private:
    Repositorio repositorio;
    vector<ItemEstoque> estoque;
    vector<Venda> vendas;
};
