#include <mikmod.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define MIKMODINFO_VERSION "1.0.1"

static void usage(void)
{
	fprintf(stderr,
			"mikmodInfo %s\n"
			"\n"
			"Usage: mikmodInfo <input.mod>\n"
			"  -h, --help              Output this help and exit\n"
			"  -V, --version           Output version and exit\n"
			"\n", MIKMODINFO_VERSION);
	exit(EXIT_FAILURE);
}

char * inputFilePath=0;

static void parse_options(int argc, char **argv)
{
	int i;

	for(i=1;i<argc;i++)
	{
		int lastarg = i==argc-1;

		if(!strcmp(argv[i],"-h") || !strcmp(argv[i], "--help"))
		{
			usage();
		}
		else if(!strcmp(argv[i],"-V") || !strcmp(argv[i], "--version"))
		{
			printf("mikmodInfo %s\n", MIKMODINFO_VERSION);
			exit(EXIT_SUCCESS);
		}
		else
		{
			break;
		}
	}

	argc -= i;
	argv += i;

	if(argc<1)
		usage();

	inputFilePath = argv[0];
}
void segfault_sigaction()
{
	//fprintf(stderr, "libmikmod caused a seg fault. Sadly, this happens.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char ** argv)
{
	MODULE * module;
	struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_sigaction;
    sigaction(SIGSEGV, &sa, NULL);

	parse_options(argc, argv);

	// Regsiter WAV Driver
	MikMod_RegisterDriver(&drv_wav);
	MikMod_RegisterAllLoaders();

	md_device = 1;
	md_mode = DMODE_INTERP | DMODE_16BITS | DMODE_HQMIXER | DMODE_STEREO | DMODE_SIMDMIXER;
	if(MikMod_Init("")!=0)
	{
		fprintf(stderr, "MikMod_Init call failed: %s\n", MikMod_strerror(MikMod_errno));
		exit(EXIT_FAILURE);
	}

	module = Player_Load(inputFilePath, 256, false);
	printf("name:%s\n", module->songname);
	printf("type:%s\n", module->modtype);
	if(module->comment)
		printf("comment:%s\n", module->comment);
	printf("channels:%d\n", module->numchn);
	printf("patterns:%d\n", module->numpat);
	printf("samples:%d\n", module->numsmp);
	Player_Free(module);

	MikMod_Exit();
}
