sed -i "1,/INSTALL_PREFIX=/s/INSTALL_PREFIX=/INSTALL_PREFIX=INSTPATH/" Makefile && \
sed -i "s,INSTALL_PREFIX=INSTPATH,INSTALL_PREFIX=$INSTPATH,g" Makefile && \
sed -i "s,INSTALLTOP=/usr/local/ssl,INSTALLTOP=\/NIX\/openssl,g" Makefile && \
sed -i "s,OPENSSLDIR=/usr/local/ssl,OPENSSLDIR=\/System\/config\/etc\/ssl,g" Makefile && \
sed -i "s/DIR=\$(OPENSSLDIR)/DIR=\/NIX\/openssl\/share/g" Makefile && \
sync