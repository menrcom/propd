#include<propagated.h>

int main(int argc, char **argv) {

	int exit_code = 0;

	struct cfg_ctx *cfg = configure(argc, argv);

	if(!cfg || !setup(cfg) || cfg->eh->run(cfg))
		exit_code = 2;

	cleanup(cfg);

	return exit_code;
}

/*
 * Local variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 *  c-indent-level: 8
 *  c-basic-offset: 8
 *  tab-width: 8
 * End:
 */
