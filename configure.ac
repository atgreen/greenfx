AC_PREREQ(2.69)

AC_INIT([greenfx], [0.1], [http://github.com/atgreen/greenfx/issues])

if ! test -f ./config; then
   echo "Copy config.sample to config and edit the contents before running configure"
   exit 1
fi

source ./config

JBOSS_FUSE_ZIP_BASENAME=`basename $JBOSS_FUSE_ZIP`

AC_SUBST(APP_IP_ADDRESS)
AC_SUBST(BOT_POSTGRESQL_PASSWORD)
AC_SUBST(JBOSS_FUSE_ZIP)
AC_SUBST(JBOSS_FUSE_ZIP_BASENAME)
AC_SUBST(JBOSS_FUSE_ADMIN_PASSWORD)
AC_SUBST(RSYSLOG_IP_ADDRESS)

AC_CONFIG_FILES([Makefile \
		 base/bootstrap.sh \
		 log/Makefile log/Dockerfile \
		 db/Makefile db/Dockerfile \
		 fuse/Makefile fuse/Dockerfile fuse/greenfx-configure-fuse.sh \
		 bot/Makefile bot/Dockerfile])
AC_OUTPUT

if ! test -d logs; then
   mkdir logs;
fi

