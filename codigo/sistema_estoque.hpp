#pragma once

#include "repositorio.hpp"

class SistemaEstoque {
public:
    bool carregarTudo() {
        if (!repositorio.carregarEstoque(estoque) ||
            !repositorio.carregarVendas(vendas)) {
            return false;
        }

        for (auto& venda : vendas) {
            if (venda.produtoId > 0) {
                const int indice = encontrarItemPorId(venda.produtoId);
                if (indice >= 0) venda.item = estoque[static_cast<size_t>(indice)].nome;
                continue;
            }

            const int indice = encontrarItem(venda.item);
            if (indice >= 0) {
                venda.produtoId = estoque[static_cast<size_t>(indice)].id;
                venda.item = estoque[static_cast<size_t>(indice)].nome;
            }
        }
        return true;
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
        sort(estoque.begin(), estoque.end(), [](const auto& a, const auto& b) {
            return a.id < b.id;
        });
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
        if (existeVendaDoItem(itemRemovido)) {
            erro = "Não é possível remover um item que possui vendas registradas.";
            return false;
        }
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
        if (quantidade > estoque[indice].quantidade) {
            erro = "A quantidade vendida não pode ser maior que o estoque disponível.";
            return false;
        }

        const auto estoqueAnterior = estoque;
        const auto vendasAnteriores = vendas;

        estoque[indice].quantidade -= quantidade;
        vendas.push_back({estoque[indice].id, estoque[indice].nome, quantidade, clienteLimpo});

        if (!salvarTudo()) {
            estoque = estoqueAnterior;
            vendas = vendasAnteriores;
            repositorio.salvarEstoque(estoque);
            repositorio.salvarVendas(vendas);
            erro = "Não foi possível salvar a venda e o estoque.";
            return false;
        }
        return true;
    }

    bool editarVenda(size_t indice, int quantidade, const string& cliente, string& erro) {
        if (indice >= vendas.size()) { erro = "Venda inválida."; return false; }
        if (quantidade <= 0 || aparar(cliente).empty()) { erro = "Preencha os dados corretamente."; return false; }

        const int indiceProduto = encontrarProdutoDaVenda(vendas[indice]);
        if (indiceProduto < 0) {
            erro = "O produto desta venda não está mais disponível no estoque.";
            return false;
        }

        const auto estoqueAnterior = estoque;
        const auto vendasAnteriores = vendas;
        ItemEstoque& item = estoque[static_cast<size_t>(indiceProduto)];
        item.quantidade += vendas[indice].quantidade;
        if (quantidade > item.quantidade) {
            estoque = estoqueAnterior;
            erro = "A nova quantidade não pode ser maior que o estoque disponível.";
            return false;
        }

        item.quantidade -= quantidade;
        vendas[indice].quantidade = quantidade;
        vendas[indice].cliente = aparar(cliente);
        if (!salvarTudo()) {
            estoque = estoqueAnterior;
            vendas = vendasAnteriores;
            repositorio.salvarEstoque(estoque);
            repositorio.salvarVendas(vendas);
            erro = "Não foi possível salvar a edição da venda e do estoque.";
            return false;
        }
        return true;
    }

    bool removerVenda(size_t indice, string& erro) {
        if (indice >= vendas.size()) { erro = "Venda inválida."; return false; }

        const int indiceProduto = encontrarProdutoDaVenda(vendas[indice]);
        if (indiceProduto < 0) {
            erro = "O produto desta venda não está mais disponível no estoque.";
            return false;
        }

        const auto estoqueAnterior = estoque;
        const auto vendasAnteriores = vendas;
        estoque[static_cast<size_t>(indiceProduto)].quantidade += vendas[indice].quantidade;
        vendas.erase(vendas.begin() + static_cast<ptrdiff_t>(indice));
        if (!salvarTudo()) {
            estoque = estoqueAnterior;
            vendas = vendasAnteriores;
            repositorio.salvarEstoque(estoque);
            repositorio.salvarVendas(vendas);
            erro = "Não foi possível remover a venda e atualizar o estoque.";
            return false;
        }
        return true;
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
    int encontrarProdutoDaVenda(const Venda& venda) const {
        if (venda.produtoId > 0) return encontrarItemPorId(venda.produtoId);
        return encontrarItem(venda.item);
    }

    bool existeVendaDoItem(const ItemEstoque& item) const {
        return any_of(vendas.begin(), vendas.end(), [&](const auto& venda) {
            return (venda.produtoId > 0 && venda.produtoId == item.id) ||
                   (venda.produtoId == 0 && venda.item == item.nome);
        });
    }

    Repositorio repositorio;
    vector<ItemEstoque> estoque;
    vector<Venda> vendas;
};
