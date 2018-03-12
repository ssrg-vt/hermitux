#!/usr/bin/env bash
set -euo pipefail

# Bubblewrap is needed to run uhyve sandboxed, on debian: sudo apt install
# bubblewrap

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
	  --dev-bind /dev/kvm /dev/kvm \
	  --bind $TMP_FOLDER /tmp \
      --unshare-all \
      --die-with-parent \
	  --setenv HERMIT_ISLE "uhyve" \
	  --setenv HERMIT_TUX "1" \
	  --setenv HERMIT_SECCOMP "1" \
      /tmp/proxy /tmp/hermitux /tmp/$1 ${@:2}) \
    11< <(getent passwd $UID 65534) \
    12< <(getent group $(id -g) 65534)

# cleanup
rm -rf $TMP_FOLDER
