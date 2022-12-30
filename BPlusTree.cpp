#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <vector>
#include <random>

using namespace std;

class Node
{

public:
  Node(Node *parent = nullptr, bool isLeaf = false, Node *prev_ = nullptr,
       Node *next_ = nullptr)
      : parent(parent), isLeaf(isLeaf), prev(prev_), next(next_)
  {
    if (next_)
    {
      next_->prev = this;
    }

    if (prev_)
    {
      prev_->next = this;
    }
  }

  vector<int> keys;
  Node *parent;

  vector<Node *> children;
  vector<int> values;
  Node *next;
  Node *prev;

  bool isLeaf;

  int indexOfChild(int key)
  {
    for (int i = 0; i < keys.size(); i++)
    {
      if (key < keys[i])
      {
        return i;
      }
    }
    return keys.size();
  }

  int indexOfKey(int key)
  {
    for (int i = 0; i < keys.size(); i++)
    {
      if (key == keys[i])
      {
        return i;
      }
    }
    return -1;
  }

  Node *getChild(int key) { return children[indexOfChild(key)]; }

  void setChild(int key, vector<Node *> value)
  {
    int i = indexOfChild(key);
    keys.insert(keys.begin() + i, key);
    children.erase(children.begin() + i);
    children.insert(children.begin() + i, value.begin(), value.end());
  }

  tuple<int, Node *, Node *> splitInternal()
  {

    Node *left = new Node(parent, false, nullptr, nullptr);
    int mid = keys.size() / 2;

    // left->keys = vector<int>(keys.begin(), keys.begin() + mid);
    copy(keys.begin(), keys.begin() + mid, back_inserter(left->keys));
    // left->children = vector<Node*>(children.begin(), children.begin() + mid +
    // 1);
    copy(children.begin(), children.begin() + mid + 1,
         back_inserter(left->children));

    for (Node *child : left->children)
    {
      child->parent = left;
    }

    int key = keys[mid];
    keys.erase(keys.begin(), keys.begin() + mid + 1);
    children.erase(children.begin(), children.begin() + mid + 1);

    return make_tuple(key, left, this);
  }

  int get(int key)
  {
    int index = -1;
    for (int i = 0; i < keys.size(); ++i)
    {
      if (keys[i] == key)
      {
        index = i;
        break;
      }
    }

    if (index == -1)
    {
      cout << "key " << key << " not found" << endl;
    }

    return values[index];
  }

  void set(int key, int value)
  {
    int i = indexOfChild(key);
    if (find(keys.begin(), keys.end(), key) == keys.end())
    {
      keys.insert(keys.begin() + i, key);
      values.insert(values.begin() + i, value);
    }
    else
    {
      values[i - 1] = value;
    }
  }

  tuple<int, Node *, Node *> splitLeaf()
  {

    Node *left = new Node(parent, true, prev, this);
    int mid = keys.size() / 2;

    left->keys = vector<int>(keys.begin(), keys.begin() + mid);
    left->values = vector<int>(values.begin(), values.begin() + mid);

    keys.erase(keys.begin(), keys.begin() + mid);
    values.erase(values.begin(), values.begin() + mid);

    return make_tuple(keys[0], left, this);
  }
};

class BPlusTree
{
public:
  BPlusTree(int _maxCapacity = 4)
  {
    root = new Node(nullptr, true, nullptr, nullptr);
    maxCapacity = _maxCapacity > 2 ? _maxCapacity : 2;
    minCapacity = maxCapacity / 2;
    depth = 0;
  }

  Node *root;
  int maxCapacity;
  int minCapacity;
  int depth;

  Node *findLeaf(int key)
  {
    Node *node = root;
    while (!node->isLeaf)
    {
      node = node->getChild(key);
    }
    return node;
  }

  int get(int key) { return findLeaf(key)->get(key); }

  void set(int key, int value)
  {
    Node *leaf = findLeaf(key);
    leaf->set(key, value);
    if (leaf->keys.size() > maxCapacity)
    {
      insert(leaf->splitLeaf());
    }
  }

  void insert(tuple<int, Node *, Node *> result)
  {
    int key = std::get<0>(result);
    Node *left = std::get<1>(result);
    Node *right = std::get<2>(result);
    Node *parent = right->parent;
    if (parent == nullptr)
    {
      left->parent = right->parent = root =
          new Node(nullptr, false, nullptr, nullptr);
      depth += 1;
      root->keys = {key};
      root->children = {left, right};
      return;
    }
    parent->setChild(key, {left, right});
    if (parent->keys.size() > maxCapacity)
    {
      insert(parent->splitInternal());
    }
  }

  void removeFromLeaf(int key, Node *node)
  {
    int index = node->indexOfKey(key);
    if (index == -1)
    {
      cout << "Key " << key << " not found! Exiting ..." << endl;
      exit(0);
    }
    node->keys.erase(node->keys.begin() + index);
    node->values.erase(node->values.begin() + index);
    if (node->parent)
    {
      int indexInParent = node->parent->indexOfChild(key);
      if (indexInParent)
        node->parent->keys[indexInParent - 1] = node->keys.front();
    }
  }

  void removeFromInternal(int key, Node *node)
  {
    int index = node->indexOfKey(key);
    if (index != -1)
    {
      Node *leftMostLeaf = node->children[index + 1];
      while (!leftMostLeaf->isLeaf)
        leftMostLeaf = leftMostLeaf->children.front();

      node->keys[index] = leftMostLeaf->keys.front();
    }
  }

  void borrowKeyFromRightLeaf(Node *node, Node *next)
  {
    node->keys.push_back(next->keys.front());
    next->keys.erase(next->keys.begin());
    node->values.push_back(next->values.front());
    next->values.erase(next->values.begin());
    for (int i = 0; i < node->parent->children.size(); i++)
    {
      if (node->parent->children[i] == next)
      {
        node->parent->keys[i - 1] = next->keys.front();
        break;
      }
    }
  }

  void borrowKeyFromLeftLeaf(Node *node, Node *prev)
  {
    node->keys.insert(node->keys.begin(), prev->keys.back());
    prev->keys.erase(prev->keys.end() - 1);
    node->values.insert(node->values.begin(), prev->values.back());
    prev->values.erase(prev->values.end() - 1);
    for (int i = 0; i < node->parent->children.size(); i++)
    {
      if (node->parent->children[i] == node)
      {
        node->parent->keys[i - 1] = node->keys.front();
        break;
      }
    }
  }

  void mergeNodeWithRightLeaf(Node *node, Node *next)
  {
    node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
    node->values.insert(node->values.end(), next->values.begin(),
                        next->values.end());
    node->next = next->next;
    if (node->next)
      node->next->prev = node;
    for (int i = 0; i < next->parent->children.size(); i++)
    {
      if (node->parent->children[i] == next)
      {
        node->parent->keys.erase(node->parent->keys.begin() + i - 1);
        node->parent->children.erase(node->parent->children.begin() + i);

        break;
      }
    }
  }

  void mergeNodeWithLeftLeaf(Node *node, Node *prev)
  {
    prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
    prev->values.insert(prev->values.end(), node->values.begin(),
                        node->values.end());

    prev->next = node->next;
    if (prev->next)
      prev->next->prev = prev;

    for (int i = 0; i < node->parent->children.size(); i++)
    {
      if (node->parent->children[i] == node)
      {
        node->parent->keys.erase(node->parent->keys.begin() + i - 1);
        node->parent->children.erase(node->parent->children.begin() + i);
        break;
      }
    }
  }

  void borrowKeyFromRightInternal(int myPositionInParent, Node *node,
                                  Node *next)
  {
    node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
    node->parent->keys[myPositionInParent] = next->keys.front();
    next->keys.erase(next->keys.begin());
    node->children.insert(node->children.end(), next->children.front());
    next->children.erase(next->children.begin());
    node->children.back()->parent = node;
  }

  void borrowKeyFromLeftInternal(int myPositionInParent, Node *node,
                                 Node *prev)
  {
    node->keys.insert(node->keys.begin(),
                      node->parent->keys[myPositionInParent - 1]);
    node->parent->keys[myPositionInParent - 1] = prev->keys.back();
    prev->keys.erase(prev->keys.end() - 1);
    node->children.insert(node->children.begin(), prev->children.back());
    prev->children.erase(prev->children.end() - 1);
    node->children.front()->parent = node;
  }

  void mergeNodeWithRightInternal(int myPositionInParent, Node *node,
                                  Node *next)
  {
    node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
    node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent);
    node->parent->children.erase(node->parent->children.begin() +
                                 myPositionInParent + 1);
    node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
    node->children.insert(node->children.end(), next->children.begin(),
                          next->children.end());
    for (Node *child : node->children)
    {
      child->parent = node;
    }
  }

  void mergeNodeWithLeftInternal(int myPositionInParent, Node *node,
                                 Node *prev)
  {
    prev->keys.insert(prev->keys.end(),
                      node->parent->keys[myPositionInParent - 1]);
    node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent -
                             1);
    node->parent->children.erase(node->parent->children.begin() +
                                 myPositionInParent);
    prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
    prev->children.insert(prev->children.end(), node->children.begin(),
                          node->children.end());
    for (Node *child : prev->children)
    {
      child->parent = prev;
    }
  }

  void remove(int key, Node *node = nullptr)
  {
    if (node == nullptr)
    {
      node = findLeaf(key);
    }
    if (node->isLeaf)
    {
      removeFromLeaf(key, node);
    }
    else
    {
      removeFromInternal(key, node);
    }

    if (node->keys.size() < minCapacity)
    {

      if (node == root)
      {
        if (root->keys.empty() && !root->children.empty())
        {
          root = root->children[0];
          root->parent = nullptr;
          depth -= 1;
        }
        return;
      }

      else if (node->isLeaf)
      {
        Node *next = node->next;
        Node *prev = node->prev;

        if (next && next->parent == node->parent &&
            next->keys.size() > minCapacity)
        {
          borrowKeyFromRightLeaf(node, next);
        }
        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() > minCapacity)
        {
          borrowKeyFromLeftLeaf(node, prev);
        }
        else if (next && next->parent == node->parent &&
                 next->keys.size() <= minCapacity)
        {
          mergeNodeWithRightLeaf(node, next);
        }
        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() <= minCapacity)
        {
          mergeNodeWithLeftLeaf(node, prev);
        }
      }
      else
      {

        int myPositionInParent = -1;

        for (int i = 0; i < node->parent->children.size(); i++)
        {
          if (node->parent->children[i] == node)
          {
            myPositionInParent = i;
            break;
          }
        }

        Node *next;
        Node *prev;

        if (node->parent->children.size() > myPositionInParent + 1)
        {
          next = node->parent->children[myPositionInParent + 1];
        }
        else
        {
          next = nullptr;
        }

        if (myPositionInParent)
        {
          prev = node->parent->children[myPositionInParent - 1];
        }
        else
        {
          prev = nullptr;
        }

        if (next && next->parent == node->parent &&
            next->keys.size() > minCapacity)
        {
          borrowKeyFromRightInternal(myPositionInParent, node, next);
        }

        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() > minCapacity)
        {
          borrowKeyFromLeftInternal(myPositionInParent, node, prev);
        }

        else if (next && next->parent == node->parent &&
                 next->keys.size() <= minCapacity)
        {
          mergeNodeWithRightInternal(myPositionInParent, node, next);
        }

        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() <= minCapacity)
        {
          mergeNodeWithLeftInternal(myPositionInParent, node, prev);
        }
      }
    }
    if (node->parent)
    {
      remove(key, node->parent);
    }
  }

  void print(Node *node = nullptr, string _prefix = "", bool _last = true)
  {
    if (!node)
      node = root;
    cout << _prefix << "├ [";
    for (int i = 0; i < node->keys.size(); i++)
    {
      cout << node->keys[i];
      if (i != node->keys.size() - 1)
      {
        cout << ", ";
      }
    }
    cout << "]" << endl;

    _prefix += _last ? "   " : "╎  ";

    if (!node->isLeaf)
    {
      for (int i = 0; i < node->children.size(); i++)
      {
        bool _last = (i == node->children.size() - 1);
        print(node->children[i], _prefix, _last);
      }
    }
  }
};

int main()
{

  BPlusTree tree(3);
  vector<int> random_list = {5, 9, 1, 3, 4, 59, 65, 45,
                             89, 29, 68, 108, 165, 298, 219, 569,
                             37, 47, 53, 6, 43, 51, 57, 49};

  vector<int> random_list_2 =
      vector<int>(random_list.begin(), random_list.end());

  for (int r : random_list_2)
  {

    for (int i : random_list)
    {
      cout << endl
           << "-------------" << endl;
      cout << "Inserting " << i << endl
           << endl;
      cout << "-------------" << endl
           << endl;

      tree.set(i, i);
      tree.print();
    }

    cout << endl
         << "-------------------------" << endl;
    cout << "All keys are inserted ..." << endl;
    cout << "-------------------------" << endl
         << endl;

    tree.print();

    shuffle(random_list.begin(), random_list.end(), default_random_engine(r));
    for (int i : random_list)
    {
      tree.remove(i);
      cout << endl
           << "-------------" << endl;
      cout << "Removing " << i << endl;
      cout << "-------------" << endl
           << endl;
      tree.print();
    }
  }

  return 0;
}
