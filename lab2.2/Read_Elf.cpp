#include "Read_Elf.h"

FILE *elf=NULL;
FILE *fstr = NULL;
Elf64_Ehdr elf64_hdr;

//Program headers
unsigned int padr=0;
unsigned int psize=0;
unsigned int pnum=0;

//Section Headers
unsigned int sadr=0;
unsigned int ssize=0;
unsigned int snum=0;

//Symbol table
unsigned int symnum=0;
unsigned int symadr=0;
unsigned int symsize=0;

//??ָʾ ????????Ƶ?ַ?????ڼ????ڣ???㿪ʼ?????
unsigned int s_index=0;

//????????ļ??????????????.symtab??debug??еķ??ű?
unsigned int stradr=0;

unsigned int int64touint(int64 a, unsigned int *p)
{
	*p = (unsigned int)a.b[0] | (unsigned int)a.b[1] << 8 | (unsigned int)a.b[2] << 16 | (unsigned int)a.b[3] << 24;
	return *p;
}

unsigned int int16touint(int16 a, unsigned int *p)
{
	*p = 0;
	*p |= (unsigned int)a.b[0] | (unsigned int)a.b[1] << 8;
	return *p;
}

unsigned long long int64toull(int64 a, unsigned long long *p)
{
	for(int i = 0; i < 8; ++i)
	{
		*(((unsigned char*)p) + i) = a.b[i];
	}
	return *p;
}

bool open_file(char* file_name)
{
	file = fopen(file_name, "r");
	if(!file)
	{
		printf("No such file.\n");
		return false;
	}
	fstr = fopen(file_name, "r");
	if(!fstr)
	{
		printf("No such file.\n");
		return false;
	}
	elf = fopen("elf.txt", "w");
	if(!elf)
	{
		printf("No output file\n");
		return false;
	}
	return true;
}

bool read_elf(char* file_name)
{
	if(!open_file(file_name))
	{
		return 0;
	}

	fprintf(elf,"ELF Header:\n");
	read_Elf_header();

	fprintf(elf,"\n\nSection Headers:\n");
	read_elf_sections();

	fprintf(elf,"\n\nProgram Headers:\n");
	read_Phdr();

	fprintf(elf,"\n\nSymbol table:\n");
	read_symtable();

	fclose(elf);

	return 1;
}

void read_Elf_header()
{
	//file should be relocated
	//buffer,size,cnt, stream
	fread(&elf64_hdr,1,sizeof(elf64_hdr),file);
		
	fprintf(elf," magic number:  ");
	for(int i = 0; i < 16; ++i)
	{
		fprintf(elf, "%02x ", elf64_hdr.e_ident[i]);
	}
	printf("\n");

	fprintf(elf," Class:  ");
	switch(elf64_hdr.e_ident[4])
	{
		case 1:
			fprintf(elf, "ELF32\n");
			break;
		case 2:
			fprintf(elf, "ELF64\n");
			break;
		default:
			break;
	}
	
	fprintf(elf," Data:  ");
	switch(elf64_hdr.e_ident[5])
	{
		case 1:
			fprintf(elf, "little-endian\n");
			break;
		case 2:
			fprintf(elf, "big-endian\n");
			break;
		default:
			break;
	}
		
	fprintf(elf," Version:                             1 (current)\n");

	fprintf(elf," OS/ABI:	                           System V ABI\n");
	
	fprintf(elf," ABI Version:                         \n");
	
	fprintf(elf," Type:                                \n");
	
	fprintf(elf," Machine:                             \n");

	fprintf(elf," Version:                             0x%x\n", elf64_hdr.e_version);

	int64touint(elf64_hdr.e_entry, &entry);
	fprintf(elf," Entry point address:                 0x%x\n", entry);

	int64touint(elf64_hdr.e_phoff, &padr);
	fprintf(elf," Start of program headers:            %d Bytes into  file\n", padr);

	int64touint(elf64_hdr.e_shoff, &sadr);
	fprintf(elf," Start of section headers:            %d Bytes into  file\n", sadr);

	fprintf(elf," Flags:  0x%x\n", elf64_hdr.e_flags);

	fprintf(elf," Size of this header:                 %d Bytes\n", *(unsigned short*)(elf64_hdr.e_ehsize.b));

	int16touint(elf64_hdr.e_phentsize, &psize);
	fprintf(elf," Size of program headers:             %d Bytes\n", psize);

	int16touint(elf64_hdr.e_phnum, &pnum);
	fprintf(elf," Number of program headers:           %d\n", pnum);

	int16touint(elf64_hdr.e_shentsize, &ssize);
	fprintf(elf," Size of section headers:             %d Bytes\n", ssize);

	int16touint(elf64_hdr.e_shnum, &snum);
	fprintf(elf," Number of section headers:           %d\n", snum);

	int16touint(elf64_hdr.e_shstrndx, &s_index);
	fprintf(elf," Section header string table index:   %d\n", s_index);
	
	fseek(fstr, s_index * ssize + sadr, SEEK_SET);
	Elf64_Shdr elf64_shdr;
	fread(&elf64_shdr, 1, sizeof(elf64_shdr), fstr);
	int64touint(elf64_shdr.sh_offset, &stradr);

	return;	
}

void read_elf_sections()
{

	Elf64_Shdr elf64_shdr;
	char name[20];
	unsigned int sh_name;

	fseek(file, sadr, SEEK_SET);
	for(int c=0;c<snum;c++)
	{
		fprintf(elf," [%3d]\n",c);
		
		//file should be relocated
		fread(&elf64_shdr,1,sizeof(elf64_shdr),file);

		fseek(fstr, stradr + *((unsigned int*)(elf64_shdr.sh_name.b)), SEEK_SET);
		fread(name, 1, 20, fstr);
		fprintf(elf," Name: %20s", name);
		if(strcmp(name, ".text") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &cadr);
			int64touint(elf64_shdr.sh_size, &csize);
			int64touint(elf64_shdr.sh_addr, &vadr);
		}
		else if(strcmp(name, ".data") == 0)
		{
			int64touint(elf64_shdr.sh_addr, &dvadr);
			int64touint(elf64_shdr.sh_offset, &dadr);
			int64touint(elf64_shdr.sh_size, &dsize);
		}
		else if(strcmp(name, ".bss") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &badr);
			int64touint(elf64_shdr.sh_size, &bsize);
			int64toull(elf64_shdr.sh_addr, &gb);
		}
		else if(strcmp(name, ".symtab") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &symadr);
			int64touint(elf64_shdr.sh_size, &symsize);
			symnum = symsize / sizeof(Elf64_Sym);
		}
		else if(strcmp(name, ".sdata") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &sdadr);
			int64touint(elf64_shdr.sh_size, &sdsize);
			int64touint(elf64_shdr.sh_addr, &sdvadr);
		}
		else if(strcmp(name, ".strtab") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &s_index);
		}

		fprintf(elf," Type: %-10d", elf64_shdr.sh_type);

		fprintf(elf," Address:  %-08x", elf64_shdr.sh_addr);

		fprintf(elf," Offest:  %-016x\n", elf64_shdr.sh_offset);

		fprintf(elf," Size:  %-19d", *((long long*)elf64_shdr.sh_size.b));

		fprintf(elf," Entsize:  %-6d", *((long long*)elf64_shdr.sh_entsize.b));

		fprintf(elf," Flags:   %-9x", elf64_shdr.sh_flags);
		
		fprintf(elf," Link:  %-08x", elf64_shdr.sh_link);

		fprintf(elf," Info:  %-08x", elf64_shdr.sh_info);

		fprintf(elf," Align: %-4d\n", *((long long*)elf64_shdr.sh_addralign.b));

 	}
}

void read_Phdr()
{
	Elf64_Phdr elf64_phdr;

	fseek(file, padr, SEEK_SET);
	for(int c=0;c<pnum;c++)
	{
		fprintf(elf," [%3d]\n",c);
			
		//file should be relocated
		fread(&elf64_phdr,1,sizeof(elf64_phdr),file);

		fprintf(elf," Type:   %x", elf64_phdr.p_type);
		
		fprintf(elf," Flags:   %x", elf64_phdr.p_flags);
		
		fprintf(elf," Offset:   %x", elf64_phdr.p_offset);

		fprintf(elf," VirtAddr:  %x", elf64_phdr.p_vaddr);
		
		fprintf(elf," PhysAddr:   %x", elf64_phdr.p_paddr);

		fprintf(elf," FileSiz:   %x", elf64_phdr.p_filesz);

		fprintf(elf," MemSiz:   %x", elf64_phdr.p_memsz);
		
		fprintf(elf," Align:   %x", elf64_phdr.p_align);

	}
}


void read_symtable()
{
	Elf64_Sym elf64_sym;
	char name[40];

	fseek(file, symadr, SEEK_SET);
	for(int c=0;c<symnum;c++)
	{
		fprintf(elf," [%3d]   ",c);
		
		//file should be relocated
		fread(&elf64_sym,1,sizeof(elf64_sym),file);
		fseek(fstr, s_index + *((unsigned int*)(elf64_sym.st_name.b)), SEEK_SET);
		fread(name, 1, 40, fstr);
		fprintf(elf," Name:  %40s   ", name);
		if(strcmp(name, "main") == 0)
		{
			int64touint(elf64_sym.st_value, &madr);
			int64touint(elf64_sym.st_value, &endPC);
			unsigned int st_size;
			int64touint(elf64_sym.st_size, &st_size);
			entry = madr;
			endPC += st_size - 4;
		}
		else if(strcmp(name, "_gp") == 0)
		{
			int64toull(elf64_sym.st_value, &gp);
		}
		/*
		else if(strcmp(name, "_exit") == 0)
		{
			unsigned int st_size;
			int64touint(elf64_sym.st_value, &endPC);
			int64touint(elf64_sym.st_size, &st_size);
			endPC += st_size - 3;
		}*/

		fprintf(elf," Bind:   %4d", elf64_sym.st_info);
		
		fprintf(elf," Type:   %4d", elf64_sym.st_info);
		
		fprintf(elf," NDX:    %4x", elf64_sym.st_shndx);
		
		unsigned int temp = 0;
		fprintf(elf," Size:   %8d", int64touint(elf64_sym.st_size, &temp));

		fprintf(elf," Value:  %8x(%d)\n", elf64_sym.st_value, int64touint(elf64_sym.st_value, &temp));

	}

}


