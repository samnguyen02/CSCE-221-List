#pragma once

#include <cstddef> // size_t
#include <iterator> // std::bidirectional_iterator_tag
#include <type_traits> // std::is_same, std::enable_if

template <class T>
class List {
    private:
    struct Node {
        Node *next, *prev;
        T data;
        explicit Node(Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev} {}
        explicit Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{data} {}
        explicit Node(T&& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{std::move(data)} {}
    };

    template <typename pointer_type, typename reference_type>
    class basic_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = pointer_type;
        using reference         = reference_type;
    private:
        friend class List<value_type>;
        using Node = typename List<value_type>::Node;

        Node* node;

        explicit basic_iterator(Node* ptr) noexcept : node{ptr} {}
        explicit basic_iterator(const Node* ptr) noexcept : node{const_cast<Node*>(ptr)} {}

    public:
        basic_iterator() : node(nullptr) {};
        basic_iterator(const basic_iterator&) = default;
        basic_iterator(basic_iterator&&) = default;
        ~basic_iterator() = default;
        basic_iterator& operator=(const basic_iterator&) = default;
        basic_iterator& operator=(basic_iterator&&) = default;

        reference operator*() const {
            return (*node).data;
        }
        pointer operator->() const {
            return &(node -> data);
        }

        // Prefix Increment: ++a
        basic_iterator& operator++() {
            node = node -> next;
            return *this;
        }
        // Postfix Increment: a++
        basic_iterator operator++(int) {
            basic_iterator tmp = basic_iterator(node);
            node = node -> next;
            return tmp;
        }
        // Prefix Decrement: --a
        basic_iterator& operator--() {
            node = node -> prev;
            return *this;
        }
        // Postfix Decrement: a--
        basic_iterator operator--(int) {
            basic_iterator tmp = *this;
            node = node -> prev;
            return tmp;
        }

        bool operator==(const basic_iterator& other) const noexcept {
            return node == other.node;
        }
        bool operator!=(const basic_iterator& other) const noexcept {
            return !(node == other.node);
        }
    };

public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = basic_iterator<pointer, reference>;
    using const_iterator  = basic_iterator<const_pointer, const_reference>;

private:
    Node head, tail;
    size_type _size;

public:
    List(): head(), tail(), _size(0){
        head.next = &tail;
        tail.prev = &head;
        // TODO - Don't forget to initialize the list beforehand
    }
    List( size_type count, const T& value ) : head(), tail(), _size(count){
        Node *curr = &head;
        for (size_type i = 0; i < count; i++){
            Node *newNode = new Node(T(value), curr);
            (*curr).next = newNode;
            curr = newNode;
        }
        tail.prev = curr;
        curr -> next = &tail;
    }
    explicit List( size_type count ) : head(), tail(), _size(count) {
        Node *curr = &head;
        for (size_type i = 0; i < count; i++){
            Node *newNode = new Node(T(), curr);
            curr->next = newNode;
            curr = newNode;
        }
        tail.prev = curr;
        curr -> next = &tail;
    }
    List( const List& other ): head(), tail(), _size(other._size) {
        // TODO - Don't forget initialize the list beforehand
        const Node *copy = &(other.head);
        Node *curr = &head;
        for (size_type i = 0; i < _size; i++){
            copy = copy -> next;
            Node *newNode = new Node(copy -> data, curr);
            (*curr).next = newNode;
            curr = newNode;
        }
        tail.prev = curr;
        curr -> next = &tail;
    }
    List( List&& other ): head(), tail(), _size(other._size) {
        // TODO - Don't forget initialize the list beforehand
        if (other._size == 0){
            head.next = &tail;
            tail.prev = &head;
            return;
        }
        head.next = other.head.next;
        tail.prev = other.tail.prev;

        head.next -> prev = &head;
        tail.prev -> next = &tail;

        other.head.next = &(other.tail);
        other.tail.prev = &(other.head);
        other._size = 0;
    }
    ~List() {
        if (head.next == &tail){
            return;
        }
        Node * curr =  &head;
        while(curr -> next != tail.next){
            curr = curr -> next;
            if (curr != &tail){
                delete curr;
            }
        }
        head.next = &tail;
        tail.prev = &head;
        _size  = 0;
    }
    List& operator=( const List& other ) {
        if(&other != this){
            clear();
            const Node *copy = &(other.head);
            Node *curr = &head;
            for(size_type i = 0; i < other._size; i++){
                copy = copy->next;
                Node *nextnode = new Node(copy->data);
                curr->next = nextnode;
                nextnode->prev = curr;
                curr = nextnode;
            }
            curr->next = &tail;
            tail.prev = curr;
            _size = other._size;
        }
        return *this;

    }
    List& operator=( List&& other ) noexcept {
        if(&other != this){
            clear();
            if(other._size == 0){
                return *this;
            }

            head.next = other.head.next;
            tail.prev = other.tail.prev;
            head.next->prev = &head;
            tail.prev->next = &tail;
            other.head.next = &(other.tail);
            other.tail.prev = &(other.head);
            _size = other._size; other._size = 0;
        }
        return *this;
    }

    reference front() {
        return head.next -> data;
    }
    const_reference front() const {
        return head.next -> data;
    }
	
    reference back() {
        return tail.prev -> data;
    }
    const_reference back() const {
        return tail.prev -> data;
    }
	
    iterator begin() noexcept {
        return iterator(head.next);
    }
    const_iterator begin() const noexcept {
        return const_iterator(head.next);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(head.next);
    }

    iterator end() noexcept {
        return iterator(&tail);
    }
    const_iterator end() const noexcept {
        return const_iterator(&tail);
    }
    const_iterator cend() const noexcept {
        return const_iterator(&tail);
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        iterator it = begin();
        while(it != end()){
            erase(it);
            it++;
        }
    }

    iterator insert( const_iterator pos, const T& value ) {
        Node *newNode = new Node(value);
        pos.node -> prev ->next = newNode;
        newNode->prev = pos.node -> prev;
        newNode -> next = pos.node;
        pos.node -> prev = newNode;
        _size++;
        return iterator(newNode);
    }
    iterator insert( const_iterator pos, T&& value ) {
        Node *newNode = new Node(std::move(value));
        pos.node -> prev ->next = newNode;
        newNode->prev = pos.node -> prev;
        newNode -> next = pos.node;
        pos.node -> prev = newNode;
        _size++;
        return iterator(newNode);
    }

    iterator erase( const_iterator pos ) {
        Node *tmp = pos.node->next;
        pos.node->prev->next = pos.node->next;
        pos.node->next->prev = pos.node->prev;
        --_size;
        delete pos.node;
        return iterator(tmp);
    }

    void push_back( const T& value ) {
        insert(end(),value);
    }
    void push_back( T&& value ) {
        insert(end(),std::move(value));
    }

    void pop_back() {
        if(empty()){
            return;
        }
        erase(--end());
    }
	
    void push_front( const T& value ) {
        insert(begin(),value);
    }
	void push_front( T&& value ) {
        insert(begin(),std::move(value));
    }

    void pop_front() {
        if(empty()){
            return;
        }
        erase(begin());
    }

    /*
      You do not need to modify these methods!
      
      These method provide the non-const complement 
      for the const_iterator methods provided above.
    */
    iterator insert( iterator pos, const T & value) { 
        return insert((const_iterator &) (pos), value);
    }

    iterator insert( iterator pos, T && value ) {
        return insert((const_iterator &) (pos), std::move(value));
    }

    iterator erase( iterator pos ) {
        return erase((const_iterator&)(pos));
    }
};


/*
    You do not need to modify these methods!

    These method provide a overload to compare const and 
    non-const iterators safely.
*/
 
namespace {
    template<typename Iter, typename ConstIter, typename T>
    using enable_for_list_iters = typename std::enable_if<
        std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::iterator, 
            Iter
        >{} && std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::const_iterator,
            ConstIter
        >{}, T>::type;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) == rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) == lhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) != rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) != lhs;
}