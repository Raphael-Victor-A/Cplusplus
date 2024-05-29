#ifndef LIVRO_H
#define LIVRO_H

#include <string>
#include <vector>

using namespace std;

// Estrutura para armazenar informações de um livro.
struct Livro {
    string ISBN;          // Identificador único do livro e chave de controle
    string titulo;        // Título do livro.
    string autor;         // Autor do livro.
    int numeroPaginas;    // Número de páginas do livro.

    // Construtor padrão.
    Livro() = default;

    // Construtor com parâmetros para inicialização dos membros.
    Livro(string isbn, string t, string a, int n)
        : ISBN(isbn), titulo(t), autor(a), numeroPaginas(n) {}
};

// Nó da árvore binária de busca, contém um livro.
struct BSTNode {
    Livro livro;          // Livro armazenado no nó.
    BSTNode* left;        // Ponteiro para o filho esquerdo.
    BSTNode* right;       // Ponteiro para o filho direito.

    // Construtor que inicializa o nó com um livro.
    BSTNode(Livro l) : livro(l), left(nullptr), right(nullptr) {}
};

// Classe para a árvore binária de busca.
class BST {
public:
    BSTNode* root;        // Raiz da árvore.

    // Construtor da árvore.
    BST() : root(nullptr) {}

    // Função para inserir um livro na árvore.
    BSTNode* insert(BSTNode* node, Livro livro) {
        if (!node) return new BSTNode(livro);  // Se o nó é nulo, cria um novo nó com o livro.

        // Decide em qual subárvore inserir baseado no ISBN.
        if (livro.ISBN < node->livro.ISBN)
            node->left = insert(node->left, livro);
        else if (livro.ISBN > node->livro.ISBN)
            node->right = insert(node->right, livro);

        return node;
    }

    // Função para remover um livro pelo ISBN.
    BSTNode* remove(BSTNode* node, string isbn) {
        if (!node) return node;  // Se o nó é nulo, retorna nulo.

        // Navega pela árvore para encontrar o livro a ser removido.
        if (isbn < node->livro.ISBN)
            node->left = remove(node->left, isbn);
        else if (isbn > node->livro.ISBN)
            node->right = remove(node->right, isbn);
        else {
            // Quando o livro é encontrado, verifica a estrutura dos filhos para remover corretamente.
            if (!node->left) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }

            // Encontra o menor valor na subárvore direita para substituir o valor do nó atual.
            BSTNode* temp = minValueNode(node->right);
            node->livro = temp->livro;
            node->right = remove(node->right, temp->livro.ISBN);
        }
        return node;
    }

    // Função para encontrar o menor nó na subárvore.
    BSTNode* minValueNode(BSTNode* node) {
        BSTNode* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    // Função para buscar um livro pelo ISBN.
    Livro* search(BSTNode* node, string isbn) {
        if (!node) return nullptr;  // Se o nó é nulo, retorna nulo.
        if (node->livro.ISBN == isbn) return &node->livro;  // Retorna o livro se encontrado.

        // Navega pela árvore conforme o valor do ISBN.
        if (isbn > node->livro.ISBN)
            return search(node->right, isbn);
        else
            return search(node->left, isbn);
    }

    // Função para percorrer a árvore em ordem e coletar os livros.
    void inorder(BSTNode* node, vector<Livro>& livros) {
        if (!node) return;
        inorder(node->left, livros);    // Visita subárvore esquerda.
        livros.push_back(node->livro);  // Visita nó atual.
        inorder(node->right, livros);   // Visita subárvore direita.
    }
};

#endif // LIVRO_H
