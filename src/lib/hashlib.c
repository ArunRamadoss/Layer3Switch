/* hashlib.c -- functions to manage and access hash tables for bash. */

/* Copyright (C) 1987, 1989, 1991 Free Software Foundation, Inc.*/

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
 *  $Id: hashlib.c,v 1.5 2011/02/04 16:46:56 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */


#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include "hashlib.h"
#include "jhash.h"

HASH_TABLE * create_hash_table (char *hashname, int buckets, 
				int (*cmp)(char *list, char *key),
				int  (*index_gen)(char *key), int key_len)
{
	HASH_TABLE * htable = NULL;
	
	int size = (sizeof (hash_bucket_t *)) * 
                   ((!buckets) ? (buckets = DEFAULT_HASH_BUCKETS) : buckets);

	htable = (HASH_TABLE *) malloc (sizeof (HASH_TABLE)); 

	if (!htable) {
		return NULL;
	}

	strcpy (htable->htname, hashname); 
	htable->nbuckets = buckets;
	htable->key_len = key_len;
	htable->cmp_func = cmp;
	htable->hash_index_gen = index_gen;
	htable->nentries = 0;

	htable->hmem_pool_id = mem_pool_create (hashname, size, buckets, 0);

	if (htable->hmem_pool_id < 0) {
		free (htable);	
		return NULL;
	}
        htable->bucket_array = (hash_bucket_t **) malloc (buckets * 
							 sizeof (hash_bucket_t *));


	if (!htable->bucket_array) {
		mem_pool_delete (htable->hmem_pool_id);
		free (htable);
	}
	return htable;
}

static hash_bucket_t * find_hash_entry (char *key, HASH_TABLE *htbl)
{
	hash_bucket_t *bkt = NULL;
	int which_bucket = -1;

	if (!htbl)
		return NULL;

	which_bucket = htbl->hash_index_gen (key);

	if (which_bucket > htbl->nbuckets) {
		printf ("HASH Error : Invalid Bucket\n");
		return NULL;
	}

	for (bkt = htbl->bucket_array[which_bucket]; bkt; bkt = bkt->next) {
		if (!htbl->cmp_func (bkt->key, key)) {
			return (bkt);
		}
	}
	return NULL;
}

void * hash_tbl_lookup (char *key, HASH_TABLE *htbl)
{
	hash_bucket_t *pbkt = find_hash_entry (key, htbl);

	if (!pbkt) {
		return NULL;
	}

	return pbkt->data;
}

static int hash_delete_entry (uint8_t *string, HASH_TABLE *htbl, void (*free_data)(void *))
{
	int the_bucket = -1;

	hash_bucket_t *prev = NULL , *temp = NULL;

	if (!htbl)
		return -1;

	the_bucket = htbl->hash_index_gen (string);

	if (the_bucket > htbl->nbuckets) {
		return -1;
	}

	for (temp = htbl->bucket_array[the_bucket]; temp; temp = temp->next) {

		if (!htbl->cmp_func (temp->key, string)) {
			if (prev)
				prev->next = temp->next;
			else
				htbl->bucket_array[the_bucket] = temp->next;

			htbl->nentries--;
			if (free_data)
				free_data (temp->data);
			else
				free (temp->data);

			free_blk (htbl->hmem_pool_id , temp);

			return 0;
		}
		prev = temp;
	}
	return -1;
}

static int hash_add_entry (uint8_t *string, HASH_TABLE *htbl, void *data)
{
	hash_bucket_t *hentry = NULL;
	int bucket = -1;

	if (!htbl) 
		return NULL;

	if (!(hentry = find_hash_entry (string, htbl))) {

		bucket = htbl->hash_index_gen (string);
	
		if (bucket > htbl->nbuckets) {
			return -1;
		}

		hentry = htbl->bucket_array[bucket];

		while (hentry && hentry->next)
			hentry = hentry->next;

		if (hentry) {
			hentry->next = alloc_block (htbl->hmem_pool_id);
			if (!hentry->next) {
				return -1;
			}
			hentry = hentry->next;
		}
		else {
			htbl->bucket_array[bucket] = alloc_block (htbl->hmem_pool_id);
			hentry = htbl->bucket_array[bucket];
			if (!hentry)
				return -1;
		}

		hentry->data = data;
		hentry->next = NULL;
		hentry->key  = malloc (htbl->key_len);
                {
			int i = 0;
			for (;i < htbl->key_len; i++)
				hentry->key[i] = string[i];
		}
		
		htbl->nentries++;
	}

	return 0;
}

int  hash_tbl_add (uint8_t *key, HASH_TABLE *htbl, void *data)
{
	if (hash_add_entry (key , htbl, data) < 0) {
		return -1;
	}
	return 0;
}

int hash_tbl_delete (char *string, HASH_TABLE *htbl, void (*free_data)(void *))
{
	if (hash_delete_entry (string, htbl, free_data) < 0) {
		return -1;
	}
	return 0;
}
void flush_hash_table (HASH_TABLE *htbl, void (*free_data) (void *data))
{
	int i = -1;
	hash_bucket_t *bucket = NULL, *hentry = NULL;

	if (!htbl)
		return;

	for (i = 0; i < htbl->nbuckets; i++) {

		bucket = htbl->bucket_array[i];

		while (bucket) {
			hentry = bucket;
			bucket = bucket->next;
			if (free_data)
				(*free_data) (hentry->data);
			else
				free (hentry->data);
			free (hentry->key);
			free_blk (htbl->hmem_pool_id , hentry);
		}
		htbl->bucket_array[i] = NULL;
	}
}

void destroy_hash_table (HASH_TABLE *htbl, void (*free_data) (void *))
{
	flush_hash_table (htbl, free_data);

	mem_pool_delete (htbl->hmem_pool_id);
}

int hash_walk (HASH_TABLE *htbl, void (*callback)(void *))
{
	register int slot, bcount;
	register hash_bucket_t *bc;
	int count = 0;

	/* Print out a count of how many strings hashed to each bucket, so we can
	   see how even the distribution is. */
	for (slot = 0; slot < htbl->nbuckets; slot++)
	{
		bc = get_hash_bucket (slot, htbl);
		if (bc) {
			for (bcount = 0; bc; bc = bc->next) {
				count++;
  				callback (bc->data);
			}
		}
	}
	return count;
}
