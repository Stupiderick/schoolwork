/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)

    Node * temp = t->right;
    t->right = temp->left;
    temp->left = t;
    t = temp;
    temp = NULL;

    t->left->height = max(heightOrNeg1(t->left->left), heightOrNeg1(t->left->right)) + 1;
}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)

    Node * temp = t->left;
    t->left = temp->right;
    temp->right = t;
    t = temp;
    temp = NULL;

    t->right->height = max(heightOrNeg1(t->right->left), heightOrNeg1(t->right->right)) + 1;
}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    *_out << __func__ << endl; // Outputs the rotation name (don't remove this)

    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    int diff = heightOrNeg1(subtree->right) - heightOrNeg1(subtree->left);

    if (diff > 1) {
        int rightDiff = heightOrNeg1(subtree->right->right) - heightOrNeg1(subtree->right->left);
        if (rightDiff == 1) {
            rotateLeft(subtree);

        } else {
            rotateRightLeft(subtree);
        }

    } else if (diff < -1) {
        int leftDiff = heightOrNeg1(subtree->left->right) - heightOrNeg1(subtree->left->left);
        if (leftDiff == -1) {
            rotateRight(subtree);

        } else {
            rotateLeftRight(subtree);
        }
    }
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    if (subtree == NULL) {
        subtree = new Node(key, value);

    } else if (key < subtree->key) {
        insert(subtree->left, key, value);
        rebalance(subtree);

    } else if (key > subtree->key) {
        insert(subtree->right, key ,value);
        rebalance(subtree);
    }

    subtree->height = max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right)) + 1;
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        remove(subtree->left, key);
        rebalance(subtree);
        subtree->height = max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right)) + 1;

    } else if (key > subtree->key) {
        remove(subtree->right, key);
        rebalance(subtree);
        subtree->height = max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right)) + 1;

    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            noChildRemove(subtree);

        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            twoChildRemove(subtree);
            subtree->height = max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right)) + 1;

        } else {
            /* one-child remove */
            oneChildRemove(subtree);
            subtree->height = max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right)) + 1;
        }
    }
}

template <class K, class V>
void AVLTree<K, V>::noChildRemove(Node*& subtree) {
    delete subtree;
    subtree = NULL;
}

template <class K, class V>
void AVLTree<K, V>::oneChildRemove(Node*& subtree) {
    Node * temp = subtree;

    if (subtree->left == NULL) {
        subtree = subtree->right;
    } else {
        subtree = subtree->left;
    }

    delete temp;
}

template <class K, class V>
void AVLTree<K, V>::twoChildRemove(Node*& subtree) {
    if (subtree->left->right == NULL) {
        Node * temp = subtree->left;
        swap(subtree, temp);
        subtree->left = temp->left;
        delete temp;

        return;
    }

    Node * iop = rightMostChild(subtree->left);
    swap(subtree, iop);

    Node * find = subtree->left;
    while (find->right != iop) {
        find = find->right;
    }

    if (iop->left == NULL) {
        delete iop;
        iop = NULL;
        find->right = NULL;

    } else {
        find->right = iop->left;
        delete iop;
        iop = NULL;
    }

    Node * findAgain = subtree->left;
    renewHeight(findAgain, find);
}

template <class K, class V>
typename AVLTree<K, V>::Node * AVLTree<K, V>::rightMostChild(Node* subtree) {
    while (subtree->right != NULL) {
        subtree = subtree->right;
    }

    return subtree;
}

template <class K, class V>
void AVLTree<K, V>::renewHeight(Node* from, Node* upto) {
    if (from == upto) {
        from->height = max(heightOrNeg1(from->left), heightOrNeg1(from->right)) + 1;
        return;
    }

    renewHeight(from->right, upto);
    from->height = max(heightOrNeg1(from->left), heightOrNeg1(from->right)) + 1;

}
