//=====================================================
// m2d_dir.h
// Low-level directory listing and handling functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2D_DIR_H
#define _M2D_DIR_H   1

#include "m2d_medos.h"


// Consolidated directory entry
typedef struct {
	char name[M2D_EXTNAME_LEN + 1];		// File name (null-termninated)
	uint16_t filenum;					// Logical file number
	uint16_t reserved;					// Reserved file flag
	uint16_t protected;					// Protected file flag
	uint32_t len;						// Length in bytes
	uint32_t ctime;						// Creation time
	uint32_t mtime;						// Modification time
	uint16_t page_tab[M2D_PAGETAB_LEN];	// Pages used by file
} dir_entry_t;


// Forward declarations
//
void m2d_traverse(FILE *f, char *filearg, bool (*callproc)(dir_entry_t *));
bool m2d_lookup_file(FILE *f, char *fn, dir_entry_t *d);

#endif