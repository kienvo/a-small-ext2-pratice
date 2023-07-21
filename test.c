#include <stdio.h>

#include "ext2.h"

int main() 
{
	FILE *f = fopen("initrd.img", "r");
	fseek(f, 0, SEEK_END);
	long fsz = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	void *part = malloc(fsz);
	fread(part, fsz, 1, f);
	fclose(f);

	ext2_sb sb;
	ext2_bgd bgd0;
	ext2_bgd bgd1;
	ext2_inode ino;
	ext2_dirent de;

	read_sb(part, &sb);

	struct dirent *d;
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);

	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 0);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 15);
	d = read_dentry(part, &sb, 15);
	d = read_dentry(part, &sb, 15);
	d = read_dentry(part, &sb, 15);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	d = read_dentry(part, &sb, 2);
	
	char *content = malloc(1024);
	read_content(part, &sb, 2, content);
	read_content(part, &sb, 16, content);

	return 0;
}