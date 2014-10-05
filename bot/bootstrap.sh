#!/bin/sh

cat <<EOM >/etc/yum.repos.d/shassard-sbcl.repo
[shassard-sbcl]
name=Copr repo for sbcl owned by shassard
baseurl=http://copr-be.cloud.fedoraproject.org/results/shassard/sbcl/epel-7-\$basearch/
skip_if_unavailable=True
gpgcheck=0
enabled=1
EOM

yum install -y sbcl openssl-devel
yum clean all
