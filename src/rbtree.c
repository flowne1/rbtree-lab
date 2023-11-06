#include "rbtree.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// 필요한 enum을 추가로 정의한다
typedef enum {
  LEFT,
  RIGHT
} direction;
// 필요한 함수를 추가로 정의한다
void insert_fixup(node_t *curr, rbtree *t);
void rotate_dir(node_t *curr, direction dir, rbtree *t);

rbtree *new_rbtree(void) {
  // rbtree 타입의 포인터 p를 선언하고 메모리 할당
  // calloc 함수는 메모리를 할당하면서 모든 바이트를 0으로 초기화한다.
  // 여기서는 rbtree 구조체 하나의 크기만큼 메모리를 할당하고 그 메모리 주소를 p에 저장한다.
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // 메모리 할당 체크
  if (!p){
    return NULL;
  }

  // nil 노드에 메모리를 할당한다
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  // 메모리 할당 체크
  if (!p->nil){
    free(p);
    return NULL;
  }
  // 초기값을 할당한다
  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  // 루트는 초기에 닐노드를 가리키도록 한다
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory

  // 루트부터 트리를 '후위순회'하면서 각 노드에 할당된 메모리를 모두 해제한다.
  // curr, prev 노드를 정의한다
  node_t *curr = t->root;
  node_t *prev = NULL;
  
  // 스택을 정의한다
  node_t *stack[20]; //일단 20으로 잡는다. 나중에 realloc등을 사용해서 크기를 조정할 필요가 있다
  int top = -1; //스택의 최상위 인덱스, 비어있으면 -1

  // 현재 노드가 nil이 아니거나 스택에 요소가 있을 때
  while (curr != t->nil || top != -1) {
    // 왼쪽 서브트리로 내려가면서 닐노드를 만날때까지 노드를 스택에 넣는다
    while (curr != t->nil) {
      stack[++top] = curr;
      curr = curr->left;
    }
    // 현재 스택에서 노드를 pop한다
    curr = stack[top];

    // 팝한 노드의 오른쪽 자식이 없거나, 이미 방문한 경우 자식이 없는것이 확정이므로 현재 메모리를 해제한다
    if (curr->right == t->nil || curr->right == prev) {
      free(curr);
      top--;
      prev = curr;
      curr = t->nil;
    // 오른쪽으로 계속 내려간다
    } 
    else {
      curr = curr->right;
    }
  }
  // 남은 메모리 해제(nil, t)
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 삽입할 위치를 찾는다.
  node_t *curr = t->root;
  node_t *parent = t->nil;
  bool is_right = false;

  while (curr != t->nil){
    // parent는 따로 받아둔다
    parent = curr;
    // 같거나 크면 오른쪽으로 간다(동일한 키값이 가능).
    if (key >= curr->key){
      is_right = true;
      curr = curr->right;
      printf("%i 오른쪽\n", key);
    }
    // 작으면 왼쪽으로 간다
    else{
      is_right = false;
      curr = curr->left;
      printf("%i 왼쪽\n", key);
    }
  }
  // 여기까지 다 돌고 나왔으면 curr은 nil이고 parent정보가 있을 것.
  // 해당 노드를 삽입한다.
  curr = (node_t*)calloc(1, sizeof(node_t));
  if (!curr){
    return t->nil;
  }
  curr->color = RBTREE_RED;
  curr->key = key;
  curr->parent = parent;
  curr->left = t->nil;
  curr->right = t->nil;
  // 해당 노드가 루트인지 아닌지에 따라 처리를 다르게 한다
  if (curr->parent == t->nil){
    t->root = curr;
    curr->color = RBTREE_BLACK;
  }else{
    if (is_right){
      printf("오른쪽에 넣을게요\n");
      parent->right = curr;
    }
    else{
      printf("왼쪽에 넣을게요\n");
      parent->left = curr;
    }
  }
  // 삽입한 후 부모가 레드라서 레드-레드 충돌이 생기는 경우 추가적인 픽스가 필요하다. 따로 함수를 정의한다.
  if (curr->parent->color == RBTREE_RED){
    printf("수정합니다\n");
    insert_fixup(curr, t);
  }
  
  return t->root;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  // nil노드를 찾을때까지 bt의 정의에 따라 노드를 서칭한다
  node_t *curr = t->root;
  while (curr != t->nil){
    // 필요한 키값을 찾으면 반환한다
    if (key == curr->key){
      break;
    // 키값을 못찾았으면 계속 서칭한다
    }else if (key > curr->key){
      curr = curr->right;
    }else{
      curr = curr->left;
    }
  }
  // 키값을 못찾으면 null, 찾으면 해당 노드를 반환한다
  if (curr == t->nil){
    return NULL;
  }else{
    return curr;
  }
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void insert_fixup(node_t *curr, rbtree *t){
  while (curr->parent->color == RBTREE_RED) {
    // 현재 노드의 부모, 할아버지, 삼촌을 찾는다
    node_t *grandparent, *uncle, *parent;
    parent = curr->parent;
    grandparent = parent->parent;
    if (grandparent->left == parent){
      uncle = grandparent->right;
    }else{
      uncle = grandparent->left;
    }
    // CASE 1. 삼촌이 모두 레드인 경우
    // 이경우는 할아버지한테 레드를 올려주고, 현재 노드를 할아버지 노드로 변경시킨 후 다시 진행한다
    if (parent->color == RBTREE_RED && uncle->color == RBTREE_RED){
      parent->color = RBTREE_BLACK;
      uncle->color = RBTREE_BLACK;
      grandparent->color = RBTREE_RED;
      curr = grandparent;
    // CASE 2. 삼촌이 블랙인 경우
    }else{
      // 전처리 과정. 꺾여있을 경우 펴준다
      if (grandparent->left->right == curr){
        printf("<로 꺾였네요\n");
        rotate_dir(curr, LEFT, t);
      } 
      else if(grandparent->right->left == curr){
        rotate_dir(curr, RIGHT, t);
      }
      // 펴진상태에서 최종 회전 처리를 한다
      // 부모/할아버지 서로 색바꾸기
      curr->color = RBTREE_BLACK;
      grandparent->color = RBTREE_RED;
      // 회전
      if (grandparent->left == curr){
        rotate_dir(curr, RIGHT, t);
      }else{
        printf("최종회전처리를 한다고?\n");
        rotate_dir(curr, LEFT, t);
      }
    }
  }
  // CASE1.에서 할아버지 노드가 루트면, 루트를 RED로 변경하는 문제가 생길 수 있다. 
  // 위에서 루트를 건드린 경우, black으로 변경한다.
  if (curr->parent == t->nil){
    curr->color = RBTREE_BLACK;
  }
}

void rotate_dir(node_t *curr, direction dir, rbtree *t){
  node_t *parent, *grandparent;
  parent = curr->parent;
  grandparent = parent->parent;

  // 왼쪽으로 회전
  if (dir == LEFT){
    // p가 root라면 gp는 Nil이다. 갱신 전 체크가 필요하다.
    if (grandparent != t->nil){
      printf("nil 아니에요\n");
      grandparent->left = curr;
      grandparent->left->parent = grandparent;
    }else{
      t->root = curr;
      curr->parent = t->nil;
    }

    parent->right = curr->left;
    parent->right->parent = parent;

    curr->left = parent;
    curr->left->parent = curr;
  // 오른쪽으로 회전
  }else{
    // p가 root라면 gp는 Nil이다. 갱신 전 체크가 필요하다.
    if (grandparent != t->nil){
      grandparent->right = curr;
      grandparent->right->parent = grandparent;
    }else{
      t->root = curr;
      curr->parent = t->nil;
    }

    parent->left = curr->right;
    parent->left->parent = parent;

    curr->right = parent;
    curr->right->parent = curr;
  }
}

// int main(){
//   rbtree *t = new_rbtree();
//   printf("t->root address: %p\n", t->root);
//   printf("t->nil address: %p\n", t->nil);
//   node_t *p1 = rbtree_insert(t, 1024);
  
//   printf("t->root address: %p\n", t->root);
//   printf("t->nil address: %p\n", t->nil);
//   // printf("t->root->key : %i\n", t->root->key);
//   // printf("t->root->color : %d\n", t->root->color);

//   // printf("t->root->right->key : %i\n", t->root->right->key);
//   // printf("t->root->right->color : %i\n", t->root->right->color);

//   // printf("t->root->left->key : %i\n", t->root->left->key);
//   // printf("t->root->left->color : %i\n", t->root->left->color);




//   return 0;
// }