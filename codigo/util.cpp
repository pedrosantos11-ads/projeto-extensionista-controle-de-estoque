#include "util.hpp"

// ============================================================
// Utilidades
// ============================================================
string aparar(const string& texto) {
    const string espacos = " \t\r\n";
    const size_t inicio = texto.find_first_not_of(espacos);
    if (inicio == string::npos) return "";
    const size_t fim = texto.find_last_not_of(espacos);
    return texto.substr(inicio, fim - inicio + 1);
}

string limparSeparador(string texto) {
    for (char& c : texto) {
        if (c == '|') c = '/';
    }
    return texto;
}

vector<string> separar(const string& linha, char delimitador) {
    vector<string> partes;
    string parte;
    stringstream ss(linha);
    while (getline(ss, parte, delimitador)) partes.push_back(parte);
    return partes;
}

bool inteiroValido(const string& texto, int& valor) {
    const string t = aparar(texto);
    if (t.empty()) return false;
    try {
        size_t pos = 0;
        const int numero = stoi(t, &pos);
        if (pos != t.size()) return false;
        valor = numero;
        return true;
    } catch (...) {
        return false;
    }
}
