#ifndef EMPRESTIMO_H
#define EMPRESTIMO_H

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int T = 3; // Define o grau mínimo T da árvore B.

struct Emprestimo {
    string tituloLivro;//indentificador unico e chave
    string idUsuario;
    string dataEmprestimo;
    string dataDevolucao;

    // Construtores para inicializar os membros da estrutura.
    Emprestimo() = default; // Construtor padrão
    Emprestimo(string tl, string iu, string de, string dd)
        : tituloLivro(tl), idUsuario(iu), dataEmprestimo(de), dataDevolucao(dd) {}
};

class BTreeNode {
public:
    vector<Emprestimo> emprestimos; // Emprestimos armazenados no nó.
    vector<BTreeNode*> filhos; // Ponteiros para os nós filhos.
    bool folha; // Marca se o nó é uma folha (não tem filhos).

    BTreeNode(bool _folha) : folha(_folha) {}
};

class BTree {
public:
    BTreeNode* root; // Ponteiro para o nó raiz da árvore.

    BTree() {
        root = new BTreeNode(true); // Inicializa a árvore com um nó raiz que é uma folha.
    }

    // Insere um novo empréstimo na árvore B.
    void insert(Emprestimo emprestimo) {
        // Se a raiz estiver cheia, cria um novo nó e divide a raiz.
        if (root->emprestimos.size() == 2 * T - 1) {
            BTreeNode* s = new BTreeNode(false);
            s->filhos.push_back(root);
            splitChild(s, 0, root);
            root = s;
        }
        insertNonFull(root, emprestimo);
    }

    // Dividir o filho y do nó x no índice i.
    void splitChild(BTreeNode* x, int i, BTreeNode* y) {
        BTreeNode* z = new BTreeNode(y->folha);
        z->emprestimos.insert(z->emprestimos.end(), y->emprestimos.begin() + T, y->emprestimos.end());
        y->emprestimos.erase(y->emprestimos.begin() + T, y->emprestimos.end());

        if (!y->folha) {
            z->filhos.insert(z->filhos.end(), y->filhos.begin() + T, y->filhos.end());
            y->filhos.erase(y->filhos.begin() + T, y->filhos.end());
        }

        x->filhos.insert(x->filhos.begin() + i + 1, z);
        x->emprestimos.insert(x->emprestimos.begin() + i, y->emprestimos[T - 1]);
        y->emprestimos.erase(y->emprestimos.begin() + T - 1);
    }

    // Insere um empréstimo em um nó que não está cheio.
    void insertNonFull(BTreeNode* x, Emprestimo emprestimo) {
        int i = x->emprestimos.size() - 1;
        if (x->folha) {
            x->emprestimos.push_back(emprestimo);
            while (i >= 0 && x->emprestimos[i].tituloLivro > emprestimo.tituloLivro) {
                x->emprestimos[i + 1] = x->emprestimos[i];
                i--;
            }
            x->emprestimos[i + 1] = emprestimo;
        } else {
            while (i >= 0 && x->emprestimos[i].tituloLivro > emprestimo.tituloLivro) {
                i--;
            }
            i++;
            if (x->filhos[i]->emprestimos.size() == 2 * T - 1) {
                splitChild(x, i, x->filhos[i]);
                if (x->emprestimos[i].tituloLivro < emprestimo.tituloLivro) {
                    i++;
                }
            }
            insertNonFull(x->filhos[i], emprestimo);
        }
    }

    // Realiza o percurso em ordem para recolher todos os empréstimos em ordem.
    void inorder(BTreeNode* node, vector<Emprestimo>& result) {
        size_t i;
        for (i = 0; i < node->emprestimos.size(); i++) {
            if (!node->folha) {
                inorder(node->filhos[i], result);
            }
            result.push_back(node->emprestimos[i]);
        }
        if (!node->folha) {
            inorder(node->filhos[i], result);
        }
    }

    // Procura um empréstimo com o título específico.
    Emprestimo* search(BTreeNode* node, const string& tituloLivro) {
        size_t i = 0;
        while (i < node->emprestimos.size() && tituloLivro > node->emprestimos[i].tituloLivro) {
            i++;
        }

        if (i < node->emprestimos.size() && node->emprestimos[i].tituloLivro == tituloLivro) {
            return &node->emprestimos[i];
        }

        if (node->folha) {
            return nullptr; // Retorna nulo se não encontrar e for folha.
        }

        return search(node->filhos[i], tituloLivro); // Procura no filho adequado.
    }

    // Remove um empréstimo com o título específico.
    BTreeNode* remove(BTreeNode* node, const string& tituloLivro) {
        if (!node) return nullptr;

        size_t idx = 0;
        while (idx < node->emprestimos.size() && node->emprestimos[idx].tituloLivro < tituloLivro) {
            idx++;
        }

        if (idx < node->emprestimos.size() && node->emprestimos[idx].tituloLivro == tituloLivro) {
            if (node->folha) {
                node->emprestimos.erase(node->emprestimos.begin() + idx);
            } else {
                if (node->filhos[idx]->emprestimos.size() >= T) {
                    BTreeNode* predecessor = node->filhos[idx];
                    while (!predecessor->folha) {
                        predecessor = predecessor->filhos.back();
                    }
                    node->emprestimos[idx] = predecessor->emprestimos.back();
                    remove(node->filhos[idx], predecessor->emprestimos.back().tituloLivro);
                } else if (node->filhos[idx + 1]->emprestimos.size() >= T) {
                    BTreeNode* successor = node->filhos[idx + 1];
                    while (!successor->folha) {
                        successor = successor->filhos.front();
                    }
                    node->emprestimos[idx] = successor->emprestimos.front();
                    remove(node->filhos[idx + 1], successor->emprestimos.front().tituloLivro);
                } else {
                    BTreeNode* child = node->filhos[idx];
                    BTreeNode* sibling = node->filhos[idx + 1];
                    child->emprestimos.push_back(node->emprestimos[idx]);
                    child->emprestimos.insert(child->emprestimos.end(), sibling->emprestimos.begin(), sibling->emprestimos.end());
                    if (!child->folha) {
                        child->filhos.insert(child->filhos.end(), sibling->filhos.begin(), sibling->filhos.end());
                    }
                    node->emprestimos.erase(node->emprestimos.begin() + idx);
                    node->filhos.erase(node->filhos.begin() + idx + 1);
                    delete sibling;
                    remove(child, tituloLivro);
                }
            }
        } else {
            if (node->folha) {
                return node;
            }

            bool flag = ((idx == node->emprestimos.size()) ? true : false);
            if (node->filhos[idx]->emprestimos.size() < T) {
                fill(node, idx);
            }

            if (flag && idx > node->emprestimos.size()) {
                remove(node->filhos[idx - 1], tituloLivro);
            } else {
                remove(node->filhos[idx], tituloLivro);
            }
        }
        return node;
    }

private:
    void fill(BTreeNode* node, int idx) {
        if (idx != 0 && node->filhos[idx - 1]->emprestimos.size() >= T) {
            borrowFromPrev(node, idx);
        } else if (idx != node->emprestimos.size() && node->filhos[idx + 1]->emprestimos.size() >= T) {
            borrowFromNext(node, idx);
        } else {
            if (idx != node->emprestimos.size()) {
                merge(node, idx);
            } else {
                merge(node, idx - 1);
            }
        }
    }

    void borrowFromPrev(BTreeNode* node, int idx) {
        BTreeNode* child = node->filhos[idx];
        BTreeNode* sibling = node->filhos[idx - 1];

        for (int i = child->emprestimos.size() - 1; i >= 0; --i) {
            child->emprestimos[i + 1] = child->emprestimos[i];
        }

        if (!child->folha) {
            for (int i = child->filhos.size() - 1; i >= 0; --i) {
                child->filhos[i + 1] = child->filhos[i];
            }
        }

        child->emprestimos[0] = node->emprestimos[idx - 1];

        if (!child->folha) {
            child->filhos[0] = sibling->filhos[sibling->filhos.size() - 1];
        }

        node->emprestimos[idx - 1] = sibling->emprestimos[sibling->emprestimos.size() - 1];

        sibling->emprestimos.pop_back();
        sibling->filhos.pop_back();

        child->emprestimos.insert(child->emprestimos.begin(), node->emprestimos[idx - 1]);
    }

    void borrowFromNext(BTreeNode* node, int idx) {
        BTreeNode* child = node->filhos[idx];
        BTreeNode* sibling = node->filhos[idx + 1];

        child->emprestimos.push_back(node->emprestimos[idx]);

        if (!child->folha) {
            child->filhos.push_back(sibling->filhos[0]);
        }

        node->emprestimos[idx] = sibling->emprestimos[0];

        sibling->emprestimos.erase(sibling->emprestimos.begin());
        sibling->filhos.erase(sibling->filhos.begin());

        child->emprestimos.push_back(node->emprestimos[idx]);
    }

    void merge(BTreeNode* node, int idx) {
        BTreeNode* child = node->filhos[idx];
        BTreeNode* sibling = node->filhos[idx + 1];

        child->emprestimos.push_back(node->emprestimos[idx]);

        for (int i = 0; i < sibling->emprestimos.size(); ++i) {
            child->emprestimos.push_back(sibling->emprestimos[i]);
        }

        if (!child->folha) {
            for (int i = 0; i <= sibling->filhos.size(); ++i) {
                child->filhos.push_back(sibling->filhos[i]);
            }
        }

        for (int i = idx + 1; i < node->emprestimos.size(); ++i) {
            node->emprestimos[i - 1] = node->emprestimos[i];
        }

        for (int i = idx + 2; i <= node->filhos.size(); ++i) {
            node->filhos[i - 1] = node->filhos[i];
        }

        node->emprestimos.pop_back();
        node->filhos.pop_back();

        delete sibling;
    }
};

#endif // EMPRESTIMO_H
