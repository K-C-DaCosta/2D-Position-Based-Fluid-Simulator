#include "node.h"

#ifndef STACK_H
#define STACK_H

template<class T> class Stack{
public:
    Stack(){
        top = NULL;
    }

    ~Stack(){

    }

    void push(T data){
        SingleNode<T>* temp = new SingleNode<T>(data);
        temp->next = top;
        top = temp;
    }

    void push(SingleNode<T> *node){
        node->next = top;
        top = node;
    }

    T* pop(){
        SingleNode<T>* old_top = top;
        if(top)
            top = top->next;
        return old_top;
    }

    void print(){
        SingleNode<T>* n = top;
        while(n){
            std::cout << "[" << n->data << "]";
            n = n->next;
        }
        std::cout << std::endl;
    }

    void cleanUp(){
        SingleNode<T>* tnext = NULL;
        while(top){
            tnext = top->next;
            delete top;
            top = tnext;
        }
    }

private:
    SingleNode<T> *top;
};
#endif // STACK_H
