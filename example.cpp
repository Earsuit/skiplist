#include <iostream>
#include <stdlib.h>
#include "skiplist.h"

using namespace std;

#define MAX_NODES 1000

int main(int argc, char** argv){
    Skiplist<int, int> skiplist;

    struct skiplist_node_t<int,int>* nodes[MAX_NODES] = {NULL};

    for(int i=0; i<MAX_NODES; i++){
        skiplist.insert(i,i,&nodes[i]);
       
    }
    skiplist.printList();

    cout<<"Above is the skiplist probability test."<<endl;
    cout<<"For 1000 nodes, all should be in level 0,"<<endl;
    cout<<"~250 nodes in level 1, ~62 nodes in level 2,"<<endl;
    cout<<"~16 nodes in level 3, ~4 nodes in level 4."<<endl;
    cout<<"Although sometimes there might be several nodes above 4"<<endl;

    for(int i=0; i<MAX_NODES; i++){
        if(skiplist.remove(&nodes[i])){
            nodes[i] = NULL;
        }
    }

    cout<<"Now you can try to insert as you wish:"<<endl;

    int idx,val,key,key1,key2;
    idx = val = key = 0;
    while(true){
        char c;
        cin>>c;
        if('i' == c){
            cin>>idx>>key>>val;
            if(!skiplist.insert(key,val,&nodes[idx])){
                cout<<"insert fail"<<endl;
            }
        }else if('r' == c){
            cin>>idx;
            if(!skiplist.remove(&nodes[idx])){
                cout<<"remove fail"<<endl;
            }
        }else if('s' == c){
            cin>>key;
            struct skiplist_node_t<int,int> *start, *end, *node;
            if(skiplist.search(key,&start, &end)){
                list_each_sl_node(start,end,node){
                    cout<<"val: "<<node->value<<endl;
                }
            }
        }else if('d' == c){
            cin>>key1>>key2;
            struct skiplist_node_t<int,int> *start, *end, *node;
            if(skiplist.search(key1,key2, &start, &end)){
                list_each_sl_node(start,end,node){
                    cout<<"val: "<<node->value<<endl;
                }
            }
        }
        
        skiplist.printList();
    }
}