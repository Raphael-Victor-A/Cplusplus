#include <iostream>
#include <vector>//é usado para armazenar uma lista de números.
#include <limits>// é usado para encontrar o valor máximo de um tipo de dado.
#include <regex>//validar ids e isbn
#include <chrono>//validar o tempo de emprestimo
#include <iomanip>// é usado para formatação de saída
#include <sstream>// é usado para converter entre uma string e um número.
#include "Livro.h"
#include "Usuario.h"
#include "Emprestimo.h"

using namespace std;

BST livros;
AVL usuarios;
BTree emprestimos;

void pausarTela() {
    cout << "Pressione Enter para continuar...";
    cin.ignore();
    cin.get();
}

bool livroEmprestado(const string& isbn) {
    vector<Emprestimo> todosEmprestimos;
    emprestimos.inorder(emprestimos.root, todosEmprestimos);
    for (const auto& emprestimo : todosEmprestimos) {
        if (emprestimo.tituloLivro == isbn) {
            return true;
        }
    }
    return false;
}

bool validarISBN(const string& isbn) {
    return regex_match(isbn, regex("^[0-9]*$"));
}

bool livroExiste(const string& isbn) {
    Livro* livro = livros.search(livros.root, isbn);
    return livro != nullptr;
}

bool usuarioExiste(const string& id) {
    Usuario* usuario = usuarios.search(usuarios.root, id);
    return usuario != nullptr;
}

bool validarDataDevolucao(const string& dataEmprestimo, const string& dataDevolucao) {
    tm tmEmprestimo = {};
    tm tmDevolucao = {};

    istringstream ssEmprestimo(dataEmprestimo);
    istringstream ssDevolucao(dataDevolucao);

    ssEmprestimo >> get_time(&tmEmprestimo, "%d-%m-%Y");
    ssDevolucao >> get_time(&tmDevolucao, "%d-%m-%Y");

    auto timeEmprestimo = mktime(&tmEmprestimo);
    auto timeDevolucao = mktime(&tmDevolucao);

    return difftime(timeDevolucao, timeEmprestimo) > 0;
}

void cadastrarLivro() {
    string isbn, titulo, autor;
    int numeroPaginas;

    do {
        cout << "ISBN: ";
        cin >> isbn;
        if (!validarISBN(isbn)) {
            cout << "ISBN invalido. Use apenas numeros. Tente novamente." << endl;
        }
    } while (!validarISBN(isbn));

    cout << "Titulo: ";
    cin.ignore();
    getline(cin, titulo);
    cout << "Autor: ";
    getline(cin, autor);
    cout << "Numero de Paginas: ";
    while (!(cin >> numeroPaginas) || numeroPaginas <= 0) {
        cout << "Entrada invalida. Insira um numero de paginas positivo: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Livro livro(isbn, titulo, autor, numeroPaginas);
    livros.root = livros.insert(livros.root, livro);
    cout << "Livro cadastrado com sucesso!" << endl;
    pausarTela();
}

void removerLivro() {
    string isbn;
    cout << "ISBN do livro a ser removido: ";
    cin >> isbn;

    if (!livroExiste(isbn)) {
        cout << "Livro nao encontrado!" << endl;
        pausarTela();
        return;
    }

    livros.root = livros.remove(livros.root, isbn);
    cout << "Livro removido com sucesso!" << endl;
    pausarTela();
}

void buscarLivro() {
    string isbn;
    cout << "ISBN do livro: ";
    cin >> isbn;

    Livro* livro = livros.search(livros.root, isbn);
    if (livro) {
        cout << "Titulo: " << livro->titulo << endl;
        cout << "Autor: " << livro->autor << endl;
        cout << "Numero de Paginas: " << livro->numeroPaginas << endl;
    } else {
        cout << "Livro nao encontrado!" << endl;
    }
    pausarTela();
}

bool validarNome(const string& nome) {
    return !regex_search(nome, regex("\\d"));
}

void cadastrarUsuario() {
    string id, nome, contato;

    cout << "ID: ";
    cin >> id;
    do {
        cout << "Nome: ";
        cin.ignore();
        getline(cin, nome);
        if (!validarNome(nome)) {
            cout << "Nome invalido. O nome nao pode conter numeros. Tente novamente." << endl;
        }
    } while (!validarNome(nome));

    cout << "Contato: ";
    getline(cin, contato);

    Usuario usuario(id, nome, contato);
    usuarios.root = usuarios.insert(usuarios.root, usuario);
    cout << "Usuario cadastrado com sucesso!" << endl;
    pausarTela();
}

void removerUsuario() {
    string id;
    cout << "ID do usuario a ser removido: ";
    cin >> id;

    if (!usuarioExiste(id)) {
        cout << "Usuario nao encontrado!" << endl;
        pausarTela();
        return;
    }

    usuarios.root = usuarios.remove(usuarios.root, id);
    cout << "Usuario removido com sucesso!" << endl;
    pausarTela();
}

void buscarUsuario() {
    string id;
    cout << "ID do usuario: ";
    cin >> id;

    Usuario* usuario = usuarios.search(usuarios.root, id);
    if (usuario) {
        cout << "Nome: " << usuario->nome << endl;
        cout << "Contato: " << usuario->contato << endl;
    } else {
        cout << "Usuario nao encontrado!" << endl;
    }
    pausarTela();
}

bool validarData(const string& data) {
    return regex_match(data, regex("^\\d{2}-\\d{2}-\\d{4}$"));
}

void registrarEmprestimo() {
    string isbnLivro, idUsuario, dataEmprestimo, dataDevolucao;

    do {
        cout << "ISBN do Livro: ";
        cin >> isbnLivro;
        if (!livroExiste(isbnLivro)) {
            cout << "Livro nao encontrado!" << endl;
            pausarTela();
            return;
        }
    } while (!validarISBN(isbnLivro));

    cout << "ID do Usuario: ";
    cin >> idUsuario;
    if (!usuarioExiste(idUsuario)) {
        cout << "Usuario nao encontrado!" << endl;
        pausarTela();
        return;
    }

    do {
        cout << "Data de Emprestimo (dd-mm-aaaa): ";
        cin >> dataEmprestimo;
        if (!validarData(dataEmprestimo)) {
            cout << "Data invalida. Use o formato dd-mm-aaaa. Tente novamente." << endl;
        }
    } while (!validarData(dataEmprestimo));

    do {
        cout << "Data de Devolucao (dd-mm-aaaa): ";
        cin >> dataDevolucao;
        if (!validarData(dataDevolucao) || !validarDataDevolucao(dataEmprestimo, dataDevolucao)) {
            cout << "Data de devolucao invalida. Deve ser posterior a data de emprestimo e no formato dd-mm-aaaa. Tente novamente." << endl;
        }
    } while (!validarData(dataDevolucao) || !validarDataDevolucao(dataEmprestimo, dataDevolucao));

    Emprestimo emprestimo(isbnLivro, idUsuario, dataEmprestimo, dataDevolucao);
    emprestimos.insert(emprestimo);
    cout << "Emprestimo registrado com sucesso!" << endl;
    pausarTela();
}

void devolverLivro() {
    string isbnLivro;
    cout << "ISBN do Livro: ";
    cin.ignore();
    getline(cin, isbnLivro);

    if (!livroExiste(isbnLivro)) {
        cout << "Livro nao encontrado!" << endl;
        pausarTela();
        return;
    }

    emprestimos.root = emprestimos.remove(emprestimos.root, isbnLivro);
    cout << "Livro devolvido com sucesso!" << endl;
    pausarTela();
}

void listarLivros() {
    vector<Livro> todosLivros;
    livros.inorder(livros.root, todosLivros);
    for (const auto& livro : todosLivros) {
        if (!livroEmprestado(livro.ISBN)) {
            cout << "ISBN: " << livro.ISBN << ", Titulo: " << livro.titulo << ", Autor: " << livro.autor << ", Paginas: " << livro.numeroPaginas << endl;
        } else {
            cout << "Nao ha livros disponiveis no momento." << endl;
        }
    }
    pausarTela();
}

int main() {
    int opcao;

    do {
        system("cls"); // Limpar a tela no início de cada iteração do loop
        cout << "Menu:\n";
        cout << "1. Cadastrar Livro\n";
        cout << "2. Remover Livro\n";
        cout << "3. Buscar Livro\n";
        cout << "4. Cadastrar Usuario\n";
        cout << "5. Remover Usuario\n";
        cout << "6. Buscar Usuario\n";
        cout << "7. Registrar Emprestimo\n";
        cout << "8. Devolver Livro\n";
        cout << "9. Listar Livros\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        // Tratamento de erro para opcao invalida
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcao = -1;
        }

        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: removerLivro(); break;
            case 3: buscarLivro(); break;
            case 4: cadastrarUsuario(); break;
            case 5: removerUsuario(); break;
            case 6: buscarUsuario(); break;
            case 7: registrarEmprestimo(); break;
            case 8: devolverLivro(); break;
            case 9: listarLivros(); break;
            case 0: cout << "Saindo..." << endl; break;
            default: cout << "Opcao invalida!" << endl; pausarTela(); break;
        }

    } while (opcao != 0);

    return 0;
}
