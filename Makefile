
# Architecture
ARCH          = $(shell root-config --arch)


#------------------------------------------------------------------------------
Classes = 	Logger Reporter XmlConfig ChainLoader HistoBook RooPlotLib Utils \
			TaskFactory TreeAnalyzer HistoAnalyzer SharedTreeAnalyzer\
			DataSource EvaluatedLeaf \
			XmlFunction FitConfidence\
			format
HeaderOnlyClasses = LoggerConfig XmlPoint XmlRange XmlGraph XmlBinnedData\
					CutCollection \
					TaskRunner IObject IConfig 
##-----------------------------------------------------------------------------

srcDir        = src
incDir        = include/jdb
libDir        = lib
objSuf        = o
srcSuf        = cpp
hdrSuf        = h
exeSuf        =

ifeq ($(ARCH),macosx)
	DllSuf     	= dylib
else
	DllSuf    	= so
endif

OutPutOpt     	= -o 

# Root libs
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTLDFLAGS    	= $(shell root-config --ldflags)

includes		= -Iinclude -Iinclude/jdb -I. 

CXX           	= g++ -o3 -Wall -Wextra -pedantic 
CXXFLAGS      	= -std=c++11 -fPIC 
LD            	= g++
LDFLAGS       	= -std=c++11 $(includes)


ifeq ($(ARCH),macosx64)
	SOFLAGS   	= -dynamiclib -single_module -undefined dynamic_lookup
else
	SOFLAGS    	= -shared
endif


CXXFLAGS     	+= $(ROOTCFLAGS) -DROOT6_FEATURES -DJDB_LOG_LEVEL=60
LIBS          	= $(ROOTLIBS) $(SYSLIBS)
LDFLAGS 		+= $(ROOTLDFLAGS)


#------------------------------------------------------------------------------
Dictionary 		= dictionary
DictionaryObj 	= $(srcDir)/$(Dictionary).$(objSuf)

ClassSource 	= $(addsuffix .$(srcSuf), $(addprefix $(srcDir)/, $(Classes)))
ClassHeaders 	= $(addsuffix .$(hdrSuf), $(addprefix $(incDir)/, $(Classes)))
ClassHeaders 	+= $(addsuffix .$(hdrSuf), $(addprefix $(incDir)/, $(HeaderOnlyClasses)))
ClassObjs 		= $(addprefix $(srcDir)/, $(addsuffix .$(objSuf), $(Classes)))
NoDocs 			= include/jdb/format.h
ClassHeaderDocs = $(filter-out $(NoDocs),$(ClassHeaders))

#------------------------------------------------------------------------------



jdbLibSo		= $(libDir)/libJDB.$(DllSuf)
jdbLibStatic	= $(libDir)/libJDB.a
sources 		:= $(shell find src -mindepth 1 -maxdepth 4 -name "*.cpp")

.PHONY: $(ClassHeaders)

#Generate the CINT dictionary using rootcint
# inputs : All headers for classes eg XmlConfig.h, Logger.h, etc. and the LinkDef.h needed by rootcint
# Output: dictionary.cxx and dictionary.h
$(srcDir)/$(Dictionary).$(srcSuf): $(ClassHeaders) $(srcDir)/Linkdef.h
	rootcling -v -f $@ $(includes) $^ 
	@cp src/dictionary_rdict.pcm lib/

$(srcDir)/%$(objSuf): $(srcDir)/%$(srcSuf)
	$(CXX) $(includes) $(CXXFLAGS) -c $< -o $@

$(DictionaryObj): $(srcDir)/$(Dictionary).$(srcSuf)
	$(CXX)  $(includes) $(CXXFLAGS) -c $< -o $@
	

dll: $(jdbLibSo)

$(jdbLibSo): $(ClassObjs) $(DictionaryObj)
	@echo $(ClassObjs)
	$(LD) $(SOFLAGS) $(LIBS) $(LDFLAGS) $^ $(OutPutOpt) $@

clean:
	@rm -rf $(srcDir)/*.$(objSuf)
	@rm -rf $(srcDir)/$(Dictionary).$(srcSuf)
	@rm -rf $(srcDir)/$(Dictionary).$(hdrSuf)

cleanLib:
	@rm -rf $(libDir)/*

build: $(ClassObjs)
	@echo "Building "


## Generate the static lib
static: $(jdbLibStatic)

$(jdbLibStatic): $(ClassObjs)
	ar rcs $@ $^

doc:
	# @echo "Generating Documentations";			\
	# @echo "HEADERS for DOCS : $(ClassHeaderDocs)\n\n"\
	
	clDoc generate $(includes) $(CXXFLAGS) -- 	\
			--report 							\
			--output html						\
			--merge html/mergedocs/				\
			$(ClassHeaderDocs)
	python /Users/danielbrandenburg/bnl/local/work/boiler/html/sanitize_std.py html/xml/

cleandoc:
	@rm -rf html/xml
	@rm html/search.json


