
#include <pcl/process.h>
#include <pcl/file.h>
#include <pcl/time.h>

int pcl_main(int argc, tchar_t **argv)
{
	pcl_proc_exec_t exec = {0};

	pcl_init(NULL, 0);

	if(argc < 2)
	{
		fprintf(stderr, "requires one argument, the command to exec: "
			"Ex. `exec \"ls -l\"`\n");

		return 1;
	}

	exec.command = argv[1];

	int flags = PCL_PREX_NONBLOCK | PCL_PREX_STDOUT | PCL_PREX_STDERR;

	if(pcl_proc_exec(&exec, flags))
	{
		PTRACE("pcl_proc_exec failed", 0);
		return 1;
	}

	int r;
	char buf[16 * 1024];

	while((r = pcl_tryread(exec.f_stdout, buf, sizeof(buf), 2000)))
	{
		if(r == -1)
		{
			if(pcl_errno == PCL_EPIPE)
				break;

			TRC();
			pcl_err_fprintf(stderr, 0, NULL);
			if(pcl_errno != PCL_EAGAIN)
			{
				exit(1);
			}

			pcl_printf("\nsleeping...\n");
			pcl_sleep(500);
		}
		else
		{
			printf("%.*s", r, buf);
		}
	}

	pcl_close(exec.f_stdout);

	while((r = pcl_read(exec.f_stderr, buf, sizeof(buf))) > 0)
	{
		pcl_fprintf(stderr, "%.*s", r, buf);
	}

	pcl_close(exec.f_stderr);

	return 0;
}
