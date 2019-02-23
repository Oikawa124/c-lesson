#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <assert.h>

#include "asm.h"

#define NOT_FOUND -1

typedef struct _Node {
    char *name;
    int value;
    struct _Node *left;
    struct _Node *right;
} Node;


static Node *mnemonic_root = NULL;
static Node *label_root = NULL;

static int mnemonic_id = 1;
static int label_id = 10000;

static char *my_strdup(struct substring *substr){
    char *dest;

    dest = malloc(sizeof(char) * (substr->len + 1)); // '\0'の分も領域確保？
    dest[substr->len] = '\0';

    strncpy(dest, substr->str, substr->len);

    return dest;
}


// Nodeの挿入
static Node *insert_node(Node *node, struct substring *substr, int id){

    if (node == NULL) {
        node = malloc(sizeof(Node));

        node->name = my_strdup(substr);
        node->value = id;

        node->left = NULL;
        node->right = NULL;

        return node;
    }

    if (strncmp(node->name, substr->str, substr->len) < 0) {
        node->left = insert_node(node->left, substr, id);
    } else {
        node->right = insert_node(node->right, substr, id);
    }

    return node;
}

// Nodeの探索
static int search_node(Node *node, struct substring *substr){
    int cond;

    while (node != NULL) {
        if ((cond = strncmp(node->name, substr->str, substr->len)) == 0) {
            return node->value;
        } else if (cond < 0){
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return NOT_FOUND;
}

// Nodeの削除
static void delete_tree(Node *node){
    if (node != NULL) {
        delete_tree(node->left);
        delete_tree(node->right);
        free(node);
    }
}


int to_mnemonic_symbol(struct substring *substr){
    int value;

    if ((value = search_node(mnemonic_root, substr)) != NOT_FOUND) {
        return value;
    }

    mnemonic_root = insert_node(mnemonic_root, substr, mnemonic_id);
    mnemonic_id++;

    return mnemonic_id - 1;
}


int to_label_symbol(struct substring *substr){
    int value;

    if ((value = search_node(label_root, substr)) != NOT_FOUND) {
        return value;
    }

    label_root = insert_node(label_root, substr, label_id);
    label_id++;

    return label_id - 1;
}


// 初期化
void initialize_mnemonic_root(){
    delete_tree(mnemonic_root);
    mnemonic_root = NULL;
    mnemonic_id = 1;
}

void initialize_label_root(){
    delete_tree(label_root);
    label_root = NULL;
    label_id = 10000;
}



/***************** Unit tests*****************/

static int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }


/*insert node*/
static void test_insert_node_when_call_once(){

    // SetUP
    struct substring input = {.str="mov", .len=3};

    char *expect_name = "mov";
    int expect_value = 1;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input, mnemonic_id);

    // Verify
    assert(expect_value == mnemonic_root->value);
    assert(streq(expect_name, mnemonic_root->name));

    // TearDown
    initialize_mnemonic_root();
}

static void test_insert_node_when_call_three_times(){

    // SetUP
    struct substring input1 = {.str="mov", .len=3};
    struct substring input2 = {.str="abcdef", .len=6};
    struct substring input3 = {.str="x", .len=1};

    char *expect_name1 = "mov";
    char *expect_name2 = "abcdef";
    char *expect_name3 = "x";

    int expect_value1 = 1;
    int expect_value2 = 2;
    int expect_value3 = 3;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input2, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input3, mnemonic_id);
    mnemonic_id++;

    // Verify
    assert(expect_value1 == mnemonic_root->value);
    assert(streq(expect_name1, mnemonic_root->name));

    assert(expect_value2 == mnemonic_root->right->value);
    assert(streq(expect_name2, mnemonic_root->right->name));

    assert(expect_value3 == mnemonic_root->left->value);
    assert(streq(expect_name3, mnemonic_root->left->name));

    // TearDown
    initialize_mnemonic_root();
}

static void test_insert_node_when_multiple_words(){

    // SetUP
    char *str = "abc def";
    struct substring input1, input2;

    input1.str = str;
    input1.len = 3;

    input2.str = str+4;
    input2.len = 3;

    char *expect_name1 = "abc";
    int expect_value1 = 1;

    char *expect_name2 = "def";
    int expect_value2 = 2;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input2, mnemonic_id);

    // Verify
    assert(expect_value1 == mnemonic_root->value);
    assert(streq(expect_name1, mnemonic_root->name));

    assert(expect_value2 == mnemonic_root->left->value);
    assert(streq(expect_name2, mnemonic_root->left->name));

    // TearDown
    initialize_mnemonic_root();
}


/*search node */
static void test_search_node_when_call_once(){

    // SetUP
    struct substring input = {.str="mov", .len=3};

    int expect_value = 1;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input, mnemonic_id);
    int actual_value = search_node(mnemonic_root, &input);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_mnemonic_root();
}

static void test_search_node_when_call_three_times(){

    // SetUP
    struct substring input1 = {.str="mov", .len=3};
    struct substring input2 = {.str="abcdef", .len=6};
    struct substring input3 = {.str="x", .len=1};

    int expect_value1 = 1;
    int expect_value2 = 2;
    int expect_value3 = 3;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input2, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input3, mnemonic_id);
    mnemonic_id++;

    int actual_value1 = search_node(mnemonic_root, &input1);
    int actual_value2 = search_node(mnemonic_root, &input2);
    int actual_value3 = search_node(mnemonic_root, &input3);


    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);
    assert(expect_value3 == actual_value3);

    // TearDown
    initialize_mnemonic_root();
}

static void test_search_node_when_multiple_words(){

    // SetUP
    char *str = "abc def";
    struct substring input1, input2;

    input1.str = str;
    input1.len = 3;

    input2.str = str+4;
    input2.len = 3;


    int expect_value1 = 1;
    int expect_value2 = 2;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, &input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, &input2, mnemonic_id);

    int actual_value1 = search_node(mnemonic_root, &input1);
    int actual_value2 = search_node(mnemonic_root, &input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}


/* mnemonic to symbol*/
static void test_to_mnemonic_symbol_when_call_once(){

    // SetUP
    struct substring input = {.str="mov", .len=3};

    int expect_value = 1;

    // Exercise
    int actual_value = to_mnemonic_symbol(&input);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_same_symbol(){

    // SetUP
    struct substring input1 = {.str="mov", .len=3};
    struct substring input2 = {.str="mov", .len=3};

    int expect_value1 = 1;
    int expect_value2 = 1;


    // Exercise
    int actual_value1 = to_mnemonic_symbol(&input1);
    int actual_value2 = to_mnemonic_symbol(&input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_different_symbol(){

    // SetUP
    struct substring input1 = {.str="mov", .len=3};
    struct substring input2 = {.str="str", .len=3};

    int expect_value1 = 1;
    int expect_value2 = 2;

    // Exercise
    int actual_value1 = to_mnemonic_symbol(&input1);
    int actual_value2 = to_mnemonic_symbol(&input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_multiple_words(){

    // SetUP
    char *str = "abc def";
    struct substring input1, input2;

    input1.str = str;
    input1.len = 3;

    input2.str = str+4;
    input2.len = 3;


    int expect_value1 = 1;
    int expect_value2 = 2;

    // Exercise
    int actual_value1 = to_mnemonic_symbol(&input1);
    int actual_value2 = to_mnemonic_symbol(&input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}


/* label to symbol*/
static void test_to_label_symbol_when_call_once(){

    // SetUP
    struct substring input = {.str="HelloWorld", .len=10};

    int expect_value = 10000;

    // Exercise
    int actual_value = to_label_symbol(&input);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_label_root();
}


static void test_to_label_symbol_when_same_symbol(){

    // SetUP
    struct substring input1 = {.str="label", .len=5};
    struct substring input2 = {.str="label", .len=5};

    int expect_value1 = 10000;
    int expect_value2 = 10000;


    // Exercise
    int actual_value1 = to_label_symbol(&input1);
    int actual_value2 = to_label_symbol(&input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_label_root();
}

static void test_to_label_symbol_when_different_symbol(){

    // SetUP
    struct substring input1 = {.str="mov", .len=3};
    struct substring input2 = {.str="str", .len=3};

    int expect_value1 = 10000;
    int expect_value2 = 10001;

    // Exercise
    int actual_value1 = to_label_symbol(&input1);
    int actual_value2 = to_label_symbol(&input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_label_root();
}

static void test_my_strdup(){

    // SetUP
    struct substring input = {.str="mov", .len=3};

    // Exercise
    char *actual;

    actual = my_strdup(&input);

    // Verify
    assert(streq(input.str, actual));

    // TearDown
    initialize_mnemonic_root();
}


static void unit_tests() {

    // insert node
    test_insert_node_when_call_once();
    test_insert_node_when_call_three_times();
    test_insert_node_when_multiple_words();

    // search node
    test_search_node_when_call_once();
    test_search_node_when_call_three_times();
    test_search_node_when_multiple_words();

    // to mnemonic symbol
    test_to_mnemonic_symbol_when_call_once();
    test_to_mnemonic_symbol_when_same_symbol();
    test_to_mnemonic_symbol_when_different_symbol();
    test_to_mnemonic_symbol_when_multiple_words();

    // to label symbol
    test_to_label_symbol_when_call_once();
    test_to_label_symbol_when_same_symbol();
    test_to_label_symbol_when_different_symbol();

    // my strdup
    test_my_strdup();
}


//
//int main() {
//
//    unit_tests();
//
//    return 0;
//}
