// Exercício de Implementação Árvore B
// Stefano Calheiros Stringhini R.A.:2312123
// Victor Ribeiro Calado R.A.: 2313553 

#include <stdio.h>
#include <stdlib.h>

//Ordem da arvoreB
#define ORDER 2

// Estrutura do nó da árvore B
typedef struct Node {
    int keys[2 * ORDER - 1];
    struct Node* children[2 * ORDER];
    int keyCount;
    int isLeaf;
} Node;

Node* createNode() {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->keyCount = 0;
    newNode->isLeaf = 1;
    return newNode;
}

int searchKeyIndex(Node* node, int key) {
    int index = 0;
    while (index < node->keyCount && key > node->keys[index]) {
        index++;
    }
    return index;
}

//Metodo Split
void splitChild(Node* parent, int index, Node* child) {
    Node* newChild = createNode();
    newChild->isLeaf = child->isLeaf;
    newChild->keyCount = ORDER - 1;

    for (int i = 0; i < ORDER - 1; i++) {
        newChild->keys[i] = child->keys[i + ORDER];
    }

    if (!child->isLeaf) {
        for (int i = 0; i < ORDER; i++) {
            newChild->children[i] = child->children[i + ORDER];
        }
    }

    child->keyCount = ORDER - 1;

    for (int i = parent->keyCount; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    parent->children[index + 1] = newChild;

    for (int i = parent->keyCount; i > index; i--) {
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->keys[index] = child->keys[ORDER - 1];
    parent->keyCount++;
}


int getP(Node* node) {
    while (!node->isLeaf) {
        node = node->children[node->keyCount];
    }
    return node->keys[node->keyCount - 1];
}

int getYgg(Node* node) {
    while (!node->isLeaf) {
        node = node->children[0];
    }
    return node->keys[0];
}


void merge(Node* node, int childIndex) {
    Node* child = node->children[childIndex];
    Node* nextChild = node->children[childIndex + 1];

    // Mova a chave do nó pai para o filho.
    child->keys[child->keyCount] = node->keys[childIndex];
    child->keyCount++;

    // Copie todas as chaves e filhos do próximo filho para o filho.
    for (int i = 0; i < nextChild->keyCount; i++) {
        child->keys[child->keyCount + i] = nextChild->keys[i];
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= nextChild->keyCount; i++) {
            child->children[child->keyCount + i] = nextChild->children[i];
        }
    }

    // Move as chaves e filhos restantes no nó pai e ajusta o número de chaves.
    for (int i = childIndex; i < node->keyCount - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }

    for (int i = childIndex + 1; i < node->keyCount; i++) {
        node->children[i] = node->children[i + 1];
    }

    node->keyCount--;
    free(nextChild);
}

void insertNonFull(Node* node, int key) {
    int i = node->keyCount - 1;

    if (node->isLeaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->keyCount++;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keyCount == 2 * ORDER - 1) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

//Função de inserir valor na arvore
void insert(Node** root, int key) {
    Node* rootNode = *root;

    if (rootNode->keyCount == 2 * ORDER - 1) {
        Node* newRoot = createNode();
        newRoot->isLeaf = 0;
        newRoot->children[0] = rootNode;
        *root = newRoot;
        splitChild(newRoot, 0, rootNode);
        insertNonFull(newRoot, key);
    } else {
        insertNonFull(rootNode, key);
    }
}

void em_ordem(Node* node) {
    if (node) {
        int i;
        for (i = 0; i < node->keyCount; i++) {
           em_ordem(node->children[i]);
            printf("%d ", node->keys[i]);
        }
        em_ordem(node->children[i]);
    }
}

int findKeyIndex(Node* node, int key) {
    int index = 0;
    while (index < node->keyCount && key > node->keys[index]) {
        index++;
    }
    return index;
}

void borrowFromRightSibling(Node* node, int childIndex) {
    Node* child = node->children[childIndex];
    Node* rightSibling = node->children[childIndex + 1];

    // Move a chave do nó pai para o filho.
    child->keys[child->keyCount] = node->keys[childIndex];
    child->keyCount++;

    // Move a primeira chave do irmão direito para o nó pai.
    node->keys[childIndex] = rightSibling->keys[0];

    // Atualiza os filhos, se o nó e o irmão direito não forem folhas.
    if (!child->isLeaf) {
        child->children[child->keyCount] = rightSibling->children[0];
    }

    // Move todas as chaves e filhos do irmão direito.
    for (int i = 0; i < rightSibling->keyCount - 1; i++) {
        rightSibling->keys[i] = rightSibling->keys[i + 1];
    }

    if (!rightSibling->isLeaf) {
        for (int i = 0; i < rightSibling->keyCount; i++) {
            rightSibling->children[i] = rightSibling->children[i + 1];
        }
    }

    rightSibling->keyCount--;
}

void borrowFromLeftSibling(Node* node, int childIndex) {
    Node* child = node->children[childIndex];
    Node* leftSibling = node->children[childIndex - 1];

    // Move todas as chaves e filhos do filho para a direita.
    for (int i = child->keyCount; i > 0; i--) {
        child->keys[i] = child->keys[i - 1];
    }

    if (!child->isLeaf) {
        for (int i = child->keyCount + 1; i > 0; i--) {
            child->children[i] = child->children[i - 1];
        }
    }

    // Move a chave do nó pai para o filho.
    child->keys[0] = node->keys[childIndex - 1];
    child->keyCount++;

    // Move a última chave do irmão esquerdo para o nó pai.
    node->keys[childIndex - 1] = leftSibling->keys[leftSibling->keyCount - 1];

    // Atualiza os filhos, se o nó e o irmão esquerdo não forem folhas.
    if (!child->isLeaf) {
        child->children[0] = leftSibling->children[leftSibling->keyCount];
    }

    leftSibling->keyCount--;
}

void removeKey(Node* node, int key) {
    int index = findKeyIndex(node, key);

    if (index < node->keyCount && node->keys[index] == key) {
        if (node->isLeaf) {
            // Caso 1: A chave esta no nó folha.
            for (int i = index; i < node->keyCount - 1; i++) {
                node->keys[i] = node->keys[i + 1];
            }
            node->keyCount--;
        } else {
            // Caso 2: A chave esta em um nó interno.
            Node* pred = node->children[index];
            Node* ygg = node->children[index + 1];

            if (pred->keyCount >= ORDER) {
                // Subcaso 2-a: O nó predecessor tem chaves suficientes para substituir.
                int predKey = getP(pred);
                removeKey(pred, predKey);
                node->keys[index] = predKey;
            } else if (ygg->keyCount >= ORDER) {
                // Subcaso 2-b: O nó sucessor tem chaves suficientes para substituir.
                int yggKey = getYgg(ygg);
                removeKey(ygg, yggKey);
                node->keys[index] = yggKey;
            } else {
                // Subcaso 2-c: Merge o nó com o nó sucessor e remova a chave.
                merge(node, index);
                removeKey(pred, key);
            }
        }
    } else {
        if (node->isLeaf) {
            printf("A chave %d não foi encontrada na árvore.\n", key);
            return;
        }

        int childIndex = index;
        if (index == node->keyCount) {
            childIndex = index - 1;
        }

        Node* child = node->children[childIndex];

        if (child->keyCount < ORDER) {
            // Caso 3: Garanta que o filho tem pelo menos t chaves.
            if (childIndex != node->keyCount && node->children[childIndex + 1]->keyCount >= ORDER) {
                // Subcaso 3-a: Se o filho à direita tiver pelo menos t chaves, empreste uma chave do filho à direita.
                borrowFromRightSibling(node, childIndex);
            } else if (childIndex != 0 && node->children[childIndex - 1]->keyCount >= ORDER) {
                // Subcaso 3-b: Se o filho à esquerda tiver pelo menos t chaves, empreste uma chave do filho à esquerda.
                borrowFromLeftSibling(node, childIndex);
            } else {
                // Subcaso 3-c: Funda o filho com o próximo nó.
                if (childIndex != node->keyCount) {
                    merge(node, childIndex);
                } else {
                    merge(node, childIndex - 1);
                }
            }
        }

        removeKey(child, key);
    }
}


int main() {
    Node* root = createNode();

    insert(&root, 3);
    insert(&root, 1);
    insert(&root, 8);
    insert(&root, 7);
    insert(&root, 5);
    

    printf("Árvore B após inserções: ");
    em_ordem(root);
    
    // manipulações na arvoreB
    insert(&root, 10);
    insert(&root, 2);
    removeKey(root, 3);
    removeKey(root, 5);
    
    printf("\nÁrvore B após manipulações: ");
    em_ordem(root);

    return 0;
}
