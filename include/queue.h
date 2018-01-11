#include "node.h"
#include <iostream>

#ifndef QUEUE_H
#define QUEUE_H
template <class T>
class Queue{
public:
    Queue(){
       tail =  head = NULL;
       size = 0;
    }

    ~Queue(){

    }

    SingleNode<T> * getHead(){
        return head;
    }

    SingleNode<T> * getTail(){
        return tail;
    }

    int getSize(){
        return size;
    }

    void enqueue(SingleNode<T> * node){
        if(head == NULL){
            head = node;
            tail = node;
        }else{
            tail->next = node;
            tail = node;
        }
        size++;
    }

    SingleNode<T>* dequeue(){
        SingleNode<T> *node = NULL;
        if(head == NULL){
            node = head;
            tail = NULL;
        }else{
            node = head;
            head = head->next;
        }
        if(node)
            node->next = NULL;

        size--;

        return node;
    }

    void print(){
        SingleNode<T> *n = head;
        while(n){
            std::cout << n->data << std::endl;
            n = n->next;
        }
    }
private:
    SingleNode<T> *head;
    SingleNode<T> *tail;
    int size;
};
#endif // QUEUE_H
