#pragma once

#include <stdint.h>
#include <string.h>   /* memset */

/*
 * HASHMAP_DEFINE(Name, K, V)
 *
 * Generates a fixed-capacity open-addressing hash map (linear probing).
 * Deletion uses backward-shift to keep probe chains clean — no tombstones,
 * so lookup always stops at the first empty slot (O(1) amortised).
 *
 * Before invoking this macro, define two static inline helpers:
 *
 *   static inline uint64_t Name##_hash(K key);
 *   static inline int      Name##_eq  (K a, K b);
 *
 * Then initialise with a caller-owned entry buffer:
 *
 *   Name##Entry buf[CAP];           // CAP must be a power of two
 *   Name        hm;
 *   Name##_init(&hm, buf, CAP);
 *
 * Generated API:
 *   void  Name_init  (Name *hm, Name##Entry *buf, int cap)
 *   V    *Name_get   (const Name *hm, K key)   — NULL if absent
 *   void  Name_insert(Name *hm, K key, V val)
 *   void  Name_remove(Name *hm, K key)
 */
#define HASHMAP_DEFINE(Name, K, V)                                               \
typedef struct { K key; V val; int occupied; } Name##Entry;                      \
typedef struct { Name##Entry *entries; int cap; int count; } Name;               \
                                                                                 \
static inline void Name##_init(Name *hm, Name##Entry *buf, int cap) {            \
    hm->entries = buf;                                                           \
    hm->cap     = cap;                                                           \
    hm->count   = 0;                                                             \
    memset(buf, 0, sizeof(Name##Entry) * (size_t)cap);                           \
}                                                                                \
                                                                                 \
/* Returns slot index of key, or -1 if absent. */                                \
static inline int Name##_find_slot(const Name *hm, K key) {                      \
    int slot = (int)(Name##_hash(key) & (uint64_t)(hm->cap - 1));                \
    for (;;) {                                                                   \
        if (!hm->entries[slot].occupied)        return -1;                       \
        if (Name##_eq(hm->entries[slot].key, key)) return slot;                  \
        slot = (slot + 1) & (hm->cap - 1);                                      \
    }                                                                            \
}                                                                                \
                                                                                 \
static inline V *Name##_get(const Name *hm, K key) {                             \
    int s = Name##_find_slot(hm, key);                                           \
    return s >= 0 ? &hm->entries[s].val : NULL;                                  \
}                                                                                \
                                                                                 \
static inline void Name##_insert(Name *hm, K key, V val) {                       \
    int slot = (int)(Name##_hash(key) & (uint64_t)(hm->cap - 1));                \
    while (hm->entries[slot].occupied &&                                         \
           !Name##_eq(hm->entries[slot].key, key))                               \
        slot = (slot + 1) & (hm->cap - 1);                                      \
    if (!hm->entries[slot].occupied) hm->count++;                                \
    hm->entries[slot].key      = key;                                            \
    hm->entries[slot].val      = val;                                            \
    hm->entries[slot].occupied = 1;                                              \
}                                                                                \
                                                                                 \
static inline void Name##_remove(Name *hm, K key) {                              \
    int hole = Name##_find_slot(hm, key);                                        \
    if (hole < 0) return;                                                        \
    hm->entries[hole].occupied = 0;                                              \
    hm->count--;                                                                 \
    /* Backward-shift deletion: pull displaced neighbours into the hole so       \
     * probe chains remain contiguous (no tombstones needed). */                  \
    int next = (hole + 1) & (hm->cap - 1);                                      \
    while (hm->entries[next].occupied) {                                         \
        int ideal = (int)(Name##_hash(hm->entries[next].key) &                   \
                          (uint64_t)(hm->cap - 1));                              \
        /* Is `next` displaced by the hole? True when the probe path from        \
         * `ideal` to `next` passes through `hole`. */                           \
        int displaced = (next > hole)                                            \
            ? (ideal <= hole || ideal > next)                                    \
            : (ideal <= hole && ideal > next);                                   \
        if (displaced) {                                                         \
            hm->entries[hole] = hm->entries[next];                               \
            hm->entries[next].occupied = 0;                                      \
            hole = next;                                                         \
        }                                                                        \
        next = (next + 1) & (hm->cap - 1);                                      \
    }                                                                            \
}
