#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_MAX_LEVEL 10

template<class KeyType, class ValueType>
struct skiplist_node_t{
    KeyType key;
    ValueType value;    
	int level;     			// from 1 to _maxLevel
	struct skiplist_node_t<KeyType,ValueType>** tail;
	struct skiplist_node_t<KeyType,ValueType>* prev;
    struct skiplist_node_t<KeyType,ValueType> *next[];
};

#define list_each_sl_node(start,end,node) \
	for(node = start, end = end->next[0];		\
		node != end; node = node->next[0])

template <class KeyType>
int defaultCompFunc(KeyType key1,KeyType key2){
    return key1-key2;
}

static bool s_isRandSeedSet = false;

template <class KeyType, class ValueType>
class Skiplist{
    private:
        int _curr_level;			//starts from 0 to _maxLevel-1
		int _maxLevel;				//larger than 1
        int _count;					//how many nodes in this Skiplist
        struct skiplist_node_t<KeyType,ValueType>* _sudoHead;
		int (*_comp)(KeyType,KeyType);
		struct skiplist_node_t<KeyType,ValueType>* _createNode(KeyType key, ValueType value, int level, struct skiplist_node_t<KeyType,ValueType>** tail);
		int _randomLevel();

    public:
		Skiplist();
		Skiplist(int maxLevel,int (*)(KeyType,KeyType));
		~Skiplist();
		bool insert(KeyType key, ValueType value, struct skiplist_node_t<KeyType,ValueType>** node);
		bool remove(struct skiplist_node_t<KeyType,ValueType>** node);
		bool search(KeyType key, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end);
		bool search(KeyType key1, KeyType key2, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end);
		int getCurrentLevel();
		int getNodesNum();
		void printList();
};

template <class KeyType, class ValueType>
inline Skiplist<KeyType, ValueType>::Skiplist(){
    _curr_level = 0;
    _count = 0;
    _maxLevel = DEFAULT_MAX_LEVEL;
    _comp = defaultCompFunc;
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

template <class KeyType, class ValueType>
inline Skiplist<KeyType, ValueType>::~Skiplist(){
    free(_sudoHead);
}

template <class KeyType, class ValueType>
inline struct skiplist_node_t<KeyType,ValueType>* Skiplist<KeyType, ValueType>::_createNode(KeyType key, ValueType value, int level, struct skiplist_node_t<KeyType,ValueType>** tail){
	struct skiplist_node_t<KeyType,ValueType>* node = (struct skiplist_node_t<KeyType,ValueType>*)malloc(sizeof(struct skiplist_node_t<KeyType,ValueType>)+ level*sizeof(struct skiplist_node_t<KeyType,ValueType>*));
	
	if(NULL == node){
		std::cout<<"create skiplist_node_t fail when create node"<<std::endl;
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
 */
template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::_randomLevel(){
	//the probability of a node has level n is (1/4)^n, i.e. every node has at level 0, 1 in 4 has level 1,
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

template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::insert(KeyType key, ValueType value, struct skiplist_node_t<KeyType,ValueType>** node){
	struct skiplist_node_t<KeyType,ValueType>* prevNodes[_maxLevel];
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;

	if(*node != NULL){
		std::cout<<"This node is already inserted"<<std::endl;
		return false;
	}

	for(int i = _curr_level; i >= 0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key) < 0)
            cursor = *(existNode->tail);

        prevNodes[i] = cursor;
	}

	if(existNode && _comp(existNode->key, key) == 0){
		std::cout<<"key exists"<<std::endl;
		*node = _createNode(key,value, existNode->level, existNode->tail);

		existNode->prev = *node;
	}else{
		int level = _randomLevel();

		if(level == _curr_level){
			_curr_level++;
		}

		*node = _createNode(key,value, level+1,NULL);
	}

	if(NULL == *node){
		std::cout<<"create node fail in insert"<<std::endl;
		return false;
	}

	for(int i = (*node)->level-1; i >= 0; i--){
		(*node)->next[i] = prevNodes[i]->next[i];
		prevNodes[i]->next[i] = *node;
	}

	_count++;

	return true;
}

template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::remove(struct skiplist_node_t<KeyType,ValueType>** node){
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
			(*node)->next[0]->prev = (*node)->prev;
		}

		for(int i = (*node)->level-1; i >= 0; i--){
			(*node)->prev->next[i] = (*node)->next[i];	
		}
	}else{

		//this node is the head of a list of nodes which have the same key
		//or the key of this node is unique in the skiplist
		struct skiplist_node_t<KeyType,ValueType>* prevNodes[_maxLevel];
		struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;

		for(int i = _curr_level; i >= 0; i--){
			while( (existNode = cursor->next[i]) && _comp(existNode->key, (*node)->key) < 0)
           		cursor = *(existNode->tail);

        	prevNodes[i] = cursor;
		}

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
			
		for(int i = _curr_level-1; i >= 0; i--)
    	{
			if(prevNodes[i]->next[i] == *node)
			{
				prevNodes[i]->next[i] = (*node)->next[i];
				
				if(_sudoHead->next[i]==NULL)
					_curr_level--;
			}
    	}

		free((*node)->tail);
	}

	_count--;
	free(*node);
	*node = NULL;

	return true;
}

template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::search(KeyType key, struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end){
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;
	*start = *end = NULL;

	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key) < 0)
            cursor = *(existNode->tail);

        if(existNode && _comp(existNode->key, key) == 0){
			*start = existNode;
			*end = *existNode->tail;
			return true;
		}
	}

	return false;
}

template <class KeyType, class ValueType>
inline bool Skiplist<KeyType, ValueType>::search(KeyType key1, KeyType key2,struct skiplist_node_t<KeyType,ValueType>** start, struct skiplist_node_t<KeyType,ValueType>** end){
	if(key1 == key2)
		return search(key1, start, end);
	
	struct skiplist_node_t<KeyType,ValueType> *existNode=NULL, *cursor=_sudoHead;
	*start = *end = NULL;

	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key1) < 0)
            cursor = *(existNode->tail);

        if(existNode && _comp(existNode->key, key1) == 0){
			*start = existNode;
		}
	}

	existNode=NULL;
	cursor=_sudoHead;
	for(int i=_curr_level-1; i>=0; i--){
		while( (existNode = cursor->next[i]) && _comp(existNode->key, key2) < 0)
            cursor = *(existNode->tail);

        if(existNode && _comp(existNode->key, key2) == 0){
			*end = existNode;
		}
	}

	if(*start != NULL && *end != NULL){
		return true;
	}else{
		return false;
	}
}

template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::getCurrentLevel(){
    return this->_curr_level;
}

template <class KeyType, class ValueType>
inline int Skiplist<KeyType, ValueType>::getNodesNum(){
    return this->_count;
}

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
        std::cout<<"count: "<<count<<std::endl;
    }
	std::cout<<std::endl;
}

#endif