#!/usr/bin/env bash
set -euo pipefail

# Set this variable to the base install path
HERMITUX_BASE=/home/pierre/Desktop/hermitux

# These should not need to be modified
PROXY=$HERMITUX_BASE/hermit-compiler/prefix/bin/proxy
KERNEL=$HERMITUX_BASE/hermit-compiler/prefix/x86_64-hermit/extra/tests/hermitux
TMP_FOLDER=/tmp/hermitux-bwrap

# Cleanup
rm -rf $TMP_FOLDER && mkdir $TMP_FOLDER

cp -f $PROXY $TMP_FOLDER
cp -f $KERNEL $TMP_FOLDER
cp -f $1 $TMP_FOLDER

(exec bwrap \
	  --ro-bind /usr /usr \
	  --ro-bind /lib /lib \
	  --ro-bind /lib64 /lib64 \
	  --ro-bind /bin /bin \
	  --ro-bind /sbin /sbin \
	  --dev-bind /dev /dev \
	  --bind $TMP_FOLDER /tmp \
      --dir /var \
      --symlink ../tmp var/tmp \
      --proc /proc \
      --ro-bind /etc/resolv.conf /etc/resolv.conf \
      --chdir / \
      --unshare-all \
      --share-net \
      --die-with-parent \
      --dir /run/user/$(id -u) \
      --setenv XDG_RUNTIME_DIR "/run/user/`id -u`" \
      --setenv PS1 "bwrap-demo$ " \
      --file 11 /etc/passwd \
      --file 12 /etc/group \
	  --setenv HERMIT_ISLE "uhyve" \
	  --setenv HERMIT_TUX "1" \
      /tmp/proxy /tmp/hermitux /tmp/syscall_asm) \
    11< <(getent passwd $UID 65534) \
    12< <(getent group $(id -g) 65534)

# cleanup
rm -rf $TMP_FOLDER
