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
void transplant(rbtree *t, node_t *pre, node_t *post);
node_t *return_successor(rbtree *t, node_t *p);
void delete_fixup(rbtree *t, node_t *target);
void inorder_trav (const rbtree *t, node_t *start, key_t *arr, int *i, const int n);

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
    }
    // 작으면 왼쪽으로 간다
    else{
      is_right = false;
      curr = curr->left;
    }
  }
  // 여기까지 다 돌고 나왔으면 curr은 nil이고 parent정보가 있을 것.
  // 해당 노드를 삽입한다.
  curr = (node_t*)calloc(1, sizeof(node_t));
  if (!curr){
    return NULL;
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
      parent->right = curr;
    }
    else{
      parent->left = curr;
    }
  }
  // 삽입한 후 부모가 레드라서 레드-레드 충돌이 생기는 경우 추가적인 픽스가 필요하다. 따로 함수를 정의한다.
  if (curr->parent->color == RBTREE_RED){
    insert_fixup(curr, t);
  }
  
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // nil노드를 찾을때까지 bt의 정의에 따라 노드를 서칭한다
  node_t *curr = t->root;
  // 루트와 키값이 같으면 바로 반환한다
  if (curr->key == key){
    return curr;
  }
  while (curr != t->nil){
    // 필요한 키값을 찾으면 반환한다
    if (key == curr->key){
      return curr;
    // 키값을 못찾았으면 계속 서칭한다
    }else if (key > curr->key){
      curr = curr->right;
    }else{
      curr = curr->left;
    }
  }
  // 키값을 못찾으면 null을 반환한다
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *curr = t->root;
  while (curr != t->nil){
    if (curr->left == t->nil){
      return curr;
    }
    curr = curr->left;
  }
  // 최소값 못찾으면 NULL(ex. root가 nil인 경우)
  return NULL;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *curr = t->root;
  while (curr != t->nil){
    if (curr->right == t->nil){
      return curr;
    }
    curr = curr->right;
  }
  // 최대값 못찾으면 NULL(ex. root가 nil인 경우)
  return NULL;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // 삭제 노드의 대체 노드, 대체 노드의 대체 노드, fixup 노드를 정의한다
  // fix-up은 target을 대상으로 한다.
  node_t *replacer, *replacer2, *target;
  int deleted_color = p->color;

  // p의 왼쪽 자식이 없는 경우
  if (p->left == t->nil){
    replacer = p->right;
    target = replacer;
    transplant(t, p, replacer);
  // p의 오른쪽 자식이 없는 경우
  }else if(p->right == t->nil){
    replacer = p->left;
    target = replacer;
    transplant(t, p, replacer);
  // 자식이 둘다 있는 경우
  }else{
    //replacer = successor를 찾고, successor를 대체할 노드를 찾는다
    replacer = return_successor(t, p);
    replacer2 = replacer->right;
    // 찾은 노드에서 추가적으로 필요한 작업
    target = replacer2;
    deleted_color = replacer->color;

    // replacer가 삭제노드의 자녀인 경우, 왼쪽 자식이 없기 때문에 그냥 올려버리면 끝
    if (replacer->parent == p){
      transplant(t, p, replacer);
      // 이식후에 왼쪽자식과의 관계를 업데이트한다
      replacer->left = p->left;
      replacer->left->parent = replacer;
      replacer->color = p->color;
      // 이건 왜하는지 모르겠지만? replacer2의 부모를 replacer로 설정한다
      replacer2->parent = replacer;
      
    // replacer가 삭제노드의 자녀 이하인 경우, replacer의 오른쪽 노드로 replacer를 대체해야 한다.
    }else{
      // replacer를 replacer2로 교체한다
      transplant(t, replacer, replacer2);
      // 이식후에 왼쪽, 오른쪽 자식과의 관계를 업데이트한다
      transplant(t, p, replacer);
      replacer->right = p->right;
      replacer->right->parent = replacer;
      replacer->left = p->left;
      replacer->left->parent = replacer;
      replacer->color = p->color;
    }
  }

  // 만약 위 과정에서 블랙 노드를 삭제했다면, 추가적인 fixup이 필요하다
  if (deleted_color == RBTREE_BLACK){
    delete_fixup(t, target);
  }
  // 할당되었던 메모리를 해제한다
  free(p);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // - `tree_to_array(tree, array, n)`
  // - RB tree의 내용을 *key 순서대로* 주어진 array로 변환 > key 오름차순 얘기하는듯? 뭔말인지 잘 모르겠음
  // - array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
  // - array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
  int i = 0;
  inorder_trav(t, t->root, arr, &i, n);
  return 0;
}

void inorder_trav(const rbtree *t, node_t *start, key_t *arr, int *ptr_i, const int n){
  if (start == t->nil || *ptr_i >= n){
    return;
  }else{
  inorder_trav(t, start->left, arr, ptr_i, n);
  arr[*ptr_i] = start->key;
  (*ptr_i)++;
  inorder_trav(t, start->right, arr, ptr_i, n);
  }
}


void insert_fixup(node_t *curr, rbtree *t){
  node_t *parent, *grandparent, *uncle;

  while (curr->parent->color == RBTREE_RED){
    parent = curr->parent;
    grandparent = parent->parent;
    // 만약 parent가 왼쪽 자식이면
    if (parent == grandparent->left){
      uncle = grandparent->right;
      // 삼촌이 레드인 경우 레드를 위로 올리고 curr = gp로 변경한다
      if (uncle->color == RBTREE_RED){
        uncle->color = RBTREE_BLACK;
        parent->color = RBTREE_BLACK;
        grandparent->color = RBTREE_RED;
        curr = grandparent;
      // 삼촌이 블랙인 경우
      }else{
        // 꺾였으면 회전처리부터 한다
        if (curr == curr->parent->right){
        curr = parent;
        rotate_dir(curr, LEFT, t);
        parent = curr->parent;
        grandparent = parent->parent;
        }
        // 펴진 상태에서 마지막 회전 처리를 한다
        parent->color = RBTREE_BLACK;
        grandparent->color = RBTREE_RED;
        rotate_dir(grandparent, RIGHT, t);
      }
    // parent가 오른쪽 자식이면
    }else{
      uncle = grandparent->left;
      if (uncle->color == RBTREE_RED){
        uncle->color = RBTREE_BLACK;
        parent->color = RBTREE_BLACK;
        grandparent->color = RBTREE_RED;
        curr = grandparent;
      }else{
        if (curr == curr->parent->left){
        curr = parent;
        rotate_dir(curr, RIGHT, t);
        parent = curr->parent;
        grandparent = parent->parent;
        }
        parent->color = RBTREE_BLACK;
        grandparent->color = RBTREE_RED;
        rotate_dir(grandparent, LEFT, t);
      }
    }
  }
  // 루트의 색을 블랙으로 변경한다
  t->root->color = RBTREE_BLACK;
}

void rotate_dir(node_t *curr, direction dir, rbtree *t){
  node_t *child;
  // 왼쪽회전
  if (dir == LEFT){
    child = curr->right;
    curr->right = child->left;
    // child->left가 nil이 아니면 부모 정보도 업데이트한다
    if (child->left != t->nil){
      child->left->parent = curr;
    }
    // child의 부모정보를 갱신한다
    child->parent = curr->parent;
    // gp가 nil인 경우, child는 루트가 된다
    if (curr->parent == t->nil){
      t->root = child;
    // p가 gp의 왼쪽 자식인 경우
    }else if (curr == curr->parent->left){
      curr->parent->left = child;
    // p가 gp의 오른쪽 자식인 경우
    }else{
      curr->parent->right = child;
    }
    // curr, child의 부자관계를 갱신한다
    child->left = curr;
    curr->parent = child;
  // 오른쪽회전
  }else{
    child = curr->left;
    curr->left = child->right;
    if (child->right != t->nil){
      child->right->parent = curr;
    }
    child->parent = curr->parent;
    if (curr->parent == t->nil){
      t->root = child;
    }else if (curr == curr->parent->left){
      curr->parent->left = child;
    }else{
      curr->parent->right = child;
    }
    child->right = curr;
    curr->parent = child;
  }
}

// 특정 노드를 다른 노드로 대체하면서, 부모-자식 관계를 갱신하는 함수
void transplant(rbtree *t, node_t *pre, node_t *post){
  // 교체 대상 노드가 루트인 경우, post를 루트로 지정한다
  if (pre->parent == t->nil){
    t->root = post;
  // 교체 대상 노드 부모의 자식 정보를 업데이트 한다
  } else if (pre->parent->left == pre){
    pre->parent->left = post;
  } else{
    pre->parent->right = post;
  }
  // post의 부모까지 업데이트한다
  post->parent = pre->parent;
}

node_t *return_successor(rbtree *t, node_t *p){
  if (p->right == t->nil){
    while (p->parent != t->nil){
      if (p->parent->left == p){
        return p->parent; 
      }
      p = p->parent;
    }
    return NULL;
  }
  else{
    node_t *successor = p->right;
    while (successor->left != t->nil){
      successor = successor->left;
    }
    return successor;
  }
}

void delete_fixup(rbtree *t, node_t *target){
  // target이 root거나 레드가 될 때까지 반복한다. 이유는 앞의 두 케이스는 삭제된 블랙을 복구하는게 매우 단순해짐.
  while (target != t->root && target->color == RBTREE_BLACK) {
    // 형제(=sibling) 및 그 자식들을 정의한다. 체크할 때 위치 정보도 같이 확인해 놓아야 한다.
    node_t *sibling, *inner, *outer;
    // Fix up, 타겟 왼쪽
    if (target->parent->left == target){
      sibling = target->parent->right;
      // CASE 1. 형제가 레드인 경우, RBT 속성을 유지하면서 타겟의 형제를 블랙으로 바꾸기 위한 전처리 작업을 한다
      if (sibling->color == RBTREE_RED){
        target->parent->color = RBTREE_RED;
        sibling->color = RBTREE_BLACK;
        rotate_dir(sibling->parent, LEFT, t);
        sibling = target->parent->right;
      }
      // CASE 2. CASE1에 의해 형제는 블랙. 이 때 형제의 자식이 모두 블랙인 경우 형제를 레드로 바꾸고 타겟을 부모로 올린다
      // 왜 체크함? 블랙을 하나 지우면서 전체의 black-height가 1 낮아졌기 때문에 부모에서 fix-up을 추가로 진행해야된다
      // 왜 이렇게 함? 궁극적으로 이렇게 올라가다보면 루트를 만나고, 루트는 더이상 fix-up을 진행하지 않아도 되기 때문이다
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
        sibling->color = RBTREE_RED;
        target->color = RBTREE_BLACK;
        target = target->parent;
      }else{
        // inner, outer 정의
        inner = sibling->left;
        outer = sibling->right;
        // CASE 3. 형제의 inner child가 RED이고 outer child BLACK인 경우
        // 적절하게 회전연산을 수행하여 CASE 4로 만든다 
        if (inner->color == RBTREE_RED && outer->color == RBTREE_BLACK){
          sibling->color = RBTREE_RED;
          inner->color = RBTREE_BLACK;
          rotate_dir(inner->parent, RIGHT, t);
          // 새로 형제 노드 정의
          sibling = target->parent->right;
          inner = sibling->left;
          outer = sibling->right;
        }
        // CASE 4. 형제의 outer child가 RED인 경우
        // 부모와 형제의 색을 변경하고, 돌린다
        if (outer->color == RBTREE_RED){
          sibling->color = sibling->parent->color;
          sibling->parent->color = RBTREE_BLACK;
          outer->color = RBTREE_BLACK;
          rotate_dir(sibling->parent, LEFT, t);
          target = t->root;
        }
      }
    // Fix up, 타겟 오른쪽(왼쪽과 l,r만 반대로 쓴다)
    }else{
      sibling = target->parent->left;
      if (sibling->color == RBTREE_RED){
        target->parent->color = RBTREE_RED;
        sibling->color = RBTREE_BLACK;
        rotate_dir(sibling->parent, RIGHT, t);
        sibling = target->parent->left;
      }
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK){
        sibling->color = RBTREE_RED;
        target->color = RBTREE_BLACK;
        target = target->parent;
      }else{
        inner = sibling->right;
        outer = sibling->left;
        if (inner->color == RBTREE_RED && outer->color == RBTREE_BLACK){
          sibling->color = RBTREE_RED;
          inner->color = RBTREE_BLACK;
          rotate_dir(inner->parent, LEFT, t);
          sibling = target->parent->left;
          inner = sibling->right;
          outer = sibling->left;
        }
        if (outer->color == RBTREE_RED){
          sibling->color = sibling->parent->color;
          sibling->parent->color = RBTREE_BLACK;
          outer->color = RBTREE_BLACK;
          rotate_dir(sibling->parent, RIGHT, t);
          target = t->root;
        }
      }
    }
  }
  // 종료전 target color를 black으로 변경해준다
  target->color = RBTREE_BLACK;
  return;
}

// }