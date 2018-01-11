#include <string.h>
#include <stdint.h>



#ifndef NODE_H
#define NODE_H

template<class T> class TreeNode{
public:
    TreeNode(){
        memset(&data,0,sizeof(T));
        child[0] = child[1] = NULL;
        balance = 0;
    }

    TreeNode(T data){
        this->data = data;
        child[0] = child[1] = NULL;
        balance = 0;
    }

    uint8_t balance;
    TreeNode<T>* child[2];
    T data;
};


template<class T>
class SingleNode{
public:
    SingleNode(){
        memset(&data,0,sizeof(T));
        next = NULL;
    }
    SingleNode(T data){
        this->data = data;
        next = NULL;
    }
    SingleNode(T data,SingleNode<T>* next){
        this->data = data;
        this->next = next;
    }

    T data;
    SingleNode<T>* next;
};


#endif // NODE_H
