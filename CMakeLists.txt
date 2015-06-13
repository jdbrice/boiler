cmake_minimum_required(VERSION 3.1)
project(roobarb)

execute_process( COMMAND root-config --ldflags OUTPUT_VARIABLE ROOT_LD_FLAGS)
execute_process( COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SOURCE_FILES
    # Header Files
    include/jdb/ChainLoader.h
    include/jdb/ConfigPoint.h
    include/jdb/ConfigRange.h
    include/jdb/CutSequence.h
    include/jdb/DataSource.h
    include/jdb/EvaluatedLeaf.h
    include/jdb/HistoAnalyzer.h
    include/jdb/HistoBins.h
    include/jdb/HistoBook.h
    include/jdb/Logger.h
    include/jdb/LoggerConfig.h
    include/jdb/RapidXmlWrapper.h
    include/jdb/Reporter.h
    include/jdb/StringUtils.h
    include/jdb/TreeAnalyzer.h
    include/jdb/TreeMap.h
    include/jdb/Utils.h
    include/jdb/XmlConfig.h
    include/rapidXML.h
	include/jdb/RooPlotLib.h

    # Source Files
    src/ChainLoader.cpp
    src/DataSource.cpp
    src/EvaluatedLeaf.cpp
    src/HistoAnalyzer.cpp
    src/HistoBook.cpp
    src/Linkdef.h
    src/Logger.cpp
    src/Engine.cpp
    src/Reporter.cpp
    src/TreeAnalyzer.cpp
    src/TreeMap.cpp
    src/XmlConfig.cpp
    src/RooPlotLib.cpp

    #UnitTesting
    include/UnitTest/UnitTest.h
    src/UnitTest/UnitTest.cpp
    include/UnitTest/TreeAnalyzerUnitTest.h
    src/UnitTest/TreeAnalyzerUnitTest.cpp
    src/UnitTest/HistoBinsUnitTest.cpp)


include_directories($ENV{ROOTSYS}/include)
include_directories(include)
include_directories(include/jdb)

add_executable(roobarb_unit_test ${SOURCE_FILES} )
target_link_libraries(roobarb_unit_test ${ROOT_LIBS})
