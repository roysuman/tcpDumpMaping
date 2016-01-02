
#TCP_DUMP_MAPPING
 This is a light weight tool which analyze the sequential servers dump file to pinpoint the bottleneck.
 
#Introduction
This tool takes input of several tcpdump file and analyze those file to find bottleneck.
 - Scenario
   Suppose you are trying to reach a server( say example.com). To connect example.com all network level packets need to cross a proxy server and then    that packet can hit example.com. Server example.com is connected to 5 DB servers. Now you are experiencing very high response time for your request.
   You simply collect tcp dump files from all servers under your request/load. Then analyze those packets using this tool. This tool will provide you    the network level behavior of all servers during your desire transaction.[ You might send a single request which providing 5 response packets..during this communication window what kind of network level transactions are getting performed by rest of the servers]

#Pre-requisites
 - c++11
 - [jsoncpp][2]
 - [pcap][1]
 - [sqlite3][3]
[1]:http://www.tcpdump.org/manpages/pcap.3pcap.html
[2]:https://github.com/open-source-parsers/jsoncpp
[3]:https://www.sqlite.org/

#Getting Started

There is make file just execute the following commands.
 - make
 - ./tcpDumpAnalyzer`

#Configuration:

 - environment.config 
  1. enable info log by turn info = true.
  2. port: the port number you are trying to send/recv packet.
  3. The ip of the server you are trying to connect.
  4. No of tcp dump file, followed by file path
  5. Delay: You can filter those packet which has taken conversation time > delay.
  6. Name of the result file.

