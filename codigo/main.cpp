#include "telas_estoque.hpp"
#include "telas_vendas.hpp"

// ============================================================
// Painel principal
// ============================================================
void dashboard(SistemaEstoque& sistema) {
    auto voltar = Button("Voltar", [] {});
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(voltar, [&] {
        return vbox({titulo("PAINEL PRINCIPAL"), text("Gerenciador de estoque") | center | color(Color::GrayLight), separator(),
                      hbox({card("ITENS", to_string(sistema.getEstoque().size()), Color::Cyan),
                            card("UNIDADES", to_string(sistema.totalUnidades()), Color::Green),
                            card("CRÍTICOS", to_string(sistema.quantidadeItensBaixos()), Color::Red),
                            card("VENDAS", to_string(sistema.getVendas().size()), Color::Yellow)}),
                      separator(),
                      text(sistema.quantidadeItensBaixos() > 0 ? "Atenção: existem itens abaixo do limite de reposição." : "Estoque sem itens em nível crítico.") |
                          center | (sistema.quantidadeItensBaixos() > 0 ? color(Color::Red) : color(Color::Green)),
                      separator(), voltar->Render() | center}) | border;
    });
    renderer = CatchEvent(renderer, [&](Event event) { if (event == Event::Escape || event == Event::Return) { tela.Exit(); return true; } return false; });
    tela.Loop(renderer);
}

int main() {
    SistemaEstoque sistema;
    if (!sistema.carregarTudo()) {
        mensagem("Aviso", "Alguns arquivos de dados não puderam ser lidos. Os registros válidos foram carregados.", true);
    }

    while (true) {
        const vector<string> opcoes = {"Painel Principal", "Estoque", "Vendas", "Salvar Estoque", "Sair"};
        int selecionado = 0;
        auto menu = Menu(&opcoes, &selecionado);
        auto tela = ScreenInteractive::Fullscreen();
        auto renderer = Renderer(menu, [&] {
            return vbox({text("GERENCIAMENTO DE ESTOQUE E VENDAS") | bold | color(Color::Cyan) | center,
                          separator(),
                          hbox({card("Itens", to_string(sistema.getEstoque().size()), Color::Cyan),
                                card("Críticos", to_string(sistema.quantidadeItensBaixos()), Color::Red),
                                card("Vendas", to_string(sistema.getVendas().size()), Color::Green)}),
                          separator(), menu->Render() | border | flex, separator(),
                          text("Setas/Tab = navegar | Enter = abrir | Esc = sair") | color(Color::GrayDark) | center}) |
                   border | bgcolor(Color::RGB(10, 15, 25));
        });
        int acao = -1;
        renderer = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Escape) { acao = 4; tela.Exit(); return true; }
            if (event == Event::Return) { acao = selecionado; tela.Exit(); return true; }
            return false;
        });
        tela.Loop(renderer);

        switch (acao) {
            case 0:
                dashboard(sistema);
                break;

            case 1:
                menuEstoque(sistema);
                break;

            case 2:
                menuVendas(sistema);
                break;

            case 3:
                if (sistema.salvarApenasEstoque()) {
                    mensagem("Salvo com sucesso", "Estoque salvo com sucesso!");
                } else {
                    mensagem("Erro", "Falha ao salvar o estoque.", true);
                }
                break;

            case 4:
                sistema.salvarTudo();
                cout << "\nSistema encerrado pelo usuário.\n";
                return 0;

            default:
                break;
        }
    }

    return 0;
}
