#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/xattr.h>
#include "selinux_internal.h"
#include "policy.h"

int lsetfilecon_raw(const char *path, const security_context_t context)
{
	int rc = lsetxattr(path, XATTR_NAME_SELINUX, context, strlen(context) + 1,
			 0);
	if (rc < 0 && errno == ENOTSUP) {
		security_context_t ccontext = NULL;
		int err = errno;
		if ((lgetfilecon_raw(path, &ccontext) >= 0) &&
		    (strcmp(context,ccontext) == 0)) {
			rc = 0;
		} else {
			errno = err;
		}
		freecon(ccontext);
	}
	return rc;
}

hidden_def(lsetfilecon_raw)

int lsetfilecon(const char *path, const security_context_t context)
{
	int ret;
	security_context_t rcontext;

	if (selinux_trans_to_raw_context(context, &rcontext))
		return -1;

	ret = lsetfilecon_raw(path, rcontext);

	freecon(rcontext);

	return ret;
}
