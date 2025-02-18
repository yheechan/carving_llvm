#include "utils/data_utils.hpp"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

///////////////////
// POINTER
///////////////////

POINTER::POINTER() : addr(0), pointee_type(0), alloc_size(0) {}

POINTER::POINTER(void *_addr, int _size)
    : addr(_addr), pointee_type(0), alloc_size(_size) {}

POINTER::POINTER(void *_addr, const char *pointee_type, int _size)
    : addr(_addr), pointee_type(pointee_type), alloc_size(_size) {}

bool POINTER::operator==(const POINTER &other) const {
  return (addr == other.addr) && (alloc_size == other.alloc_size);
}

///////////////////
// IVAR
///////////////////

IVAR::IVAR() : type(INPUT_TYPE::CHAR), name(0) {}
IVAR::IVAR(char *name, enum INPUT_TYPE type) : name(name), type(type) {}
IVAR::~IVAR() {  // free(name);
}

///////////////////
// VAR
///////////////////

template <class input_type>
VAR<input_type>::VAR() : input(0), pointer_offset(0), IVAR() {}

template <class input_type>
VAR<input_type>::VAR(input_type _input, char *_name, enum INPUT_TYPE _type)
    : input(_input), IVAR(_name, _type) {}

template <class input_type>
VAR<input_type>::VAR(input_type pointer_index, char *_name, int _pointer_offset,
                     enum INPUT_TYPE _type)
    : input(pointer_index),
      IVAR(_name, _type),
      pointer_offset(_pointer_offset) {}

// copy constructor
template <class input_type>
VAR<input_type>::VAR(const VAR<input_type> &other)
    : input(other.input),
      IVAR(strdup(other.name), other.type),
      pointer_offset(other.pointer_offset) {}

// move constructor
template <class input_type>
VAR<input_type>::VAR(VAR<input_type> &&other)
    : input(other.input),
      IVAR(other.name, other.type),
      pointer_offset(other.pointer_offset) {}

template <class input_type>
VAR<input_type> &VAR<input_type>::operator=(const VAR<input_type> &other) {
  input = other.input;
  name = strdup(other.name);
  type = other.type;
  pointer_offset = other.pointer_offset;
  return *this;
}

template <class input_type>
VAR<input_type> &VAR<input_type>::operator=(VAR<input_type> &&other) {
  input = other.input;
  name = other.name;
  type = other.type;
  pointer_offset = other.pointer_offset;
  other.name = 0;
  return *this;
}

template <class elem_type>
vector<elem_type>::vector()
    : capacity(128), num_elem(0), data(new elem_type[128]) {}

template <class elem_type>
vector<elem_type>::vector(const vector<elem_type> &other)
    : capacity(other.capacity),
      num_elem(other.num_elem),
      data(new elem_type[other.capacity]) {
  for (int i = 0; i < num_elem; i++) {
    data[i] = other.data[i];
  }
}

template <class elem_type>
vector<elem_type>::vector(vector<elem_type> &&other)
    : capacity(other.capacity), num_elem(other.num_elem), data(other.data) {
  other.data = 0;
}

template <class elem_type>
vector<elem_type> &vector<elem_type>::operator=(
    const vector<elem_type> &other) {
  if (this != &other) {
    delete[] data;
    capacity = other.capacity;
    num_elem = other.num_elem;
    data = new elem_type[capacity];
    for (int i = 0; i < num_elem; i++) {
      data[i] = other.data[i];
    }
  }
  return *this;
}

template <class elem_type>
vector<elem_type> &vector<elem_type>::operator=(vector<elem_type> &&other) {
  if (this != &other) {
    delete[] data;
    capacity = other.capacity;
    num_elem = other.num_elem;
    data = other.data;
    other.data = 0;
  }
  return *this;
}

template <class elem_type>
void vector<elem_type>::increase_capacity() {
  capacity *= 2;

  elem_type *tmp = new elem_type[capacity];
  int idx;
  for (idx = 0; idx < num_elem; idx++) {
    tmp[idx] = data[idx];
  }

  delete[] data;
  data = tmp;
}

template <class elem_type>
void vector<elem_type>::push_back(elem_type elem) {
  data[num_elem] = elem;
  num_elem++;
  if (num_elem >= capacity) {
    increase_capacity();
  }
}

template <class elem_type>
void vector<elem_type>::insert(int idx, elem_type elem) {
  if (idx == num_elem) {
    push_back(elem);
  } else if ((idx < num_elem) && (idx >= 0)) {
    if ((num_elem + 1) >= capacity) {
      increase_capacity();
    }

    for (int i = num_elem; i > idx; i--) {
      data[i] = data[i - 1];
    }
    data[idx] = elem;
  } else {
    return;
  }
}

template <class elem_type>
void vector<elem_type>::pop_back() {
  if (num_elem == 0) return;
  num_elem--;
  return;
}

template <class elem_type>
elem_type *vector<elem_type>::operator[](int idx) {
  if (idx >= num_elem) {
    return 0;
  }
  return &(data[idx]);
}

template <class elem_type>
elem_type *vector<elem_type>::get(int idx) {
  if (idx >= num_elem) {
    return 0;
  }
  return &(data[idx]);
}

template <class elem_type>
int vector<elem_type>::size() {
  return num_elem;
}

template <class elem_type>
void vector<elem_type>::remove(int idx) {
  if (idx >= num_elem) {
    return;
  }

  while (idx < num_elem - 1) {
    data[idx] = data[idx + 1];
    idx++;
  }

  num_elem--;
}

template <class elem_type>
elem_type *vector<elem_type>::back() {
  if (num_elem == 0) return NULL;
  return &(data[num_elem - 1]);
}

template <class elem_type>
void vector<elem_type>::clear() {
  num_elem = 0;
}

template <class elem_type>
vector<elem_type>::~vector() {
  delete[] data;
}

template <class Key, class Elem>
map<Key, Elem>::data_node::data_node()
    : key(0), elem(0), left(0), right(0), parent(0) {}

template <class Key, class Elem>
map<Key, Elem>::data_node::data_node(Key _key, Elem _elem, data_node *_left,
                                     data_node *_right, data_node *_parent) {
  key = _key;
  elem = _elem;
  left = _left;
  right = _right;
  parent = _parent;
}

template <class Key, class Elem>
map<Key, Elem>::data_node::data_node(const data_node &other) {
  key = other.key;
  elem = other.elem;
  left = other.left;
  right = other.right;
  parent = other.parent;
}

template <class Key, class Elem>
map<Key, Elem>::data_node::data_node(data_node &&other) {
  key = other.key;
  elem = other.elem;
  left = other.left;
  right = other.right;
  parent = other.parent;
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node &map<Key, Elem>::data_node::operator=(
    const data_node &other) {
  key = other.key;
  elem = other.elem;
  left = other.left;
  right = other.right;
  parent = other.parent;
  return *this;
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node &map<Key, Elem>::data_node::operator=(
    data_node &&other) {
  key = other.key;
  elem = other.elem;
  left = other.left;
  right = other.right;
  parent = other.parent;
  return *this;
}

template <class Key, class Elem>
void map<Key, Elem>::data_node::insert_right(data_node *node) {
  if (node == NULL) {
    return;
  }
  if (right == NULL) {
    right = node;
    node->parent = this;
    return;
  }

  data_node *ptr = right;
  while (ptr->right != NULL) {
    ptr = ptr->right;
  }
  ptr->right = node;
  node->parent = ptr;
  return;
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node *map<Key, Elem>::data_node::find_node(
    Key _key, data_node **parent_ptr) {
  data_node *search_node = this;

  while (search_node->key != _key) {
    if (search_node->key < _key) {
      if (search_node->right == NULL) {
        if (parent_ptr != NULL) {
          *parent_ptr = search_node;
        }
        return NULL;
      }
      search_node = search_node->right;
    } else {
      if (search_node->left == NULL) {
        if (parent_ptr != NULL) {
          *parent_ptr = search_node;
        }
        return NULL;
      }
      search_node = search_node->left;
    }
  }

  if (parent_ptr != NULL) {
    *parent_ptr = search_node;
  }
  return search_node;
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node *
map<Key, Elem>::data_node::find_small_closest_node(Key _key) {
  data_node *search_node = this;
  data_node *closest = NULL;

  while (search_node->key != _key) {
    if (search_node->key < _key) {
      closest = search_node;
      if (search_node->right == NULL) {
        return closest;
      }
      search_node = search_node->right;
    } else {
      if (search_node->left == NULL) {
        return closest;
      }
      search_node = search_node->left;
    }
  }
  return search_node;
}

template <class Key, class Elem>
map<Key, Elem>::map() {
  root = NULL;
  capacity = 128;
  num_nodes = 0;
  nodes = new data_node[capacity];
}

template <class Key, class Elem>
map<Key, Elem>::~map() {
  delete[] nodes;
}

template <class Key, class Elem>
Elem *map<Key, Elem>::find(Key key) {
  if (root == NULL) {
    return NULL;
  }
  data_node *search_node = root->find_node(key, NULL);
  if (search_node == NULL) {
    return NULL;
  }
  return &(search_node->elem);
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node *map<Key, Elem>::find_small_closest(
    Key key) {
  if (root == NULL) {
    return NULL;
  }
  data_node *search_node = root->find_small_closest_node(key);
  if (search_node == NULL) {
    return NULL;
  }
  return search_node;
}

template <class Key, class Elem>
typename map<Key, Elem>::data_node *map<Key, Elem>::get_by_idx(int idx) {
  if (idx >= num_nodes) {
    return NULL;
  }
  return &(nodes[idx]);
}

template <class Key, class Elem>
void map<Key, Elem>::insert(Key key, Elem elem) {
  if (root == NULL) {
    root = nodes;
    nodes[0] = data_node(key, elem, NULL, NULL, NULL);
    num_nodes++;
    return;
  }

  data_node *parent_node = root;
  data_node *key_node = root->find_node(key, &parent_node);
  if (key_node == NULL) {
    data_node *new_node = nodes + num_nodes;
    *new_node = data_node(key, elem, NULL, NULL, parent_node);
    num_nodes++;
    if (key < parent_node->key) {
      parent_node->left = new_node;
    } else {
      parent_node->right = new_node;
    }

    if (num_nodes == capacity) {
      capacity *= 2;
      data_node *new_nodes = new data_node[capacity];

      if (new_nodes > nodes) {
        unsigned long ptr_diff = (char *)new_nodes - (char *)nodes;
        for (int i = 0; i < num_nodes; i++) {
          new_nodes[i] = nodes[i];
          if (nodes[i].left != NULL) {
            new_nodes[i].left =
                (data_node *)((char *)new_nodes[i].left + ptr_diff);
          }
          if (nodes[i].right != NULL) {
            new_nodes[i].right =
                (data_node *)((char *)new_nodes[i].right + ptr_diff);
          }
          if (nodes[i].parent != NULL) {
            new_nodes[i].parent =
                (data_node *)((char *)new_nodes[i].parent + ptr_diff);
          }
        }
      } else {
        unsigned long ptr_diff = (char *)nodes - (char *)new_nodes;

        for (int i = 0; i < num_nodes; i++) {
          new_nodes[i] = nodes[i];
          if (nodes[i].left != NULL) {
            new_nodes[i].left =
                (data_node *)((char *)new_nodes[i].left - ptr_diff);
          }
          if (nodes[i].right != NULL) {
            new_nodes[i].right =
                (data_node *)((char *)new_nodes[i].right - ptr_diff);
          }
          if (nodes[i].parent != NULL) {
            new_nodes[i].parent =
                (data_node *)((char *)new_nodes[i].parent - ptr_diff);
          }
        }
      }

      delete[] nodes;
      nodes = new_nodes;
      root = nodes;
    }
  } else {
    key_node->elem = elem;
  }
  return;
}

template <class Key, class Elem>
void map<Key, Elem>::move_or_remove_last(data_node *node) {
  if (node == nodes + num_nodes - 1) {
    num_nodes--;
    return;
  }

  data_node *last_node = nodes + num_nodes - 1;
  data_node *parent_node = last_node->parent;
  if (parent_node->left == last_node) {
    parent_node->left = node;
  } else {
    parent_node->right = node;
  }

  if (last_node->left != NULL) {
    last_node->left->parent = node;
  }
  if (last_node->right != NULL) {
    last_node->right->parent = node;
  }

  node->key = last_node->key;
  node->elem = last_node->elem;
  node->parent = parent_node;
  node->left = last_node->left;
  node->right = last_node->right;

  num_nodes--;
}

template <class Key, class Elem>
void map<Key, Elem>::remove(Key key) {
  if (root == NULL) {
    return;
  }

  if (root->key == key) {
    if (root->left != NULL) {
      data_node *to_be_root = root->left;
      data_node *root_right = root->right;
      root->key = to_be_root->key;
      root->elem = to_be_root->elem;
      root->left = to_be_root->left;
      root->right = to_be_root->right;

      if (root->left != NULL) {
        root->left->parent = root;
      }
      if (root->right != NULL) {
        root->right->parent = root;
      }

      if (root_right != NULL) {
        if (root->right == NULL) {
          root->right = root_right;
          root_right->parent = root;
        } else {
          root->right->insert_right(root_right);
        }
      }

      move_or_remove_last(to_be_root);
    } else {
      if (root->right == NULL) {
        root = NULL;
        num_nodes = 0;
      } else {
        data_node *to_be_root = root->right;
        root->key = to_be_root->key;
        root->elem = to_be_root->elem;
        root->left = to_be_root->left;
        root->right = to_be_root->right;

        if (root->left != NULL) {
          root->left->parent = root;
        }
        if (root->right != NULL) {
          root->right->parent = root;
        }

        move_or_remove_last(to_be_root);
      }
    }

    return;
  }

  data_node *key_node = root->find_node(key, NULL);

  if (key_node == NULL) {
    return;
  }

  if (key_node->left != NULL) {
    if (key_node->parent->left == key_node) {
      key_node->parent->left = key_node->left;
    } else {
      key_node->parent->right = key_node->left;
    }
    key_node->left->parent = key_node->parent;
    key_node->left->insert_right(key_node->right);
  } else if (key_node->right != NULL) {
    key_node->right->parent = key_node->parent;
    if (key_node->parent->left == key_node) {
      key_node->parent->left = key_node->right;
    } else {
      key_node->parent->right = key_node->right;
    }
  } else {
    if (key_node->parent->left == key_node) {
      key_node->parent->left = NULL;
    } else {
      key_node->parent->right = NULL;
    }
  }

  move_or_remove_last(key_node);
  return;
}

template <class Key, class Elem>
int map<Key, Elem>::size() {
  return num_nodes;
}

template <class Key, class Elem>
void map<Key, Elem>::clear() {
  root = NULL;
  capacity = 128;
  num_nodes = 0;
  delete[] nodes;
  nodes = new data_node[capacity];
}

FUNC_CONTEXT::FUNC_CONTEXT()
    : carved_ptr_begin_idx(0),
      carving_index(0),
      func_call_idx(0),
      func_id(0),
      is_carved(false) {}

FUNC_CONTEXT::FUNC_CONTEXT(int _carved_idx, int _func_call_idx, int _func_id)
    : carving_index(_carved_idx),
      func_call_idx(_func_call_idx),
      carved_ptr_begin_idx(0),
      inputs(),
      carved_ptrs(),
      func_id(_func_id),
      is_carved(true) {}

FUNC_CONTEXT::FUNC_CONTEXT(const FUNC_CONTEXT &other)
    : carving_index(other.carving_index),
      func_call_idx(other.func_call_idx),
      carved_ptr_begin_idx(other.carved_ptr_begin_idx),
      inputs(other.inputs),
      carved_ptrs(other.carved_ptrs),
      func_id(other.func_id),
      is_carved(other.is_carved) {}

FUNC_CONTEXT::FUNC_CONTEXT(FUNC_CONTEXT &&other)
    : carving_index(other.carving_index),
      func_call_idx(other.func_call_idx),
      carved_ptr_begin_idx(other.carved_ptr_begin_idx),
      inputs(other.inputs),
      carved_ptrs(other.carved_ptrs),
      func_id(other.func_id),
      is_carved(other.is_carved) {}

FUNC_CONTEXT &FUNC_CONTEXT::operator=(const FUNC_CONTEXT &other) {
  carving_index = other.carving_index;
  func_call_idx = other.func_call_idx;
  carved_ptr_begin_idx = other.carved_ptr_begin_idx;
  inputs = other.inputs;
  carved_ptrs = other.carved_ptrs;
  func_id = other.func_id;
  is_carved = other.is_carved;
  return *this;
}

FUNC_CONTEXT &FUNC_CONTEXT::operator=(FUNC_CONTEXT &&other) {
  carving_index = other.carving_index;
  func_call_idx = other.func_call_idx;
  carved_ptr_begin_idx = other.carved_ptr_begin_idx;
  inputs = other.inputs;
  carved_ptrs = other.carved_ptrs;
  func_id = other.func_id;
  is_carved = other.is_carved;
  return *this;
}

void FUNC_CONTEXT::update_carved_ptr_begin_idx() {
  carved_ptr_begin_idx = carved_ptrs.size();
}

typeinfo::typeinfo(char *type_name, int size) {
  this->type_name = type_name;
  this->size = size;
}

typeinfo::typeinfo() {
  this->type_name = NULL;
  this->size = 0;
}

typeinfo::typeinfo(int) {
  type_name = NULL;
  size = 0;
}

typeinfo::typeinfo(const typeinfo &other) {
  type_name = other.type_name;
  size = other.size;
}

typeinfo::typeinfo(typeinfo &&other) {
  type_name = other.type_name;
  size = other.size;
  other.type_name = NULL;
  other.size = 0;
}

typeinfo &typeinfo::operator=(const typeinfo &other) {
  type_name = other.type_name;
  size = other.size;
  return *this;
}

typeinfo &typeinfo::operator=(typeinfo &&other) {
  type_name = other.type_name;
  size = other.size;
  other.type_name = NULL;
  other.size = 0;
  return *this;
}

classinfo::classinfo(int class_index, int size) {
  this->class_index = class_index;
  this->size = size;
}

classinfo::classinfo() {
  this->class_index = 0;
  this->size = 0;
}

classinfo::classinfo(int) {
  class_index = 0;
  size = 0;
}

classinfo::classinfo(const classinfo &other) {
  class_index = other.class_index;
  size = other.size;
}

classinfo::classinfo(classinfo &&other) {
  class_index = other.class_index;
  size = other.size;
  other.class_index = 0;
  other.size = 0;
}

classinfo &classinfo::operator=(const classinfo &other) {
  class_index = other.class_index;
  size = other.size;
  return *this;
}

classinfo &classinfo::operator=(classinfo &&other) {
  class_index = other.class_index;
  size = other.size;
  other.class_index = 0;
  other.size = 0;
  return *this;
}

template class VAR<char>;
template class VAR<short>;
template class VAR<int>;
template class VAR<long>;
template class VAR<long long>;
template class VAR<float>;
template class VAR<double>;
template class VAR<long double>;
template class VAR<void *>;
template class VAR<char *>;

template class vector<int>;
template class vector<IVAR *>;
template class vector<POINTER>;
template class vector<char *>;
template class vector<FUNC_CONTEXT>;
template class vector<bool>;

template class map<void *, int>;
template class map<void *, char>;
template class map<char *, classinfo>;
template class map<void *, char *>;
template class map<void *, typeinfo>;
template class map<char const *, unsigned int>;
template class map<char *, char *>;
template class map<char *, unsigned int>;
template class map<int, char>;