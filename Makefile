all: filterTcpDump
LDFLAGS = -L. -ljson_linux-gcc-4.8_libmt
filterTcpDump: CORE/init.cpp
	g++ -o tcpDumpAnalyzer startUp.cpp CORE/init.cpp CORE/init.hpp CORE/sniffing.cpp CORE/sniffing.hpp CORE/databaseManagement.cpp CORE/databaseManagement.hpp CORE/loghandler.cpp CORE/loghandler.hpp CORE/packetDataStructure.hpp CORE/grupPackets.cpp CORE/grupPackets.hpp CORE/buffer.cpp CORE/buffer.hpp CORE/report.cpp CORE/report.hpp CORE/reportStruct.hpp -Wall -Wextra  -std=c++11 -ljson_linux-gcc-4.8_libmt -lsqlite3 -lpcap
clean:
	rm -rf *.o *.hpp.gch 

