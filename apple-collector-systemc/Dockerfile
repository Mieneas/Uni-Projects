FROM gcc:latest

RUN apt-get update && apt-get install -y cmake libgtest-dev libboost-test-dev && rm -rf /var/lib/apt/lists/* 
# RUN = execute during build

RUN wget -O systemc.tar.gz https://accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz \
 && tar -xzf systemc.tar.gz \
 && rm systemc.tar.gz
RUN cd /systemc-2.3.3 \
 && mkdir objdir \
 && cd objdir \
 && ../configure prefix=/usr/local/systemc-2.3.3 \
 && make \
 && make install

ENV LD_LIBRARY_PATH /usr/local/systemc-2.3.3/lib-linux64:$LD_LIBRARY_PATH

# COMMAND = execute when running

ENV SYSTEMC_HOME /usr/local/systemc-2.3.3
