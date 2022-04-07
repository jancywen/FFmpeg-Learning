#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NodeData{
    int length;
} NodeData;



// 节点
typedef struct QNode
{
   NodeData data;
   struct QNode *next;
}QNode;
 
// 队列
typedef struct ChainQueue
{
    struct QNode *first_node, *last_node;
    int nb_nodes;
} ChainQueue;

// 初始化队列
int initQueue(ChainQueue *q) {
    memset(q, 0, sizeof(ChainQueue));

    return 0;
}

// 入队
int node_put(ChainQueue *q, NodeData *data) {


    QNode *node = (QNode *)malloc(sizeof(QNode));
    node->data = *data;
    node->next = NULL;

    if (!q->last_node) 
        q->first_node = node;
    else 
        q->last_node->next = node;
    
    q->last_node = node;
    q->nb_nodes++;
    return 0;
}

// 出队
int node_get(ChainQueue *q, NodeData *data) {

    int ret = 0;
    QNode *temp = NULL;
    temp = q->first_node;
    if (temp) {
        q->first_node = temp->next;
        if (!q->first_node)
            q->last_node = NULL;
        q->nb_nodes--;
        *data = temp->data;
        free(temp);
    }else {
        ret = -1;
    }

    return ret;
}



int main(int argc, char const *argv[])
{
    ChainQueue queue;
    // if (initQueue(&queue) < 0)
    //     return -1;
    printf("node number: %d\n", queue.nb_nodes);
    
    NodeData data1, data2, data3;
    data1.length = rand();
    printf("data1: %d\n", data1.length);
    data2.length = rand();
    printf("data2: %d\n", data2.length);
    data3.length = rand();  
    printf("data3: %d\n", data3.length);

    node_put(&queue, &data1);
    printf("node number: %d\n", queue.nb_nodes);

    node_put(&queue, &data2);
    printf("node number: %d\n", queue.nb_nodes);

    node_put(&queue, &data3);
    printf("node number: %d\n", queue.nb_nodes);


    NodeData d1, d2, d3;

    node_get(&queue, &d1);
    printf("d1: %d\n", d1.length);
    printf("node number: %d\n", queue.nb_nodes);

    node_get(&queue, &d2);
    printf("d2: %d\n", d2.length);
    printf("node number: %d\n", queue.nb_nodes);

    node_get(&queue, &d3);
    printf("d3: %d\n", d3.length);
    printf("node number: %d\n", queue.nb_nodes);

    return 0;
}
