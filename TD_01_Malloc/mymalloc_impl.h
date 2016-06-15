#ifndef _MYMALLOC_IMPL_H_
#define _MYMALLOC_IMPL_H_

/*
Mettez ici toutes les information dont mymalloc.c a besoin mais qui n'ont pas à
être visible du code "client (main_mymalloc*.c).
*/

struct Header {
	size_t nheaders; // taille en nombre d'en-têtes
	struct Header *next; // liste des blocs libres
};

#define HEADER_SZ sizeof(struct Header)

#define MINSYSTBLOCK 8192

#endif

