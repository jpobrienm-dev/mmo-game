#pragma once

/*
 * POOL_DEFINE(Name, T, CAP)
 *
 * Generates a fixed-size object pool for type T with capacity CAP.
 * Allocation and deallocation are O(1) via a singly-linked free list.
 *
 * Generated API:
 *   void  Name_init      (Name *p)         — initialise (all slots free)
 *   int   Name_alloc     (Name *p)         — returns slot index, or -1 if full
 *   void  Name_free_slot (Name *p, int i)  — return slot i to the free list
 *   T    *Name_at        (Name *p, int i)  — pointer to item at index i
 *
 * CAP must be a compile-time constant (integer literal or #define).
 */
#define POOL_DEFINE(Name, T, CAP)                                            \
typedef struct {                                                             \
    T   items[CAP];                                                          \
    int next_free[CAP]; /* free-list chain; -1 = end */                      \
    int used[CAP];                                                           \
    int free_head;      /* index of first free slot, -1 if full */           \
    int count;          /* number of occupied slots */                       \
} Name;                                                                      \
                                                                             \
static inline void Name##_init(Name *p) {                                    \
    p->count     = 0;                                                        \
    p->free_head = 0;                                                        \
    for (int i = 0; i < (CAP) - 1; i++) {                                   \
        p->next_free[i] = i + 1;                                             \
        p->used[i]      = 0;                                                 \
    }                                                                        \
    p->next_free[(CAP) - 1] = -1;                                            \
    p->used[(CAP) - 1]      = 0;                                             \
}                                                                            \
                                                                             \
static inline int Name##_alloc(Name *p) {                                    \
    int i = p->free_head;                                                    \
    if (i < 0) return -1;                                                    \
    p->free_head  = p->next_free[i];                                         \
    p->used[i]    = 1;                                                       \
    p->count++;                                                              \
    return i;                                                                \
}                                                                            \
                                                                             \
static inline void Name##_free_slot(Name *p, int i) {                        \
    p->used[i]      = 0;                                                     \
    p->next_free[i] = p->free_head;                                          \
    p->free_head    = i;                                                     \
    p->count--;                                                              \
}                                                                            \
                                                                             \
static inline T *Name##_at(Name *p, int i) { return &p->items[i]; }
