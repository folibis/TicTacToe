#ifndef LIST_H
#define LIST_H


typedef struct DictNode
{
    char *key;
    char *data;
    struct DictNode *next;
} DictNode;

extern DictNode* create_dict(char *data, char *pair_delimiter, char *value_delimiter);
extern char* get_dict_value(DictNode *dict, char *key);
extern void remove_dict(DictNode *dict);

#endif // LIST_H
