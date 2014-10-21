FROM green/greenfx-base

MAINTAINER green@spindazzle.org

RUN yum install -y libcurl activemq-cpp && yum clean all

RUN useradd oanda
ADD ticks-oanda /home/oanda/ticks-oanda

CMD rsyslogd && su - oanda -c './ticks-oanda'


