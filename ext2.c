#include <memory.h>
#include <sys/stat.h>
#include <dirent.h>

#include "ext2.h"

int read_sb(void *part, ext2_sb *sb) 
{
	char *p = part;
	memcpy(sb, &p[1024*1], sizeof(ext2_sb));
}

uint64_t pow2(uint32_t n)
{
	uint64_t p = 1;
	for (int i=0; i<n; i++) p *= 2;
	
	return p;
}

#define N_GR(sb) ((sb)->n_blk/(sb)->blk_per_gr)
#define BLK_SZ(sb) pow2((sb)->blk_sz+10)

static int read_BGDT(void *part, ext2_sb *sb, ext2_bgd *bgd, int idx)
{
	uint32_t ngr = N_GR(sb);
	if (idx > ngr) return -1;
	char *p = part;
	uint32_t table_base = 1024*2; // Table located at byte 2048
	uint32_t bgd_of_idx = table_base + idx*sizeof(ext2_bgd);
	memcpy(bgd, &p[bgd_of_idx], sizeof(ext2_bgd));
}

static int read_inode(void *part, ext2_sb *sb, ext2_inode *ino, int inode)
{
	if (inode < sb->first_ino && inode !=2 /*root*/) return -1; // inode reserved
	
	char *p = part;
	uint32_t blk_gr = (inode-1) / sb->ino_per_gr;
	
	ext2_bgd bgd;
	read_BGDT(part, sb, &bgd, blk_gr);
	
	uint32_t index = (inode -1) % sb->ino_per_gr;
	// uint32_t contain_blk = (index * sb->ino_sz)/BLK_SZ(sb);
	
	memcpy(ino, &p[index*sb->ino_sz + bgd.i_table*BLK_SZ(sb)], sizeof(ext2_inode));
	return 0;
}

#define DIRENT_SZ (sizeof(ext2_dirent)-sizeof(void*))
#define ALIGN4(n) ((n)%4)? ((n)+(4-(n)%4)):(n)


static ext2_dirent *_get_1dirent(void *loc)
{
	static ext2_dirent d;
	char name[256];
	d.name = name;

	memcpy(&d, loc, DIRENT_SZ);
	if (d.inode == 0) return 0; // Should be skipped
	memcpy(d.name, (((char*)loc) + DIRENT_SZ), d.name_len);
	return &d;
}


struct dirent *read_dentry(void *part, ext2_sb *sb, uint32_t inode)
{
	static uint32_t last_inode;
	if (inode == 0) {	// Rewind
		last_inode = 0;
		return 0;
	}

	ext2_inode ino;
	read_inode(part, sb, &ino, inode);
	
	if (! S_ISDIR(ino.mode)) return 0; // Inode is not a directory
	
	static struct dirent dir;
	static uint32_t idx = 0;

	static ext2_dirent *de;
	char *p = part;
	uint32_t blksz = BLK_SZ(sb);
	uint32_t cur_blk_offs = ino.blk_p[idx]*blksz;

	if (last_inode != inode) { // First time huh?
		last_inode = inode; // To determine when ever a inode is read again
		idx = 0;
		dir.d_off = ino.blk_p[idx]*blksz;
	} else {
		if (dir.d_off+dir.d_reclen >= (cur_blk_offs + blksz)) // if over the of block
		{ 	// Looking for a new block pointer in the given inode
			idx++;
			if (idx>=12) { // TODO: expand to indirect pointer
				last_inode = 0;
				return 0; // reach the end
			}
			/* Update new current block offset*/
			cur_blk_offs = ino.blk_p[idx]*blksz;
			dir.d_off = cur_blk_offs;

			if (ino.blk_p[idx] == 0) {
				last_inode = 0;
				return 0; // reach the end
			}
		} else {
			dir.d_off += dir.d_reclen;
			dir.d_off = ALIGN4(dir.d_off);
		}
	}
	
	de = _get_1dirent(&p[dir.d_off]);
	if (!de) {
		last_inode = 0;
		return 0; // reach the end
	}
	/* Copy ext2 dirent to dirent*/
	dir.d_ino = de->inode;
	dir.d_type = de->type;
	dir.d_reclen = de->sz;
	memcpy(dir.d_name, de->name, de->name_len);
	dir.d_name[de->name_len] = 0; // Terminator!
	
	return &dir;
}

int read_content(void *part, ext2_sb *sb, uint32_t ino, void *content)
{
	char *p = part;
	uint32_t blksz = BLK_SZ(sb);
	uint64_t rblk = 0;
	int i;
	//uint64_t sz = (inode.sz_h << 32) | ino->sz_l;

	ext2_inode inode;
	read_inode(part, sb, &inode, ino);

	for (i=0; i<12; i++) {
		if (!inode.blk_p[i]) break;
		memcpy(((char*)content)+rblk, &p[inode.blk_p[i]*blksz], blksz);
		rblk += blksz;
	}

	if (inode.sblk_p) {
		ext2_inode _ino;
		read_inode(p, sb, &_ino,inode.sblk_p);
		rblk += read_content(p, sb, &_ino, ((char*)content)+rblk);
	}

	if (inode.dblk_p) {
		ext2_inode _ino;
		read_inode(p, sb, &_ino,inode.sblk_p);
		rblk += read_content(p, sb, &_ino, ((char*)content)+rblk);
	}

	if (inode.tblk_p) {
		ext2_inode _ino;
		read_inode(p, sb, &_ino,inode.sblk_p);
		rblk += read_content(p, sb, &_ino, ((char*)content)+rblk);
	}
	return rblk;
}
