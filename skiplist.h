/*
  Skiplist.h - a free library of Skiplist. 

  Version 1.0
  Created by Yudi Ren, Sep 13, 2019.
  renyudicn@outlook.com
*/
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_MAX_LEVEL 10

template<class KeyType, class ValueType>
struct skiplist_node_t{
    KeyType key;			//the key of this node
    ValueType value;    	//the value of this node
	int level;     			//how many levels this node has, from 1 to _maxLevel
	struct skiplist_node_t<KeyType,ValueType>** tail;	//Pointer of a pointer that points to the tail node of a list of nodes that have the same key
														//all nodes in that list share the same memory, 
														//which allows us to rearrange the tail pointer of each member in one process.
														//The last node of that list points to itself
														//If the key is unique, then it points to the node itself as well
	struct skiplist_node_t<KeyType,ValueType>* prev;	//if the key is not unique, and the node is not the head os a list of nodes that have the same key
														//it points to the prev node
														//it is NULL if the node is the head or the key is unique
    struct skiplist_node_t<KeyType,ValueType> *next[];	//an array that holds the pointers to next nodes at each level
};

/*
 * Loop through the nodes from start to end.
 * The caller has to ensure the start and end are not NULL
 */ 
#define list_each_sl_node(start,end,node) \
	for(node = start, end = end->next[0];		\
		node != end; node = node->next[0])

/*
 * Default function for comparing keys. 
 * user defined key compare function should obey the same return rule
 * 
 * @param key1
 *   	the first key
 * @param key1
 *   	the second key
 * 
 * @return
 * 		return 0 if equal
 * 		return <0 if key1 < key2
 * 		return >0 if key1 > key2
 */ 
template <class KeyType>
int defaultCompFunc(KeyType key1,KeyType key2){
    return key1-key2;
}

//Static variable to ensure the random seed is initialised only once
static bool s_isRandSeedSet = false;

//Class for Skiplist
template <class KeyType, class ValueType>
class Skiplist{
    private:
        int _curr_level;			//starts from 0 to _maxLevel-1
		int _maxLevel;				//larger than 1
        int _count;					//how many nodes in this Skiplist
        struct skiplist_node_t<KeyType,ValueType>* _sudoHead;
		int (*_comp)(KeyType,KeyType);		//key compare function
		struct skiplist_node_t<KeyType,ValueType>* _createNode(KeyType key, ValueType value, int level, struct skiplist_node_t<KeyType,ValueType>** tail);
		int _randomLevel();

    public:
		Skiplist();
		Skiplist(int maxLevel,int (*)(KeyType,KeyType));
		~Skiplist();
		bool insert(KeyType key, ValueType value, struct skiplist_node_t<KeyType,ValueType>** node);
		bool del(struct skiplist_node_t<KeyType,ValueType>** node);
		bool search(KeyType key, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end);
		bool search(KeyType key1, KeyType key2, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end);
		int getCurrentLevel();
		int getNodesNum();
		void printList();
};

/*
 * Default constructor which initialises the key compare function to the defaultCompFunc
 * The KeyType has to be int (uint8_t, uint16_t...) or float(float, double...)
 * The max level is set to DEFAULT_MAX_LEVEL
 */ 
template <class KeyType, class ValueType>
inline Skiplist<KeyType, ValueType>::Skiplist(){
    _curr_level = 0;
    _count = 0;
    _maxLevel = DEFAULT_MAX_LEVEL;
    _comp = defaultCompFunc;

	//assign memory to the _sodoHead
    _sudoHead = (struct skiplist_node_t<KeyType,ValueType>*)malloc(sizeof(struct skiplist_node_t<KeyType,ValueType>) + _maxLevel*sizeof(struct skiplist_node_t<KeyType,ValueType>*));

	if(NULL == _sudoHead){
		std::cout<<"create skiplist_node_t fail when creating skiplist"<<std::endl;
	}

	//set all heads to NULL 
    for(int i=0;i<_maxLevel;i++){
        _sudoHead->next[i] = NULL;
    }

	//set random seed if not set
	if(!s_isRandSeedSet){
		srand48(time(0));
		s_isRandSeedSet = true;
	}
}

/*
 * Constructor which requires to specify the key compare function and max level
 * Therefore, the KeyType can be user defined
 * 
 * @param maxLevel
 * 		user specified max level
 * @param comp
 * 		user defined key compare function, has to follow the return rule of the default key compare function
 */ 
template <class KeyType, class ValueType>
inline Skiplist<KeyType, ValueType>::Skiplist(int maxLevel,int (*comp)(KeyType,KeyType)){
    _curr_level = 0;
    _count = 0;
    _maxLevel = maxLevel;
    _comp = comp;
    _sudoHead = (struct skiplist_node_t<KeyType,ValueType>*)malloc(sizeof(struct skiplist_node_t<KeyType,ValueType>) + _maxLevel*sizeof(struct skiplist_node_t<KeyType,ValueType>*));

	if(NULL == _sudoHead){
		std::cout<<"create skiplist_node_t fail when creating skiplist"<<std::endl;
	}

    for(int i=0;i<_maxLevel;i++){
        _sudoHead->next[i] = NULL;
    }

	if(!s_isRandSeedSet){
		srand48(time(0));
		s_isRandSeedSet = true;
	}
}

/*
 * Default destructor
 */
template <class KeyType, class ValueType>
inline Skiplist<KeyType, ValueType>::~Skiplist(){
    free(_sudoHead);
}

/*
 * Create a node with specified key, value, levels and tail
 * 
 * @param key
 * 		the key of this node
 * @param value
 * 		the value of this node
 * @param level
 * 		how many levels this node has
 * @param tail
 * 		the address of a pointer points to the tail of a list of nodes with the same key
 * 		it's NULL if the key is unique when inserting
 * 
 * @return
 * 		return the created node if success.
 */
template <class KeyType, class ValueType>
inline struct skiplist_node_t<KeyType,ValueType>* Skiplist<KeyType, ValueType>::_createNode(KeyType key, ValueType value, int level, struct skiplist_node_t<KeyType,ValueType>** tail){
	struct skiplist_node_t<KeyType,ValueType>* node = (struct skiplist_node_t<KeyType,ValueType>*)malloc(sizeof(struct skiplist_node_t<KeyType,ValueType>)+ level*sizeof(struct skiplist_node_t<KeyType,ValueType>*));
	
	if(NULL == node){
		std::cout<<"create skiplist_node_t fail when create node"<<std::endl;
		return NULL;
	}
	
	node->level = level;
	node->key = key;
	node->value = value;

	//the prev pointer can only be not NULL when the node is the head of a list of nodes which have the same key
	node->prev = NULL;
	
	//all nodes with the same key share the same memory for tail
	if(NULL == tail){
		node->tail = (struct skiplist_node_t<KeyType,ValueType>**)malloc(sizeof(struct skiplist_node_t<KeyType,ValueType>*));
		*(node->tail) = node;
	}else{
		node->tail = tail;
	}

	return node;
}

/*
 * Compute the level for a node.
 * 
 * @return 
 * 		the level
 */
template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::_randomLevel(){
	//the probability of a node has level n is (1/4)^n, i.e. every node has level 0, 1 in 4 has level 1,
	//1 in 16 has level 2, 1 in 64 has level 3, etc.
	unsigned int rand = lrand48() & (UINT32_MAX - 1);
	unsigned int level = ((rand == 0) ? _maxLevel : (__builtin_ctz(rand)-1) / 2);

	//restrict the levels
	if (level > _curr_level)
		level = _curr_level;
	if (level >= _maxLevel)
		level = _maxLevel-1;

	return level;
}

/* 
 * Insert a node into the skiplist
 * 
 * @param key
 * 		the key of the node
 * @param value
 * 		the value of the node
 * @param node
 * 		the address of a pointer points to the ndoe that will be allocated memory in this function, 
 * 		has to be NULL when calling this function
 * 
 * @return 
 * 		return true if success
 */
template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::insert(KeyType key, ValueType value, struct skiplist_node_t<KeyType,ValueType>** node){
	struct skiplist_node_t<KeyType,ValueType>* prevNodes[_maxLevel];
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;

	//the *node should be NULL
	//otherwise it is inserted
	if(*node != NULL){
		std::cout<<"This node is already inserted"<<std::endl;
		return false;
	}

	//Search for the prev nodes at each level from the top level
	for(int i = _curr_level; i >= 0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key) < 0)
            cursor = *(existNode->tail);	//Update the cursor to the tail, 
											//as tail is either pointed to the existNode or the tail of the nodes with the same key
		//If the key of existNode is equal or larger than the given key,
		//the cursor is the prev node at that level
        prevNodes[i] = cursor;
	}

	//Key exists
	if(existNode && _comp(existNode->key, key) == 0){
		//the nodes with the same key have the same number of levels
		*node = _createNode(key,value, existNode->level, existNode->tail);

		//insert the new node before the existNode
		existNode->prev = *node;
	}else{
		//compute the number of levels
		int level = _randomLevel();

		//if the level equals the current level,
		//increase the current level
		if(level == _curr_level){
			_curr_level++;
		}

		//as level starts from 0
		//pass level+1 to create a new node
		*node = _createNode(key,value, level+1,NULL);
	}

	if(NULL == *node){
		std::cout<<"create node fail in insert"<<std::endl;
		return false;
	}

	//Insert the node after the prev nodes at each level,
	//starts from the top level
	for(int i = (*node)->level-1; i >= 0; i--){
		(*node)->next[i] = prevNodes[i]->next[i];
		prevNodes[i]->next[i] = *node;
	}

	_count++;

	return true;
}

/*
 * Remove a given node from the list
 * 
 * @param node
 * 		an address of a pointer points to the node that needs to be removed
 * 
 * @return
 * 		return true if success
 */
template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::del(struct skiplist_node_t<KeyType,ValueType>** node){
    //If the *node is NULL, means it's not inserted
	if(NULL == *node){
		std::cout<<"This node is not inserted"<<std::endl;
		return false;
	}

	if((*node)->prev != NULL){
		//this node is not the head of a list of nodes which have the same key

		if(*(*node)->tail == *node){
			//if this node is the tail of a list of nodes which have the same key
			//change the tail pointer of all nodes in this list
			*(*node)->prev->tail = (*node)->prev;
		}else{
			//otherwise this node is one in the middle
			//rearrange the prev pointer
			(*node)->next[0]->prev = (*node)->prev;
		}

		//update the next pointers of the prev nodes from the top level
		for(int i = (*node)->level-1; i >= 0; i--){
			(*node)->prev->next[i] = (*node)->next[i];	
		}
	}else{
		//this node is the head of a list of nodes which have the same key
		//or the key of this node is unique in the skiplist
		struct skiplist_node_t<KeyType,ValueType>* prevNodes[_maxLevel];
		struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;

		//search for the prev nodes at each level
		for(int i = _curr_level; i >= 0; i--){
			while( (existNode = cursor->next[i]) && _comp(existNode->key, (*node)->key) < 0)
           		cursor = *(existNode->tail);

        	prevNodes[i] = cursor;
		}

		//if the found node is not the node that we want to remove, something went wrong
		if(existNode != *node){
			std::cout<<"list error"<<std::endl;
			return false;
		}

		if((*node)->next[0]){
			//update the prev pointer of the following node to NULL
			//as the next[0] can only be a head of a list of nodes which have the same key
			//or the key of next[0] is unique in the skiplist
			(*node)->next[0]->prev = NULL;	
		}
			
		//update the next pointers of the prev nodes from the top level
		for(int i = _curr_level-1; i >= 0; i--)
    	{
			if(prevNodes[i]->next[i] == *node)
			{
				prevNodes[i]->next[i] = (*node)->next[i];
				
				//if the removed node is the last one at a level, lower down the current level
				if(_sudoHead->next[i]==NULL)
					_curr_level--;
			}
    	}

		//as this node is the only one has the key in the skiplist,
		//free the tail
		free((*node)->tail);
	}

	_count--;
	free(*node);
	//point the *node to NULL, so we can reinsert
	*node = NULL;

	return true;
}

/*
 * Search for nodes with a given key
 * There might be multiple nodes with the same key
 * If nodes with the key exist, the *start and *end are not NULL. 
 * User don't need to check whether they are NULL if the marco: list_each_sl_node is called to loop through the output
 * 
 * @param key
 * 		a given key
 * @param start
 * 		An address of a pointer points to the start of a list of nodes with the given key, served as an output
 * @param end 
 * 		An address of a pointer points to the tail of a list of nodes with the given key, served as an output
 * 
 * @return 
 * 		if nodes with the given key exist, return true and the *start and *end are not NULL,
 * 		otherwise return false, and the *start == *end == NULL
 */
template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::search(KeyType key, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end){
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;
	//set the *start and *end to NULL
	*start = *end = NULL;

	//Search for nodes have the given key from the top
	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key) < 0)
            cursor = *(existNode->tail);

		//stop if found
        if(existNode && _comp(existNode->key, key) == 0){
			*start = existNode;
			*end = *existNode->tail;
			return true;
		}
	}

	return false;
}

/*
 * Search for nodes with keys within a given range (key1 <= key2)
 * It can be a rough range, i.e. the key1 and key2 don't need to be in the skiplist
 * There might be multiple nodes with the same key
 * If there are nodes within the given range, the *start and *end are not NULL
 * User don't need to check whether they are NULL if the marco: list_each_sl_node is called to loop through the output
 * 
 * @param key1
 * 		a given key1 served as the lower bound
 * @param key2
 * 		a given key1 served as the upper bound
 * @param start
 * 		An address of a pointer points to the start of a list of nodes with the given key, served as an output
 * @param end 
 * 		An address of a pointer points to the tail of a list of nodes with the given key, served as an output
 * 
 * @return 
 * 		if nodes with the given key exist, return true and the *start and *end are not NULL,
 * 		otherwise return false, and the *start == *end == NULL
 */
template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::search(KeyType key1, KeyType key2,struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end){
	//downgrade to normal search
	if(key1 == key2)
		return search(key1, start, end);
	
	if(key1 > key2){
		std::cout<<"key1: "<<key1<<" is larger than key2: "<<key2<<std::endl;
		return false;
	}
	
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;
	struct skiplist_node_t<KeyType,ValueType>* prevNodes[_maxLevel];
	//set the *start and *end to NULL
	*start = *end = NULL;

	//search for the node that has key equals to the key1 or just larger than key1
	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key1) < 0)
            cursor = *(existNode->tail);

		//if key1 exists, stop the search
        if(existNode && _comp(existNode->key, key1) == 0){
			break;
		}
	}

	//there are two situations:
	//1. existNode has the key1
	//2. the key1 doesn't exist, so the existNode is the one with a key that is just larger than the given key1
	*start = existNode;

	//reset the two pointers
	existNode=NULL;
	cursor=_sudoHead;

	//search for the node with a key that is just larger than the key2
	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key2) < 0)
            cursor = *(existNode->tail);
		
		//if key2 exists, stop search and set *end to the existNode
        if(existNode && _comp(existNode->key, key2) == 0){
			*end = existNode;
			break;
		}
		
		//update the prev nodes at each level
		prevNodes[i] = cursor;
	}

	//if key2 doesn't exist, set *end to the prev node at level 0, whose key2 is just smaller than the key2
	if(NULL == *end){
		*end = prevNodes[0];
	}

	if(*start != NULL && *end != NULL){
		return true;
	}else{
		return false;
	}
}

/*
 * Get the current number of levels
 * 
 * @return 
 * 		the current number of levels
 */
template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::getCurrentLevel(){
    return this->_curr_level;
}

/*
 * Get the number of nodes
 * 
 * @return 
 * 		the number of nodes
 */
template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::getNodesNum(){
    return this->_count;
}

/*
 * Print the nodes at each level, start from the current top level to level 0
 */
template <class KeyType, class ValueType>
inline void Skiplist<KeyType, ValueType>::printList(){
	struct skiplist_node_t<KeyType,ValueType>* node;
    
	std::cout<<"Skiplist has "<<_count<<" nodes."<<std::endl;
    for(int i=_curr_level-1; i >= 0; --i)
    {
        node = _sudoHead->next[i];
		int count = 0;
        std::cout<<"level "<<i<<":"<<std::endl;
		while(node)
        {	
			count++;
            std::cout<<"("<<node->key<<", "<<node->value<<")";
            node = node->next[i];
			if(NULL == node){
				std::cout<<std::endl;
			}else{
				std::cout<<"->";
			}
        }
		//print the number of nodes at this level
        std::cout<<"count: "<<count<<std::endl;
    }
	std::cout<<std::endl;
}

#endif