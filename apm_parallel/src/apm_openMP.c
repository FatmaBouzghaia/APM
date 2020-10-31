/**
 * APPROXIMATE PATTERN MATCHING
 *
 * INF560 X2016
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>

#define APM_DEBUG 0

char *
read_input_file(char *filename, int *size)
{
	char *buf;
	off_t fsize;
	int fd = 0;
	int n_bytes = 1;

	/* Open the text file */
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "Unable to open the text file <%s>\n", filename);
		return NULL;
	}

	/* Get the number of characters in the textfile */
	fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

#if APM_DEBUG
	printf("File length: %lld\n", fsize);
#endif

	/* Allocate data to copy the target text */
	buf = (char *)malloc(fsize * sizeof(char));
	if (buf == NULL)
	{
		fprintf(stderr, "Unable to allocate %lld byte(s) for main array\n",
						fsize);
		return NULL;
	}

	n_bytes = read(fd, buf, fsize);
	if (n_bytes != fsize)
	{
		fprintf(stderr,
						"Unable to copy %lld byte(s) from text file (%d byte(s) copied)\n",
						fsize, n_bytes);
		return NULL;
	}

#if APM_DEBUG
	printf("Number of read bytes: %d\n", n_bytes);
#endif

	*size = n_bytes;

	close(fd);

	return buf;
}

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2, int len, int *column)
{
	unsigned int x, y, lastdiag, olddiag;

	for (y = 1; y <= len; y++)
	{
		column[y] = y;
	}
	for (x = 1; x <= len; x++)
	{
		column[0] = x;
		lastdiag = x - 1;
		for (y = 1; y <= len; y++)
		{
			olddiag = column[y];
			column[y] = MIN3(
					column[y] + 1,
					column[y - 1] + 1,
					lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return (column[len]);
}

int main(int argc, char **argv)
{
	char **pattern;
	char *filename;
	int approx_factor = 0;
	int nb_patterns = 0;
	int i, j;
	char *buf;
	struct timeval t1, t2;
	double duration;
	int n_bytes;
	int *n_matches;

	/* Check number of arguments */
	if (argc < 4)
	{
		printf("Usage: %s approximation_factor "
					 "dna_database pattern1 pattern2 ...\n",
					 argv[0]);
		return 1;
	}

	/* Get the distance factor */
	approx_factor = atoi(argv[1]);
	/* Grab the filename containing the target text */
	filename = argv[2];

	/* Get the number of patterns that the user wants to search for */
	nb_patterns = argc - 3;

	/* Fill the pattern array */
	pattern = (char **)malloc(nb_patterns * sizeof(char *));
	if (pattern == NULL)
	{
		fprintf(stderr,
						"Unable to allocate array of pattern of size %d\n",
						nb_patterns);
		return 1;
	}

	/* Grab the patterns */
	//#pragma omp parallel for shared(nb_patterns, pattern) private(i)
	for (i = 0; i < nb_patterns; i++)
	{
		int l;

		l = strlen(argv[i + 3]);
		if (l <= 0)
		{
			fprintf(stderr, "Error while parsing argument %d\n", i + 3);
			return 1;
		}

		pattern[i] = (char *)malloc((l + 1) * sizeof(char));
		if (pattern[i] == NULL)
		{
			fprintf(stderr, "Unable to allocate string of size %d\n", l);
			return 1;
		}

		strncpy(pattern[i], argv[i + 3], (l + 1));
	}

	printf("Approximate Pattern Mathing: "
				 "looking for %d pattern(s) in file %s w/ distance of %d\n",
				 nb_patterns, filename, approx_factor);

	buf = read_input_file(filename, &n_bytes);
	if (buf == NULL)
	{
		return 1;
	}

	/* Allocate the array of matches */
	n_matches = (int *)malloc(nb_patterns * sizeof(int));
	if (n_matches == NULL)
	{
		fprintf(stderr, "Error: unable to allocate memory for %ldB\n",
						nb_patterns * sizeof(int));
		return 1;
	}

	/*****
   * BEGIN MAIN LOOP
   ******/
	/* Timer start */
	gettimeofday(&t1, NULL);
	omp_set_num_threads(6);

	int size_pattern;
	int *column;

#pragma omp parallel for shared(nb_patterns, n_bytes) private(j, size_pattern, column, i)
	for (i = 0; i < nb_patterns; i++)
	{
		printf("In patterns loop: rank = %d and i = %d\n", omp_get_thread_num(), i);
		size_pattern = strlen(pattern[i]);

		n_matches[i] = 0;
		int n_match = 0;

		int distance;
		int size;

		for (j = 0; j < n_bytes; j++)
		{
			// printf("Rank = %d and j = %d\n", omp_get_thread_num(), j);
			column = (int *)malloc((size_pattern + 1) * sizeof(int));
			if (column == NULL)
			{
				fprintf(stderr, "Error: unable to allocate memory for column (%ldB)\n", (size_pattern + 1) * sizeof(int));
			}

#if APM_DEBUG
			if (j % 100 == 0)
			{
				printf("Procesing byte %d (out of %d)\n", j, n_bytes);
			}
#endif
			distance = 0;
			size = size_pattern;
			if (n_bytes - j < size_pattern)
			{
				size = n_bytes - j;
			}

			distance = levenshtein(pattern[i], &buf[j], size, column) + size_pattern - size;

			if (distance <= approx_factor)
			{
				n_match++;
			}
			free(column);
		}
		n_matches[i] += n_match;
		//printf("pattern[%d] = %s \n", i, pattern[i]);
	}

	/* Timer stop */
	gettimeofday(&t2, NULL);

	duration = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec) / 1e6);

	printf("APM done in %lf s\n", duration);

	/*****
   * END MAIN LOOP
   ******/
	for (i = 0; i < nb_patterns; i++)
	{
		printf("Number of matches for pattern <%s>: %d\n",
					 pattern[i], n_matches[i]);
	}

	return 0;
}