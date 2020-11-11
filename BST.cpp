#define _CRT_SECURE_NO_WARNINGS  // To Suppress error about fopen() etc.. remove this when the compiler is not Visual C/C++

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


/////////// Tree structure : DO NOT MODIFY - begin
#define MAX_FILE_NAME_LEN 128
#define MAX_CMD_SIZE 128
#define MAX_NUM_FULL_BT_NODES_4_VISUAL 4096
#define MAX_NUM_FULL_BT_TREE_HEIGHT_4_VISUAL 10
#define MAX_NUM_FULL_BT_LEAVES 1024



///////////////////////////////
/// 이 부분은 사용하기만 하고 정의를 변경하지 마시오 - begin
///////////////////////////////
typedef struct BSTNode {
    int key_val; //Key value
    struct BSTNode* parent; //the pointer to the parent node
    struct BSTNode* left_child;  //the pointer to the left child
    struct BSTNode* right_child; //the pointer to the right child

    //For visualization. Do not modify. - begin
    int node_id;
    //For visualization. Do not modify. - end
}Node;

typedef struct BST {
    Node* root;
}BST;

//BST
BST global_bst;

Node* create_node(int key_val);
void free_node(Node** node);
///////////////////////////////
/// 이 부분은 사용하기만 하고 정의를 변경하지 마시오 - end
///////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//////////// These are for test and tree visuallization. DO NOT MODIFY - begin
/////////////////////////////////////////////////////////////////////////////
Node* g_tree_visual[MAX_NUM_FULL_BT_NODES_4_VISUAL];
Node* FindNodeByNodeID(int node_id, Node* root);
void TreePrint(Node* root);

void construct_test_tree();
void clear_tree();
void clear_all_nodes(Node* root);

/////////////////////////////////////////////////////////////////////////////
//////////// These are for tree visuallization. DO NOT MODIFY - end
/////////////////////////////////////////////////////////////////////////////



/*****************************************************************************
* 아래 함수들의 내용을 변경하시오, 만약 새 함수를 만든다면 이 부분에 선언하시오 - begin
******************************************************************************/
Node* find_key(int search_key, Node* start_node);
bool insert_key(int insertion_key, Node* start_node);
bool remove_key(int deletion_key, Node* start_node);

Node* find_left_max_node(Node* start_node); // 왼쪽 서브트리 중 가장 큰 key값을 가진 Node를 반환
Node* find_right_min_node(Node* start_node); // 오른쪽 서브트리 중 가장 작은 key값을 가진 Node를 반환
/*****************************************************************************
* 아래 함수들의 내용을 변경하시오, 만약 새 함수를 만든다면 이 부분에 선언하시오 -  end
******************************************************************************/




int main(int argc, char** argv) {

    FILE* input_cmd_fp = NULL;
    char* input_cmd_file = NULL;

    global_bst.root = NULL;

    if (argc < 2) {
        printf("Usage: bst [in_cmd_file]\n");
        return 0;
    }

    input_cmd_file = argv[1];
    input_cmd_fp = fopen(input_cmd_file, "r");
    printf("Input cmd file %s is opened\n", input_cmd_file);

    char input_cmd[MAX_CMD_SIZE + 1];
    memset(&input_cmd, 0, sizeof(char) * (MAX_CMD_SIZE + 1));

    int key_val = 0;
    if (input_cmd_fp != NULL) {
        // Input command
        // [cmd] [key]\n
        // cmd =>  f : find, i : insert, r : remove
        // key =>  integer key value
        // special command
        // clear 0: Clear all nodes
        // build 0: Build pre-defind BST tree for search test
        // print 0: Print tree        
        while (true) {
            int read_data_size = fscanf(input_cmd_fp, "%s %d\n", input_cmd, &key_val);
            if (read_data_size < 1) {
                break;
            }
            //Cmd execute            
            printf("> cmd: %s", input_cmd);
            if (strcmp(input_cmd, "i") == 0) {
                //Insert
                printf(", key:% d\n", key_val);
                bool success = insert_key(key_val, global_bst.root);
                if (success) {
                    printf("Inserted\n");
                }
                else {
                    printf("exists\n");
                }
            }
            else if (strcmp(input_cmd, "f") == 0) {
                //Find
                printf(", key:% d\n", key_val);
                Node* found_node = find_key(key_val, global_bst.root);
                if (found_node != NULL) {
                    printf("key %d is found at node id %d\n", found_node->key_val, found_node->node_id);
                }
                else {
                    printf("key %d is not found\n", key_val);
                }
            }
            else if (strcmp(input_cmd, "r") == 0) {
                //Remove
                printf(", key:% d\n", key_val);
                bool success = remove_key(key_val, global_bst.root);
                if (success) {
                    printf("Key %d is removed\n", key_val);
                }
                else {
                    printf("Key %d does not exist\n", key_val);
                }
            }
            else if (strcmp(input_cmd, "clear") == 0) {
                printf("\n");
                clear_tree();
            }
            else if (strcmp(input_cmd, "build") == 0) {
                printf("\n");
                construct_test_tree();
            }
            else if (strcmp(input_cmd, "print") == 0) {
                printf("\n");
                TreePrint(global_bst.root);
            }

        }
        fclose(input_cmd_fp);

    }

    printf("All finished. Bye~\n");
    return 0;
}



//////////////// Use this function to create new Node - Do not modify this function - begin
Node* create_node(int key_val) {
    Node* node = (Node*)malloc(sizeof(Node));

    node->key_val = key_val;
    node->parent = NULL;
    node->left_child = NULL;
    node->right_child = NULL;

    return node;
}

void free_node(Node** node) {
    free(*node);
    *node = NULL;
}
//////////////// Use this function to create new Node - Do not modify this function - end

/*
    처음에 재귀 방식으로 구현했다가 꼬여버려서 그냥 교재에 슈도 코드 보고 비슷하게 구현함.
    print가 안돼서 global_bst.root 과 연결해줘야 된다는 것을 뒤늦게 알아서 연결해주고 해결.
*/

bool insert_key(int insertion_key, Node* start_node) {
    /*
        insertion_key를 트리에 삽입.
        키값이 중복되지 않고 성공적으로 삽입됐다면 true,
        중복되어 삽입 불가능 하다면 false를 반환.
    */
    bool succeed = false;
    //여기다 작성하시오. - begin

    Node*& current_node = start_node; // start_node를 레퍼런스 타입 current_node 라는 이름으로 재정의
    Node* parent_node = NULL; // parent_node는 존재하지 않을 수 있으니 default로 NULL값을 부여한다.

    while (current_node != NULL) { // 루트 노드가 존재하는 경우

        // 이원 탐색 트리는 중복 key값을 허용하지 않기 때문에 중복 값이 발견되면 false를 반환하고 함수를 종료한다.
        if (insertion_key == current_node->key_val) return false;

        // 부모 노드에 현재 노드를 삽입해주고, 현재 노드를 자식 노드로 치환.
        // 한 단계씩 트리를 타고 내려가는 역할을 수행.
        parent_node = current_node;
        if (insertion_key < current_node->key_val) { // 삽입하려는 key가 현재 노드의 key보다 작을 경우 
            current_node = current_node->left_child;
        }
        else { // 삽입하려는 key가 현재 노드의 key보다 클 경우
            current_node = current_node->right_child;
        }
    }

    // 위 반복문을 거치고 내려오면 current_node는 무조건 NULL값이다.
    // current_node는 새 노드를 할당받는다.
    // 만약 current_node가 루트 노드라면 부모 노드 역시 default인 NULL이 삽입된다.
    current_node = create_node(insertion_key); 
    current_node->parent = parent_node;

    // 부모가 NULL인 경우는 current_node가 루트 노드인 경우, 오직 한 가지만 존재한다.
    // 루트 노드이기 때문에, 출력을 위해서 global_bst.root에 연결해준다.
    if (parent_node == NULL) {
        global_bst.root = current_node;
        return true;
    }

    // 부모 노드의 자식 노드에 대소 비교 후 current_node를 대입해준다.
    if (insertion_key < parent_node->key_val) parent_node->left_child = current_node;
    else parent_node->right_child = current_node;
    succeed = true;

    //여기다 작성하시오. - end
    return succeed;
}

Node* find_key(int search_key, Node* start_node) {
    /*
        search_key값을 가진 Node가 Tree에서 존재하는지 검색하여,
        존재하면 해당 노드의 포인터를 반환. 존재하지 않으면 NULL을 반환.
    */
    Node* found_node = NULL;
    //여기다 작성하시오. - begin

    // 재귀적 방법으로 구현함.

    // 찾는 노드가 존재하지 않는 경우에는 found_node(NULL)을 반환한다.
    if (start_node == NULL) return found_node;
    
    // 찾는 값이 존재하는 경우, 그 노드 자체를 반환한다.
    if (search_key == start_node->key_val) {
        found_node = start_node;
    }

    // 찾는 노드의 key가 현재 노드의 key보다 작을 경우 왼쪽 노드로 이동해서 다시 함수를 호출한다.
    else if (search_key < start_node->key_val) {
        found_node = find_key(search_key, start_node->left_child);
    }
    
    // 찾는 노드의 key가 현재 노드의 key보다 클 경우 오른쪽 노드로 이동해서 다시 함수를 호출한다.
    else { // search_key > start_node->key_val
        found_node = find_key(search_key, start_node->right_child);
    }

    //여기다 작성하시오. - end
    return found_node;
}

bool remove_key(int deletion_key, Node* start_node) {
    /*
        deletion_key를 트리에서 삭제.
        삭제하려는 키값이 존재해서 성공적으로 삭제됐다면 true,
        값을 찾을 수 없어 삭제 불가능하다면 false를 반환.

        이원 탐색 트리에서 Node를 삭제할 때,
        삭제하는 Node의 자식 Node의 개수에 따라 각기 다른 방식으로 삭제 연산을 수행한다.

        Case 1: 삭제하려는 Node의 자식 Node가 없을 때
            >>> 바로 할당 해제한다.

        Case 2: 삭제하려는 Node의 자식 Node가 1개 일 때
            >>> 삭제하려는 Node의 부모 Node와 자식 Node를 연결시켜주고 할당 해제한다.

        Case 3: 삭제하려는 Node의 자식 Node가 2개 일 때
            >>> 삭제하려는 Node의 왼쪽 서브트리 중 가장 큰 값 또는 오른쪽 서브트리 중 가장 작은 값으로 대체 후 할당 해제한다.
    */
    bool succeed = false;
    //여기다 작성하시오. - begin

    // 우리가 삭제할 노드를 target_node라고 하자.
    // 미리 구현해둔 find_key의 반환값을 통해, 삭제하려는 노드의 존재유무를 먼저 파악한다.
    Node* target_node = find_key(deletion_key, start_node);

    // find_key() 함수로부터 NULL값이 반환되었다면, 트리에 삭제하려는 key의 노드가 존재하지 않는다는 뜻이므로,
    // succeed 값을 그대로(false) 반환하고 함수를 종료한다.
    if (target_node == NULL) {
        // 삭제하려는 Node가 존재하지 않는 경우
        return succeed;
    }

    // 여기부터는 무조건 삭제하려는 노드가 존재하는 경우이다.
    // 여기서도 마찬가지로, parent_node가 NULL값을 가지고 있다면 target_node가 루트 노드라고 판단한다.
    Node* parent_node = target_node->parent;
    
    /*
        Case 1의 경우
    */
    if (target_node->left_child == NULL && target_node->right_child == NULL) {

        // 부모도 NULL이며, 자식도 둘 다 NULL을 가지고 있는 경우이다.
        // 루트 노드가 단독 노드일 경우인데, global_bst.root 에도 역시 NULL을 삽입한다.
        // 그리고 바로 target_node를 할당 해제한다.
        if (parent_node == NULL) {
            global_bst.root = NULL;
            free_node(&target_node);
            succeed = true;
            return succeed;
        }

        // 부모 노드의 왼쪽 자식을 삭제하는 경우, 왼쪽 자식에 NULL을 삽입하고 target_node를 할당 해제한다.
        if (parent_node->left_child != NULL && parent_node->left_child->key_val == deletion_key) { 
            parent_node->left_child = NULL;
        }

        // 부모 노드의 오른쪽 자식을 삭제하는 경우, 오른쪽 자식에 NULL을 삽입하고 target_node를 할당 해제한다.
        else { // parent_node->right_child != NULL && parent_node->right_child->key_val == deletion_key
            parent_node->right_child = NULL;
        }

        free_node(&target_node);
        succeed = true;
    }

    /*
        Case 2의 경우
    */
    else if (target_node->left_child == NULL || target_node->right_child == NULL) {
        
        // 부모가 NULL이며, 자식은 한 쪽만 가지고 있는 경우이다.
        // 한 쪽의 자식을 global_bst.root에 연결시켜주고, target_node를 할당 해제한다.
        if (parent_node == NULL) {
            if (target_node->left_child == NULL) {
                // 오른쪽 자식이 있는 경우
                target_node->right_child->parent = NULL;
                global_bst.root = target_node->right_child;
            }
            else {
                // 왼쪽 자식이 있는 경우
                target_node->left_child->parent = NULL;
                global_bst.root = target_node->left_child;
            }
            free_node(&target_node);
            return true;
        }

        // 부모 Node의 왼쪽 자식 Node가 Target Node일 경우
        if (parent_node->left_child != NULL && parent_node->left_child->key_val == target_node->key_val) {
            // target_node의 자식 노드를 부모 노드에게 연결시켜 준다.
            Node* child_node = target_node->left_child == NULL ? target_node->right_child : target_node->left_child;
            parent_node->left_child = child_node;
            child_node->parent = parent_node;
        }

        // 부모 Node의 오른쪽 자식 Node가 Target Node일 경우
        else { // parent_node->right_child != NULL && parent_node->right_child->key_val == target_node->key_val
            // target_node의 자식 노드를 부모 노드에게 연결시켜 준다.
            Node* child_node = target_node->left_child == NULL ? target_node->right_child : target_node->left_child;
            parent_node->right_child = child_node;
            child_node->parent = parent_node;
        }

        free_node(&target_node);
        succeed = true;
    }

    /*
        Case 3의 경우
    */
    else {
        /*
            Target Node를 왼쪽 서브트리 중 가장 큰 key값을 가진 Node로 바꾸거나,
            오른쪽 서브트리 중 가장 작은 key값을 가진 Node로 바꾸면 된다.

            필요한 두 Node를 찾는 함수를 모두 구현했으나,
            하나의 함수만 있어도 충분하다.

            본인은 "왼쪽 가장 큰 값"으로 변경하겠다.
        */
        
        // 왼쪽 가장 큰 값을 가진 노드를 left_max_node라는 변수에 대입한다.
        Node* left_max_node = find_left_max_node(target_node->left_child);

        // 삭제하려는 노드에 left_max_node의 key값을 대입한다.
        target_node->key_val = left_max_node->key_val;

        // 이미 삭제하려는 노드에는 대체 key값이 들어갔으니 건드릴 필요가 없고,
        // 중복되는 key값을 가지는 서브트리의 노드를 찾아서 제거해야 한다.
        // 이는 remove_key 함수를 재호출해서, 중복되는 값을 삭제해준다.
        remove_key(target_node->key_val, target_node->left_child);
        
        succeed = true;
    }

    //여기다 작성하시오. - end
    return succeed;
}

Node* find_left_max_node(Node* start_node) {
    // 왼쪽 서브트리 중 가장 큰 key값을 가진 Node를 반환
    // 반복문으로 NULL값을 가진 노드가 나올 때 까지 반복

    Node* curr = start_node;

    while (curr->right_child != NULL) {
        curr = curr->right_child;
    }

    return curr;
}

Node* find_right_min_node(Node* start_node) {
    // 오른쪽 서브트리 중 가장 작은 key값을 가진 Node를 반환
    // 반복문으로 NULL값을 가진 노드가 나올 때 까지 반복
    
    Node* curr = start_node;

    while (curr->left_child != NULL) {
        curr = curr->left_child;
    }

    return curr;
}

/***************************************************************************
For Test : Remove all Nodes. DO NOT MODIFY - begin
****************************************************************************/
void construct_test_tree() {
    if (global_bst.root != NULL) { //Tree already exist. clear existing tree
        clear_all_nodes(global_bst.root);
    }

    Node* root_15 = create_node(15); //Root -> 15
    global_bst.root = root_15;

    Node* lc_of_root = create_node(5); //Left child of root -> 5
    lc_of_root->parent = root_15;
    root_15->left_child = lc_of_root;

    Node* rc_of_root = create_node(30); //Right child of root -> 15
    rc_of_root->parent = root_15;
    root_15->right_child = rc_of_root;

    Node* node_2 = create_node(2); //Left child of lc_of_root -> 2
    node_2->parent = lc_of_root;
    lc_of_root->left_child = node_2;

    Node* node_8 = create_node(8); //Left child of lc_of_root -> 8
    node_8->parent = lc_of_root;
    lc_of_root->right_child = node_8;

    Node* node_35 = create_node(35); //Right child of rc_of_root -> 35
    node_35->parent = rc_of_root;
    rc_of_root->right_child = node_35;

    Node* node_33 = create_node(33); //Left child of 35 -> 33
    node_33->parent = node_35;
    node_35->left_child = node_33;

    Node* node_40 = create_node(40); //Right child of 35 -> 40
    node_40->parent = node_35;
    node_35->right_child = node_40;
}

void clear_tree() {
    if (global_bst.root != NULL) { //Tree already exist. clear existing tree
        clear_all_nodes(global_bst.root);
        global_bst.root = NULL;
    }
}

void clear_all_nodes(Node* cur_node) {
    if (cur_node == NULL) {
        return;
    }
    if (cur_node->left_child == NULL && cur_node->right_child == NULL) { // I am a leaf. delete
        if (cur_node->parent != NULL) { //If i have a child of my parent, then remove the parent's pointer to me 
            if (cur_node->parent->left_child == cur_node) { // I am the left child of my parent.
                cur_node->parent->left_child = NULL;
            }
            else if (cur_node->parent->right_child == cur_node) { // I am the right child of my parent.
                cur_node->parent->right_child = NULL;
            }
        }
        free_node(&cur_node); //free the memory allocated to me.
    }
    else { //I have left child. go to my left child.
        clear_all_nodes(cur_node->left_child);
        clear_all_nodes(cur_node->right_child);
        clear_all_nodes(cur_node);
    }
}
/***************************************************************************
For Test : Remove all Nodes. DO NOT MODIFY - end
****************************************************************************/



/////////////////////////////////////////////////////////////////////////////
//////////// These two functions are for visuallization. Do not touch - begin
/////////////////////////////////////////////////////////////////////////////
Node* FindNodeByNodeID(int node_id, Node* root) {
    if (node_id >= MAX_NUM_FULL_BT_NODES_4_VISUAL) {
        return NULL;
    }
    int reverse_path[MAX_NUM_FULL_BT_TREE_HEIGHT_4_VISUAL];
    int path_cursor = 0;
    memset(reverse_path, 0, sizeof(reverse_path));

    if (node_id == 0) {
        return root;
    }
    else {
        int start_node_id = node_id;
        for (int id = start_node_id; id > 0; ) {
            if (id % 2 == 0) { //right 
                reverse_path[path_cursor] = 2;//right
                path_cursor++;
                id = (id - 2) / 2;
            }
            else { //left
                reverse_path[path_cursor] = 1;//left
                path_cursor++;
                id = (id - 1) / 2;
            }
        }
        Node* search_node = root;
        for (int lr = path_cursor - 1; lr >= 0; lr--) {
            if (reverse_path[lr] == 1) { //Left
                if (search_node->left_child == NULL) {
                    return NULL;
                }
                else {
                    search_node = search_node->left_child;
                }
            }
            else if (reverse_path[lr] == 2) { //Right
                if (search_node->right_child == NULL) {
                    return NULL;
                }
                else {
                    search_node = search_node->right_child;
                }
            }
        }
        return search_node;
    }
}

void TreePrint(Node* root) {
    memset(g_tree_visual, 0, sizeof(g_tree_visual));
    int node_id = 0;
    int tree_max_height = 0;
    for (int i = 0; i < MAX_NUM_FULL_BT_TREE_HEIGHT_4_VISUAL + 1; i++) {
        int num_max_nodes_in_this_height = (int)pow(2, i);
        int num_null_nodes_in_this_height = 0;
        for (int j = 0; j < num_max_nodes_in_this_height; j++) {
            Node* node = FindNodeByNodeID(node_id, root);
            g_tree_visual[node_id] = node;
            if (node == NULL) {
                num_null_nodes_in_this_height++;
            }
            else {
                node->node_id = node_id;
            }

            node_id++;
        }
        if (num_max_nodes_in_this_height <= num_null_nodes_in_this_height) {
            tree_max_height = i - 1;
            break;
        }
    }

    printf("==============> Tree print \n");
    int processed_num = 0;
    for (int i = 0; i < tree_max_height + 1; i++) {
        int num_max_nodes_in_this_height = (int)pow(2, i);
        for (int j = 0; j < num_max_nodes_in_this_height; j++) {
            if (g_tree_visual[processed_num] != NULL) {
                int node_id = g_tree_visual[processed_num]->node_id;
                int key_val = g_tree_visual[processed_num]->key_val;
                printf("(id: %d, key: %d), ", node_id, key_val);
            }
            else {
                printf("EMPTY, ");
            }
            processed_num++;
        }
        printf("\n");
    }
    printf("==============================\n");
}
/////////////////////////////////////////////////////////////////////////////
//////////// these tow functions are for visuallization. Do not touch - end
/////////////////////////////////////////////////////////////////////////////
