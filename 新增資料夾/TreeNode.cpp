#include <iostream>
#include <queue>

#include "TreeNode.h"

using namespace std;

/*功能函式*/
void BST::Pre_Insert(TreeNode* parent,TreeNode* &temp,int val){
    if (temp == NULL) {
        temp = new TreeNode(val);
        temp->parent = parent;
    }
    else{
        if(temp->left == NULL) Pre_Insert(temp,temp->left,val);
        else if (temp->right == NULL) Pre_Insert(temp,temp->right,val);
        else Pre_Insert(temp->left,temp->left,val);
    }
}

void BST::Pre_Insert(TreeNode* &root,int val){
    if (root == NULL) {
        root = new TreeNode(val);
        root->parent = root;
    }
    else{
        if(root->left == NULL) Pre_Insert(root,root->left,val);
        else if (root->right == NULL) Pre_Insert(root,root->right,val);
        else Pre_Insert(root->left,root->left,val);
    }
}

void BST::L_Insert(TreeNode* &root,int val){
    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* temp = q.front();
        q.pop();

        if (temp->left != NULL) q.push(temp->left);
        else {
            temp->left = new TreeNode(val);
            temp->left->parent = temp;
            return;
        }

        if (temp->right != NULL) q.push(temp->right);
        else {
            temp->right = new TreeNode(val);
            temp->right->parent = temp;
            return;
        }
    }
}

// bool BST::indicate_Insert(TreeNode* &target, TreeNode* &mov_node){ // 1_to_1
//     // 拔起
//     if(mov_node->parent->left == mov_node) mov_node->parent->left = NULL;
//     else mov_node->parent->right = NULL;
    
//     // 插入
//     if(target->left == NULL) {
//         mov_node->parent = target;
//         target->left = mov_node;
//     }
//     else if(target->right == NULL){
//         mov_node->parent = target;
//         target->right = mov_node;
//     }
//     else return 0;

//     return 1;
// }

bool BST::indicate_Insert(TreeNode* &target, TreeNode* &mov_node){ // 1_to_1
    // 拔起
    if(mov_node->parent->left == mov_node) {
        // mov_node->parent->left = mov_node->parent->right;
        mov_node->parent->left = NULL;
    }
    else mov_node->parent->right = NULL;
    
    // 插入
    if(target->left == NULL) {
        mov_node->parent = target;
        target->left = mov_node;
    }
    else if(target->right == NULL){
        mov_node->parent = target;
        target->right = mov_node;
    }
    else return 0;

    return 1;
}

TreeNode* BST::Search(int val){
    TreeNode *temp = root;

    while (temp != NULL && val != temp->data){
        if (val < temp->data) temp = temp->left;
        else temp = temp->right;
    }

    if (temp == NULL) cout << "NULL" << endl;
    else{
        cout << temp->data << " " << temp;
        if(temp->parent != NULL) cout << " |p|" << temp->parent->data << " " << temp->parent;
        if(temp->left != NULL) cout << " |l|" << temp->left->data << " " << temp->left;
        if(temp->right != NULL) cout << " |r|" << temp->right->data << " " << temp->right;
        cout << endl;
    }

    return temp;
}

int BST::Delete(int val){ // delete the first element which data is equal to val. if delete success return 0, else return non-zero. //O(logn)
    TreeNode* temp = Search(val);

    if(temp != NULL) return Delete(temp);
    else return 1;
}

TreeNode* BST::leftmost(TreeNode *temp){
    while (temp->left != NULL){
        temp = temp->left;
    }

    return temp;
}

TreeNode* BST::rightmost(TreeNode *temp){
    while (temp->right != NULL){
        temp = temp->right;
    }

    return temp;
}

int BST::Delete(TreeNode* &temp){ // directly delete element by pointer  //O(1)
    TreeNode *new_root;

    if(temp != NULL && temp != root){ // root不刪
        if(temp->left == NULL && temp->right == NULL){ // 尾端
            if(temp->parent->left == temp) temp->parent->left = NULL;
            if(temp->parent->right == temp) temp->parent->right = NULL;
        }
        else if(temp->left != NULL){
            if(temp->right == NULL){ // 直接往上接
                temp->left->parent = temp->parent;
                if(temp->parent->left == temp) temp->parent->left = temp->left;
                if(temp->parent->right == temp) temp->parent->right = temp->left;
            }
            else{
                if(temp->right->left != NULL){
                    new_root = leftmost(temp->right);
                    // cout << new_root->data << endl;

                    // 解開 new_root
                    new_root->parent->left = new_root->right;
                    if(new_root->right != NULL) new_root->right->parent = new_root->parent;
                    
                    new_root->parent = temp->parent;
                    if(temp->parent == NULL) root = new_root;
                    else{
                        if(temp->parent->left == temp) temp->parent->left = new_root;
                        if(temp->parent->right == temp) temp->parent->right = new_root;
                    }

                    // if(temp->parent != NULL && temp->parent->left == temp) temp->parent->left = new_root;
                    // if(temp->parent != NULL && temp->parent->right == temp) temp->parent->right = new_root;

                    new_root->left = temp->left;
                    new_root->right = temp->right;
                    temp->left->parent = new_root;
                    temp->right->parent = new_root;
                }
                else{
                    temp->right->parent = temp->parent;
                    if(temp->parent->left == temp) temp->parent->left = temp->right;
                    if(temp->parent->right == temp) temp->parent->right = temp->right;
            
                    temp->right->left = temp->left;
                    temp->left->parent = temp->right;
                }
            }
        }
        else{ // temp->left == NULL
            temp->right->parent = temp->parent;
            if(temp->parent->left == temp) temp->parent->left = temp->right;
            if(temp->parent->right == temp) temp->parent->right = temp->right;
        }

        temp->left = NULL;
        temp->right = NULL;

        // delete temp;
        return 1; // 刪除成功
    }
    else return 0; // 刪除失敗
}

// int BST::Delete(TreeNode* &temp){ // directly delete element by pointer  //O(1)
//     TreeNode *new_root;

//     if(temp != NULL){
//         if(temp->left == NULL && temp->right == NULL){ // 尾端
//             if(temp->parent->left == temp) temp->parent->left = NULL;
//             if(temp->parent->right == temp) temp->parent->right = NULL;
//         }
//         else if(temp->left != NULL){
//             if(temp->right == NULL){
//                 temp->left->parent = temp->parent;
//                 if(temp->parent->left == temp) temp->parent->left = temp->left;
//                 if(temp->parent->right == temp) temp->parent->right = temp->left;
//             }
//             else{
//                 if(temp->right->left != NULL){
//                     new_root = leftmost(temp->right);
//                     // cout << new_root->data << endl;

//                     // 解開 new_root
//                     new_root->parent->left = new_root->right;
//                     if(new_root->right != NULL) new_root->right->parent = new_root->parent;
                    
//                     new_root->parent = temp->parent;
//                     if(temp->parent == NULL) root = new_root;
//                     else{
//                         if(temp->parent->left == temp) temp->parent->left = new_root;
//                         if(temp->parent->right == temp) temp->parent->right = new_root;
//                     }

//                     // if(temp->parent != NULL && temp->parent->left == temp) temp->parent->left = new_root;
//                     // if(temp->parent != NULL && temp->parent->right == temp) temp->parent->right = new_root;

//                     new_root->left = temp->left;
//                     new_root->right = temp->right;
//                     temp->left->parent = new_root;
//                     temp->right->parent = new_root;
//                 }
//                 else{
//                     temp->right->parent = temp->parent;
//                     if(temp->parent->left == temp) temp->parent->left = temp->right;
//                     if(temp->parent->right == temp) temp->parent->right = temp->right;
            
//                     temp->right->left = temp->left;
//                     temp->left->parent = temp->right;
//                 }
//             }
//         }
//         else{ // temp->left == NULL
//             temp->right->parent = temp->parent;
//             if(temp->parent->left == temp) temp->parent->left = temp->right;
//             if(temp->parent->right == temp) temp->parent->right = temp->right;
//         }

//         // delete temp;
//         return 1; // 刪除成功
//     }
//     else return 0; // 刪除失敗
// }

void BST::Tree_Delete(TreeNode* &temp){
    if(temp == NULL) return;
    
    // PostOrder
    Tree_Delete(temp->left);
    Tree_Delete(temp->right);

    // delete temp;
    // temp = NULL;

    if(temp != root) { // 留root
        delete temp;
        temp = NULL;
    }
};

/* display */
void BST::InOrder(TreeNode* temp){ // 左中右
    if(temp == NULL) return;

    InOrder(temp->left);
    cout << temp->data << " ";
    InOrder(temp->right);
}

void BST::PreOrder(TreeNode* temp){ // 中左右
    if(temp == NULL) return;

    cout << temp->data << " ";
    PreOrder(temp->left);
    PreOrder(temp->right);
}

void BST::PostOrder(TreeNode* temp){ // 左右中
    if(temp == NULL) return;

    PostOrder(temp->left);
    PostOrder(temp->right);
    cout << temp->data << " ";
}

void BST::LevelOrder(TreeNode* root){
    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()){
        TreeNode *target = q.front();
        q.pop();           

        cout << target->data << " ";
        
        if (target->left != NULL) q.push(target->left);
        if (target->right != NULL) q.push(target->right);
    }
    cout << endl;
}