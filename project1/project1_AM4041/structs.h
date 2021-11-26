#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <cassert>
#include <stdlib.h>

using namespace std;

class LLNode {
private:
    pthread_mutex_t lock;
    long postID;
    bool marked;
    LLNode *next;
public:

    LLNode(){
        this->postID = -1;
        this->next = nullptr;
        this->marked = false;
        this->lock = PTHREAD_MUTEX_INITIALIZER;
    };

    long get_postID(){
        return this->postID;
    }

    void init_postID(long ID){
        this->postID = ID;
    }

    bool get_marked(){
        return this->marked;
    }
  
    void set_marked(){
        this->marked = true;
    } 
    
    LLNode* get_next(){
        return this->next;
    }
  
    void set_next(LLNode* next){
        this->next = next;
    }   

    void get_lock(){
        assert(pthread_mutex_lock(&(this->lock)) == 0);
    }

    void release_lock(){
        assert(pthread_mutex_unlock(&(this->lock)) == 0);
    }
};

class SinglyLinkedList {
private:
    LLNode *head;
    LLNode *tail;
public:

    SinglyLinkedList(){
        this->head = nullptr;
        this->tail = nullptr;
    }

    void set_head(LLNode* new_head){
        this->head = new_head;
    }

    LLNode* get_head(){
        return this->head;
    }

    void set_tail(LLNode* new_tail){
        this->tail = new_tail;
    }

    long index_numb(){
        long counter = 0;
        LLNode *iterator = this->head->get_next();
        while(iterator != tail)
        {
            counter++;
            iterator = iterator->get_next();
        }
        return counter;
    }

    long index_sum(){
        long sum = 0;
        LLNode *iterator = this->head->get_next();
        while(iterator != tail)
        {
            sum += iterator->get_postID();
            iterator = iterator->get_next();
        }
        return sum;
    }
};

class QueueNode {
private:
    long postID;
    QueueNode *next;
public:

    QueueNode(){
        this->postID = -1;
        this->next = nullptr;
    }

    void set_postID(long postID){
        this->postID = postID;
    }

    long get_postID(){
        return this->postID;
    }

    QueueNode* get_next(){
        return this->next;
    }
  
    void set_next(QueueNode* next){
        this->next = next;
    }   
};

class Queue {
private:
    pthread_mutex_t headLock;
    pthread_mutex_t tailLock;
    QueueNode *Head;
    QueueNode *Tail;
public:

    Queue(){
        this->Head = nullptr;
        this->Tail = nullptr;
        this->headLock = PTHREAD_MUTEX_INITIALIZER;
        this->tailLock = PTHREAD_MUTEX_INITIALIZER;
    }

    void set_head(QueueNode *new_Head){
        assert(new_Head != nullptr);
        this->Head = new_Head;
    }

    QueueNode* get_head(){
        return this->Head;
    }

    void set_tail(QueueNode *new_Tail){
        assert(new_Tail != nullptr);
        this->Tail = new_Tail;
    }

    QueueNode* get_tail(){
        return this->Tail;
    }

    void set_new_head(QueueNode *new_Head){
        assert(new_Head != nullptr && Head != nullptr);
        this->Head = new_Head;
    }

    void set_new_tail(QueueNode *new_Tail){
        assert(new_Tail != nullptr && Tail != nullptr);
        this->Tail->set_next(new_Tail);
        this->Tail = new_Tail;
    }

    void get_Hlock(){
        assert(pthread_mutex_lock(&(this->headLock)) == 0);
    }

    void release_Hlock(){
        assert(pthread_mutex_unlock(&(this->headLock)) == 0);
    }

    void get_Tlock(){
        assert(pthread_mutex_lock(&(this->tailLock)) == 0);
    }

    void release_Tlock(){
        assert(pthread_mutex_unlock(&(this->tailLock)) == 0);
    }
  
    long index_numb(){
        long counter = 0;
        QueueNode *iterator = this->Head->get_next();
        while(iterator != nullptr)
        {
            counter++;
            iterator = iterator->get_next();
        }
        return counter;
    }

    long index_sum(){
        long sum = 0;
        QueueNode *iterator = this->Head->get_next();
        while(iterator != nullptr)
        {
            sum += iterator->get_postID();
            iterator = iterator->get_next();
        }
        return sum;
    }
};

class treeNode {
private:
    pthread_mutex_t lock;
    long postID;
    bool IsRightThreaded;
    bool IsLeftThreaded;
    treeNode *rc;
    treeNode *lc;
public:

    treeNode(){
        this->postID = -1;
        this->lc = nullptr;
        this->rc = nullptr;
        this->IsRightThreaded  = false;
        this->IsLeftThreaded = false;
        this->lock = PTHREAD_MUTEX_INITIALIZER;
    }

    void set_postID(long postID){
        this->postID = postID;
    }

    long get_postID(){
        return this->postID;
    }

    void set_IsRightThreaded(bool IsRightThreaded){
        this->IsRightThreaded = IsRightThreaded;
    }

    bool get_IsRightThreaded(){
        return this->IsRightThreaded;
    }

    void set_IsLeftThreaded(bool IsLeftThreaded){
        this->IsLeftThreaded = IsLeftThreaded;
    }

    bool get_IsLeftThreaded(){
        return this->IsLeftThreaded;
    }

    treeNode* get_right_child(){
        return this->rc;
    }

    void set_right_child(treeNode* right_child){
        this->rc = right_child;
    }

    treeNode* get_left_child(){
        return this->lc;
    }

    void set_left_child(treeNode* left_child){
        this->lc = left_child;
    }

    void add_left_child(treeNode* new_node){
        new_node->lc = this->lc;
		new_node->rc = this;
		new_node->IsLeftThreaded = false;
		new_node->IsRightThreaded = false;

        this->lc = new_node;
        this->IsLeftThreaded = true;
    }

    void add_right_child(treeNode* new_node){
        new_node->rc = this->rc;
		new_node->lc = this;
		new_node->IsLeftThreaded = false;
		new_node->IsRightThreaded = false;

        this->rc = new_node;
        this->IsRightThreaded = true;
    }

    void get_lock(){
        assert(pthread_mutex_lock(&(this->lock)) == 0);
    }

    void release_lock(){
        assert(pthread_mutex_unlock(&(this->lock)) == 0);
    }
};

#endif