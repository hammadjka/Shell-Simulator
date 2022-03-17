#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct LogNode {
    struct LogNode* next;

    //max size of command name can be an environment variable with an extremely large name and value i.e 256 bytes each!
    char name[512];
    struct tm* time;
    int value;
    char cur_time[30];

};
struct LogList {
    struct LogNode* head;
    struct LogNode* tail;
    int size;
};

struct LogNode* createLogNode(const char* name, const int value) {
    time_t current_time = time(NULL);

    struct LogNode* temp = (struct LogNode*)malloc(sizeof(struct LogNode));
    strcpy(temp->name, name);

    temp->time = localtime(&current_time);
    strcpy(temp->cur_time, asctime(temp->time));
    temp->value = value;
    temp->next = NULL;

    return temp;
}

void insert(struct LogList* l, struct LogNode* temp) {

    if (l->head == NULL) {
        l->head = temp;
        l->tail = l->head;
        l->size = 1;
        return;
    }
    l->tail->next = temp;
    l->tail = temp;
    l->size++;
}

void print(struct LogList* l) {
    struct LogNode* temp = l->head;
    while (temp) {
        printf("%s", temp->cur_time);
        printf("%s %d \n", temp->name, temp->value);
        temp = temp->next;
    }
}

void freeLogList(struct LogList *l){
    struct LogNode *temp = l->head;
    struct LogNode *temp2 = l->head;

    while(temp){
        temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    free(l);
}