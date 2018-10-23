/**
 * @file heap.cpp
 * Implementation of a heap class.
 */

template <class T, class Compare>
size_t heap<T, Compare>::root() const
{
    return 1;
}

template <class T, class Compare>
size_t heap<T, Compare>::leftChild(size_t currentIdx) const
{
    return currentIdx * 2;
}

template <class T, class Compare>
size_t heap<T, Compare>::rightChild(size_t currentIdx) const
{
    return currentIdx * 2 + 1;
}

template <class T, class Compare>
size_t heap<T, Compare>::parent(size_t currentIdx) const
{
    if (currentIdx == 1) {
        return 0;
    } else {
        int parentIdx = currentIdx / 2;
        return parentIdx;
    }
}

template <class T, class Compare>
bool heap<T, Compare>::hasAChild(size_t currentIdx) const
{
    return currentIdx * 2 < _elems.size();
}

template <class T, class Compare>
size_t heap<T, Compare>::maxPriorityChild(size_t currentIdx) const
{
    if (!hasAChild(currentIdx)) {
        return 0;
    }

    int theLeftChild = leftChild(currentIdx);
    int theRightChild = rightChild(currentIdx);

    if (theRightChild >= int(_elems.size())) {
        return theLeftChild;
    }

    if (higherPriority(_elems[theLeftChild], _elems[theRightChild])) {
        return theLeftChild;
    } else {
        return theRightChild;
    }
}

template <class T, class Compare>
void heap<T, Compare>::heapifyDown(size_t currentIdx)
{
    if (hasAChild(currentIdx)) {
        int priorityChildIdx = maxPriorityChild(currentIdx);
        if (!higherPriority(_elems[currentIdx], _elems[priorityChildIdx])) {
            std::swap(_elems[currentIdx], _elems[priorityChildIdx]);
            heapifyDown(priorityChildIdx);
        }
    }
}

template <class T, class Compare>
void heap<T, Compare>::heapifyUp(size_t currentIdx)
{
    if (currentIdx == root())
        return;
    size_t parentIdx = parent(currentIdx);
    if (higherPriority(_elems[currentIdx], _elems[parentIdx])) {
        std::swap(_elems[currentIdx], _elems[parentIdx]);
        heapifyUp(parentIdx);
    }
}

template <class T, class Compare>
heap<T, Compare>::heap()
{
    _elems.push_back(T());
}

template <class T, class Compare>
heap<T, Compare>::heap(const std::vector<T>& elems)
{
    _elems.push_back(T());
    
    for (auto item : elems) {
        _elems.push_back(item);
    }

    for (unsigned i = parent(_elems.size() - 1); i > 0; i--) {
        heapifyDown(i);
    }
}

template <class T, class Compare>
T heap<T, Compare>::pop()
{
    if (!empty()) {
        T ret = _elems[1];
        _elems[1] = _elems[_elems.size() - 1];
        _elems.pop_back();
        heapifyDown(1);

        return ret;
    }

    return T();
}

template <class T, class Compare>
T heap<T, Compare>::peek() const
{
    if (!empty()) {
        return _elems[1];
    }
    return T();
}

template <class T, class Compare>
void heap<T, Compare>::push(const T& elem)
{
    _elems.push_back(elem);
    heapifyUp(_elems.size() - 1);
}

template <class T, class Compare>
bool heap<T, Compare>::empty() const
{
    return _elems.size() == 1;
}
