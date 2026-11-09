#pragma once

#include "sistema_estoque.hpp"

Element titulo(const string& texto);
Element card(const string& cabecalho, const string& valor, Color cor);
void mensagem(const string& cabecalho, const string& texto, bool erro = false);
bool confirmarAcao(const string& cabecalho, const string& pergunta);
ResultadoFormulario formulario(const string& tituloFormulario,
                               const vector<pair<string, string>>& campos,
                               const string& textoBotao = "Salvar");
void executarTela(const string& nome, const Component& componente);
