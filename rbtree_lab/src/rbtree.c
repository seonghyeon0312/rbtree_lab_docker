#include "rbtree.h"

#include <stdlib.h>
#include <limits.h>

static void leftRotation(rbtree* t, node_t* node){
  node_t* y = node->right;
  node->right = y->left;

  if(y->left!=t->nil){
    y->left->parent  = node;
  }
  y->parent = node->parent;

  if(node->parent == t->nil){
    t->root = y;
  }else if(node == node->parent->left){
    node->parent->left = y;
  }else{
    node->parent->right = y;
  }
  y->left= node;
  node->parent=y;
}

static void rightRotation(rbtree* t, node_t* node){
  node_t* y = node->left;
  node->left = y->right;

  if(y->right!=t->nil){
    y->right->parent  = node;
  }
  y->parent = node->parent;

  if(node->parent == t->nil){
    t->root = y;
  }else if(node == node->parent->right){
    node->parent->right = y;
  }else{
    node->parent->left = y;
  }
  y->right= node;
  node->parent= y;
}

static node_t *subtree_min(rbtree* t, node_t *node) {
  // TODO: implement find
  // node의 왼쪽 자식 중 가장 작은 값
  while(node->left != t->nil){
    node=node->left;
  }
  return node;
}

static node_t* makeNilNode(void){
  node_t *init = (node_t*)calloc(1,sizeof(node_t));
    init->color=RBTREE_BLACK;
    init->key=0;
    init->left=init;
    init->parent=init;
    init->right=init;
  return init;
}

static node_t* makeNewNode(rbtree* t, const key_t key){
  node_t* new_node = (node_t*)calloc(1,sizeof(node_t));
    new_node->color=RBTREE_RED;
    new_node->key=key;
    new_node->left=t->nil;
    new_node->right=t->nil;
    new_node->parent = t->nil;
    return new_node;
}

static void insertFixup(rbtree* t, node_t* node){
  node_t* uncle;
  while(node->parent->color == RBTREE_RED){
    if(node->parent == node->parent->parent->left){
      uncle = node->parent->parent->right;
      if(uncle->color == RBTREE_RED){
        node->parent->color =RBTREE_BLACK;
        uncle->color=RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }else{
        if(node==node->parent->right){
          node=node->parent;
          leftRotation(t, node);
        }
        node->parent->color= RBTREE_BLACK;
        node->parent->parent->color= RBTREE_RED;
        rightRotation(t, node->parent->parent);
      }
    }else{
      if(node->parent == node->parent->parent->right){
      uncle = node->parent->parent->left;
      if(uncle->color == RBTREE_RED){
        node->parent->color =RBTREE_BLACK;
        uncle->color=RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }else{
        if(node==node->parent->left){
          node=node->parent;
          rightRotation(t, node);
        }
        node->parent->color= RBTREE_BLACK;
        node->parent->parent->color= RBTREE_RED;
        leftRotation(t, node->parent->parent);
      }
    }
  }
 }
 t->root->color= RBTREE_BLACK;
}

static void rbtreeDeleteFixup(rbtree* t, node_t* node){
  node_t* w = t->nil;

  while((node!= t->root) && (node->color == RBTREE_BLACK)){
    if(node==node->parent->left){
      w = node->parent->right;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        node->parent->color=RBTREE_RED;
        leftRotation(t,node->parent);
        w= node->parent->right;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        node = node->parent;
      }else{
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rightRotation(t, w);
          w=node->parent->right;
        }
        node->color= node->parent->color;
        node->parent->color= RBTREE_BLACK;
        w->right->color= RBTREE_BLACK;
        leftRotation(t, node->parent);
        node = t->root;
      }
    }else{
      w=node->parent->left;
      if(w->color==RBTREE_RED){
        w->color=RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        rightRotation(t, node->parent);
        w=node->parent->left;
      }
      if(w->right->color==RBTREE_BLACK && w->left->color==RBTREE_BLACK){
        w->color=RBTREE_RED;
        node=node->parent;
      }else{
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color=RBTREE_RED;
          leftRotation(t,w);
          w=node->parent->left;
        }
        w->color=node->parent->color;
        node->parent->color=RBTREE_BLACK;
        w->left->color=RBTREE_BLACK;
        rightRotation(t, node->parent);
        node = t->root;
      }
    }
  }
  node->color=RBTREE_BLACK;
}

static void rbtree_transplant(rbtree* t, node_t* u, node_t* v){
  
  if(u->parent == t->nil){
    t->root = v; //u가 루트 노드일 경우 루트로 v 지정
  }else if(u == u->parent->left){
    u->parent->left = v; // u가 기존 부모의 왼쪽 자식일 경우
  }else{
    u->parent->right = v; // u가 기존 부모의 오른쪽 자식일 경우
  }
  v->parent = u->parent; //v의 부모를 u에서 u->parent로 변경
}

static void _delete_rbtree(rbtree* t, node_t* node){
  if(node == t->nil){
    return;
  }
  _delete_rbtree(t, node->left);
  _delete_rbtree(t, node->right);
  free(node);
}


void inOrderTraversal(const rbtree* t,node_t* node, key_t* arr, size_t n, int* idx){
  if(node!=t->nil && (*idx)<n){
    inOrderTraversal(t,node->left, arr,n,idx);
    arr[(*idx)]=node->key;
    (*idx)++;
    inOrderTraversal(t,node->right,arr,n,idx); 
  }
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t* init = makeNilNode();
  p->root=init;
  p->nil=init;
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  _delete_rbtree(t,t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t* new = makeNewNode(t, key);
  node_t* target = t->root;
  node_t* y = t->nil;
  while(target != t->nil){
    y=target;
    if(new->key < target->key){
      target = target->left;
    }else{
      target = target->right;
    }
  }
  new->parent = y;

  if(y == t->nil){
    t->root = new;
  }else if(new->key <= y->key){
    y->left = new;
  }else if(new->key > y->key){
    y->right = new;
  }

  insertFixup(t,new);
  return new;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find

  if(t->root == t->nil){
    return NULL;
  }

  node_t* node = t->root;

  while(node != t->nil){
    if(node->key == key){
      break;
    }else if(node->key > key){
      node = node->left;
    }else if(node->key < key){
      node = node->right;
    }else{
      node = t->nil;
    }

  }
  if(node == t->nil){
    return NULL;
  }
  return node;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t* node = t->root;

  while(node->left != t->nil){
    node=node->left;
  }
  return node;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t* node = t->root;

  while(node->right != t->nil){
    node=node->right;
  }
  return node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  if(t==NULL || t->root == t->nil){
    return 0;
  }
  // TODO: implement erase
  node_t* x = t->nil; // 
  node_t* y = p; //삭제 노드
  color_t yOriginalColor = y->color; // 삭제 노드의 색

  if(p->left == t->nil){
    //삭제 대상 노드의 왼쪽 자식이 NIL일때 
    x= p->right; // 올리는 노드를 오른쪽 자식으로 설정
    rbtree_transplant(t,p,p->right); // 삭제
  }else if(p->right == t->nil){
    //삭제 대상 노드의 오른쪽 자식이 NIL일때
    x=p->left; // 올리는 노드를 왼쪽 자식으로 설정
    rbtree_transplant(t,p,p->left); // 삭제
  }else{
    y= subtree_min(t,p->right); // 후행자 찾아서 삭제 대상 노드로 지정
    yOriginalColor= y->color; // 후행자의 색깔을 삭제 색으로 판단
    x=y->right; // 삭제 대상 노드의 오른쪽 자식을 저장

    // 1. 삭제 대상 노드가 삭제 노드의 자식이 아닐 때
    if((y != p->right)){
      // 
      rbtree_transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }else{
      x->parent = y;
    }
    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color= p->color;
  }
  if(yOriginalColor == RBTREE_BLACK){
    rbtreeDeleteFixup(t, x);
  }
  free(p);
  p = NULL;
  return 0;
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int idx= 0;
  inOrderTraversal(t,t->root, arr,n,&idx);
  return 0;
}