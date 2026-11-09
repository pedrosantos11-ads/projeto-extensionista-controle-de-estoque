#include "ui.hpp"

// ============================================================
// Interface FTXUI
// ============================================================
Element titulo(const string& texto) {
    return window(text("  " + texto + "  ") | bold | color(Color::White), text("")) |
           bgcolor(Color::RGB(35, 45, 65));
}

Element card(const string& cabecalho, const string& valor, Color cor) {
    return window(text(" " + cabecalho + " ") | bold | color(cor),
                  text("  " + valor + "  ") | bold | center) | flex;
}

void mensagem(const string& cabecalho, const string& texto, bool erro) {
    auto ok = Button("OK", [] {});
    auto tela = ScreenInteractive::Fullscreen();
    auto componente = Renderer(ok, [&] {
        return window(text(" " + cabecalho + " ") | bold | color(erro ? Color::Red : Color::Green),
                      vbox({paragraph(texto) | center, separator(), ok->Render() | center})) |
               size(WIDTH, GREATER_THAN, 60) | center;
    });
    componente = CatchEvent(componente, [&](Event event) {
        if (event == Event::Return || event == Event::Escape) {
            tela.Exit();
            return true;
        }
        return false;
    });
    tela.Loop(componente);
}

bool confirmarAcao(const string& cabecalho, const string& pergunta) {
    bool resposta = false;
    bool decidido = false;

    auto tela = ScreenInteractive::Fullscreen();

    auto sim = Button("SIM", [&] {
        resposta = true;
        decidido = true;
        tela.Exit();
    });

    auto nao = Button("NAO", [&] {
        resposta = false;
        decidido = true;
        tela.Exit();
    });

    auto botoes = Container::Horizontal({sim, nao});

    auto componente = Renderer(botoes, [&] {
        return window(
            text(" " + cabecalho + " ") | bold | color(Color::Red),
            vbox({
                paragraph(pergunta) | center,
                separator(),
                text("Escolha uma opção:") | bold | center,
                separator(),
                hbox({
                    sim->Render() | center,
                    nao->Render() | center
                }) | center
            })
        ) | size(WIDTH, GREATER_THAN, 65) | center;
    });

    componente = CatchEvent(componente, [&](Event event) {
        if (event == Event::Escape) {
            resposta = false;
            decidido = true;
            tela.Exit();
            return true;
        }

        return false;
    });

    tela.Loop(componente);
    return decidido && resposta;
}

ResultadoFormulario formulario(const string& tituloFormulario,
                               const vector<pair<string, string>>& campos,
                               const string& textoBotao) {
    vector<string> valores;
    vector<Component> inputs;
    valores.reserve(campos.size());
    inputs.reserve(campos.size());

    for (const auto& campo : campos) {
        valores.push_back(campo.second);
        inputs.push_back(Input(&valores.back(), campo.first));
    }

    auto container = Container::Vertical(inputs);
    auto salvar = Button(textoBotao, [] {});
    auto cancelar = Button("Cancelar", [] {});
    auto botoes = Container::Horizontal({salvar, cancelar});
    auto raiz = Container::Vertical({container, botoes});

    bool confirmado = false;
    auto renderer = Renderer(raiz, [&] {
        Elements linhas;
        for (size_t i = 0; i < valores.size(); ++i) {
            linhas.push_back(hbox({text(campos[i].first + ": ") | size(WIDTH, EQUAL, 22),
                                   inputs[i]->Render() | flex}) |
                             border | size(HEIGHT, EQUAL, 3));
        }
        return window(text(" " + tituloFormulario + " ") | bold | color(Color::Cyan),
                      vbox({vbox(std::move(linhas)), separator(), botoes->Render() | center})) |
               size(WIDTH, GREATER_THAN, 65) | center;
    });

    auto tela = ScreenInteractive::Fullscreen();
    auto app = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        if (event == Event::Return) { confirmado = true; tela.Exit(); return true; }
        return false;
    });
    tela.Loop(app);
    return {confirmado, valores};
}

void executarTela(const string& nome, const Component& componente) {
    auto tela = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(componente, [&] {
        return vbox({text("SISTEMA DE ESTOQUE") | bold | color(Color::Cyan) | center,
                      text(nome) | bold | color(Color::Yellow) | center,
                      separator(), componente->Render() | flex, separator(),
                      text("Setas/Tab = navegar | Enter = selecionar | Esc = voltar") |
                          color(Color::GrayDark) | center}) |
               border | bgcolor(Color::RGB(15, 20, 30));
    });
    renderer = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) { tela.Exit(); return true; }
        return false;
    });
    tela.Loop(renderer);
}
