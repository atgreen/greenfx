FROM green/greenfx-base

MAINTAINER green@spindazzle.org

ADD influxdb-latest-1.x86_64.rpm /root/influxdb-latest-1.x86_64.rpm
RUN rpm -hiv /root/influxdb-latest-1.x86_64.rpm
RUN mkdir /home/influxdb && chown influxdb.influxdb /home/influxdb

EXPOSE 8086 8083

CMD rsyslogd && su - influxdb -c "influxdb -config=/opt/influxdb/shared/config.toml"


