#ifndef JEMALLOC_INTERNAL_RTREE_H
#define JEMALLOC_INTERNAL_RTREE_H

#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/rtree_tsd.h"
#include "jemalloc/internal/sc.h"
#include "jemalloc/internal/tsd.h"

/*
 * This radix tree implementation is tailored to the singular purpose of
 * associating metadata with extents that are currently owned by jemalloc.
 *
 *******************************************************************************
 */

/* Number of high insignificant bits. */
#define RTREE_NHIB ((1U << (LG_SIZEOF_PTR+3)) - LG_VADDR)
/* Number of low insigificant bits. */
#define RTREE_NLIB LG_PAGE
/* Number of significant bits. */
#define RTREE_NSB (LG_VADDR - RTREE_NLIB)
/* Number of levels in radix tree. */
#if RTREE_NSB <= 10
#  define RTREE_HEIGHT 1
#elif RTREE_NSB <= 36
#  define RTREE_HEIGHT 2
#elif RTREE_NSB <= 52
#  define RTREE_HEIGHT 3
#else
#  error Unsupported number of significant virtual address bits
#endif
/* Use compact leaf representation if virtual address encoding allows. */
#if RTREE_NHIB >= LG_CEIL(SC_NSIZES)
#  define RTREE_LEAF_COMPACT
#endif

/* Needed for initialization only. */
#define RTREE_LEAFKEY_INVALID ((uintptr_t)1)

typedef struct rtree_node_elm_s rtree_node_elm_t;
struct rtree_node_elm_s {
	atomic_p_t	child; /* (rtree_{node,leaf}_elm_t *) */
};

typedef struct rtree_metadata_s rtree_metadata_t;
struct rtree_metadata_s {
	szind_t szind;
	bool slab;
};

typedef struct rtree_contents_s rtree_contents_t;
struct rtree_contents_s {
	edata_t *edata;
	rtree_metadata_t metadata;
};

struct rtree_leaf_elm_s {
#ifdef RTREE_LEAF_COMPACT
	/*
	 * Single pointer-width field containing all three leaf element fields.
	 * For example, on a 64-bit x64 system with 48 significant virtual
	 * memory address bits, the index, edata, and slab fields are packed as
	 * such:
	 *
	 * x: index
	 * e: edata
	 * b: slab
	 *
	 *   00000000 xxxxxxxx eeeeeeee [...] eeeeeeee eeee000b
	 */
	atomic_p_t	le_bits;
#else
	atomic_p_t	le_edata; /* (edata_t *) */
	/*
	 * slab is stored in the low bit; szind is stored in the next lowest
	 * bits.
	 */
	atomic_u_t	le_metadata;
#endif
};

typedef struct rtree_level_s rtree_level_t;
struct rtree_level_s {
	/* Number of key bits distinguished by this level. */
	unsigned		bits;
	/*
	 * Cumulative number of key bits distinguished by traversing to
	 * corresponding tree level.
	 */
	unsigned		cumbits;
};

typedef struct rtree_s rtree_t;
struct rtree_s {
	base_t			*base;
	malloc_mutex_t		init_lock;
	/* Number of elements based on rtree_levels[0].bits. */
#if RTREE_HEIGHT > 1
	rtree_node_elm_t	root[1U << (RTREE_NSB/RTREE_HEIGHT)];
#else
	rtree_leaf_elm_t	root[1U << (RTREE_NSB/RTREE_HEIGHT)];
#endif
};

/*
 * Split the bits into one to three partitions depending on number of
 * significant bits.  It the number of bits does not divide evenly into the
 * number of levels, place one remainder bit per level starting at the leaf
 * level.
 */
static const rtree_level_t rtree_levels[] = {
#if RTREE_HEIGHT == 1
	{RTREE_NSB, RTREE_NHIB + RTREE_NSB}
#elif RTREE_HEIGHT == 2
	{RTREE_NSB/2, RTREE_NHIB + RTREE_NSB/2},
	{RTREE_NSB/2 + RTREE_NSB%2, RTREE_NHIB + RTREE_NSB}
#elif RTREE_HEIGHT == 3
	{RTREE_NSB/3, RTREE_NHIB + RTREE_NSB/3},
	{RTREE_NSB/3 + RTREE_NSB%3/2,
	    RTREE_NHIB + RTREE_NSB/3*2 + RTREE_NSB%3/2},
	{RTREE_NSB/3 + RTREE_NSB%3 - RTREE_NSB%3/2, RTREE_NHIB + RTREE_NSB}
#else
#  error Unsupported rtree height
#endif
};

bool rtree_new(rtree_t *rtree, base_t *base, bool zeroed);

rtree_leaf_elm_t *rtree_leaf_elm_lookup_hard(tsdn_t *tsdn, rtree_t *rtree,
    rtree_ctx_t *rtree_ctx, uintptr_t key, bool dependent, bool init_missing);

JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_leafkey(uintptr_t key) {
	unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
	unsigned cumbits = (rtree_levels[RTREE_HEIGHT-1].cumbits -
	    rtree_levels[RTREE_HEIGHT-1].bits);
	unsigned maskbits = ptrbits - cumbits;
	uintptr_t mask = ~((ZU(1) << maskbits) - 1);
	return (key & mask);
}

JEMALLOC_ALWAYS_INLINE size_t
rtree_cache_direct_map(uintptr_t key) {
	unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
	unsigned cumbits = (rtree_levels[RTREE_HEIGHT-1].cumbits -
	    rtree_levels[RTREE_HEIGHT-1].bits);
	unsigned maskbits = ptrbits - cumbits;
	return (size_t)((key >> maskbits) & (RTREE_CTX_NCACHE - 1));
}

JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_subkey(uintptr_t key, unsigned level) {
	unsigned ptrbits = ZU(1) << (LG_SIZEOF_PTR+3);
	unsigned cumbits = rtree_levels[level].cumbits;
	unsigned shiftbits = ptrbits - cumbits;
	unsigned maskbits = rtree_levels[level].bits;
	uintptr_t mask = (ZU(1) << maskbits) - 1;
	return ((key >> shiftbits) & mask);
}

/*
 * Atomic getters.
 *
 * dependent: Reading a value on behalf of a pointer to a valid allocation
 *            is guaranteed to be a clean read even without synchronization,
 *            because the rtree update became visible in memory before the
 *            pointer came into existence.
 * !dependent: An arbitrary read, e.g. on behalf of ivsalloc(), may not be
 *             dependent on a previous rtree write, which means a stale read
 *             could result if synchronization were omitted here.
 */
#  ifdef RTREE_LEAF_COMPACT
JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_leaf_elm_bits_read(tsdn_t *tsdn, rtree_t *rtree,
    rtree_leaf_elm_t *elm, bool dependent) {
	return (uintptr_t)atomic_load_p(&elm->le_bits, dependent
	    ? ATOMIC_RELAXED : ATOMIC_ACQUIRE);
}

JEMALLOC_ALWAYS_INLINE uintptr_t
rtree_leaf_elm_bits_encode(rtree_contents_t contents) {
	uintptr_t edata_bits = (uintptr_t)contents.edata
	    & (((uintptr_t)1 << LG_VADDR) - 1);
	uintptr_t szind_bits = (uintptr_t)contents.metadata.szind << LG_VADDR;
	/*
	 * Slab shares the low bit of edata; we know edata is on an even address
	 * (in fact, it's 128 bytes on 64-bit systems; we can enforce this
	 * alignment if we want to steal 6 extra rtree leaf bits someday.
	 */
	uintptr_t slab_bits = (uintptr_t)contents.metadata.slab;
	return szind_bits | edata_bits | slab_bits;
}

JEMALLOC_ALWAYS_INLINE rtree_contents_t
rtree_leaf_elm_bits_decode(uintptr_t bits) {
	rtree_contents_t contents;
	/* Do the easy things first. */
	contents.metadata.szind = bits >> LG_VADDR;
	contents.metadata.slab = (bool)(bits & 1);
#    ifdef __aarch64__
	/*
	 * aarch64 doesn't sign extend the highest virtual address bit to set
	 * the higher ones.  Instead, the high bits get zeroed.
	 */
	uintptr_t high_bit_mask = ((uintptr_t)1 << LG_VADDR) - 1;
	/* Mask off the slab bit. */
	uintptr_t low_bit_mask = ~(uintptr_t)1;
	uintptr_t mask = high_bit_mask & low_bit_mask;
	contents.edata = (edata_t *)(bits & mask);
#    else
	/* Restore sign-extended high bits, mask slab bit. */
	contents.edata = (edata_t *)((uintptr_t)((intptr_t)(bits << RTREE_NHIB)
	    >> RTREE_NHIB) & ~((uintptr_t)0x1));
#    endif
	return contents;
}

#  endif /* RTREE_LEAF_COMPACT */

JEMALLOC_ALWAYS_INLINE rtree_contents_t
rtree_leaf_elm_read(tsdn_t *tsdn, rtree_t *rtree, rtree_leaf_elm_t *elm,
    bool dependent) {
#ifdef RTREE_LEAF_COMPACT
	uintptr_t bits = rtree_leaf_elm_bits_read(tsdn, rtree, elm, dependent);
	rtree_contents_t contents = rtree_leaf_elm_bits_decode(bits);
	return contents;
#else
	rtree_contents_t contents;
	unsigned metadata_bits = atomic_load_u(&elm->le_metadata, dependent
	    ? ATOMIC_RELAXED : ATOMIC_ACQUIRE);
	contents.metadata.slab = (bool)(metadata_bits & 1);
	contents.metadata.szind = (metadata_bits >> 1);

	contents.edata = (edata_t *)atomic_load_p(&elm->le_edata, dependent
	    ? ATOMIC_RELAXED : ATOMIC_ACQUIRE);

	return contents;
#endif
}

static inline void
rtree_leaf_elm_write(tsdn_t *tsdn, rtree_t *rtree,
    rtree_leaf_elm_t *elm, rtree_contents_t contents) {
#ifdef RTREE_LEAF_COMPACT
	uintptr_t bits = rtree_leaf_elm_bits_encode(contents);
	atomic_store_p(&elm->le_bits, (void *)bits, ATOMIC_RELEASE);
#else
	unsigned metadata_bits = ((unsigned)contents.metadata.slab
	    | ((unsigned)contents.metadata.szind << 1));
	atomic_store_u(&elm->le_metadata, metadata_bits, ATOMIC_RELEASE);
	/*
	 * Write edata last, since the element is atomically considered valid
	 * as soon as the edata field is non-NULL.
	 */
	atomic_store_p(&elm->le_edata, contents.edata, ATOMIC_RELEASE);
#endif
}

/*
 * Tries to look up the key in the L1 cache, returning it if there's a hit, or
 * NULL if there's a miss.
 * Key is allowed to be NULL; returns NULL in this case.
 */
JEMALLOC_ALWAYS_INLINE rtree_leaf_elm_t *
rtree_leaf_elm_lookup_fast(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key) {
	rtree_leaf_elm_t *elm;

	size_t slot = rtree_cache_direct_map(key);
	uintptr_t leafkey = rtree_leafkey(key);
	assert(leafkey != RTREE_LEAFKEY_INVALID);

	if (likely(rtree_ctx->cache[slot].leafkey == leafkey)) {
		rtree_leaf_elm_t *leaf = rtree_ctx->cache[slot].leaf;
		assert(leaf != NULL);
		uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1);
		elm = &leaf[subkey];
		return elm;
	} else {
		return NULL;
	}
}

JEMALLOC_ALWAYS_INLINE rtree_leaf_elm_t *
rtree_leaf_elm_lookup(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key, bool dependent, bool init_missing) {
	assert(key != 0);
	assert(!dependent || !init_missing);

	size_t slot = rtree_cache_direct_map(key);
	uintptr_t leafkey = rtree_leafkey(key);
	assert(leafkey != RTREE_LEAFKEY_INVALID);

	/* Fast path: L1 direct mapped cache. */
	if (likely(rtree_ctx->cache[slot].leafkey == leafkey)) {
		rtree_leaf_elm_t *leaf = rtree_ctx->cache[slot].leaf;
		assert(leaf != NULL);
		uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1);
		return &leaf[subkey];
	}
	/*
	 * Search the L2 LRU cache.  On hit, swap the matching element into the
	 * slot in L1 cache, and move the position in L2 up by 1.
	 */
#define RTREE_CACHE_CHECK_L2(i) do {					\
	if (likely(rtree_ctx->l2_cache[i].leafkey == leafkey)) {	\
		rtree_leaf_elm_t *leaf = rtree_ctx->l2_cache[i].leaf;	\
		assert(leaf != NULL);					\
		if (i > 0) {						\
			/* Bubble up by one. */				\
			rtree_ctx->l2_cache[i].leafkey =		\
				rtree_ctx->l2_cache[i - 1].leafkey;	\
			rtree_ctx->l2_cache[i].leaf =			\
				rtree_ctx->l2_cache[i - 1].leaf;	\
			rtree_ctx->l2_cache[i - 1].leafkey =		\
			    rtree_ctx->cache[slot].leafkey;		\
			rtree_ctx->l2_cache[i - 1].leaf =		\
			    rtree_ctx->cache[slot].leaf;		\
		} else {						\
			rtree_ctx->l2_cache[0].leafkey =		\
			    rtree_ctx->cache[slot].leafkey;		\
			rtree_ctx->l2_cache[0].leaf =			\
			    rtree_ctx->cache[slot].leaf;		\
		}							\
		rtree_ctx->cache[slot].leafkey = leafkey;		\
		rtree_ctx->cache[slot].leaf = leaf;			\
		uintptr_t subkey = rtree_subkey(key, RTREE_HEIGHT-1);	\
		return &leaf[subkey];					\
	}								\
} while (0)
	/* Check the first cache entry. */
	RTREE_CACHE_CHECK_L2(0);
	/* Search the remaining cache elements. */
	for (unsigned i = 1; i < RTREE_CTX_NCACHE_L2; i++) {
		RTREE_CACHE_CHECK_L2(i);
	}
#undef RTREE_CACHE_CHECK_L2

	return rtree_leaf_elm_lookup_hard(tsdn, rtree, rtree_ctx, key,
	    dependent, init_missing);
}

/*
 * Returns true on lookup failure.
 */
static inline bool
rtree_read_independent(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key, rtree_contents_t *r_contents) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
	    key, /* dependent */ false, /* init_missing */ false);
	if (elm == NULL) {
		return true;
	}
	*r_contents = rtree_leaf_elm_read(tsdn, rtree, elm,
	    /* dependent */ false);
	return false;
}

static inline rtree_contents_t
rtree_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
	    key, /* dependent */ true, /* init_missing */ false);
	assert(elm != NULL);
	return rtree_leaf_elm_read(tsdn, rtree, elm, /* dependent */ true);
}

static inline rtree_metadata_t
rtree_metadata_read(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
	    key, /* dependent */ true, /* init_missing */ false);
	assert(elm != NULL);
	return rtree_leaf_elm_read(tsdn, rtree, elm,
	    /* dependent */ true).metadata;
}

/*
 * Returns true on error.
 */
static inline bool
rtree_metadata_try_read_fast(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key, rtree_metadata_t *r_rtree_metadata) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup_fast(tsdn, rtree, rtree_ctx,
	    key);
	if (elm == NULL) {
		return true;
	}
	*r_rtree_metadata = rtree_leaf_elm_read(tsdn, rtree, elm,
	    /* dependent */ true).metadata;
	return false;
}

static inline bool
rtree_write(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx, uintptr_t key,
    rtree_contents_t contents) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
	    key, /* dependent */ false, /* init_missing */ true);
	if (elm == NULL) {
		return true;
	}

	rtree_leaf_elm_write(tsdn, rtree, elm, contents);

	return false;
}

static inline void
rtree_clear(tsdn_t *tsdn, rtree_t *rtree, rtree_ctx_t *rtree_ctx,
    uintptr_t key) {
	rtree_leaf_elm_t *elm = rtree_leaf_elm_lookup(tsdn, rtree, rtree_ctx,
	    key, /* dependent */ true, /* init_missing */ false);
	assert(elm != NULL);
	assert(rtree_leaf_elm_read(tsdn, rtree, elm,
	    /* dependent */ true).edata != NULL);
	rtree_contents_t contents;
	contents.edata = NULL;
	contents.metadata.szind = SC_NSIZES;
	contents.metadata.slab = false;
	rtree_leaf_elm_write(tsdn, rtree, elm, contents);
}

#endif /* JEMALLOC_INTERNAL_RTREE_H */