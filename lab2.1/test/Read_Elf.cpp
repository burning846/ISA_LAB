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

void int64touint(int64 a, unsigned int *p)
{
	*p = (unsigned int)a.b[0] | (unsigned int)a.b[1] << 8 | (unsigned int)a.b[2] << 16 | (unsigned int)a.b[3] << 24;
	return;
}

void int16touint(int16 a, unsigned int *p)
{
	*p = 0;
	*p |= (unsigned int)a.b[0] | (unsigned int)a.b[1] << 8;
	return;
}

void int64toull(int64 a, unsigned long long *p)
{
	for(int i = 0; i < 8; ++i)
	{
		*((unsigned char*)p + i) = a.b[i];
	}
	return;
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

void read_elf(char* file_name)
{
	if(!open_file(file_name))
	{
		return;
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
}

void read_Elf_header()
{
	//file should be relocated
	//buffer,size,cnt, stream
	fread(&elf64_hdr,1,sizeof(elf64_hdr),file);
		
	fprintf(elf," magic number:  ");
	for(int i = 0; i < 16; ++i)
	{
		fprintf(elf, "%x ", elf64_hdr.e_ident[i]);
	}

	fprintf(elf," Class:  ELFCLASS32\n");
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
	
	fprintf(elf," Data:  little-endian\n");
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
		
	fprintf(elf," Version:   1 (current)\n");

	fprintf(elf," OS/ABI:	 System V ABI\n");
	
	fprintf(elf," ABI Version:   \n");
	
	fprintf(elf," Type:  ");
	
	fprintf(elf," Machine:   \n");

	fprintf(elf," Version:  0x%x\n", elf64_hdr.e_version);

	int64touint(elf64_hdr.e_entry, &entry);
	fprintf(elf," Entry point address:  0x%x\n", entry);

	int64touint(elf64_hdr.e_phoff, &padr);
	fprintf(elf," Start of program headers:  %d  bytes into  file\n", padr);

	int64touint(elf64_hdr.e_shoff, &sadr);
	fprintf(elf," Start of section headers:  %d  bytes into  file\n", sadr);

	fprintf(elf," Flags:  0x%x\n", elf64_hdr.e_flags);

	fprintf(elf," Size of this header:   %d Bytes\n", elf64_hdr.e_ehsize);

	int16touint(elf64_hdr.e_phentsize, &psize);
	fprintf(elf," Size of program headers:   %d Bytes\n", psize);

	int16touint(elf64_hdr.e_phnum, &pnum);
	fprintf(elf," Number of program headers:   %d\n", pnum);

	int16touint(elf64_hdr.e_shentsize, &ssize);
	fprintf(elf," Size of section headers:    %d Bytes\n", ssize);

	int16touint(elf64_hdr.e_shnum, &snum);
	fprintf(elf," Number of section headers:  %d\n", snum);

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
		fprintf(elf," Name: %s", name);
		if(strcmp(name, ".text") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &cadr);
			int64touint(elf64_shdr.sh_size, &csize);
			int64touint(elf64_shdr.sh_addr, &vadr);
		}
		else if(strcmp(name, ".data") == 0)
		{
			int64toull(elf64_shdr.sh_addr, &gp);
		}
		else if(strcmp(name, ".symtab") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &symadr);
			int64touint(elf64_shdr.sh_size, &symsize);
			symnum = symsize / sizeof(Elf64_Sym);
			printf("%x, %x, %d\n", symadr, symsize, symnum);
		}
		else if(strcmp(name, ".strtab") == 0)
		{
			int64touint(elf64_shdr.sh_offset, &s_index);
		}

		fprintf(elf," Type: %d", elf64_shdr.sh_type);

		fprintf(elf," Address:  %x", elf64_shdr.sh_addr);

		fprintf(elf," Offest:  %x\n", elf64_shdr.sh_offset);

		fprintf(elf," Size:  %x", elf64_shdr.sh_size);

		fprintf(elf," Entsize:  %x", elf64_shdr.sh_entsize);

		fprintf(elf," Flags:   %x", elf64_shdr.sh_flags);
		
		fprintf(elf," Link:  %x", elf64_shdr.sh_link);

		fprintf(elf," Info:  %x", elf64_shdr.sh_info);

		fprintf(elf," Align: %x\n", elf64_shdr.sh_addralign);

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
		}
		else if(strcmp(name, "_exit") == 0)
		{
			unsigned int st_size;
			int64touint(elf64_sym.st_value, &endPC);
			int64touint(elf64_sym.st_size, &st_size);
			endPC += st_size;
		}

		fprintf(elf," Bind:   %d", elf64_sym.st_info);
		
		fprintf(elf," Type:   %d", elf64_sym.st_info);
		
		fprintf(elf," NDX:   %x", elf64_sym.st_shndx);
		
		fprintf(elf," Size:   %x", elf64_sym.st_size);

		fprintf(elf," Value:   %x\n", elf64_sym.st_value);

	}

}


