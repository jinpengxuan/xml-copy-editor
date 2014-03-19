/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef STRINGSET_H
#define STRINGSET_H

#include <stdint.h>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <cmath>
#include <climits>
#include <cstring>
#ifdef __WXMSW__
#include <mem.h>
#endif
#include <boost/utility.hpp>

template<class T>
class StringSetNode : private boost::noncopyable
{
	public:
		StringSetNode ( T *keyParameter, size_t lenParameter )
		{
			next = NULL;
			len = lenParameter;
			key = new T[len];
			memcpy ( key, keyParameter, len * sizeof ( T ) );
		}
		~StringSetNode()
		{
			delete[] key;
		}
		StringSetNode<T> *next;
		T *key;
		size_t len;
};

template<class T>
class StringSet
{
	public:
		StringSet (
		    int hashSizePower = 19
		);
		~StringSet();
		StringSet ( const StringSet<T>& );
		StringSet& operator= ( const StringSet<T>& );
		inline bool empty();
		inline int count();
		StringSetNode<T> *insert ( std::basic_string<T> &s );
		StringSetNode<T> *insert ( T *s, size_t len, uint32_t hash = UINT_MAX );
		StringSetNode<T> *find ( std::basic_string<T> &s );
		StringSetNode<T> *find ( T *s, size_t len, uint32_t hash = UINT_MAX );
		void clear();
	private:
		uint32_t hashSize, hashMask, nodeCount;
		StringSetNode<T> **table;
		uint32_t hash ( const char *s, size_t len );
		void allocateHashTable ( uint32_t hashSize );
};

template<class T>
StringSet<T>::StringSet ( int hashSizePower )
{
	if ( hashSizePower < 1 )
		throw std::runtime_error ( "StringSet: invalid parameter" );
	hashSize = ( int ) pow ( ( double ) 2, ( double ) hashSizePower );
	hashMask = hashSize - 1;
	nodeCount = 0;
	allocateHashTable ( hashSize );
}

template<class T>
StringSet<T>::~StringSet()
{
	if ( !empty() )
		clear();
	delete[] table;
}

template<class T>
StringSet<T>::StringSet ( const StringSet<T>& d )
{
	hashSize = d.hashSize;
	hashMask = hashSize - 1;
	nodeCount = 0;
	allocateHashTable ( hashSize );
	StringSetNode<T> *np;
	for ( uint32_t i = 0; i < d.hashSize; ++i )
		for ( np = d.table[i]; np != NULL; np = np->next )
			insert ( np->key, np->len, i );
}

template<class T>
StringSet<T>& StringSet<T>::operator= ( const StringSet<T>& d )
{
	if ( this != &d )
	{
		clear();
		StringSetNode<T> *np;
		for ( uint32_t i = 0; i < d.hashSize; ++i )
			for ( np = d.table[i]; np != NULL; np = np->next )
				insert ( np->key, np->len, i );
	}
	return *this;
}

// see 'one-at-a-time hash' (http://burtleburtle.net/bob/hash/doobs.html)
template<class T>
uint32_t StringSet<T>::hash ( const char *key, size_t len )
{
	uint32_t hash;
	for ( hash = 0; len--; ++key )
	{
		hash += *key;
		hash += ( hash << 10 );
		hash ^= ( hash >> 6 );
	}
	hash += ( hash << 3 );
	hash ^= ( hash >> 11 );
	hash += ( hash << 15 );
	return ( hash & hashMask );
}

template<class T>
StringSetNode<T> *StringSet<T>::find ( std::basic_string<T> &s )
{
	return find ( ( T * ) s.data(), s.size() );
}

template<class T>
StringSetNode<T> *StringSet<T>::find ( T *s, size_t len, uint32_t hashValue )
{
	if ( hashValue == UINT_MAX )
		hashValue = hash ( ( const char* ) s, len * sizeof ( T ) );
	StringSetNode<T> *np;
	for ( np = table[hashValue]; np != NULL; np = np->next )
		if ( len == np->len && ( memcmp ( s, np->key, len ) == 0 ) )
			return np;
	return NULL;
}

template<class T>
StringSetNode<T> *StringSet<T>::insert ( std::basic_string<T> &s )
{
	return insert ( ( T * ) s.data(), s.size() );
}

template<class T>
StringSetNode<T> *StringSet<T>::insert ( T *s, size_t len, uint32_t hashValue )
{
	if ( hashValue == UINT_MAX )
		hashValue = hash ( ( const char* ) s, len * sizeof ( T ) );
	StringSetNode<T> *np;
	if ( ( np = find ( s, len, hashValue ) ) == NULL )
	{
		np = new StringSetNode<T> ( s, len );
		np->next = table[hashValue];
		table[hashValue] = np;
		++nodeCount;
	}
	return np;
}

template<class T>
void StringSet<T>::clear()
{
	if ( !nodeCount )
		return;
	StringSetNode<T> *np, *memory;
	for ( uint32_t i = 0; i < hashSize; ++i )
	{
		for ( np = table[i]; np != NULL; np = memory )
		{
			memory = np->next;
			delete np;
		}
		table[i] = NULL;
	}
	nodeCount = 0;
}

template<class T>
int StringSet<T>::count()
{
	return nodeCount;
}

template<class T>
bool StringSet<T>::empty()
{
	return ( nodeCount ) ? false : true;
}

template<class T>
void StringSet<T>::allocateHashTable ( uint32_t hashSize )
{
	table = new StringSetNode<T> *[hashSize];
	memset ( table, 0, sizeof ( StringSetNode<T> * ) * hashSize );
}

#endif
