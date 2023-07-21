#ifndef __EXT2_H__
#define __EXT2_H__

#include <stdint.h>

typedef struct ext2_sb
{
	uint32_t n_ino; 	// Total number of inodes in file system
	uint32_t n_blk; 	// Total number of blocks in file system
	uint32_t n_rsv; 	// Number of blocks reserved for superuser (see offset 80)
	uint32_t n_ublk; 	// Total number of unallocated blocks
	uint32_t n_uino; 	// Total number of unallocated inodes
	uint32_t sp_blknum; // Block number of the block containing the superblock (also the starting block number, NOT always zero.)
	uint32_t blk_sz; 	// = log2(block size) - 10; ex. = 10 mean block size = 1024
	uint32_t frag_sz; 	// = log2(fragment size) - 10;
	uint32_t blk_per_gr;	// Number of blocks in each block group
	uint32_t frag_per_gr;	// Number of fragments in each block group
	uint32_t ino_per_gr;	// Number of inodes in each block group
	uint32_t last_mnt;	// Last mount time (in POSIX time)
	uint32_t last_wr;	// Last written time (in POSIX time)
	uint16_t n_mnt;		// Number of times the volume has been mounted since its last consistency check (fsck)
	uint16_t n_mnt_allow; // Number of mounts allowed before a consistency check (fsck) must be done
	uint16_t sign;		// Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume
	uint16_t fs_state;	// File system state, 1 clean, 2 errors.
	uint16_t err_bh;	// What to do when an error is detected. 1 ignore, 2 remount as read only, 3 kernel panic
	uint16_t min_ver;	// Minor portion of version (combine with Major portion below to construct full version field)
	uint32_t last_chk;	// POSIX time of last consistency check (fsck)
	uint32_t interval_chk; // Interval (in POSIX time) between forced consistency checks (fsck)
	uint32_t os_id;		// Operating system ID from which the filesystem on this volume was created (see below)
	uint32_t major_ver;	// Major portion of version (combine with Minor portion above to construct full version field)
	uint16_t uid;		// User ID that can use reserved blocks
	uint16_t gid;		// Group ID that can use reserved blocks
	/* Extended Superblock Fields */
	uint32_t first_ino; // First non-reserved inode in file system. (In versions < 1.0, this is fixed as 11)
	uint16_t ino_sz; 	// Size of each inode structure in bytes. (In versions < 1.0, this is fixed as 128)
	uint16_t sb_bak; 	// Block group that this superblock is part of (if backup copy)
	uint32_t features; 	// Optional features present (features that are not required to read or write, but usually result in a performance increase. see below)
	uint32_t req_feat; 	// Required features present (features that are required to be supported to read or write. see below)
	uint32_t nofeat; 	// Features that if not supported, the volume must be mounted read-only see below)
	uint8_t fs_id[16]; 	// File system ID (what is output by blkid)
	uint8_t vol_name[16]; // Volume name (C-style string: characters terminated by a 0 byte)
	uint8_t last_path[64];// Path volume was last mounted to (C-style string: characters terminated by a 0 byte)
	uint32_t compress; 	// Compression algorithms used (see Required features above)
	uint8_t	blk_file; 	// Number of blocks to preallocate for files
	uint8_t	blk_dir; 	// Number of blocks to preallocate for directories
	uint16_t unused; 	// (Unused)
	uint8_t jour_id; 	// Journal ID (same style as the File system ID above)
	uint32_t jour_ino; 	// Journal inode
	uint32_t jour_dev; 	// Journal device
	uint32_t head_ino; 	// Head of orphan inode list
} ext2_sb;

typedef struct ext2_bgd
{
	uint32_t bl_addr;	// Block address of block usage bitmap
	uint32_t i_addr;	// Block address of inode usage bitmap
	uint32_t i_table;	// Starting block address of inode table
	uint16_t n_ublk;	// Number of unallocated blocks in group
	uint16_t n_uino;	// Number of unallocated inodes in group
	uint16_t n_dir;	// Number of directories in group
	uint8_t  unused[14];
} ext2_bgd;

typedef struct ext2_inode
{
	uint16_t mode; // Type and Permissions
	uint16_t uid; // User ID
	uint32_t sz_l; // Lower 32 bits of size in bytes
	uint32_t last_acce; // Last Access Time (in POSIX time)
	uint32_t created; // Creation Time (in POSIX time)
	uint32_t last_mod; // Last Modification time (in POSIX time)
	uint32_t del_time; // Deletion time (in POSIX time)
	uint16_t gid; // Group ID
	uint16_t nlink; // Count of hard links (directory entries) to this inode. When this reaches 0, the data blocks are marked as unallocated.
	uint32_t nsector; // Count of disk sectors (not Ext2 blocks) in use by this inode, not counting the actual inode structure nor directory entries linking to the inode.
	uint32_t flags; // Flags
	uint32_t os_val1; // Operating System Specific value #1
	uint32_t blk_p[12]; // Direct Block Pointer [0-11]
	uint32_t sblk_p; // Singly Indirect Block Pointer (Points to a block that is a list of block pointers to data)
	uint32_t dblk_p; // Doubly Indirect Block Pointer (Points to a block that is a list of block pointers to Singly Indirect Blocks)
	uint32_t tblk_p; // Triply Indirect Block Pointer (Points to a block that is a list of block pointers to Doubly Indirect Blocks)
	uint32_t gen_num; // Generation number (Primarily used for NFS)
	uint32_t ext_att; // In Ext2 version 0, this field is reserved. In version >= 1, Extended attribute block (File ACL).
	uint32_t sz_h; // In Ext2 version 0, this field is reserved. In version >= 1, Upper 32 bits of file size (if feature bit set) if it's a file, Directory ACL if it's a directory
	uint32_t frag_addr; // Block address of fragment
	char os_val2[12]; // Operating System Specific Value #2
} ext2_inode;

typedef struct ext2_dirent
{
	uint32_t inode;	//	Inode
	uint16_t sz;	//	Total size of this entry (Including all subfields)
	uint8_t	name_len;//	Name Length least-significant 8 bits
	
	/**	0	Unknown type
		1	Regular file
		2	Directory
		3	Character device
		4	Block device
		5	FIFO
		6	Socket
		7	Symbolic link (soft link)	 */
	uint8_t	type;//	Type indicator (only if the feature bit for "directory entries have file type byte" is set, else this is the most-significant 8 bits of the Name Length)
	char *name; // Name characters
} ext2_dirent;


int read_sb(void *part, ext2_sb *sb);


/**
 * @brief 		Get directory entry
 * 
 * @param		part
 * @param		sb
 * @param		inode the directory inode need to read. If inode = 0, rewind.
 * @return		struct dirent* 
 */
struct dirent *read_dentry(void *part, ext2_sb *sb, uint32_t inode);

/**
 * @brief 		Read content of all inode types, even directory
 * @return 		number of blocks has read
 */
int read_content(void *part, ext2_sb *sb, uint32_t ino, void *content);




#endif /* __EXT2_H__ */
