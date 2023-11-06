// _RBTREE_H_ 매크로가 정의되지 않았을 때 아래를 실행한다. 이유는 헤더파일을 중복해서 컴파일하는 것을 방지하기 위해서이다.
#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

// 필요에 따라 각각 color_t, key_t를 정의하고 접근을 쉽게하기 위해 별칭으로 뺀다.
typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;
typedef int key_t;

// 구조체 node_t를 선언한다. 멤버로 색, 키, 연결된 노드의 포인터들을 가진다
typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

// 구조체 rbtree를 선언한다.
// root 포인터는 rbtree 전체를 순회하기 위해 필요하다.
// nil 포인터는 하나만 선언한다. 개념적으로 nil노드는 여러개이지만, 어차피 같은 속성이므로 하나만 선언해두고 다 여기를 가리키게 한다.
typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

// rbtree를 반환하는, new_rbtree 함수를 선언한다. 인자는 받지 않는다.
rbtree *new_rbtree(void);
// delete_rbtree 함수를 선언한다. rbtree 포인터를 인자로 준다.
// 왜 포인터를 줄까? 함수에 주는 인자는 '복사본'이라서 실제 값을 변경할 수 없는데, '복사본 주소'를 통해 실제 값을 가리킬 수 있기 때문이다.
void delete_rbtree(rbtree *);

// 인자를 건드리지 않아야 하는 것들은 다 const로 주어져있다(ex. 검색연산)
// 삽입에서는 rbtree.root가 변할 수 있어서 const로 안줬을거임
node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);

// 
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
