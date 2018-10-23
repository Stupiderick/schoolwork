/**
 * @file list.cpp
 * Doubly Linked List (MP 3).
 *
 * @author Chase Geigle
 * @date (created) Fall 2011
 * @date (modified) Spring 2012, Fall 2012
 *
 * @author Jack Toole
 * @date (modified) Fall 2011
 */

/**
 * Destroys the current List. This function should ensure that
 * memory does not leak on destruction of a list.
 */
template <class T>
List<T>::~List()
{
    clear();
}

/**
 * Destroys all dynamically allocated memory associated with the current
 * List class.
 */
template <class T>
void List<T>::clear()
{
  ListNode * temp = tail;

  if (length == 1) {
    delete head;

  } else {
    while (tail != NULL) {
      tail = tail->prev;
      delete temp;
      temp = tail;
      if (tail != NULL)
        tail->next = NULL;
    }

    temp = NULL;
  }

  head = NULL;
  tail = NULL;
  length = 0;
}

/**
 * Inserts a new node at the front of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <class T>
void List<T>::insertFront(T const& ndata)
{
    ListNode * frontListNode = new ListNode(ndata);
    ListNode * temp;

    if (head == NULL || tail == NULL || length == 0) {
      head = frontListNode;
      tail = frontListNode;
      length = 1;
      frontListNode = NULL;

    } else {
      temp = head;
      head = frontListNode;
      frontListNode = NULL;
      head->next = temp;
      temp->prev = head;
      temp = NULL;

      length++;
    }
}

/**
 * Inserts a new node at the back of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <class T>
void List<T>::insertBack(const T& ndata)
{
    ListNode * backListNode = new ListNode(ndata);
    ListNode * temp;

    if (head == NULL || tail == NULL || length == 0) {
      head = backListNode;
      tail = backListNode;
      length = 1;
      backListNode = NULL;

    } else {
      temp = tail;
      tail = backListNode;
      backListNode = NULL;
      tail->prev = temp;
      temp->next = tail;
      temp = NULL;

      length++;
    }
}

/**
 * Reverses the current List.
 */
template <class T>
void List<T>::reverse()
{
    reverse(head, tail);
}

/**
 * Helper function to reverse a sequence of linked memory inside a List,
 * starting at startPoint and ending at endPoint. You are responsible for
 * updating startPoint and endPoint to point to the new starting and ending
 * points of the rearranged sequence of linked memory in question.
 *
 * @param startPoint A pointer reference to the first node in the sequence
 *  to be reversed.
 * @param endPoint A pointer reference to the last node in the sequence to
 *  be reversed.
 */
template <class T>
void List<T>::reverse(ListNode*& startPoint, ListNode*& endPoint)
{
    if (startPoint == endPoint) {
      return;
    }

    int FLAG;
    ListNode * tempStartPrev;
    ListNode * tempStart = startPoint;
    ListNode * tempEnd = endPoint;
    ListNode * tempEndPrev = endPoint->prev;
    ListNode * tempEndNext;

    if (tempEnd->next == NULL) {
      FLAG = 0;

    } else {
      FLAG = 1;
      tempEndNext = endPoint->next;
    }

    ListNode * t = tempStart->prev;
    if (t == NULL) {
      startPoint = endPoint;
      endPoint->prev = NULL;

    } else {
      tempStartPrev = tempStart->prev;
      tempStartPrev->next = endPoint;
      endPoint->prev = tempStartPrev;

      t = NULL;
    }

    while (tempEndPrev != tempStart) {
      tempEnd->next = tempEndPrev;
      tempEndPrev = tempEndPrev->prev;
      tempEnd = tempEnd->next;
      tempEnd->prev = tempEnd->next;
    }

    tempEnd->next = tempEndPrev;
    tempEndPrev->prev = tempEnd;

    if (FLAG == 0) {
      tempEndPrev->next = NULL;
      endPoint = tempEndPrev;

    } else {
      tempEndPrev->next = tempEndNext;
      tempEndNext->prev = tempEndPrev;
    }

    tempStartPrev = NULL;
    tempStart = NULL;
    tempEnd = NULL;
    tempEndPrev = NULL;
    tempEndNext = NULL;
}

/**
 * Reverses blocks of size n in the current List. You should use your
 * reverse( ListNode * &, ListNode * & ) helper function in this method!
 *
 * @param n The size of the blocks in the List to be reversed.
 */
template <class T>
void List<T>::reverseNth(int n)
{
    if (n <= 1) {
      return;
    }

    if (head == NULL || tail == NULL || length == 0) {
      return;
    }

    if (head == tail) {
      return;
    }

    if (n == length) {
      reverse(head, tail);
      return;
    }

    int FLAG = 0;
    int ANOTHER_FLAG = 0;
    ListNode * tempStartNth = head;
    ListNode * tempEndNth = head;
    ListNode * tempNextStartNth = head;

    for (int i = 0; i < n; i++) {
      if (tempNextStartNth != NULL) {
        tempNextStartNth = tempNextStartNth->next;
      } else {
        ANOTHER_FLAG = 1;
      }
    }

    while (tempNextStartNth != NULL) {
      for (int j = 0; j < n - 1; j++) {
        if (tempEndNth->next != NULL) {
          tempEndNth = tempEndNth->next;
        }
      }

      if (FLAG == 0) {
        reverse(head, tempEndNth);

      } else {
        reverse(tempStartNth, tempEndNth);
      }

      tempStartNth = tempNextStartNth;
      tempEndNth = tempNextStartNth;

      for (int j = 0; j < n; j++) {
        if (tempNextStartNth != NULL) {
          tempNextStartNth = tempNextStartNth->next;
        }
      }

      FLAG++;
    }

    if (ANOTHER_FLAG == 0) {
      reverse(tempStartNth, tail);
    } else {
      reverse(head, tail);
    }
}

/**
 * Modifies the List using the waterfall algorithm.
 * Every other node (starting from the second one) is removed from the
 * List, but appended at the back, becoming the new tail. This continues
 * until the next thing to be removed is either the tail (**not necessarily
 * the original tail!**) or NULL.  You may **NOT** allocate new ListNodes.
 * Note that since the tail should be continuously updated, some nodes will
 * be moved more than once.
 */
template <class T>
void List<T>::waterfall()
{
    if (head == NULL || tail == NULL || length == 0) {
      return;
    }

    if (head == tail) {
      return;
    }

    if (head->next == tail) {
      return;
    }

    ListNode * curr = head;
    ListNode * currNext = curr->next;

    while (currNext->next != tail) {
      curr->next = currNext->next;
      curr = curr->next;
      curr->prev = currNext->prev;
      currNext->prev = tail;
      currNext->next = NULL;
      tail = currNext;
      currNext->prev->next = currNext;
      currNext = curr->next;
    }

    curr->next = tail;
    tail->prev = curr;
    tail->next = currNext;
    currNext->prev = tail;
    currNext->next = NULL;
    tail = currNext;

    curr = NULL;
    currNext = NULL;
}

/**
 * Splits the given list into two parts by dividing it at the splitPoint.
 *
 * @param splitPoint Point at which the list should be split into two.
 * @return The second list created from the split.
 */
template <class T>
List<T> List<T>::split(int splitPoint)
{
    if (splitPoint > length)
        return List<T>();

    if (splitPoint < 0)
        splitPoint = 0;

    ListNode* secondHead = split(head, splitPoint);

    int oldLength = length;
    if (secondHead == head) {
        // current list is going to be empty
        head = NULL;
        tail = NULL;
        length = 0;
    } else {
        // set up current list
        tail = head;
        while (tail->next != NULL)
            tail = tail->next;
        length = splitPoint;
    }

    // set up the returned list
    List<T> ret;
    ret.head = secondHead;
    ret.tail = secondHead;
    if (ret.tail != NULL) {
        while (ret.tail->next != NULL)
            ret.tail = ret.tail->next;
    }
    ret.length = oldLength - splitPoint;
    return ret;
}

/**
 * Helper function to split a sequence of linked memory at the node
 * splitPoint steps **after** start. In other words, it should disconnect
 * the sequence of linked memory after the given number of nodes, and
 * return a pointer to the starting node of the new sequence of linked
 * memory.
 *
 * This function **SHOULD NOT** create **ANY** new List objects!
 *
 * @param start The node to start from.
 * @param splitPoint The number of steps to walk before splitting.
 * @return The starting node of the sequence that was split off.
 */
template <class T>
typename List<T>::ListNode* List<T>::split(ListNode* start, int splitPoint)
{
    ListNode * theSecondHead = start;

    if (splitPoint == 0) {
      return theSecondHead;
    }

    for (int i = 0; i < splitPoint; i++) {
      if (theSecondHead->next != NULL) {
        theSecondHead = theSecondHead->next;
      }
    }

    if (theSecondHead->prev != NULL) {
      theSecondHead->prev->next = NULL;
      theSecondHead->prev = NULL;
    }

    return theSecondHead;
}

/**
 * Merges the given sorted list into the current sorted list.
 *
 * @param otherList List to be merged into the current list.
 */
template <class T>
void List<T>::mergeWith(List<T>& otherList)
{
    // set up the current list
    head = merge(head, otherList.head);
    tail = head;

    // make sure there is a node in the new list
    if (tail != NULL) {
        while (tail->next != NULL)
            tail = tail->next;
    }
    length = length + otherList.length;

    // empty out the parameter list
    otherList.head = NULL;
    otherList.tail = NULL;
    otherList.length = 0;
}

/**
 * Helper function to merge two **sorted** and **independent** sequences of
 * linked memory. The result should be a single sequence that is itself
 * sorted.
 *
 * This function **SHOULD NOT** create **ANY** new List objects.
 *
 * @param first The starting node of the first sequence.
 * @param second The starting node of the second sequence.
 * @return The starting node of the resulting, sorted sequence.
 */
template <class T>
typename List<T>::ListNode* List<T>::merge(ListNode* first, ListNode* second)
{
    if (first == NULL && second != NULL) {
      return second;
    }

    if (first != NULL && second == NULL) {
      return first;
    }

    if (first == NULL && second == NULL) {
      return NULL;
    }

    ListNode * newFirst;
    ListNode * newSecond;

    if (first->data < second->data) {
      newFirst = first;
      newSecond = second;

    } else {
      newFirst = second;
      newSecond = first;
    }

    ListNode * tempFirst = newFirst;
    ListNode * tempSecond = newSecond;
    ListNode * tempFirstNext = newFirst->next;
    ListNode * tempSecondNext = newSecond->next;

    while ((tempFirstNext != NULL) && (tempSecondNext != NULL)) {
      if ((tempFirst->data < tempSecond->data) && (tempFirstNext->data < tempSecond->data)) {
        tempFirst = tempFirst->next;
        tempFirstNext = tempFirstNext->next;

      } else {
        tempFirst->next = tempSecond;
        tempSecond->prev = tempFirst;
        tempSecond->next = tempFirstNext;
        tempFirstNext->prev = tempSecond;
        tempSecondNext->prev = NULL;

        tempSecond = tempSecondNext;
        tempSecondNext = tempSecondNext->next;

        tempFirst = tempFirst->next;
      }
    }

    if (tempFirstNext == NULL) {
      tempSecond->prev = tempFirst;
      tempFirst->next = tempSecond;

    } else {
      while (tempFirstNext != NULL) {
        if ((tempFirst->data < tempSecond->data) && (tempFirstNext->data < tempSecond->data)) {
          tempFirst = tempFirst->next;
          tempFirstNext = tempFirstNext->next;

        } else {
          tempFirst->next = tempSecond;
          tempSecond->prev = tempFirst;
          tempSecond->next = tempFirstNext;
          tempFirstNext->prev = tempSecond;

          return newFirst;
        }
      }

      tempFirst->next = tempSecond;
      tempSecond->prev = tempFirst;
    }

    return newFirst;
}

/**
 * Sorts the current list by applying the Mergesort algorithm.
 */
template <class T>
void List<T>::sort()
{
    if (empty())
        return;
    head = mergesort(head, length);
    tail = head;
    while (tail->next != NULL)
        tail = tail->next;
}

/**
 * Sorts a chain of linked memory given a start node and a size.
 * This is the recursive helper for the Mergesort algorithm (i.e., this is
 * the divide-and-conquer step).
 *
 * @param start Starting point of the chain.
 * @param chainLength Size of the chain to be sorted.
 * @return A pointer to the beginning of the now sorted chain.
 */
template <class T>
typename List<T>::ListNode* List<T>::mergesort(ListNode* start, int chainLength)
{
    if (chainLength <= 1) {
      return start;
    }

    ListNode * secondStart = start;

    for (int i = 0; i < chainLength / 2; i++) {
      secondStart = secondStart->next;
    }

    secondStart->prev->next = NULL;
    secondStart->prev = NULL;

    start = mergesort(start, chainLength / 2);
    secondStart = mergesort(secondStart, chainLength - chainLength / 2);

    return merge(start, secondStart);
}
