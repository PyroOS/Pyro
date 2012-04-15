# Clear all paths
export PATH=""
export LIBRARY_PATH=""
export PKG_CONFIG_PATH=""
export MANPATH=""
export C_INCLUDE_PATH=""

# Export paths
for COMPONENTDIR in $HOME/Software/NIX /boot/Software/NIX /boot/NIX ; do
	if [ -d "$COMPONENTDIR" ] ; then
		for COMPONENT in `/bin/ls -1 $COMPONENTDIR`; do
			if [ "`/boot/System/bin/attribcheck $COMPONENTDIR/$COMPONENT os::Category`" != "Ignore" ] ; then

				# Export bin path
				if [ -d "$COMPONENTDIR/$COMPONENT/bin" ] ; then
					export PATH=$PATH:$COMPONENTDIR/$COMPONENT/bin
				fi

				# Export sbin path
				if [ -d "$COMPONENTDIR/$COMPONENT/sbin" ] ; then
					export PATH=$PATH:$COMPONENTDIR/$COMPONENT/sbin
				fi

				# Export lib path
				if [ -d "$COMPONENTDIR/$COMPONENT/lib" ] ; then
					export LIBRARY_PATH=$LIBRARY_PATH:$COMPONENTDIR/$COMPONENT/lib
				# Export pkgconfig path
					if [ -d "$COMPONENTDIR/$COMPONENT/lib/pkgconfig" ] ; then
						export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$COMPONENTDIR/$COMPONENT/lib/pkgconfig
				fi fi

				# Export man path
				if [ -d "$COMPONENTDIR/$COMPONENT/share/man" ] ; then
					export MANPATH=$MANPATH:$COMPONENTDIR/$COMPONENT/share/man
				else if [ -d "$COMPONENTDIR/$COMPONENT/man" ] ; then
					export MANPATH=$MANPATH:$COMPONENTDIR/$COMPONENT/man
				fi fi

				# Export include path
				if [ -d "$COMPONENTDIR/$COMPONENT/include" ] ; then
					export C_INCLUDE_PATH=$C_INCLUDE_PATH:$COMPONENTDIR/$COMPONENT/include
				fi

			fi
		done;
	fi
done;

# Export system paths
export PATH=${PATH:1}:/boot/System/nix/bin:/boot/System/nix/sbin:/boot/System/binary
export LIBRARY_PATH=${LIBRARY_PATH:1}:/boot/System/nix/lib:/boot/System/framework:/boot/System/libraries
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH:1}:/boot/System/nix/lib/pkgconfig
export MANPATH=${MANPATH:1}:/boot/System/nix/share/man
export C_INCLUDE_PATH=${C_INCLUDE_PATH:1}:/boot/System/nix/include

# Export duplicate paths
export DLL_PATH=$LIBRARY_PATH:\@bindir\@/.
export CPLUS_INCLUDE_PATH=$C_INCLUDE_PATH

# Development exports
source /boot/System/scripts/development-exports.sh
