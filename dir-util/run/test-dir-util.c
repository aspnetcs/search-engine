#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "mhook/mhook.h"
#include "dir-util.h"

int foreach_file_callbk(const char *filename, void *arg)
{
	char *path = (char*) arg;
	char *ext = filename_ext(filename);

	printf("%s/%s (ext:`%s')\n", path, filename,
	       (ext == NULL) ? "null": ext);
	return 0;
}

enum ds_ret
dir_search_callbk(const char* path, const char *srchpath,
                  uint32_t level, void *arg)
{
	char *test_str = (char *)arg;
	printf("%s@ level %u\n", test_str, level);
	printf("path=%s (srchpath=%s):\n", path, srchpath);

	foreach_files_in(path, &foreach_file_callbk, (void*)path);
	printf("\n");

	if (level > 3) {
		printf("max search level reached, terminate.\n");
		return DS_RET_STOP_ALLDIR;
	} else {
		return DS_RET_CONTINUE;
	}
}

int main(int argc, char* argv[])
{
	int opt;
	char *path = NULL;
	char test_str[] = "test string";

	mkdir_p("./tmp/1/2/3");

	/* handle program arguments */
	while ((opt = getopt(argc, argv, "hp:")) != -1) {
		switch (opt) {
		case 'h':
			printf("DESCRIPTION:\n");
			printf("test filesys functions given a path. \n");
			printf("\n");
			printf("USAGE:\n");
			printf("%s -h | -p <path>\n", argv[0]);
			goto exit;
		
		case 'p':
			path = strdup(optarg);
			break;

		default:
			printf("bad argument(s). \n");
			goto exit;
		}
	}

	if (path) {
		printf("index at %s\n", path);
	} else {
		printf("no path specified.\n");
		goto exit;
	}

	printf("path %s ", path);
	if (dir_exists(path))
		printf("is directory");
	else
		printf("is not directory");
	printf("\n");

	printf("path %s ", path);
	if (file_exists(path))
		printf("is regular file.");
	else
		printf("is not regular file.");
	printf("\n");

	if (dir_exists(path)) {
		printf("[DFS ...]\n");
		dir_search_podfs(path, &dir_search_callbk, test_str);
		printf("\n");

		printf("[BFS ...]\n");
		dir_search_bfs(path, &dir_search_callbk, test_str);
		printf("\n");
	}

	free(path);
exit:

	mhook_print_unfree();
	return 0;
}
