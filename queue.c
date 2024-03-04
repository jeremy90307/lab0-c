#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *node, *temp;
    list_for_each_entry_safe (node, temp, head, list) {
        list_del(&node->list);
        q_release_element(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);  //獲取第一個節點
    if (!sp && bufsize <= 0)
        return NULL;
    strncpy(sp, node->value, bufsize);
    list_del(&node->list);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    if (!sp && bufsize <= 0)
        return NULL;
    strncpy(sp, node->value, bufsize);
    list_del(&node->list);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int sum = 0;
    if (!head || list_empty(head))
        return 0;
    element_t *node;
    list_for_each_entry (node, head, list) {
        sum++;
    }
    return sum;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *prev = head->prev;
    head->prev->next = NULL;
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    while (fast != NULL && fast->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_del(slow);  // prev->next = slow->next;
    q_release_element(list_entry(slow, element_t, list));
    head->prev = prev;
    prev->next = head;
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    bool flag = false;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head &&
            strcmp(entry->value, safe->value) == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = true;
        } else if (flag) {
            list_del(&entry->list);
            q_release_element(entry);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *newhead = head, *cur = newhead->next;
    for (; cur != head && cur->next != head; newhead = cur, cur = cur->next) {
        list_move(cur->next, newhead);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    };
    safe = head->next;
    head->next = head->prev;
    head->prev = safe;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *curr, *newhead = head;
    int times = k / q_size(head), j = 1;
    for (int i = 0; i < times; i++) {
        for (curr = newhead->next; j < k; j++) {
            list_move(curr->next, newhead);
        }
        newhead = curr;
    }
}

/*Merge the two lists in a one sorted list*/
struct list_head *q_merge_two(struct list_head *list1, struct list_head *list2)
{
    struct list_head *new_head = NULL;
    struct list_head **ptr = &new_head;

    for (struct list_head **node = NULL; list1 && list2;
         *node = (*node)->next) {
        node = strcmp(list_entry(list1, element_t, list)->value,
                      list_entry(list2, element_t, list)->value) >= 0
                   ? &list2
                   : &list1;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uint64_t) list1 | (uint64_t) list2);
    return new_head;
}

/**/
struct list_head *mergesort(struct list_head *l)
{
    if (!l || l->next == NULL)
        return l;
    struct list_head *fast = l, *slow = l;
    while (fast->next != NULL && fast->next->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;
    return (q_merge_two(mergesort(l), mergesort(mid)));
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (head == NULL || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    struct list_head *cur = head;
    struct list_head *next = head->next;
    while (next) {
        next->prev = cur;
        cur = next;
        next = next->next;
    }
    cur->next = head;
    head->prev = cur;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *curr = head->prev;
    while (curr != head && curr->prev != head) {
        element_t *curr_entry = list_entry(curr, element_t, list);
        element_t *prev_entry = list_entry(curr->prev, element_t, list);
        if (strcmp(curr_entry->value, prev_entry->value) > 0) {
            list_del(&prev_entry->list);
            q_release_element(prev_entry);

        } else {
            curr = curr->prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}