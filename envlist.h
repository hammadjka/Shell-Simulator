struct EnvNode
{
    char name[256];
    char value[256];
    struct EnvNode* next;
};

struct EnvList {
    struct EnvNode* head;
    struct EnvNode* tail;
};

struct EnvNode* createEnvNode(char *name, char* val)
{
    struct EnvNode *temp = (struct EnvNode*)malloc(sizeof(struct EnvNode));
     strcpy(temp->name, name);
     strcpy(temp->value, val);
     temp->next = NULL;
    return temp;
}

void insertEnvNode(struct EnvList* l, struct EnvNode* temp) {

    if (l->head == NULL) {
        l->head = temp;
        l->tail = l->head;
        return;
    }
    l->tail->next = temp;
    l->tail = temp;
}

int search(struct EnvList* l, char* var){
    struct EnvNode *temp = l->head;

    while(temp){
        if(strcmp(temp->name, var) == 0){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}
char* find(struct EnvList* l, char* var){
    struct EnvNode *temp = l->head;

    while(temp){
        if(strcmp(temp->name, var) == 0){
            return temp->value;
        }
        temp=temp->next;
    }
    return var;
}
struct EnvNode* exists(struct EnvList* l, char* var){
    struct EnvNode *temp = l->head;

    while(temp){
        if(strcmp(temp->name, var) == 0){
            return temp;
        }
        temp=temp->next;
    }
    return NULL;
}
void setArg(struct EnvNode *node, char *val){
    if(node){
        strcpy(node->value, val);
    }
}
void printEnvList(struct EnvList* l) {
    struct EnvNode* temp = l->head;
    while (temp) {
        printf("%s %s\n", temp->name, temp->value);
        temp = temp->next;
    }
}
void freeEnvList(struct EnvList *l){
    struct EnvNode *temp = l->head;
    struct EnvNode *temp2 = l->head;

    while(temp){
        temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    free(l);
}