#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define END_OF_FILE -1
#define NBR_FILES_ALLOCATED 1024

typedef struct cursor
{
	int file;
	int is_next;
	int next;
}cursor;

typedef struct file
{
	int fd;
	unsigned int pid;
}file;

typedef struct file_list
{
	file list[NBR_FILES_ALLOCATED];
	int nbr_file_used;
}file_list;

int get_fd(file_list *f, int pid);

void init_file_list(file_list *l)
{
	l->nbr_file_used = 0;
}

void init_cursor(cursor *c)
{
	c->is_next = 0;
}

int get_char(cursor *c)
{
	char a;

	if(c->is_next)
	{
		c->is_next = 0;
		return c->next;
	}

	switch(read(c->file,&a,1))
	{
		case -1:
			perror("read()");
		case 0:
			printf("end of file\n");
			return END_OF_FILE;
		default:
			return a;
	}
}

int next_char(cursor *c)
{
	if(c->is_next)
		return c->next;

	c->next = get_char(c);
	c->is_next = 1;

	return c->next;

}

int wrap_file(file_list *f, int ppid, int pid)
{
	if(f->nbr_file_used > NBR_FILES_ALLOCATED)
	{
		printf("file list full\n");
		return -1;
	}

	f->list[f->nbr_file_used].fd = get_fd(f,ppid);	

	if(f->list[f->nbr_file_used].fd == -1)
	{
		printf("error get_fd()\n");
		return -1;
	}

	f->list[f->nbr_file_used].pid = pid;	
	f->nbr_file_used++;

	return 0;
}

int add_new_file(file_list *f,char *name, int pid)
{
	if(f->nbr_file_used > NBR_FILES_ALLOCATED)
	{
		printf("file list full\n");
		return -1;
	}

	f->list[f->nbr_file_used].fd = open(name,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);	

	if(f->list[f->nbr_file_used].fd == -1)
	{
		perror("open()");
		return -1;
	}

	f->list[f->nbr_file_used].pid = pid;	
	f->nbr_file_used++;

	return 0;
}

int get_fd(file_list *f, int pid)
{
	int i;

	for(i=0;i<f->nbr_file_used;i++)
	{
		if(f->list[i].pid == pid)
			return f->list[i].fd;
	}

	return -1;
}

void free_file_list(file_list *f)
{
	int i;

	for(i=0;i<f->nbr_file_used;i++)
	{
		close(f->list[i].fd);
	}
}

void free_cursor(cursor *c)
{
	close(c->file);
}

int parse_pid(cursor *c)
{
	int pid;

	pid = 0;
	for(;;)
	{
		switch(next_char(c))
		{
			case '\n':
			case '\r':
			case ',':
				return pid;
			case END_OF_FILE:
				return -1;
			default:
				pid = pid*10 + (get_char(c)-'0');
				break;
		}
	}
}

int parse_new_process(cursor *c, file_list *f)
{
	int pid,ppid;
	char name[512];
	int size,is_end,size_pid;
	char a;

	pid = parse_pid(c);

	if(pid == -1)
		return -1;

	get_char(c);// ','
	a = next_char(c);// '"' or 0-9
	printf("nex char %c \n",a);
	if(a == '"')
	{
		get_char(c);// '"'
		size_pid = sprintf(name,"%u_",pid);
		size = size_pid;
		is_end = 0;
		while(!is_end)
		{
			switch(next_char(c))
			{
				case '\\':
					get_char(c);
					size = size_pid;
					break;
				case '"':
					get_char(c);// '"'
					get_char(c);// '\r'
					get_char(c);// '\n'
					is_end = 1;
					break;
				case END_OF_FILE:
					return -1;
				default:
					if(size < (507-size_pid))
					{
						name[size] = get_char(c);
						size++;
					}
					break;
			}
		}

		name[size] = '.';
		name[size+1] = 't';
		name[size+2] = 'x';
		name[size+3] = 't';
		name[size+4] = '\0';
		printf("parsed %s\n",name);
		add_new_file(f,name,pid);
	}else
	{
		ppid = parse_pid(c);
		printf("parsed %u\n",ppid);

		if(ppid == -1)
			return -1;

		get_char(c);// '\r'
		get_char(c);// '\n'

		wrap_file(f,ppid,pid);
	}
	return 0;
}

int parse_new_action(cursor *c, file_list *f)
{
	int fd,pid,a;

	pid = parse_pid(c);

	if(pid == -1)
	{
		printf("here\n");
		return -1;
	}

	fd = get_fd(f,pid);

	if(fd == -1)
	{
		fd = 1;
	}

	get_char(c);// ','

	while(get_char(c) != ','); //skip handler

	for(;;)
	{
		a = get_char(c);
		switch(a)
		{
			case END_OF_FILE:
				return -1;

			case '\r':
				get_char(c);// '\n'
				if(write(fd,"\n",1) != 1)
				{
					perror("write()");
					return -1;
				}
				return 0;

			default :
				if(write(fd,&a,1) != 1)
				{
					perror("write()");
					return -1;
				}
				break;
		}
	}
}

int parse_proc_img(char *file, file_list *f)
{
	cursor c;

	init_cursor(&c);

	c.file = open(file,O_RDONLY);

	if(c.file == -1)
	{
		perror("open()");
		return -1;
	}

	for(;;)
	{
		switch(next_char(&c))
		{
			case END_OF_FILE:
				free_cursor(&c);
				return 0;
			break;
			default:
				if(parse_new_process(&c,f))
					return -1;
			break;
		}
	}
}

int main(int argc, char **argv)
{
	cursor c;
	file_list f;

	init_cursor(&c);
	init_file_list(&f);

	if(argc != 3)
	{
		printf("give me log.txt and pid.txt\n");
		return 0;
	}

	if(parse_proc_img(argv[2],&f))
		return -1;

	c.file = open(argv[1],O_RDONLY);

	if(c.file == -1)
	{
		perror("open(argv[1])");
		return -1;
	}

	for(;;)
	{
		switch(next_char(&c))
		{
			case END_OF_FILE:
				free_file_list(&f);
				free_cursor(&c);
				return 0;
			break;
			default:
				if(parse_new_action(&c,&f))
					return -1;
			break;
		}
	}
}
