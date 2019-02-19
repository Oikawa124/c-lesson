#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <assert.h>

#define NOT_FOUND -1

typedef struct _Node {
    char *name;
    int value;
    struct _Node *left;
    struct _Node *right;
} Node;

Node *mnemonic_root = NULL;
Node *label_loot = NULL;

int mnemonic_id = 1;
int label_id = 10000;


static char *my_strdup(char *name){
    char *dest;

    dest = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(dest, name);

    return dest;
}


// Nodeの挿入
static Node *insert_node(Node *node, char *name, int id){

    if (node == NULL) {
        node = malloc(sizeof(Node));

        node->name = my_strdup(name);
        node->value = id;

        node->left = NULL;
        node->right = NULL;

        return node;
    }

    if (strcmp(node->name, name) < 0) {
        node->left = insert_node(node->left, name, id);
    } else {
        node->right = insert_node(node->right, name, id);
    }

    return node;
}

// Nodeの探索
static int search_node(Node *node, char *name){
    int cond;

    while (node != NULL) {
        if ((cond = strcmp(node->name, name)) == 0) {
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


int to_mnemonic_symbol(char *str){ // substringを渡せばよい？
    int value;

    if ((value = search_node(mnemonic_root, str)) != NOT_FOUND) {
        return value;
    }

    mnemonic_root = insert_node(mnemonic_root, str, mnemonic_id);
    mnemonic_id++;

    return mnemonic_id - 1;
}


// 初期化
void initialize_mnemonic_root(){
    delete_tree(mnemonic_root);
    mnemonic_root = NULL;
    mnemonic_id = 1;
}



/***************** Unit tests*****************/

static int streq(char *s1, char *s2) { return 0 == strcmp(s1, s2); }


static void test_insert_node_when_call_once(){

    // SetUP
    char *input = "mov";

    char *expect_name = "mov";
    int expect_value = 1;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, input, mnemonic_id);

    // Verify
    assert(expect_value == mnemonic_root->value);
    assert(streq(expect_name, mnemonic_root->name));

    // TearDown
    initialize_mnemonic_root();
}

static void test_insert_node_when_call_three_times(){

    // SetUP
    char *input1 = "mov";
    char *input2 = "abcdef";
    char *input3 = "x";

    char *expect_name1 = "mov";
    char *expect_name2 = "abcdef";
    char *expect_name3 = "x";

    int expect_value1 = 1;
    int expect_value2 = 2;
    int expect_value3 = 3;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, input2, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, input3, mnemonic_id);
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

static void test_search_node_when_call_once(){

    // SetUP
    char *input = "mov";

    int expect_value = 1;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, input, mnemonic_id);
    int actual_value = search_node(mnemonic_root, input);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_mnemonic_root();
}

static void test_search_node_when_call_three_times(){

    // SetUP
    char *input1 = "mov";
    char *input2 = "abcdef";
    char *input3 = "x";

    int expect_value1 = 1;
    int expect_value2 = 2;
    int expect_value3 = 3;

    // Exercise
    mnemonic_root = insert_node(mnemonic_root, input1, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, input2, mnemonic_id);
    mnemonic_id++;

    mnemonic_root = insert_node(mnemonic_root, input3, mnemonic_id);
    mnemonic_id++;

    int actual_value1 = search_node(mnemonic_root, input1);
    int actual_value2 = search_node(mnemonic_root, input2);
    int actual_value3 = search_node(mnemonic_root, input3);


    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);
    assert(expect_value3 == actual_value3);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_call_once(){

    // SetUP
    char *input = "mov";

    int expect_value = 1;

    // Exercise
    int actual_value = to_mnemonic_symbol(input);

    // Verify
    assert(expect_value == actual_value);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_same_symbol(){

    // SetUP
    char *input1 = "mov";
    char *input2 = "mov";

    int expect_value1 = 1;
    int expect_value2 = 1;


    // Exercise
    int actual_value1 = to_mnemonic_symbol(input1);
    int actual_value2 = to_mnemonic_symbol(input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}

static void test_to_mnemonic_symbol_when_different_symbol(){

    // SetUP
    char *input1 = "mov";
    char *input2 = "str";

    int expect_value1 = 1;
    int expect_value2 = 2;

    // Exercise
    int actual_value1 = to_mnemonic_symbol(input1);
    int actual_value2 = to_mnemonic_symbol(input2);

    // Verify
    assert(expect_value1 == actual_value1);
    assert(expect_value2 == actual_value2);

    // TearDown
    initialize_mnemonic_root();
}


static void test_my_strdup(){

    // SetUP
    char *input = "mov";

    char *expect_name = "mov";

    // Exercise
    char *actual_name = my_strdup(input);

    // Verify
    assert(streq(expect_name, actual_name));

    // TearDown
    initialize_mnemonic_root();
}


static void unit_tests() {
    test_insert_node_when_call_once();
    test_insert_node_when_call_three_times();

    test_search_node_when_call_once();
    test_search_node_when_call_three_times();

    test_to_mnemonic_symbol_when_call_once();
    test_to_mnemonic_symbol_when_same_symbol();
    test_to_mnemonic_symbol_when_different_symbol();

    test_my_strdup();
}


int main() {

    unit_tests();

    return 1;
}