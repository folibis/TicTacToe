#include <string.h>
#include <stdlib.h>
#include "List.h"


DictNode* create_dict(char *data, char *pair_delimiter, char *value_delimiter)
{
    DictNode *ptr = NULL;
    DictNode *curr = NULL;
    DictNode *prev = NULL;

    char *token = strtok(data, pair_delimiter);
    while(token != NULL)
    {
        char *pos = strstr(token, ":");
        if(pos)
        {
            // trim key trailing spaces
            char *temp = pos;
            *pos = '\0';
            while(temp != data && *temp == ' ')
            {
                *temp = '\0';
                temp --;
            }
            // trim key beginning spaces
            while(*token == ' ')
                token ++;

            pos++;
            // trim value beginning spaces
            while(*pos == ' ')
                pos ++;
        }

        curr = malloc(sizeof(DictNode));
        curr->next = NULL;
        if(ptr == NULL)
        {
            ptr = curr;
        }
        if(prev != NULL)
        {
            prev->next = curr;
        }
        curr->key = token;
        curr->data = pos;
        prev = curr;

        token = strtok(NULL, ",");
    }

    return ptr;
}

char* get_dict_value(DictNode *dict, char *key)
{
    DictNode *curr = dict;
    while(curr != NULL)
    {
        if(strcmp(key, curr->key) == 0)
        {
            return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

void remove_dict(DictNode *dict)
{
    DictNode *curr = dict;
    while(curr != NULL)
    {
        DictNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
}
