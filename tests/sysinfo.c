#include <pcl/init.h>
#include <pcl/sysinfo.h>
#include <math.h>

int main(int argc, char **argv)
{
	pcl_sysinfo_t info;

	pcl_init(NULL, 0);
	pcl_sysinfo(&info);

	pcl_printf(
		"cpu_sockets: %d\n"
		"cpu_cores:   %d\n"
		"cpu_model:   %s\n"
		"gmtoff:      %ld\n"
		"tzabbr:      %s\n"
		"hostname:    %s\n"
		"name:        %ts\n"
		"memory:      %dG (%llu)\n"
		"version:     %s\n"
		"vendor:      %s\n",
		info.cpu_sockets,
		info.cpu_cores,
		info.cpu_model,
		info.gmtoff,
		info.tzabbr,
		info.hostname,
		info.name,
		(int)floor(info.memory/1024/1024/1024), info.memory,
		info.version,
		info.vendor
	);

	return 0;
}
