#include <assert.h>
#include <malloc.h>
#include "clesson.h"

#define TABLE_SIZE 1024
// テスト用
//#define TABLE_SIZE 4

struct Node {
    char *key;
    struct Element value;
    struct Node *next;
};

struct Node *array[TABLE_SIZE];


static int hash(char *str){
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    return (int)(val%TABLE_SIZE);
}

static struct Node *create_node(char *key, struct Element *elem){
    struct Node *new_node;

    new_node = malloc(sizeof(struct Node));
    new_node->key= key;
    new_node->value = *elem;
    new_node->next = NULL;

    return new_node;
}

static void update_or_insert_list(struct Node *head, char *key, struct Element *elem){
    static struct Node *pos;
    struct Node *new_node;
    struct Node *prev = head;


    for (pos = head; pos !=NULL; pos = pos->next){

        if (streq(pos->key, key)) {
            pos->value = *elem;
            return;
        }

        if (pos != NULL){prev =pos;}
    }

    // ここに行った時があやしい 同じhash値で違う名前だと、pos->next = new_nodeでエラーがでる。
    new_node = create_node(key, elem);
    new_node->next = prev->next;
    prev->next = new_node;
}



void dict_put(char *key, struct Element *elem){
    int idx = hash(key);
    struct Node *head = array[idx];

    if (head == NULL) {
        head = malloc(sizeof(struct Node));
        head->value  = *elem;
        head->next = NULL;
        head->key = key;

        array[idx] = head;
        return;
    }
    update_or_insert_list(head, key, elem);
}


int dict_get(char *key, struct Element *out_elem){
    struct Node *pos;
    for (pos = array[hash(key)]; pos != NULL; pos = pos->next ) {
        if (streq(pos->key, key)){
            *out_elem = pos->value;
            return 1;
        }
    }
    return 0;
}


void dict_print_all(){
    int i;
    struct Node *pos;
    static int no_dict_flag = 1;

    for (i=0; i < TABLE_SIZE; i++){
        for (pos = array[i]; pos != NULL; pos = pos->next){
            printf("key: %s etype: %d value: %d\n", pos->key
                    , pos->value.etype, pos->value.u.number);
            no_dict_flag = 0;
        }
    }

    if (no_dict_flag) {
        printf("NO ELEMENT");
    }
}


void dict_clear(){
    int i = 0;
    struct Node *pos;
    struct Node *temp;

    while (i < TABLE_SIZE) {
        pos = array[i];

        while (pos != NULL) {
            temp = pos->next;
            free(pos);
            pos = temp;
        }

        array[i] = NULL;
        i++;
    }

}




static void test_dict_one_times(){
    char *input_key = "ten";
    struct Element expect_elem = {ELEMENT_NUMBER, {10}};

    dict_put(input_key, &expect_elem);

    struct Element actual = {NO_ELEMENT, {0}};
    dict_get(input_key, &actual);

    assert(expect_elem.u.number == actual.u.number);

    dict_clear();
}

static void test_dict_two_times(){
    char *input_key1 = "one";
    struct Element input_elem1 = {ELEMENT_NUMBER, {1}};
    char *input_key2 = "two";
    struct Element input_elem2 = {ELEMENT_NUMBER, {2}};

    dict_put(input_key1, &input_elem1);
    dict_put(input_key2, &input_elem2);

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    dict_get(input_key1, &actual1);
    dict_get(input_key2, &actual2);

    assert(input_elem1.u.number == actual1.u.number);
    assert(input_elem2.u.number == actual2.u.number);

    dict_clear();
}

static void test_dict_same_key(){
    char *input_key = "one";
    struct Element input_elem = {ELEMENT_NUMBER, {1}};

    dict_put(input_key, &input_elem);
    dict_put(input_key, &input_elem);


    struct Element actual = {NO_ELEMENT, {0}};
    dict_get(input_key, &actual);

    assert(input_elem.u.number == actual.u.number);
    assert(array[hash(input_key)]->next == NULL);

    dict_clear();
}

static void test_dict_get(){
    struct Element expect_elem1 = {ELEMENT_NUMBER, {5}};
    int expect_dict_get_result = 1;

    char *input_key = "five";

    dict_put(input_key, &expect_elem1);

    struct Element actual = {NO_ELEMENT, {0}};

    int dict_get_result = dict_get(input_key, &actual);

    assert(expect_dict_get_result == dict_get_result);
    assert(expect_elem1.u.number == actual.u.number);
    assert(expect_elem1.etype == actual.etype);

    dict_clear();
}

static void test_dict_not_exist_key(){
    struct Element expect_elem1 = {ELEMENT_NUMBER, {5}};
    int expect_dict_get_result = 0;

    char *input_key = "five";
    char *not_exist_key = "not_exist_key";

    dict_put(input_key, &expect_elem1);

    struct Element actual = {NO_ELEMENT, {0}};

    int dict_get_result = dict_get(not_exist_key, &actual);

    assert(expect_dict_get_result == dict_get_result);

    dict_clear();
}

static void test_dict_same_hash_no_same_keyname(){
    // key1とkey2はTABLE_SIZEが4のとき同じハッシュ値になる。
    char *input_key1 = "one";
    struct Element input_elem1 = {ELEMENT_NUMBER, {1}};
    char *input_key2 = "two";
    struct Element input_elem2 = {ELEMENT_NUMBER, {2}};

    dict_put(input_key1, &input_elem1);
    dict_put(input_key2, &input_elem2);

    struct Element actual1 = {NO_ELEMENT, {0}};
    struct Element actual2 = {NO_ELEMENT, {0}};

    dict_get(input_key1, &actual1);
    dict_get(input_key2, &actual2);

    assert(input_elem1.u.number == actual1.u.number);
    assert(input_elem2.u.number == actual2.u.number);

}


static void unit_test(){
    test_dict_one_times();
    test_dict_two_times();
    test_dict_same_key();
    test_dict_get();
    test_dict_not_exist_key();
    test_dict_same_hash_no_same_keyname();
}

#if 0
int main(){
    unit_test();
//    dict_print_all();

    return 1;
}
#endif