#ifndef GDD_NEWDEL_H
#define GDD_NEWDEL_H

/*
 * Author: Jim Kowalkowski
 * Date: 2/96
 *
 * $Id: gddNewDel.h,v 1.1 1997-02-10 19:37:45 saunders Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  1996/06/25 19:11:47  jbk
 * new in EPICS base
 *
 *
 * *Revision 1.2  1996/06/24 03:15:37  jbk
 * *name changes and fixes for aitString and fixed string functions
 * *Revision 1.1  1996/05/31 13:15:33  jbk
 * *add new stuff
 *
 */

// this file if formatted with tab stop = 4

#include <stdlib.h>

// Avoid using templates at the cost of very poor readability.
// This forces the user to have a static data member named "gddNewDel_freelist"
// This also forces the user to have a data member named "gddNewDel_next"

// To use this stuff:
//
// ** In class description header file:
//	class myClass
//	{
//	public:
//		gdd_NEWDEL_FUNC(myClass)
//	private:
//		gdd_NEWDEL_DATA(myClass)
//	};
//
// ** In source file where functions for class are written:
//	gdd_NEWDEL_STAT(myClass)
//	gdd_NEWDEL_DEL(myClass)
//	gdd_NEWDEL_NEW(myClass)

#define gdd_CHUNK_NUM 20
#define gdd_CHUNK(mine) (gdd_CHUNK_NUM*sizeof(mine))

// private data to add to a class
#define gdd_NEWDEL_DATA(clas) \
	clas* newdel_next; \
	static clas* newdel_freelist; \

// public interface for the new/delete stuff
#define gdd_NEWDEL_FUNC(clas) void* operator new(size_t); \
		 void operator delete(void*); \
		 clas* next(void) { return newdel_next; } \
		 void setNext(clas* n) { newdel_next=n; }

// declaration of the static variable for the free list
#define gdd_NEWDEL_STAT(clas) \
	clas* clas::newdel_freelist=NULL; \

// code for the delete function
#define gdd_NEWDEL_DEL(clas) void clas::operator delete(void* v) { \
 clas* dn = (clas*)v; \
 if(dn->newdel_next==(clas*)(-1)) free((char*)v); \
 else { \
   dn->newdel_next=clas::newdel_freelist; clas::newdel_freelist=dn; \
 } \
}

// following function assumes that reading/writing address is atomic

// code for the new function
#define gdd_NEWDEL_NEW(clas) void* clas::operator new(size_t size) { \
 int tot; \
 clas *nn,*dn; \
 if(!clas::newdel_freelist) { \
   tot=gdd_CHUNK_NUM; \
   nn=(clas*)malloc(gdd_CHUNK(clas)); \
   gddCleanUp::Add(nn); \
   for(dn=nn;--tot;dn++) dn->newdel_next=dn+1; \
   (dn)->newdel_next=clas::newdel_freelist; \
   clas::newdel_freelist=nn; \
 } \
 if(size==sizeof(clas)) { \
   dn=clas::newdel_freelist; \
   clas::newdel_freelist=clas::newdel_freelist->newdel_next; \
   dn->newdel_next=NULL; \
 } else { \
   dn=(clas*)malloc(size); \
   dn->newdel_next=(clas*)(-1); \
 } \
 return (void*)dn; \
}

class gddCleanUpNode
{
public:
	void* buffer;
	gddCleanUpNode* next;
};

class gddCleanUp
{
public:
	gddCleanUp(void);
	~gddCleanUp(void);

	static void Add(void*);
	static void CleanUp(void);
private:
	static gddCleanUpNode* bufs;
};

#endif

