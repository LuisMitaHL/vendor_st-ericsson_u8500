#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	DIR *dir;
	struct dirent *dirent;
	long *loc = NULL;
	char **name = NULL;
	int entries = 0;
	int count;

	if (argc != 2)
	{
		printf("Error: incorrect number of arguments\n");
		fflush(NULL);
		exit(1);
	}

	dir = opendir(argv[1]);
	if (!dir)
	{
		perror("opendir");
		fflush(NULL);
		exit(1);
	}

	printf("<<<LEARNING<<<\n");
	do
	{
		entries++;
		loc = realloc(loc, entries * sizeof (long));
		name = realloc(name, entries * sizeof (char *));

		loc[entries - 1] = telldir(dir);

		errno = 0;
		dirent = readdir(dir);
		if (!dirent && errno != 0)
		{
			perror("readdir");
			fflush(NULL);
			exit(1);
		}
		if (dirent)
			name[entries - 1] = strdup(dirent->d_name);
		else
			name[entries - 1] = NULL;

		printf("[% 3d:% 11ld]: '%s'\n",
			entries - 1, loc[entries - 1],
			name[entries - 1]);
	} while (dirent);

	entries++;
	loc = realloc(loc, entries * sizeof (long));
	name = realloc(name, entries * sizeof (char *));

	loc[entries - 1] = telldir(dir);
	name[entries - 1] = NULL;
	printf("[% 3d:% 11ld]: '%s'\n",
		entries - 1, loc[entries - 1],
		name[entries - 1]);
	printf(">>>DONE LEARNING>>>\n");

	if (entries < 2)
	{
		printf("Error: too few directory entries, may cause errors in test...\n");
		fflush(NULL);
		exit(1);
	}
	if (entries > 32)
	{
		printf("Error: too many directory entries, testing will take too much time...\n");
		fflush(NULL);
		exit(1);
	}

	for (count = entries; count > 0; count--)
	{
		int offset;

		for (offset = 0; offset + count - 1 < entries; offset++)
		{
			int first = offset;
			int last = offset + count - 1;
			int entry = first;

			seekdir(dir, loc[entry]);

			printf("<<<%d@%d<<<\n", count, offset);
			for (entry = first; entry <= last; entry++)
			{
				errno = 0;
				dirent = readdir(dir);
				if (!dirent && errno != 0)
				{
					perror("readdir");
					fflush(NULL);
					exit(1);
				}

				printf("[% 3d:% 11ld]'%s' =? '%s'\n",
						entry, loc[entry],
						dirent && dirent->d_name ? dirent->d_name : "",
						name[entry] ? name[entry] : "");

				if (dirent && strcmp(dirent->d_name, name[entry]))
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
				else if (!dirent && name[entry] != NULL)
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
			}

			printf(">>>%d@%d>>>\n", count, offset);
		}
	}

	for (count = entries; count > 0; count--)
	{
		int offset;

		for (offset = 0; offset + count - 1 < entries; offset++)
		{
			int first = offset;
			int last = offset + count - 1;
			int entry = first;

			printf("<<<%d@%d<<<\n", count, offset);
			for (entry = first; entry <= last; entry++)
			{
				seekdir(dir, loc[entry]);

				errno = 0;
				dirent = readdir(dir);
				if (!dirent && errno != 0)
				{
					perror("readdir");
					fflush(NULL);
					exit(1);
				}

				printf("[% 3d:% 11ld]'%s' =? '%s'\n",
						entry, loc[entry],
						dirent && dirent->d_name ? dirent->d_name : "",
						name[entry] ? name[entry] : "");

				if (dirent && strcmp(dirent->d_name, name[entry]))
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
				else if (!dirent && name[entry] != NULL)
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
			}
			printf(">>>%d@%d>>>\n", count, offset);
		}
	}

	for (count = entries; count > 0; count--)
	{
		int offset;

		for (offset = 0; offset + count - 1 < entries; offset++)
		{
			int first = offset;
			int last = offset + count - 1;
			int entry = first;

			printf("<<<%d@%d<<<\n", count, offset);
			for (entry = first; entry <= last; entry++)
			{
				seekdir(dir, loc[last - entry]);

				errno = 0;
				dirent = readdir(dir);
				if (!dirent && errno != 0)
				{
					perror("readdir");
					fflush(NULL);
					exit(1);
				}

				printf("[% 3d:% 11ld]'%s' =? '%s'\n",
						last - entry, loc[last - entry],
						dirent && dirent->d_name ? dirent->d_name : "",
						name[last - entry] ? name[last - entry] : "");

				if (dirent && strcmp(dirent->d_name, name[last - entry]))
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
				else if (!dirent && name[last - entry] != NULL)
				{
					printf("Error: Inconsistent directory entry!");
					fflush(NULL);
					exit(1);
				}
			}
			printf(">>>%d@%d>>>\n", count, offset);
		}
	}

	if (closedir(dir) < 0)
	{
		perror("closedir");
		fflush(NULL);
		exit(1);
	}

	exit(0);
}
