FROM ubuntu:16.04
MAINTAINER idi0tn3

# make version configurable
ARG SQL_VER=5.7.29
ENV MYSQL_HOME /tmp/mysql-${SQL_VER}
ENV PATH ${PATH}:/usr/local/mysql/bin:/usr/local/mysql/lib

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak && \
mkdir -p /usr/local/boost

ADD ./sources.list /etc/apt/sources.list
ADD ./mysql-${SQL_VER}.tar.gz /tmp/
ADD ./boost_1_59_0.tar.gz /usr/local/boost

VOLUME ["/TinyServer", "/usr/local/mysql/data/"]

# set up tool kit for develop
RUN apt-get update && \
apt-get install -y apt-transport-https ca-certificates && \
apt-get install -y build-essential libssl-dev gcc g++ cmake bison libncurses5-dev pkg-config 

# compile mysql here
RUN mkdir -p /usr/local/mysql && \
cd /tmp/mysql-${SQL_VER} && \
cmake . \
-DCMAKE_INSTALL_PREFIX=/usr/local/mysql \
-DMYSQL_USER=root  \
-DMYSQL_DATADIR=/usr/local/mysql/data  \
-DSYSCONFDIR=/etc  \
-DSYSTEMD_PID_DIR=/usr/local/mysql \
-DWITH_INNOBASE_STORAGE_ENGINE=1  \
-DWITH_ARCHIVE_STORAGE_ENGINE=1  \
-DWITH_BLACKHOLE_STORAGE_ENGINE=1  \
-DWITH_PERFSCHEMA_STORAGE_ENGINE=1  \
-DDEFAULT_CHARSET=utf8  \
-DDEFAULT_COLLATION=utf8_general_ci  \
-DWITH_EXTRA_CHARSETS=all  \
-DWITH_READLINE=1  \
-DOPTIMIZER_TRACE=1  \
-DDOWNLOAD_BOOST=1 \
-DWITH_BOOST=boost_1_59_0 && \
make && \
make install

COPY ./my.cnf /etc/my.cnf
COPY ./startup.sh /tmp/startup.sh

RUN chmod 755 /tmp/startup.sh && \
cp /usr/local/mysql/support-files/mysql.server /etc/init.d/mysql && \
rm -fr /tmp/mysql-${SQL_VER}

EXPOSE 3306
EXPOSE 50000