#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Estrutura para armazenar informações de um usuário.
struct Usuario {
    string id;        // Identificador único do usuário.
    string nome;      // Nome do usuário.
    string contato;   // Contato do usuário.

    // Construtores padrão e parametrizado.
    Usuario() = default;
    Usuario(string i, string n, string c) : id(i), nome(n), contato(c) {}
};

// Nó da árvore AVL que armazena dados de um usuário.
struct AVLNode {
    Usuario usuario;  // Dados do usuário armazenados no nó.
    AVLNode* left;    // Ponteiro para o filho esquerdo.
    AVLNode* right;   // Ponteiro para o filho direito.
    int height;       // Altura do nó na árvore e chave de controle

    // Construtor que inicializa o nó com um usuário.
    AVLNode(Usuario u) : usuario(u), left(nullptr), right(nullptr), height(1) {}
};

// Classe para gerenciar a árvore AVL.
class AVL {
public:
    AVLNode* root;    // Raiz da árvore.

    // Construtor da árvore.
    AVL() : root(nullptr) {}

    // Retorna a altura de um nó, ou 0 se nulo.
    int height(AVLNode* node) {
        return node ? node->height : 0;
    }

    // Calcula o fator de balanceamento de um nó como diferença entre as alturas das subárvores esquerda e direita.
    int balanceFactor(AVLNode* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    // Rotação à direita para balancear a árvore.
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x;
    }

    // Rotação à esquerda para balancear a árvore.
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    // Insere um usuário na árvore e rebalanceia se necessário.
    AVLNode* insert(AVLNode* node, Usuario usuario) {
        if (!node) return new AVLNode(usuario);  // Cria um novo nó se o local de inserção é nulo.

        // Inserção de acordo com o ID do usuário.
        if (usuario.id < node->usuario.id)
            node->left = insert(node->left, usuario);
        else if (usuario.id > node->usuario.id)
            node->right = insert(node->right, usuario);
        else
            return node;  // ID duplicado não é inserido.

        // Atualiza a altura e verifica o balanceamento.
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = balanceFactor(node);

        // Quatro casos de desbalanceamento e suas correções.
        if (balance > 1 && usuario.id < node->left->usuario.id)
            return rotateRight(node);
        if (balance < -1 && usuario.id > node->right->usuario.id)
            return rotateLeft(node);
        if (balance > 1 && usuario.id > node->left->usuario.id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && usuario.id < node->right->usuario.id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;  // Retorna o nó rebalanceado.
    }

    // Encontra o nó com o menor valor em uma subárvore.
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    // Remove um usuário da árvore e rebalanceia se necessário.
    AVLNode* remove(AVLNode* node, string id) {
        if (!node) return node;  // Retorna nulo se o nó é nulo.

        // Encontra o usuário a ser removido.
        if (id < node->usuario.id)
            node->left = remove(node->left, id);
        else if (id > node->usuario.id)
            node->right = remove(node->right, id);
        else {
            // Casos com zero ou um filho.
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else *node = *temp;
                delete temp;
            } else {
                // Caso com dois filhos.
                AVLNode* temp = minValueNode(node->right);
                node->usuario = temp->usuario;
                node->right = remove(node->right, temp->usuario.id);
            }
        }

        if (!node) return node;  // Retorna nulo se o nó foi removido.

        // Atualiza altura e balanceamento.
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = balanceFactor(node);

        // Correções de desbalanceamento após remoção.
        if (balance > 1 && balanceFactor(node->left) >= 0)
            return rotateRight(node);
        if (balance > 1 && balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && balanceFactor(node->right) <= 0)
            return rotateLeft(node);
        if (balance < -1 && balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;  // Retorna o nó rebalanceado.
    }

    // Busca um usuário pelo ID.
    Usuario* search(AVLNode* node, string id) {
        if (!node) return nullptr;  // Retorna nulo se o nó é nulo.
        if (node->usuario.id == id) return &node->usuario;  // Retorna o usuário se encontrado.

        // Busca recursiva de acordo com o ID.
        if (id > node->usuario.id)
            return search(node->right, id);
        return search(node->left, id);
    }

    // Percorre a árvore em ordem e coleta os usuários.
    void inorder(AVLNode* node, vector<Usuario>& usuarios) {
        if (!node) return;
        inorder(node->left, usuarios);  // Visita subárvore esquerda.
        usuarios.push_back(node->usuario);  // Visita nó atual.
        inorder(node->right, usuarios);  // Visita subárvore direita.
    }
};

#endif // USUARIO_H
