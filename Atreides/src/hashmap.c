#include "../include/hashmap.h"

hashmap *HASMAP_createTable(int size)
{
    hashmap *t = (hashmap *)malloc(sizeof(hashmap));
    t->size = size;
    t->list = (node **)malloc(sizeof(node *) * size);
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;
    return t;
}
int HASMAP_hashCode(hashmap *t, int key)
{
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}
void HASMAP_insert(hashmap *t, int key, user val)
{
    int pos = HASMAP_hashCode(t, key);
    node *list = t->list[pos];
    //BidirectionalList aux = t->list[pos]->val;
    node *newNode = (node *)malloc(sizeof(node));
    node *temp = list;
    while (temp)
    {
        if (temp->key == key)
        {
            BIDIRECTIONALLIST_addAfter(&temp->val,val);
            //temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    //crear hashmap
    BidirectionalList bidi = BIDIRECTIONALLIST_create();
    BIDIRECTIONALLIST_addAfter(&bidi,val);
    newNode->val = bidi;
    newNode->next = list;
    t->list[pos] = newNode;
}
BidirectionalList HASMAP_lookup(hashmap *t, int key)
{
    int pos = HASMAP_hashCode(t, key);
    node *list = t->list[pos];
    node *temp = list;
    while (temp)
    {
        if (temp->key == key)
        {
            return temp->val;
        }
        temp = temp->next;
    }
    
    return BIDIRECTIONALLIST_create();
}