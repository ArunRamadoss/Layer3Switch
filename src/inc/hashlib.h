/* hashlib.h -- the data structures used in hashing in Bash. */
/* Copyright (C) 1993 Free Software Foundation, Inc.
   This file is part of GNU Bash, the Bourne Again SHell.*/

/**************************************************************
 *  XXX: Note : This file is picked from bash source code 
 *       and modified as per the requirement.Used the core 
 *       code as it is. Please refer the original source code
 *       present in the dir SWITCH/opensource/
 ************************************************************/

/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: hashlib.h,v 1.2 2011/02/04 16:46:57 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */


#if !defined (_HASHLIB_H_)
#define _HASHLIB_H_

/* Redefine the function as a macro for speed. */
#define get_hash_bucket(bucket, table) \
	((table && (bucket < table->nbuckets)) ?  \
	 table->bucket_array[bucket] : \
	 NULL)


#define MAX_NAME  8
/* Default number of buckets in the hash table. */
#define DEFAULT_HASH_BUCKETS 52	

typedef struct hash_bucket {
	struct hash_bucket *next;	/* Link to next hashed key in this bucket. */
	unsigned char *key;			/* What we look up. */
	void *data;			/* What we really want. */
} hash_bucket_t;

typedef struct hash_table {
	hash_bucket_t **bucket_array;	/* Where the data is kept. */
	char htname[MAX_NAME];
	int hmem_pool_id;
	int  key_len;
	int nbuckets;			/* How many buckets does this table have. */
	int nentries;			/* How many entries does this table have. */
	int (*cmp_func)(char *list, char *key);
	int (*hash_index_gen) (char *key);
} HASH_TABLE;

#endif /* _HASHLIB_H */
