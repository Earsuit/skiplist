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
        if(skiplist.del(&nodes[i])){
            nodes[i] = NULL;
        }
    }

    cout<<"Now you can try to insert as you wish:"<<endl<<endl;
    cout<<"insert: i [idx] [key] [val]"<<endl;
    cout<<"delete: d [idx]"<<endl;
    cout<<"search for nodes with a given key: s [key]"<<endl;
    cout<<"search for nodes within a given range: r [key1] [key2]"<<endl<<endl;

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
        }else if('d' == c){
            cin>>idx;
            if(!skiplist.del(&nodes[idx])){
                cout<<"remove fail"<<endl;
            }
        }else if('s' == c){
            cin>>key;
            struct skiplist_node_t<int,int> *start, *end, *node;
            if(skiplist.search(key,&start, &end)){
                //if search success
                list_each_sl_node(start,end,node){
                    //do something
                    cout<<"val: "<<node->value<<endl;
                }
            }else{
                cout<<"not find nodes with key"<<key<<endl;
            }
        }else if('r' == c){
            cin>>key1>>key2;
            struct skiplist_node_t<int,int> *start, *end, *node;
            if(skiplist.search(key1,key2, &start, &end)){
                //if search success
                list_each_sl_node(start,end,node){
                    //do something
                    cout<<"val: "<<node->value<<endl;
                }
            }else{
                cout<<"not find nodes within the range: "<<key1<<" - "<<key2<<endl;
            }
        }
        
        skiplist.printList();
    }
}