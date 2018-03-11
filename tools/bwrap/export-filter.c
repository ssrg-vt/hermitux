/**
 * Seccomp BPF export program
 *
 * Copyright (c) 2017 valoq <valoq@mailbox.org>
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 */


/*
 * compile with: gcc exportFilter.c -lseccomp -o exportFilter
 */

#include <seccomp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#define DENY_RULE(call) { if (seccomp_rule_add (ctx, SCMP_ACT_KILL, SCMP_SYS(call), 0) < 0) goto out; }
#define ALLOW_RULE(call) { if (seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS(call), 0) < 0) goto out; }

void print_usage(int arc, char **argv) {
	fprintf(stderr, "Usage: %s <output file>\n", argv[0]);
}

int main(int argc, char *argv[]) {
    int rc = -1;
    scmp_filter_ctx ctx;
    int filter_fd;

	if(argc !=2) {
		print_usage(argc, argv);
		return -1;
	}

    /* for whitelisting */

    /* ctx = seccomp_init(SCMP_ACT_KILL); */
    /* if (ctx == NULL) */
    /* 	goto out; */


    /* for blacklisting */
    
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (ctx == NULL)
	goto out;

    
    /* start of syscall filter list */


    /* common blacklist with dangerous and rarely used syscalls */
    
    DENY_RULE (_sysctl);
    DENY_RULE (acct);
    DENY_RULE (add_key);
    DENY_RULE (adjtimex);
    DENY_RULE (chroot);
    DENY_RULE (clock_adjtime);
    DENY_RULE (create_module);
    DENY_RULE (delete_module);
    DENY_RULE (fanotify_init);
    DENY_RULE (finit_module);
    DENY_RULE (get_kernel_syms);
    DENY_RULE (get_mempolicy);
    DENY_RULE (init_module);
    DENY_RULE (io_cancel);
    DENY_RULE (io_destroy);
    DENY_RULE (io_getevents);
    DENY_RULE (io_setup);
    DENY_RULE (io_submit);
    DENY_RULE (ioperm);
    DENY_RULE (iopl);
    DENY_RULE (ioprio_set);
    DENY_RULE (kcmp);
    DENY_RULE (kexec_file_load);
    DENY_RULE (kexec_load);
    DENY_RULE (keyctl);
    DENY_RULE (lookup_dcookie);
    DENY_RULE (mbind);
    DENY_RULE (nfsservctl);
    DENY_RULE (migrate_pages);
    DENY_RULE (modify_ldt);
    DENY_RULE (mount);
    DENY_RULE (move_pages);
    DENY_RULE (name_to_handle_at);
    DENY_RULE (open_by_handle_at);
    DENY_RULE (perf_event_open);
    DENY_RULE (pivot_root);
    DENY_RULE (process_vm_readv);
    DENY_RULE (process_vm_writev);
    DENY_RULE (ptrace);
    DENY_RULE (reboot);
    DENY_RULE (remap_file_pages);
    DENY_RULE (request_key);
    DENY_RULE (set_mempolicy);
    DENY_RULE (swapoff);
    DENY_RULE (swapon);
    DENY_RULE (sysfs);
    DENY_RULE (syslog);
    DENY_RULE (tuxcall);
    DENY_RULE (umount2);
    DENY_RULE (uselib);
    DENY_RULE (vmsplice);


    /* end of syscall filter list */
    
	    
    filter_fd = open("/tmp/seccomp_filter.bpf", O_CREAT | O_WRONLY, 0644);
    if (filter_fd == -1) {
	rc = -errno;
	goto out;
    }

    rc = seccomp_export_bpf(ctx, filter_fd);
    if (rc < 0) {
	close(filter_fd);
	goto out;
    }
    close(filter_fd);


 out:
    seccomp_release(ctx);
    return -rc;
}
