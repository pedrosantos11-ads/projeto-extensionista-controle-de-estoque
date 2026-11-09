#include "telas_vendas.hpp"

// ============================================================
// Vendas
// ============================================================
void registrarVenda(SistemaEstoque& sistema) {
    if (sistema.getEstoque().empty()) { mensagem("Sem estoque", "Cadastre pelo menos um item antes de registrar uma venda.", true); return; }
    vector<string> nomes;
    for (const auto& item : sistema.getEstoque()) nomes.push_back(item.nome);
    int selecionado = 0;
    auto menu = Menu(&nomes, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] {
        return vbox({titulo("REGISTRAR VENDA"), text("Selecione o item vendido") | center, separator(),
                      menu->Render() | border | flex, separator(), text("Enter = continuar | Esc = voltar") | color(Color::GrayDark) | center}) | border;
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const size_t indice = static_cast<size_t>(selecionado);
            tela.Exit();
            const auto r = formulario("DADOS DA VENDA", {{"Quantidade vendida", "1"}, {"Cliente", ""}});
            if (!r.confirmado) return true;
            int quantidade = 0;
            if (!inteiroValido(r.valores[0], quantidade) || quantidade <= 0) { mensagem("Erro", "Quantidade inválida.", true); return true; }
            const int disponivel = sistema.getEstoque()[indice].quantidade;
            if (quantidade > disponivel) {
                mensagem("Quantidade acima do estoque", "A venda será registrada, mas apenas " + to_string(disponivel) + " unidades serão retiradas do estoque.", true);
            }
            string erro;
            if (!sistema.registrarVenda(indice, quantidade, r.valores[1], erro)) mensagem("Erro", erro, true);
            else mensagem("Salvo com sucesso", "Venda salva e estoque atualizado.");
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

void listarVendas(SistemaEstoque& sistema) {
    const auto& vendas = sistema.getVendas();
    if (vendas.empty()) { mensagem("Histórico vazio", "Nenhuma venda registrada."); return; }
    vector<string> linhas;
    for (size_t i = 0; i < vendas.size(); ++i) linhas.push_back(to_string(i + 1) + ". " + vendas[i].item + " | Qtd: " + to_string(vendas[i].quantidade) + " | Cliente: " + vendas[i].cliente);
    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] { return vbox({titulo("HISTÓRICO DE VENDAS"), text("Vendas registradas: " + to_string(vendas.size())) | center, separator(), menu->Render() | frame | vscroll_indicator | border | flex, separator(), text("Enter = detalhes | Esc = voltar") | color(Color::GrayDark) | center}) | border; });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) { const auto& v = sistema.getVendas()[static_cast<size_t>(selecionado)]; mensagem("Detalhes da venda", "Item: " + v.item + "\nQuantidade: " + to_string(v.quantidade) + "\nCliente: " + v.cliente); return true; }
        return false;
    });
    tela.Loop(renderer);
}

void editarVenda(SistemaEstoque& sistema) {
    if (sistema.getVendas().empty()) { mensagem("Histórico vazio", "Não há vendas para editar."); return; }
    vector<string> linhas;
    for (const auto& v : sistema.getVendas()) linhas.push_back(v.item + " | Qtd: " + to_string(v.quantidade) + " | Cliente: " + v.cliente);
    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] { return vbox({titulo("EDITAR VENDA"), menu->Render() | border | flex, separator(), text("Enter = editar | Esc = voltar") | color(Color::GrayDark) | center}) | border; });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const size_t indice = static_cast<size_t>(selecionado);
            const auto venda = sistema.getVendas()[indice];
            tela.Exit();
            const auto r = formulario("EDITAR VENDA", {{"Quantidade", to_string(venda.quantidade)}, {"Cliente", venda.cliente}});
            if (!r.confirmado) return true;
            int quantidade = 0;
            if (!inteiroValido(r.valores[0], quantidade) || quantidade <= 0) { mensagem("Erro", "Quantidade inválida.", true); return true; }
            string erro;
            if (!sistema.editarVenda(indice, quantidade, r.valores[1], erro)) mensagem("Erro", erro, true);
            else mensagem("Salvo com sucesso", "A edição altera o histórico da venda.");
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

void removerVenda(SistemaEstoque& sistema) {
    while (true) {
        if (sistema.getVendas().empty()) { mensagem("Histórico vazio", "Não há vendas para remover."); return; }
        vector<string> linhas;
        for (const auto& v : sistema.getVendas()) linhas.push_back(v.item + " | Qtd: " + to_string(v.quantidade) + " | Cliente: " + v.cliente);
        int selecionado = 0;
        auto menu = Menu(&linhas, &selecionado);
        auto tela = ScreenInteractive::Fullscreen();
        bool voltarMenu = false;

        auto renderer = Renderer(menu, [&] { return vbox({titulo("REMOVER VENDA"), menu->Render() | border | flex, separator(), text("Enter = remover | Esc = voltar") | color(Color::GrayDark) | center}) | border; });
        renderer = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Escape) { voltarMenu = true; tela.Exit(); return true; }
            if (event == Event::Return) {
                if (confirmarAcao("Confirmação", "Você realmente deseja excluir esta venda?")) {
                    string erro;
                    if (!sistema.removerVenda(static_cast<size_t>(selecionado), erro)) {
                        mensagem("Erro", erro, true);
                    } else {
                        mensagem("Salvo com sucesso", "A venda foi removida do histórico.");
                    }
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

void mostrarRankingItens(SistemaEstoque& sistema) {
    const auto ranking = sistema.rankingItens();
    if (ranking.empty()) { mensagem("Ranking vazio", "Ainda não existem vendas para calcular o ranking."); return; }
    vector<string> linhas;
    for (size_t i = 0; i < ranking.size(); ++i) linhas.push_back(to_string(i + 1) + "º  " + ranking[i].nome + " | Total vendido: " + to_string(ranking[i].totalVendido));
    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    executarTela("RANKING DE VENDAS", menu);
}

void tabelaClientes(SistemaEstoque& sistema) {
    const auto ranking = sistema.rankingClientes();
    if (ranking.empty()) { mensagem("Tabela vazia", "Ainda não existem clientes com compras registradas."); return; }
    vector<string> linhas;
    for (size_t i = 0; i < ranking.size(); ++i) linhas.push_back(to_string(i + 1) + "º  " + ranking[i].nome + " | Total comprado: " + to_string(ranking[i].totalComprado));
    int selecionado = 0;
    auto menu = Menu(&linhas, &selecionado);
    executarTela("CLIENTES", menu);
}

void editarClientes(SistemaEstoque& sistema) {
    const auto ranking = sistema.rankingClientes();
    if (ranking.empty()) { mensagem("Tabela vazia", "Não há clientes para editar."); return; }
    vector<string> nomes;
    for (const auto& c : ranking) nomes.push_back(c.nome);
    int selecionado = 0;
    auto menu = Menu(&nomes, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] { return vbox({titulo("EDITAR CLIENTE"), menu->Render() | border | flex, separator(), text("Enter = renomear | Esc = voltar") | color(Color::GrayDark) | center}) | border; });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const string antigo = ranking[static_cast<size_t>(selecionado)].nome;
            tela.Exit();
            const auto r = formulario("RENOMEAR CLIENTE", {{"Novo nome", antigo}});
            if (!r.confirmado) return true;
            string erro;
            if (!sistema.renomearCliente(antigo, r.valores[0], erro)) mensagem("Erro", erro, true);
            else mensagem("Salvo com sucesso", "O nome do cliente foi atualizado.");
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

void menuVendas(SistemaEstoque& sistema) {
    const vector<string> opcoes = {"Registrar Venda", "Ver Histórico de Vendas", "Editar Venda", "Remover Venda", "Ranking de Vendas", "Tabela de Clientes", "Editar Clientes", "Voltar"};
    int selecionado = 0;
    auto menu = Menu(&opcoes, &selecionado);
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(menu, [&] {
        return vbox({titulo("VENDAS"),
                      hbox({card("VENDAS", to_string(sistema.getVendas().size()), Color::Green)}),
                      separator(), menu->Render() | border | flex, separator(), text("Enter = selecionar | Esc = voltar") | color(Color::GrayDark) | center}) | border;
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) {
            const int acao = selecionado;
            tela.Exit();
            switch (acao) {
                case 0: registrarVenda(sistema); break;
                case 1: listarVendas(sistema); break;
                case 2: editarVenda(sistema); break;
                case 3: removerVenda(sistema); break;
                case 4: mostrarRankingItens(sistema); break;
                case 5: tabelaClientes(sistema); break;
                case 6: editarClientes(sistema); break;
                default: break;
            }
            return true;
        }
        return false;
    });
    tela.Loop(renderer);
}

