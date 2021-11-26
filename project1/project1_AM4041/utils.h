#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include "structs.h"
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <vector>

using namespace std;
extern SinglyLinkedList manager;
extern long N;
extern long M;
extern vector<Queue> Categories;
extern treeNode* root;

bool Validate(LLNode* pred, LLNode* curr){
    assert(pred);
    assert(curr);
    if( pred->get_marked() == false && 
        curr->get_marked() == false &&
        pred->get_next()   == curr)
        return true;
    else
        return false;
}

bool Search (long postID){
    LLNode *curr;
    bool result;

    curr = manager.get_head();

    while(curr->get_postID() < postID)
            curr = curr->get_next();

    if(curr->get_marked() != true && curr->get_postID() == postID)
        return true;
    else
        return false;
}

bool Insert(long postID){
    LLNode* pred  = new LLNode();
    LLNode* curr = new LLNode();
    bool result;
    bool return_flag = false;
    
    while(true)
    {
        pred = manager.get_head();
        curr = pred->get_next();
            
        while(  curr->get_postID() != -1 && 
                curr->get_postID() < postID)
        {
            pred = curr;
            curr = curr->get_next();
        }
         
        pred->get_lock();
        curr->get_lock();
        
        if(Validate(pred,curr))
        {
            if(postID == curr->get_postID())
                result = false;
            else
            {
                LLNode* new_node = new LLNode();
                new_node->init_postID(postID);
                new_node->set_next(curr);
                pred->set_next(new_node);
                result = true;
            }
            return_flag = true;
        }

        pred->release_lock();
        curr->release_lock();

        if(return_flag)
            return result;
    }
}

bool Delete(long postID){
    LLNode *curr,*pred;
    bool result;
    bool return_flag = false;

    while(true)
    {
        pred = manager.get_head();
        curr = pred->get_next();
       
        while(  curr->get_postID() != -1 && 
                curr->get_postID() < postID)
        {
            pred = curr;
            curr = curr->get_next();
        }

        pred->get_lock();
        curr->get_lock();
        
        if(Validate(pred,curr))
        {
            if(postID != curr->get_postID())
                result = false;
            else
            {
                curr->set_marked();
                pred->set_next(curr->get_next());
                result = true;
            }
            return_flag = true;
        }
        
        pred->release_lock();
        curr->release_lock();
        
        if(return_flag)
            return result;
    }
}

bool Enq(long postID, long queue_num){
    QueueNode* new_qnode = new QueueNode();
    new_qnode->set_postID(postID);
    Categories.at(queue_num).get_Tlock();
    Categories.at(queue_num).set_new_tail(new_qnode);
    Categories.at(queue_num).release_Tlock();
    return true;
}

long Deq(long queue_num) {
    long result = 0;

    Categories.at(queue_num).get_Hlock();
    if (Categories.at(queue_num).get_head()->get_next() == Categories.at(queue_num).get_tail())
        result = -2;
    else {
        result = Categories.at(queue_num).get_head()->get_next()->get_postID();
        Categories.at(queue_num).set_new_head(Categories.at(queue_num).get_head()->get_next());
    }
    Categories.at(queue_num).release_Hlock();
    return result;
} 

bool BTSinsert(long postID) {
	treeNode* curr = new treeNode();
    treeNode* pred = new treeNode();
    treeNode* new_node = new treeNode();
    new_node->set_postID(postID);

	bool result = true;
	long side_right = -1;
    
	root->get_lock();
	pred = root;
	curr = pred->get_left_child();
	
	if (pred == curr) 
    {
		root->add_left_child(new_node);
		root->release_lock();
		return result;
	}
	else 
    {
        curr->get_lock();
        
		while (true)
		{
			if (curr->get_postID() > postID)
            {
				side_right = 0;
                if (!curr->get_IsLeftThreaded())
                    break;
                pred->release_lock();
                pred = curr;
                curr = curr->get_left_child();
                curr->get_lock();
			}
			else if (curr->get_postID() < postID)
            {
				side_right = 1;
                if (!curr->get_IsRightThreaded())
                    break;
			    pred->release_lock();
                pred = curr;
                curr = curr->get_right_child();
                curr->get_lock();
			}
			else
            {
				result = false;
				break;
			}
		}
	    	
        pred->release_lock();    
		
        if (result) {
			if (side_right == 0) 
            {	
                curr->add_left_child(new_node);
			}
			else if (side_right == 1) 
            {
				curr->add_right_child(new_node);
			}
		}
		curr->release_lock();
		return result;
	}
}

bool BTSsearch(long postID) {
	treeNode* curr = new treeNode();
    treeNode* pred = new treeNode();
    
	bool result = true;
    
	root->get_lock();
	pred = root;
	curr = pred->get_left_child();

	if (root == curr) 
    {
        root->release_lock();
		result = false;
	}
	else 
    {     
        curr->get_lock();
		while (true)
		{
			if (curr->get_postID() > postID)
            {
				if (!curr->get_IsLeftThreaded())
                {
					result = false;
					break;
				}
				pred->release_lock();
                pred = curr;
                curr = curr->get_left_child();
                curr->get_lock();
			} 
			else if (curr->get_postID() < postID)
            {

				if (!curr->get_IsRightThreaded())
                {
					result = false;
					break;
				}
				pred->release_lock();
                pred = curr;
                curr = curr->get_right_child();
                curr->get_lock();
			}
			else 
            {
				result = true;
				break;
			}
		}
        pred->release_lock();
        curr->release_lock();
	}
	return result;
}

treeNode* predecessor_inorder(treeNode* node) {

	if (!node->get_IsLeftThreaded()) 
		return node->get_left_child();
	else 
    {
        treeNode* dump = new treeNode();
        dump = node->get_left_child();
        dump->get_lock();

		while (dump->get_IsRightThreaded())
		{
			dump->release_lock();
			dump = dump->get_right_child();
			dump->get_lock();
		}
        return dump;
	}
}

treeNode* successor_inorder(treeNode* node) {

	if (!node->get_IsRightThreaded())
		return node->get_right_child();
	else 
    {
		treeNode* dump = new treeNode();
        dump = node->get_right_child();
        dump->get_lock();

		while (dump->get_IsLeftThreaded())
		{
			dump->release_lock();
			dump = dump->get_left_child();
			dump->get_lock();
		}
        return dump;
	}
}

bool BTSdelete(long postID) {
	treeNode* curr = new treeNode();
    treeNode* pred = new treeNode();
   
	bool result = true;
	long side_right = -1;
    
	root->get_lock();
	pred = root;
	curr = pred->get_left_child();

    if (pred == curr) 
    {
		result = false;
	    root->release_lock();
		return result;
	}
   
    curr->get_lock();

    while (true)
    {
        if (curr->get_postID() > postID)
        {
            side_right = 0;
            if (!curr->get_IsLeftThreaded())
            {
                result = false;
                break;
            }
            pred->release_lock();
            pred = curr;
            curr = curr->get_left_child();
            curr->get_lock();
        } 
        else if (curr->get_postID() < postID)
        {
            side_right = 1;
            if (!curr->get_IsRightThreaded())
            {
                result = false;
                break;
            }
            pred->release_lock();
            pred = curr;
            curr = curr->get_right_child();
            curr->get_lock();
        }
        else 
        {
            if(side_right == -1)
            {
                side_right = 0;
            }
            result = true;
            break;
        }
    }
    

    if (result) 
    {
        treeNode* predecessor = new treeNode();
        treeNode* successor = new treeNode();

        if (!curr->get_IsLeftThreaded() && !curr->get_IsRightThreaded()) 
        {
            predecessor = predecessor_inorder(curr);
            successor = successor_inorder(curr);
            
            if (side_right == 0) 
            {
                successor->set_left_child(predecessor);
                successor->set_IsLeftThreaded(false);
            }
            else 
            {
                predecessor->set_right_child(successor);
                predecessor->set_IsRightThreaded(false);
            }
        }
        else if (curr->get_IsLeftThreaded() && !curr->get_IsRightThreaded()) 
        {
            predecessor = predecessor_inorder(curr);
            successor = successor_inorder(curr);

            predecessor->set_right_child(successor);
            if (side_right == 0)
                pred->set_left_child(curr->get_left_child());
            else
                pred->set_right_child(curr->get_left_child());
            predecessor->release_lock();
        }
        else if (!curr->get_IsLeftThreaded() && curr->get_IsRightThreaded())
        {
            successor = successor_inorder(curr);
            predecessor = predecessor_inorder(curr);
           
            successor->set_left_child(predecessor);
            if (side_right == 0)
                pred->set_left_child(curr->get_right_child());
            else
                pred->set_right_child(curr->get_right_child());
            
            successor->release_lock();
        }
        else 
        {
            curr->get_right_child()->get_lock();
            successor = curr->get_right_child();
            predecessor = predecessor_inorder(curr);

            if(successor->get_IsLeftThreaded())
            {
                treeNode* pred_of_successor = new treeNode();
                pred_of_successor = successor;
                successor = successor->get_left_child();
                successor->get_lock();
                
                while (successor->get_IsLeftThreaded()) 
                {
                    pred_of_successor->release_lock();
                    pred_of_successor = successor;
                    successor = successor->get_left_child();
                    successor->get_lock();
                }
                
                predecessor->set_right_child(successor);
                predecessor->release_lock();

                successor->set_left_child(curr->get_left_child());
                successor->set_IsLeftThreaded(curr->get_IsLeftThreaded());
               
                if (side_right == 0)
                    pred->set_left_child(successor);
                else
                    pred->set_right_child(successor);

                if (successor->get_IsRightThreaded()) 
                    pred_of_successor->set_left_child(successor->get_right_child());
                else 
                    pred_of_successor->set_IsLeftThreaded(false);

                successor->set_right_child(curr->get_right_child());
                successor->set_IsRightThreaded(curr->get_IsRightThreaded());
                pred_of_successor->release_lock();
            }
            else
            {
                predecessor->set_right_child(successor);
                predecessor->release_lock();
                if (side_right == 0)
                    pred->set_left_child(successor);
                else
                    pred->set_right_child(successor);
                successor->set_left_child(curr->get_left_child());
                successor->set_IsLeftThreaded(curr->get_IsLeftThreaded());
            }
            successor->release_lock();
        }
    }
    pred->release_lock();
    curr->release_lock();
    return result;
}

void check_indexes_phase1(long expexted_size, long expexted_sum){
    long index_numb = 0, index_sum = 0;

    if(expexted_size != -1){
        index_numb = manager.index_numb();
        if(expexted_size == index_numb)
        {
            cout << "Total list size counted (expected: " 
                << expexted_size << ", found: " 
                << index_numb << ")" << endl;
        }
        else
        {
            cout << "PHASE 1 ERROR IN COUNT" << endl;
            assert(false);
        }
    }
    
    if(expexted_sum != -1)
    {
        index_sum = manager.index_sum();
        cout << "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE keysum counted (expected: " 
                    << expexted_sum << ", found: " 
                    << index_sum << ")" << endl;
        if(expexted_sum == index_sum)
        {
                cout << "Total keysum counted (expected: " 
                    << expexted_sum << ", found: " 
                    << index_sum << ")" << endl;
        }
        else
        {
            cout << "PHASE 1 ERROR IN SUM" << endl;
            assert(false);
        }
    }
}

void check_indexes_phase2(long expexted_size, long expexted_sum){
    long index_numb = 0, index_sum = 0;
  
    if(expexted_size != -1)
    {
        for(long i = 0; i < N/4; i++)
        {
            index_numb = Categories.at(i).index_numb();
            if(expexted_size == index_numb)
            {
                cout << "Categories["<< i << "] queue's total size counted (expected: " 
                    << expexted_size << ", found: " 
                    << index_numb << ")" << endl;
            }
            else
            {
                cout << "PHASE 2 ERROR IN COUNT" << endl;
                assert(false);
            }
        }
    }
   
    if(expexted_sum != -1)
    {
        for(long i = 0; i < N/4; i++)
        {
            index_sum += Categories.at(i).index_sum();
        }
        if(expexted_sum == index_sum)
        {
            cout << "Total keysum check counted (expected: "  
                << expexted_sum << ", found: " 
                << index_sum << ")" << endl;
        }
        else
        {
            cout << "PHASE 2 ERROR IN SUM" << endl;
            assert(false);
        }
    }
}

long binarytree_count_recursive(treeNode *root){
    long count = 1;

    if (root->get_IsLeftThreaded()) {
       count += binarytree_count_recursive(root->get_left_child());
    }
    if (root->get_IsRightThreaded()) {
        count += binarytree_count_recursive(root->get_right_child());
    }
    if(::root == root){
        count = 0;
    }
    return count;
}

void check_indexes_phase3(long expexted_size){
    long index_numb = 0;

    if(expexted_size != -1)
    {  
        index_numb = binarytree_count_recursive(root->get_left_child()); 
        if(expexted_size == index_numb)
        {
            cout << "Tree total size finished (expected: " 
                << expexted_size << ", found: " 
                << index_numb << ")" << endl;
        }
        else
        {
            cout << "PHASE 3 ERROR IN COUNT" << endl;
            assert(false);
        }
    }
}
#endif