#include "telas_estoque.hpp"

// ============================================================
// Telas de estoque
// ============================================================
void cadastrarItem(SistemaEstoque& sistema) {
    const auto r = formulario("ADICIONAR ITEM AO ESTOQUE", {
        {"ID do item", ""}, {"Nome do item", ""}, {"Categoria", ""},
        {"Quantidade", "0"}, {"Observação/Nota", ""}
    });
    if (!r.confirmado) return;

    int id = 0;
    int quantidade = 0;
    if (!inteiroValido(r.valores[0], id) || id <= 0) {
        mensagem("Erro", "O ID deve ser um número inteiro maior que zero.", true);
        return;
    }
    if (!inteiroValido(r.valores[3], quantidade) || quantidade < 0) {
        mensagem("Erro", "A quantidade deve ser um número inteiro maior ou igual a zero.", true);
        return;
    }
    string erro;
    if (!sistema.adicionarItem(id, r.valores[1], r.valores[2], quantidade, r.valores[4], erro)) {
        mensagem("Erro", erro, true);
        return;
    }
    mensagem("Salvo com sucesso", "Item adicionado em estoque.");
}

void listarEstoque(SistemaEstoque& sistema) {
    const auto& estoque = sistema.getEstoque();
    if (estoque.empty()) { mensagem("Estoque vazio", "Nenhum item cadastrado."); return; }

    vector<string> linhas;
    for (const auto& item : estoque) {
        linhas.push_back("ID: " + to_string(item.id) + " | " + item.nome +
                         " | Categoria: " + item.categoria +
                         " | Qtd: " + to_string(item.quantidade) +
                         " | " + (item.quantidade < config::LIMITE_ESTOQUE_BAIXO ? "CRÍTICO" :
                                   item.quantidade < config::LIMITE_ESTOQUE_BAIXO * 2 ? "BAIXO" : "OK") +
                         " | Nota: " + (item.observacao.empty() ? "-" : item.observacao));
    }

    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] {
        return vbox({titulo("ESTOQUE ATUAL"),
                      text("CRÍTICO < 10 | BAIXO 10-19 | OK >= 20") | center,
                      separator(), menu->Render() | frame | vscroll_indicator | border | flex,
                      separator(), text("Enter = detalhes | Esc = voltar") | color(Color::GrayDark) | center}) |
               border;
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const auto& item = sistema.getEstoque()[static_cast<size_t>(selecionado)];
            mensagem("Detalhes do item", "Item: " + item.nome +
                     "\nID: " + to_string(item.id) +
                     "\nCategoria: " + item.categoria +
                     "\nQuantidade: " + to_string(item.quantidade) +
                     "\nStatus: " + (item.quantidade < config::LIMITE_ESTOQUE_BAIXO ? "CRÍTICO" :
                                        item.quantidade < config::LIMITE_ESTOQUE_BAIXO * 2 ? "BAIXO" : "OK") +
                     "\nObservação: " + (item.observacao.empty() ? "Nenhuma" : item.observacao));
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

void editarEstoque(SistemaEstoque& sistema) {
    if (sistema.getEstoque().empty()) { mensagem("Estoque vazio", "Não há itens para editar."); return; }

    vector<string> nomes;
    for (const auto& item : sistema.getEstoque()) nomes.push_back(item.nome);
    int selecionado = 0;
    auto menu = Menu(&nomes, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] {
        return vbox({titulo("EDITAR ESTOQUE"), text("Selecione o item") | center, separator(),
                      menu->Render() | border | flex, separator(),
                      text("Enter = editar | Esc = voltar") | color(Color::GrayDark) | center}) | border;
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const size_t indice = static_cast<size_t>(selecionado);
            const auto item = sistema.getEstoque()[indice];
            tela.Exit();
            const auto r = formulario("EDITAR ITEM", {
                {"Categoria", item.categoria},
                {"Quantidade", to_string(item.quantidade)},
                {"Observação/Nota", item.observacao}
            });
            if (!r.confirmado) return true;
            int quantidade = 0;
            if (!inteiroValido(r.valores[1], quantidade) || quantidade < 0) {
                mensagem("Erro", "Quantidade inválida.", true);
                return true;
            }
            string erro;
            if (!sistema.editarItem(indice, r.valores[0], quantidade, r.valores[2], erro)) mensagem("Erro", erro, true);
            else mensagem("Salvo com sucesso", quantidade < config::LIMITE_ESTOQUE_BAIXO ? "ATENÇÃO: item em nível crítico." : "Item atualizado com sucesso.");
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

// FIX: Remoção com atualização dinâmica da tela sem quebrar a navegação
void removerItem(SistemaEstoque& sistema) {
    while (true) {
        if (sistema.getEstoque().empty()) {
            mensagem("Estoque vazio", "Não há itens para remover.");
            return;
        }

        vector<string> nomes;
        for (const auto& item : sistema.getEstoque()) {
            nomes.push_back("ID: " + to_string(item.id) + " | " + item.nome +
                            " (Qtd: " + to_string(item.quantidade) + ")");
        }

        int selecionado = 0;
        auto menu = Menu(&nomes, &selecionado);
        auto tela = ScreenInteractive::Fullscreen();
        
        bool voltarMenu = false;

        auto renderer = Renderer(menu, [&] {
            return vbox({
                titulo("REMOVER ITEM DO ESTOQUE"),
                text("Selecione o item que deseja excluir definitivamente") | center,
                separator(),
                menu->Render() | border | flex,
                separator(),
                text("Enter = remover | Esc = voltar ao menu") | color(Color::GrayDark) | center
            }) | border;
        });

        renderer = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Escape) {
                voltarMenu = true;
                tela.Exit();
                return true;
            }
            if (event == Event::Return) {
                const size_t idx = static_cast<size_t>(selecionado);
                const string nomeProduto = sistema.getEstoque()[idx].nome;

                if (confirmarAcao("Confirmação de Exclusão", "Você realmente deseja excluir este produto (" + nomeProduto + ")?")) {
                    string nomeRemovido, erro;
                    if (sistema.removerItem(idx, nomeRemovido, erro)) {
                        mensagem("Remoção concluída",
                                 nomeRemovido + " foi removido definitivamente do estoque.");
                    } else {
                        mensagem("Erro ao remover", erro, true);
                    }
                } else {
                    // Se escolher NAO, nada é alterado.
                    mensagem("Remoção cancelada",
                             "O item continua no estoque.");
                }

                tela.Exit();
                return true;
            }
            return false;
        });

        tela.Loop(renderer);

        if (voltarMenu) break;
    }
}

void itensCriticos(SistemaEstoque& sistema) {
    vector<string> linhas;
    for (const auto& item : sistema.getEstoque())
        if (item.quantidade < config::LIMITE_ESTOQUE_BAIXO)
            linhas.push_back(item.nome + " | Quantidade: " + to_string(item.quantidade));

    if (linhas.empty()) { mensagem("Estoque em dia", "Nenhum item está abaixo de 10 unidades."); return; }
    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    executarTela("REPOSIÇÃO", menu);
}

void menuEstoque(SistemaEstoque& sistema) {
    const vector<string> opcoes = {"Adicionar Item", "Ver Estoque", "Editar Estoque", "Remover Item", "O que Precisa Comprar", "Voltar"};
    int selecionado = 0;
    auto menu = Menu(&opcoes, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] {
        return vbox({titulo("ESTOQUE"),
                      hbox({card("ITENS", to_string(sistema.getEstoque().size()), Color::Cyan),
                            card("CRÍTICOS", to_string(sistema.quantidadeItensBaixos()), Color::Red),
                            card("UNIDADES", to_string(sistema.totalUnidades()), Color::Green)}),
                      separator(), menu->Render() | border | flex, separator(),
                      text("Enter = selecionar | Esc = voltar") | color(Color::GrayDark) | center}) | border;
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const int acao = selecionado;
            tela.Exit();
            switch (acao) {
                case 0: cadastrarItem(sistema); break;
                case 1: listarEstoque(sistema); break;
                case 2: editarEstoque(sistema); break;
                case 3: removerItem(sistema); break;
                case 4: itensCriticos(sistema); break;
                default: break;
            }
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

