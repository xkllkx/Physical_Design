#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

class TreeNode;
class BST;

class TreeNode{
private:
	
public:
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent;
	int data;

	TreeNode() :left(NULL), right(NULL), parent(NULL), data(0) {};
	TreeNode(int val) :left(NULL), right(NULL), parent(NULL), data(val) {};
	~TreeNode(){
		if (left) delete left;
		if (right) delete right;
	}

	friend class BST;
};

class BST{
private:

public:
	TreeNode* root;

	BST(){root = new TreeNode();};
	BST(int val){root = new TreeNode(val);};
	~BST(){};
	
	// insert TreeNode(val) to BST
	void Pre_Insert(TreeNode* parent,TreeNode* &temp,int val); // PreOrder
	void Pre_Insert(TreeNode* &root,int val); // PreOrder
	void L_Insert(TreeNode* &root,int val); // LevelOrder
	
	bool indicate_Insert(TreeNode* &target, TreeNode* &mov_node);

	TreeNode* Search(int val); // return the first pointer of treenode if its data==val;if val not exist in tree, return NULL;

	TreeNode* leftmost(TreeNode*);
	TreeNode* rightmost(TreeNode*);
	int Delete(TreeNode* &temp); // directly delete element by pointer  //O(1)
	int Delete(int val); // delete the first element which data is equal to val. if delete success return 0, else return non-zero. //O(logn)
	
	void Tree_Delete(TreeNode* &temp);

	// display
	void InOrder(TreeNode* temp);
	void PreOrder(TreeNode* temp);
	void PostOrder(TreeNode* temp);
	void LevelOrder(TreeNode* root);
};

#endif //_TREE_NODE_H_