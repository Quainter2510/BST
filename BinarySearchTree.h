#pragma once

#include <iostream>
#include <utility>
#include <cstddef>
#include <deque>
#include <queue>
#include <stack>
/*!
    Имплементация бинарного дерева поиска
    Допускается дублирование ключей (аналог multimap)
*/
template <typename Key, typename Value>
class BinarySearchTree
{
    struct Node
    {
        Node(Key key,
             Value value,
             Node* _parent = nullptr,
             Node* _left = nullptr,
             Node* _right = nullptr) {
            keyValuePair = std::make_pair(key, value);
            parent = _parent;
            left = _left;
            right = _right;
        }

        std::pair<Key, Value> keyValuePair;
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
    };

public:
    //! Конструктор по умолчанию
    BinarySearchTree() = default;

    //! Копирование
    explicit BinarySearchTree(const BinarySearchTree& other) {
        std::queue<Node*> q;
        Node* node;
        q.push(other._root);
        while (!q.empty()) {
            if (q.top()) {
                node = q.top();
                insert(node->keyValuePair.first, node->keyValuePair.second);
                q.push(node->left);
                q.push(node->right);
            }
            q.pop();
        }
    }

    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            std::queue<Node *> q;
            Node *node;
            q.push(other._root);
            while (!q.empty()) {
                if (q.front()) {
                    node = q.front();
                    insert(node->keyValuePair.first, node->keyValuePair.second);
                    q.push(node->left);
                    q.push(node->right);
                }
                q.pop();
            }
        }
        return *this;
    }
    //! Перемещение
    explicit BinarySearchTree(BinarySearchTree&& other) noexcept {
        _root = other._root;
        _size = other._size;
        other._root = nullptr;
        other._size = 0;
    }
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept {
        if (this != &other) {
            _root = other._root;
            _size = other._size;
            other._root = nullptr;
            other._size = 0;
        }
        return *this;
    }

    //! Деструктор
    ~BinarySearchTree() {
        std::queue<Node*> q;
        Node* node;
        q.push(_root);
        while (!q.empty()) {
            if (q.front()) {
                node = q.front();
                q.push(node->left);
                q.push(node->right);
                delete node;
            }
            q.pop();
        }
    }

    /*!
        Итератор бинарного дерева поиска

        Обходит дерево последовательно от меньшего ключа к большему
    */
    class Iterator
    {
    public:
        explicit Iterator(Node* node) {
            _node = node;
        }

        std::pair<Key, Value>& operator*() {
            return _node->keyValuePair;
        }

        const std::pair<Key, Value>& operator*() const {
            return _node->keyValuePair;
        }

        std::pair<Key, Value>* operator->() {
            return &_node->keyValuePair;
        }
        const std::pair<Key, Value>* operator->() const {
            return &_node->keyValuePair;
        }

        Iterator operator++() {
            if (_node->right) {
                _node = _node->right;
                while (_node->left) {
                    _node = _node->left;
                }
            }
            else {
                Key elemKey = _node->keyValuePair.first;
                _node = _node->parent;
                while (_node != nullptr && elemKey > _node->keyValuePair.first) {
                    _node = _node->parent;
                }
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        Iterator operator--() {
            if (_node->left) {
                _node = _node->left;
                while (_node->right) {
                    _node = _node->right;
                }
            }
            else {
                Key elemKey = _node->keyValuePair.first;
                _node = _node->parent;
                while (_node != nullptr && elemKey < _node->keyValuePair.first) {
                    _node = _node->parent;
                }
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return _node == other._node;
        }

        bool operator!=(const Iterator& other) const {
            return _node != other._node;
        }

    private:
        Node* _node;
    };

    /*!
        Константный итератор
    */
    class ConstIterator
    {
    public:
        explicit ConstIterator(const Node* node) {
            _node = node;
        }

        const std::pair<Key, Value>& operator*() const {
            return _node->keyValuePair;
        }

        const std::pair<Key, Value>* operator->() const {
            return &_node->keyValuePair;
        }

        ConstIterator operator++() {
            if (_node->right) {
                _node = _node->right;
                while (_node->left) {
                    _node = _node->left;
                }
            }
            else {
                Key elemKey = _node->keyValuePair.first;
                _node = _node->parent;
                while (_node != nullptr && elemKey > _node->keyValuePair.first) {
                    _node = _node->parent;
                }
            }
            return *this;
        }
        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++*this;
            return tmp;
        }

        ConstIterator operator--() {
            if (_node->left) {
                _node = _node->left;
                while (_node->right) {
                    _node = _node->right;
                }
            }
            else {
                Key elemKey = _node->keyValuePair.first;
                _node = _node->parent;
                while (_node != nullptr && elemKey < _node->keyValuePair.first) {
                    _node = _node->parent;
                }
            }
            return *this;
        }

        ConstIterator operator--(int) {
            BinarySearchTree::Iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const ConstIterator& other) const {
            return _node == other._node;
        }

        bool operator!=(const ConstIterator& other) const {
            return _node != other._node;
        }

    private:
        const Node* _node;
    };

    // вставить элемент с ключем key и значением value
    void insert(const Key& key, const Value& value) {
        Node* node = _root;
        if (node == nullptr) {
            _root = new Node(key, value, nullptr);
            _size++;
            return;
        }
        while (true) {
            if (node->keyValuePair.first >= key) {
                if (node->left) {
                    node = node->left;
                }
                else {
                    node->left = new Node(key, value, node);
                    break;
                }
            }
            else {
                if (node->right) {
                    node = node->right;
                }
                else {
                    node->right = new Node(key, value, node);
                    break;
                }
            }
        }
        _size++;
    }

    Node* getFirstNode(const Key& key) {
        if (find(key) != end()) {
            Node* node = _root;
            while(node != nullptr){
                if (node->keyValuePair.first > key) {
                    node = node->left;
                }
                else if (node->keyValuePair.first < key) {
                    node = node->right;
                }
                else {
                    return node;
                }
            }
        }
        return nullptr;
    }

    Node* getLastNode(const Key& key) {
        if (find(key) != end()) {
            Node* node = getFirstNode(key);
            while (node->left && node->left->keyValuePair.first == key) {
                node = node->left;
            }
            return node;
        }
        return nullptr;
    }

    // удалить все элементы с ключем key
    void erase(const Key& key) {
        Node* last = getLastNode(key);
        if (last) {
            Node* child = last->left;
            while (last->parent && last->parent->keyValuePair.first == key) {
                last = last->parent;
                delete last->left;
                _size--;
            }
            if (last->right) {
                Node* swap = last->right;

                while (swap->left) {
                    swap = swap->left;
                }
                if (swap->parent->left == swap) {
                    swap->parent->left = nullptr;
                }
                else {
                    swap->parent->right = nullptr;
                }
                last->keyValuePair = swap->keyValuePair;
                last->left = nullptr;
                if (child) {
                    last->left = child;
                    child->parent = last;
                }

                delete swap;
                _size--;
            }
            else {
                if (child) {
                    last->keyValuePair = child->keyValuePair;
                    last->left = child->left;
                    last->right = child->right;
                    delete child;
                    _size--;
                }
            }
        }
        if (find(key) != end()) {
            erase(key);
        }
    }

    // найти первый элемент в дереве, равный ключу key
    ConstIterator find(const Key& key) const {
        ConstIterator it = cbegin();
        while (it != cend() && it->first != key) {
            it++;
        }
        return it;
    }


    Iterator find(const Key& key) {
        Iterator it = begin();
        while (it != end() && it->first != key) {
            it++;
        }
        return it;
    }

    // найти все элементы, у которых ключ равен key
    // первый итератор пары - первый элемент в дереве, равный key
    // второй итератор пары - первый элемент в дереве больший, чем key
    // [pair.first, pair.second) - полуинтервал, содержащий все элементы с ключем key
    std::pair<Iterator, Iterator> equalRange(const Key& key) {
        Iterator start = find(key);
        Iterator finish = start;
        while (finish != end() && finish->first == key) {
            finish++;
        }
        return std::make_pair(start, finish);
    }

    std::pair<ConstIterator, ConstIterator> equalRange(const Key& key) const {
        ConstIterator start = find(key);
        ConstIterator finish = start;
        while (finish != cend() && finish->first == key) {
            finish++;
        }
        return std::make_pair(start, finish);
    }

    // получить итератор на минимальное значение в дереве
    ConstIterator min(const Key& key) const {
        std::pair<ConstIterator, ConstIterator> it = equalRange(key);
        ConstIterator min = it.first;
        while (it.first != it.second) {
            if (it.first->second < min->second) {
                min = it.first;
            }
            it.first++;
        }
        return min;
    }
    // получить итератор на максимальное значение в дереве
    ConstIterator max(const Key& key) const {
        std::pair<ConstIterator, ConstIterator> it = equalRange(key);
        ConstIterator max = it.first;
        while (it.first != it.second) {
            if (it.first->second > max->second) {
                max = it.first;
            }
            it.first++;
        }
        return max;
    }

    Iterator begin() {
        Node* node = _root;
        if (node == nullptr) {
            return Iterator(nullptr);
        }
        while (node->left) {
            node = node->left;
        }
        return Iterator(node);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    ConstIterator cbegin() const {
        Node* node = _root;
        if (node == nullptr) {
            return ConstIterator(nullptr);
        }
        while (node->left) {
            node = node->left;
        }
        return ConstIterator(node);
    }

    ConstIterator cend() const {
        return ConstIterator(nullptr);
    }

    std::size_t size() const {
        return _size;
    }

private:
    std::size_t _size = 0;
    Node* _root = nullptr; //!< корневой узел дерева
};


/*!
    Имплементация словаря
    Не допускается дублирование ключей (аналог std::map)
*/
template <typename Key, typename Value>
class Map
{
    BinarySearchTree<Key, Value> _tree;
public:
    using MapIterator = typename BinarySearchTree<Key, Value>::Iterator;
    using ConstMapIterator = typename BinarySearchTree<Key, Value>::ConstIterator;

    Map() = default;

    explicit Map(const Map& other) = default;
    Map& operator=(const Map& other) = default;

    explicit Map(Map&& other) noexcept = default;
    Map& operator=(Map&& other) noexcept = default;

    ~Map() = default;

    // вставить элемент с ключем key и значением value
    // если узел с ключем key уже представлен, то заменить его значение на value
    void insert(const Key& key, const Value& value) {
        MapIterator it = find(key);
        if (it == end()) {
            _tree.insert(key, value);
            return;
        }
        it->second = value;
    }

    // удалить элемент с ключем key
    void erase(const Key& key) {
        _tree.erase(key);
    }

    // найти элемент, равный ключу key
    ConstMapIterator find(const Key& key) const {
        return _tree.find(key);
    }

    MapIterator find(const Key& key) {
        return _tree.find(key);
    }

    // доступ к элементу по ключу
    // если в момент обращения элемента не существует, создать его,
    // ключ равен key, value равно дефолтному значению для типа Value
    const Value& operator[](const Key& key) const {
        MapIterator it = _tree.find(key);
        if (it != end()) {
            return it->first;
        }
        _tree.insert(key, Value());
        return Value();
    }

    Value& operator[](const Key& key) {
        MapIterator it = _tree.find(key);
        if (it != end()) {
            return it->first;
        }
        _tree.insert(key, Value());
        return Value();
    }

    MapIterator begin() {
        return _tree.begin();
    }
    MapIterator end() {
        return _tree.end();
    }

    ConstMapIterator cbegin() const {
        return _tree.cbegin();
    }
    ConstMapIterator cend() const {
        return _tree.cend();
    }

    size_t size() const {
        return _tree.size();
    }
};


/*!
    Имплементация множества
    Не допускается дублирование ключей (аналог std::set)
*/
template <typename Value>
class Set
{
    Map<Value, Value> _map;

public:
    using SetIterator = typename Map<Value, Value>::MapIterator;
    using ConstSetIterator = typename Map<Value, Value>::ConstMapIterator;

    Set() = default;

    explicit Set(const Set& other) = default;
    Set& operator=(const Set& other) = default;

    explicit Set(Set&& other) noexcept = default;
    Set& operator=(Set&& other) noexcept = default;

    ~Set() = default;

    void insert(const Value& value) {
        _map.insert(value, value);
    }

    void erase(const Value& value) {
        _map.erase(value);
    }

    ConstSetIterator find(const Value& value) const {
        return _map.find(value);
    }
    SetIterator find(const Value& key) {
        return _map.find(key);
    }

    bool contains(const Value& value) const {
        return find(value) != ConstSetIterator(nullptr);
    }
};