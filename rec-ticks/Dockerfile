FROM green/greenfx-base

MAINTAINER green@spindazzle.org

RUN yum install -y libcurl activemq-cpp && yum clean all

RUN useradd greenfx
ADD rec-ticks /home/greenfx/rec-ticks

CMD rsyslogd && su - greenfx -c './rec-ticks'


