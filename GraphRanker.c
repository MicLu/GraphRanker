//
// Created by Michele on 22/12/2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <malloc.h>

#define BUFFERSIZE 1024

// CUSTOM FUNCTIONS ----------------------------------------------
void my_readLine(char buffer[]){

    int i = 0;
    char c;
    do {
        c = (char)getchar_unlocked();
        buffer[i] = c;
        i++;
    } while (c != '\n' && c != EOF);

}

int my_strtok(char * s, char delimiter, int start){

    int i = start;
    while ( s[i] != '\r' && s[i] != '\n' && s[i] != '\0'){

        if (s[i] == delimiter){
            return i;
        } else{
            i++;
        }
    }
    return i;
}

char intToChar(int n){
    switch (n) {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        default:
            return '-';
    }
}

int my_strlen(char* s){

    int len = 0;
    while ( /*s[len] != '\r' && s[len] != '\n' &&*/ s[len] != '\0') {
        len++;
    }
    return len;

}

int my_strcmp(char* s, char* toCompare){

    int len = my_strlen(toCompare);

    int i = 0;
    while ( i < len ){
        if (s[i] != toCompare[i]){
            return 1;
        }
        i++;
    }
    return 0;

}

int my_strtol(char* s, int start, int end){

    int numero = 0;
    int grado = (end) - (start);

    for (int i = start; i <= end; i++) {
//        numero += charToInt(s[i]) * my10Pow(grado);
        numero = numero * 10 + (s[i] - '0');
        grado--;
    }

//    start = end;
    return numero;
}

void my_printIDGrafo(int idGrafo){

    if (idGrafo/10 != 0){
        my_printIDGrafo(idGrafo/10);
    }
    putchar_unlocked( intToChar(idGrafo%10) );
//    putchar_unlocked( idGrafo%10 + '0' );

}
//----------------------------------------------------------------

//Numero di nodi
static int d;
//Lunghezza della classifica
static int k;

//Numero di grafi inseriti
int grafiAggiunti;

//CamminoMinimo massimo tra i k migliori grafi inseriti
int camminoMinimoMAX;

// (deprecated) Struct for dijkstra's priority queue ------------------------------
struct MinHeapNode{
    int to;
    int cost;
};

struct MinHeap{
    int size;
    struct MinHeapNode* priorityQueue;
};

// Struct for topK (store inserted graph) ------------------------------
struct Grafo{
    int camminoMinimo;
    int idGrafo;
};

struct RBTreeNode{
    struct RBTreeNode* father;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
//    int data;
    struct Grafo grafo;
    char color;
};

struct RBTreeNode NULLNode;
struct RBTreeNode* RBTreeRoot;

// Prototype -----------------------------------------------------------------------
void aggiungiGrafo(int **M);
void topK();

//(deprecated) dijkstra
struct MinHeapNode * newMinHeapNode(int from, int to, int cost){
    struct MinHeapNode *minHeapNode = malloc(sizeof (struct MinHeapNode));
    minHeapNode->to = to;
    minHeapNode->cost = cost;

    return minHeapNode;
}
struct MinHeap * newMinHeap(){
    struct MinHeap *minHeap = malloc(sizeof (struct MinHeap));
    minHeap->size = 0;
    minHeap->priorityQueue = malloc( (d*d) * sizeof(struct MinHeapNode) );
    return minHeap;
}
void swapElement(struct MinHeapNode *a, struct MinHeapNode *b);
struct MinHeapNode minHeap_PopMin(struct MinHeap *minHeap);
void minHeap_Add(struct MinHeap *minHeap, struct MinHeapNode nodeToAdd);
void minHeap_HeapifyUp(struct MinHeap *minHeap);
void minHeap_HeapifyDown(struct MinHeap *minHeap);
int minHeap_GetParent(int index){
    return floor((index - 1)/2);

}
int minHeap_GetLeft(int index){
    return 2*index + 1;

}
int minHeap_GetRight(int index){
    return 2*index + 2;

}
int dijkstra(int **matrix, struct MinHeap* mainMinHeap);

//dijkstra without priority queue
int selectMinDistanceVertex(int* dist, int* sptSet);
int dijkstraNoQueue(int** graph);

//topk
struct RBTreeNode* newRBTreeNode(int idGrafo, int camminoMinimo);
void RBTree_Visit(struct RBTreeNode* rbTreeNode, int* limiteStampa);
void RBTree_LeftRotate(struct RBTreeNode* rbTreeNode);
void RBTree_RightRotate(struct RBTreeNode* node_y);
void RBTree_InsertFix(struct RBTreeNode* z);
void RBTree_Insert(struct RBTreeNode* nodeToAdd);
void RBTree_Transplant(struct RBTreeNode* u, struct RBTreeNode* v);
struct RBTreeNode* RBTree_getMax();
void RBTree_DeleteMax();
void RBTree_DeleteFix(struct RBTreeNode* x);


// Prototype implementation ------------------------------------------------------------

// (deprecated) dijkstra (old version)
void swapElement(struct MinHeapNode *a, struct MinHeapNode *b){
    struct MinHeapNode temp = *b;
    *b = *a;
    *a = temp;
}
struct MinHeapNode minHeap_PopMin(struct MinHeap *minHeap){
    if( minHeap->size==0 ){
//        return newMinHeapNode(0,0,0);
    }
    struct MinHeapNode minDistanceNode = minHeap->priorityQueue[0];
    swapElement(&minHeap->priorityQueue[0], &minHeap->priorityQueue[minHeap->size - 1]);
//    free(&minHeap->priorityQueue[minHeap->size - 1]);
    minHeap->size--;

    //ripristino proprietà del MinHeap
    minHeap_HeapifyDown(minHeap);

    return minDistanceNode;
}
void minHeap_Add(struct MinHeap *minHeap, struct MinHeapNode nodeToAdd){

    minHeap->priorityQueue[minHeap->size] = nodeToAdd;
    minHeap->size++;

    //ripristino proprietà del MinHeap
    minHeap_HeapifyUp(minHeap);

}
void minHeap_HeapifyUp(struct MinHeap *minHeap){

    int index = minHeap->size - 1;
    while ( index > 0 && minHeap->priorityQueue[minHeap_GetParent(index)].cost > minHeap->priorityQueue[index].cost ){
        swapElement(&minHeap->priorityQueue[minHeap_GetParent(index)] , &minHeap->priorityQueue[index] );
        index = minHeap_GetParent(index);
    }

}
void minHeap_HeapifyDown(struct MinHeap *minHeap){

    int current = 0;
    int leftIndex = minHeap_GetLeft(current);
    int rightIndex = minHeap_GetRight(current);

    //Fintanto che esista il figlio sinistro e sia minore del padre o
    //che esista il figlio destro e sia minore del padre
    while ( (leftIndex < minHeap->size && minHeap->priorityQueue[leftIndex].cost < minHeap->priorityQueue[current].cost) ||
            (rightIndex < minHeap->size && minHeap->priorityQueue[rightIndex].cost < minHeap->priorityQueue[current].cost) ){

        //Se esistono entrambi i figli
        if( leftIndex < minHeap->size && rightIndex < minHeap->size ){

            //responsabile dell'ordine di estrazione di elementi doppi (< vs <=)
            //elemento minore è a sinistra
            if( minHeap->priorityQueue[leftIndex].cost <= minHeap->priorityQueue[rightIndex].cost ){
                swapElement(&minHeap->priorityQueue[current], &minHeap->priorityQueue[leftIndex]);
                current = leftIndex;
            }
                //elemento minore è a destra
            else{
                swapElement(&minHeap->priorityQueue[current], &minHeap->priorityQueue[rightIndex]);
                current = rightIndex;
            }

        }
            //esiste solo un figlio, scambaire solo con il sinistro
        else{
            swapElement(&minHeap->priorityQueue[current], &minHeap->priorityQueue[leftIndex]);
            current = leftIndex;
        }

        leftIndex = minHeap_GetLeft(current);
        rightIndex = minHeap_GetRight(current);

    }

}
int dijkstra(int **matrix, struct MinHeap* mainMinHeap){

    //Array dei costi dei percorsi minimi
    int pathCost[d];
    //Costo nodo sorgente
    pathCost[0] = 0;
    for (int i = 1; i < d; ++i) {
        //Costo altri nodi
        pathCost[i] = INT_MAX;
    }
    struct MinHeap* dijkstraMinHeap = mainMinHeap;

    for (int i = 0; i < d; ++i) {
        //C'è un lato uscente dalla sorgente
        if (matrix[0][i] != 0) {
            //evitare questo malloc per risparmiare tempo
//            struct MinHeapNode *minHeapNode = setMinHeapNode(mainMinHeapNode, 0, i, matrix[0][i]);
            struct MinHeapNode minHeapNode;
            minHeapNode.to = i;
            minHeapNode.cost = matrix[0][i];
            minHeap_Add(dijkstraMinHeap, minHeapNode);
//            free(minHeapNode);
        }
    }

    //Fintantoché ci sono nodi non ancora visitati
    // FIXME: aggiungere controllo se ci sono ancora nodi con costo INT_MAX?
    while (dijkstraMinHeap->size != 0){

        //estraggo il nodo adiacente a costo minimo
        struct MinHeapNode minAdjacentNode = minHeap_PopMin(dijkstraMinHeap);

        //Controllo che non ci sia un percorso verso questo vertice
        if(pathCost[minAdjacentNode.to]==INT_MAX){
            pathCost[minAdjacentNode.to] = minAdjacentNode.cost;

            //controllo tutti i nodi adiacenti a quello appena estratto
            for (int i = 0; i < d; ++i) {
                //Solo se non è un auto-anello -- e -- solo se c'è un lato uscente
                if (minAdjacentNode.to != i && matrix[minAdjacentNode.to][i] != 0) {
                    //se non c'è un percorso verso questo nodo di arrivo
                    if (pathCost[i] == INT_MAX){
//                        struct MinHeapNode *minHeapNode =
//                                newMinHeapNode(minAdjacentNode.to, i,pathCost[minAdjacentNode.to] + matrix[minAdjacentNode.to][i]);
//                        minHeap_Add(dijkstraMinHeap, minHeapNode);

//                        struct MinHeapNode *nodeToAdd = setMinHeapNode(mainMinHeapNode, minAdjacentNode.to, i,
//                                                                       pathCost[minAdjacentNode.to] +
//                                                                       matrix[minAdjacentNode.to][i]);
                        struct MinHeapNode nodeToAdd;
                        nodeToAdd.to = i;
                        nodeToAdd.cost = pathCost[minAdjacentNode.to] + matrix[minAdjacentNode.to][i];
                        minHeap_Add(dijkstraMinHeap, nodeToAdd );
//                        free(nodeToAdd);
                    }
                }
            }
        }
    }


//    free(dijkstraMinHeap->priorityQueue);
//    free(dijkstraMinHeap);

    int pathCostSum = 0;
    //Somma dei costi di tutti i cammini minimi
    for (int i = 0; i < d; ++i) {
        if (pathCost[i] != INT_MAX) {
            pathCostSum += pathCost[i];
            //Se la somma supera il costo del cammino massimo vuol dire che
            //sicuramente questo grafo non andrà in classifica, quindi non lo aggiungo
            if (pathCostSum >= camminoMinimoMAX && grafiAggiunti >= k){
                return -1;
            }
        }
    }

    //FIXME: debug only
//    printf("Aggiunto grafo: [%2d] %d\n", grafiAggiunti, pathCostSum);

    return pathCostSum;

}

//dijkstra without priority queue
int selectMinDistanceVertex(int* dist, int* sptSet){
    // Initialize min value
    int min = INT_MAX;
    int min_index=0;

    for (int v = 0; v < d; v++) {
        if (sptSet[v] == 0 && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}
int dijkstraNoQueue(int** graph){

    //Array dei costi dei percorsi minimi dal nodo sorgente
    // Contains the shortest distance from src to i
    int pathCost[d];

    //1 se il vertice è stato aggiunto nel shortest path tree
    int shortestPathTree[d];

    // Initialize all distances as INF and shortestPathTree[] as false (0)
    for (int i = 0; i < d; i++){
        pathCost[i] = INT_MAX;
        shortestPathTree[i] = 0;
    }

    // Distance of source vertex from itself is always 0
    pathCost[0] = 0;

    // Find the shortest path for all vertices
    for (int count = 0; count < d - 1; count++) {

        //Scelgo il vertice a distanza minore tra quelli che non sono ancora stati aggiunti al shortest path tree
        int minDistVertex = selectMinDistanceVertex(pathCost, shortestPathTree);

        // Mark the picked vertex as processed (1)
        shortestPathTree[minDistVertex] = 1;

        // Update pathCost value of the adjacent vertices of the picked vertex.
        for (int vertex = 0; vertex < d; vertex++)

            // Update pathCost[vertex] only if:
            // -No auto-anelli
            // -Vertex is not in the shortestPathTree
            // -there is an edge from minDistVertex to vertex
            // -Total weight of path from src to vertex through minDistVertex is smaller than current value of pathCost[vertex]
            if (minDistVertex != vertex && shortestPathTree[vertex] == 0 && graph[minDistVertex][vertex] != 0 && pathCost[minDistVertex] != INT_MAX
                && pathCost[minDistVertex] + graph[minDistVertex][vertex] < pathCost[vertex] ) {

                pathCost[vertex] = pathCost[minDistVertex] + graph[minDistVertex][vertex];
            }
    }

    //Sum the minimum cost of all the reachable vertex from source
    int pathCostSum = 0;
    for (int i = 0; i < d; ++i) {
        if (pathCost[i] != INT_MAX) {
            pathCostSum += pathCost[i];
            //Se la somma supera il costo del cammino massimo vuol dire che
            //sicuramente questo grafo non andrà in classifica, quindi non lo aggiungo
            if (pathCostSum >= camminoMinimoMAX && grafiAggiunti >= k){
                return -1;
            }
        }
    }
//    printf("Aggiunto grafo: [%2d] %d\n", grafiAggiunti, pathCostSum);
    return pathCostSum;
}

//topk
struct RBTreeNode* newRBTreeNode(int idGrafo, int camminoMinimo){

    struct RBTreeNode* newNode = malloc(sizeof(struct RBTreeNode));
    newNode->left = &NULLNode;
    newNode->right = &NULLNode;
    newNode->father = &NULLNode;
    newNode->grafo.idGrafo = idGrafo;
    newNode->grafo.camminoMinimo = camminoMinimo;
    newNode->color = 'r';

    return newNode;
}
void RBTree_Visit(struct RBTreeNode* rbTreeNode, int* limiteStampa){
    if (*limiteStampa > 0) {
        //Non ho aggiunto nessun grafo oppure k = 0
        //    if (limiteStampa == 0){
        //        printf("fine\n");
        //        return;
        //    }
        if (rbTreeNode == &NULLNode) {
            return;
        }
        //Visita in ordine: Left-current-right
        //    if (rbTreeNode->left != NULL) {
        RBTree_Visit(rbTreeNode->left, limiteStampa);
        //    }
        if (*limiteStampa > 1) {
            //FIXME: debug only
//            printf("[id: %2d] - costo: %6d - (%c)\n", rbTreeNode->grafo.idGrafo, rbTreeNode->grafo.camminoMinimo, rbTreeNode->color);
//            printf("%d ", rbTreeNode->grafo.idGrafo);
            my_printIDGrafo(rbTreeNode->grafo.idGrafo);
            putchar_unlocked(' ');
            (*limiteStampa)--;
        }
            //Ultimo indice stampato senza spazio dopo
        else if (*limiteStampa == 1){
//            printf("%d", rbTreeNode->grafo.idGrafo);
            my_printIDGrafo(rbTreeNode->grafo.idGrafo);
            (*limiteStampa)--;
        }
        else{
            return;
        }

        //    if (rbTreeNode->right != NULL) {
        RBTree_Visit(rbTreeNode->right, limiteStampa);
        //    }
    }

    else if (*limiteStampa == 0){
        printf("\n");
    }

}
void RBTree_LeftRotate(struct RBTreeNode* rbTreeNode){

    struct RBTreeNode* node_y = rbTreeNode->right;
    rbTreeNode->right = node_y->left;
    if (node_y->left != &NULLNode){
        node_y->left->father = rbTreeNode;
    }
    node_y->father = rbTreeNode->father;

    //Ruoto la radice
    if (rbTreeNode->father == &NULLNode){
        RBTreeRoot = node_y;
    } else if (rbTreeNode == rbTreeNode->father->left){
        rbTreeNode->father->left = node_y;
    } else{
        rbTreeNode->father->right = node_y;
    }

    node_y->left = rbTreeNode;
    rbTreeNode->father = node_y;

}
void RBTree_RightRotate(struct RBTreeNode* node_y){

    struct RBTreeNode* node_x = node_y->left;
    node_y->left = node_x->right;
    if (node_x->right != &NULLNode){
        node_x->right->father = node_y;
    }
    node_x->father = node_y->father;

    //Ruoto la radice
    if (node_y->father == &NULLNode){
        RBTreeRoot = node_x;
    } else if (node_y == node_y->father->left){
        node_y->father->left = node_x;
    } else{
        node_y->father->right = node_x;
    }

    node_x->right = node_y;
    node_y->father = node_x;

}
void RBTree_Insert(struct RBTreeNode* nodeToAdd){

    struct RBTreeNode* pre = &NULLNode;
//    struct RBTreeNode* current = rbTreeRoot;
    struct RBTreeNode* current = RBTreeRoot;

    //RB non è vuoto
    while (current != &NULLNode) {

        pre = current;
        if (nodeToAdd->grafo.camminoMinimo < current->grafo.camminoMinimo) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    nodeToAdd->father = pre;

    //Controllo dove inserire il nuovo nodo
    if (pre == &NULLNode){
        //Primo nodo aggiunto all'albero
        RBTreeRoot = nodeToAdd;
    } else if (nodeToAdd->grafo.camminoMinimo < pre->grafo.camminoMinimo){
        pre->left = nodeToAdd;
    } else{
        //Elementi con lo stesso valore vengono aggiunti a dx
        pre->right = nodeToAdd;
    }

    //Aggiorno il valore del costo del camminoMinimo Massimo tra gli elem. inseriti
    if (nodeToAdd->grafo.camminoMinimo > camminoMinimoMAX){
        camminoMinimoMAX = nodeToAdd->grafo.camminoMinimo;
    }

    nodeToAdd->left = &NULLNode;
    nodeToAdd->right = &NULLNode;
    nodeToAdd->color = 'r';
    //Ripristina bilanciamento
    RBTree_InsertFix(nodeToAdd);
}
void RBTree_InsertFix(struct RBTreeNode* z){

    //Controllo che non è la radice
    if (z->father != &NULLNode) {
        while (z->father != &NULLNode && z->father->color == 'r') {
            if (z->father == z->father->father->left) { //Caso A: Genitore di z è figlio sinistro del nonno di z
                struct RBTreeNode *y = z->father->father->right;
                if (y!=&NULLNode && y->color == 'r') {   //Caso 1: zio di z è figlio sinistro del nonno di z
                    z->father->color = 'b';
                    y->color = 'b';
                    z->father->father->color = 'r';
                    z = z->father->father;
                } else {
                    if (z == z->father->right) {  //Caso 2: z è figlio destro -> rotazione sinistra
                        z = z->father;
                        RBTree_LeftRotate(z);
                    }
                    z->father->color = 'b'; //Caso 3
                    z->father->father->color = 'r';
                    if (z == z->father->right){
                        RBTree_LeftRotate(z->father->father);
                    } else {
                        RBTree_RightRotate(z->father->father);
                    }
                }
            } else {    //Caso B: Genitore di z è figlio destro del nonno di z
                struct RBTreeNode *y = z->father->father->left;
                if (y!=&NULLNode && y->color == 'r') {   //Caso 1
                    z->father->color = 'b';
                    y->color = 'b';
                    z->father->father->color = 'r';
                    z = z->father->father;
                } else {
                    if (z == z->father->left) {  //Caso 2: z è figlio sinistro -> rotazione destra
                        z = z->father;
                        RBTree_RightRotate(z);
                    }
                    z->father->color = 'b'; //Caso 3
                    z->father->father->color = 'r';
                    if (z == z->father->right){
                        RBTree_LeftRotate(z->father->father);
                    } else {
                        RBTree_RightRotate(z->father->father);
                    }
                }
            }
        }
    }
    RBTreeRoot->color = 'b';
}
void RBTree_Transplant(struct RBTreeNode* u, struct RBTreeNode* v){

    if (u->father == &NULLNode){
        RBTreeRoot = v;
    } else if (u == u->father->left){
        u->father->left = v;
    } else{
        u->father->right = v;
    }
    v->father = u->father;

}
struct RBTreeNode* RBTree_getMax(){

    struct RBTreeNode* pre = &NULLNode;
    struct RBTreeNode* current = RBTreeRoot;

    //RB non è vuoto
    while (current != &NULLNode) {

        pre = current;
        current = current->right;
//        if (current->right != NULL) {
//        }
    }

    return pre;

}
void RBTree_DeleteMax(){

    struct RBTreeNode* z = RBTree_getMax();
    char z_originalColor = z->color;

    struct RBTreeNode* x = NULL;

    if (z->left == &NULLNode){
        x = z->right;
        RBTree_Transplant(z,z->right);
    } else if (z->right == &NULLNode){
        x = z->left;
        RBTree_Transplant(z,z->left);
    }

    if (z_originalColor == 'b'){
        RBTree_DeleteFix(x);
    }

    //Aggiorno il valore del cammino minimoMassimo dopo la rimozione dell'elemento
    camminoMinimoMAX = RBTree_getMax()->grafo.camminoMinimo;

}
void RBTree_DeleteFix(struct RBTreeNode* x){

    while (x != RBTreeRoot && x->color == 'b'){

        if (x == x->father->left){
            //w è il fratello di x
            struct RBTreeNode* w = x->father->right;
            if (w->color == 'r'){
                //CASO 1
                w->color = 'b';
                x->father->color = 'r';
                RBTree_LeftRotate(x->father);
                w = x->father->right;
            }
            if (w->left->color == 'b' && w->right->color == 'b'){
                //CASO 2
                w->color = 'r';
                x = x->father;
            } else{
                if (w->right->color == 'b'){
                    //CASO 3
                    w->left->color = 'b';
                    w->color = 'r';
                    RBTree_RightRotate(w);
                    w = x->father->right;
                }
                //CASO 4
                w->color = x->father->color;
                x->father->color = 'b';
                w->right->color = 'b';
                RBTree_LeftRotate(x->father);
                x = RBTreeRoot;
            }
        } else{
            //w è il fratello di x
            struct RBTreeNode* w = x->father->left;
            if (w->color == 'r'){
                w->color = 'b';
                x->father->color = 'r';
                RBTree_RightRotate(x->father);
                w = x->father->left;
            }
            if (w->right->color == 'b' && w->left->color == 'b'){
                w->color = 'r';
                x = x->father;
            } else{
                if (w->left->color == 'b'){
                    w->right->color = 'b';
                    w->color = 'r';
                    RBTree_LeftRotate(w);
                    w = x->father->left;
                }
                w->color = x->father->color;
                x->father->color = 'b';
                w->left->color = 'b';
                RBTree_RightRotate(x->father);
                x = RBTreeRoot;
            }
        }

    }
    x->color = 'b';

}


int main() {

    NULLNode.grafo.idGrafo = -1;
    NULLNode.grafo.camminoMinimo = -1;
    NULLNode.color = 'b';
    RBTreeRoot = &NULLNode;

    char buffer[BUFFERSIZE];
    int startToken = 0;
    int endToken = 0;
//    char *token = malloc(15 * sizeof(char));;
//    if ( fgets(buffer, BUFFERSIZE, stdin) ){}

    my_readLine(buffer);

    //Leggo d
    endToken = my_strtok(buffer, ' ', startToken);
    d = my_strtol(buffer, startToken, endToken-1);
    startToken = endToken + 1;

    //Leggo k
    endToken = my_strtok(buffer, ' ', startToken);
    k = my_strtol(buffer, startToken, endToken-1);

    //    printf("Nodi = %d \nClassifica = %d\n",d,k);

    grafiAggiunti = 0;
    camminoMinimoMAX = 0;

    //alloco dinamicamente la matrice
    int **M = malloc(d * sizeof(int*));
    for (int r = 0; r < d; ++r) {
        M[r] = malloc(d * sizeof(int) );
    }

    //Alloco dinamicamente
    struct MinHeap* minHeap = newMinHeap();

    while (1) {
//        if ( fgets(buffer, BUFFERSIZE, stdin) == NULL ){
        my_readLine(buffer);
        if ( buffer[0] == EOF ){
            //Libero memoria allocata
            for (int r = 0; r < d; ++r) {
                free(M[r]);
            }
            free(M);
//            free(token);

            free(minHeap->priorityQueue);
            free(minHeap);

            //TODO: free del RBTree
            return 0;
        }
//        token = my_strtok(buffer, '\r', token);
        endToken = my_strtok(buffer, '\r', startToken);
        if (my_strcmp(buffer, "AggiungiGrafo") == 0) {
            aggiungiGrafo(M);

        } else if (my_strcmp(buffer, "TopK") == 0) {
            topK();
        }
    }

}

void aggiungiGrafo(int **M){

//    printf("\nFunzione aggiungi grafo\n");

    //2^32 - 1  -> valore di INT_MAX per cui servono 10 caratteri per essere rappresentati
    //d-1 -------> virgole che separano tutti i d numeri
    //TODO: aggiungere un ulteriore carattere per il carattere terminatore di fine stringa?
    int lunghezzaInput = 10*d + d-1;
    char buffer[ lunghezzaInput ];
//    getchar(); //per pulire stdin
//    fgets(buffer, BUFFERSIZE , stdin);
//    const char s[] = ",|[\\n]";
//    char *token = mainToken;

    int startToken = 0;
    int endToken = 0;

    int r,c;
    for (r = 0; r < d; r++){
        startToken = 0;
        endToken = 0;

        //Primo token
        my_readLine(buffer);
        endToken = my_strtok(buffer, ',', startToken);

        //Ciclo sugli altri token
        for (c = 0; c < d; c++){
            M[r][c]= my_strtol(buffer, startToken, endToken-1);
            startToken = endToken + 1;
            endToken = my_strtok(buffer, ',', startToken);
        }
    }

    int idGrafo = grafiAggiunti;
//    int camminoMinimo = dijkstra(M, mainMinHeap);
    int camminoMinimo = dijkstraNoQueue(M);

    if (camminoMinimo != -1){

        //La classifica non è ancora completa
        if (grafiAggiunti < k){
            RBTree_Insert( newRBTreeNode(idGrafo, camminoMinimo) );
        }
        //La classifica è già piena
        else{
            //So già che è un cammino migliore del MAX inserito
            //-cancello l'ultimo e poi inserisco
            RBTree_DeleteMax(); // Aggiorna anche camminoMinimoMAX
            RBTree_Insert( newRBTreeNode(idGrafo, camminoMinimo) );
        }

    }


    grafiAggiunti++;


}

void topK(){

//    printf("\nFunzione classifica\n");
    //Alloco come puntatore in modo che non vari durante le chiamate ricorsive
    int* limiteStampa = malloc(sizeof(int*));

    if (k <= grafiAggiunti){
        *limiteStampa = k;
    } else{
        *limiteStampa = grafiAggiunti;
    }

    RBTree_Visit(RBTreeRoot, limiteStampa);

    free(limiteStampa);
}