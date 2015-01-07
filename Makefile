
# Architecture
ARCH          = $(shell root-config --arch)


#------------------------------------------------------------------------------
Classes = Logger Reporter XmlConfig ChainLoader HistoBook TreeAnalyzer HistoAnalyzer XmlConfig TreeMap
HeaderOnlyClasses = LoggerConfig StringUtils Utils ConfigPoint ConfigRange 
##-----------------------------------------------------------------------------

srcDir        = src
incDir        = include/jdb
libDir        = lib
objSuf        = o
srcSuf        = cpp
hdrSuf        = h
exeSuf        =

ifeq ($(ARCH),macosx)
	DllSuf        = dylib
else
	DllSuf        = so
endif

OutPutOpt     = -o 

# Root libs
ROOTCFLAGS    	= $(shell root-config --cflags)
ROOTLDFLAGS    	= $(shell root-config --ldflags)
ROOTLIBS      	= $(shell root-config --libs)
ROOTLDFLAGS    	= $(shell root-config --ldflags)

includes		= -Iinclude -Iinclude/jdb -I. -I$(ROOTSYS)/include -I$(ROOTDEV)/include 

CXX           = g++
CXXFLAGS      = -std=c++0x -fPIC 
LD            = g++
LDFLAGS       = -std=c++0x $(includes)



ifeq ($(ARCH),macosx)
	SOFLAGS       = -dynamiclib -single_module -undefined dynamic_lookup
else
	SOFLAGS       = -shared
endif


CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
LDFLAGS 	+= $(ROOTLDFLAGS)


#------------------------------------------------------------------------------
Dictionary = dictionary
DictionaryObj = $(srcDir)/$(Dictionary).$(objSuf)

ClassSource = $(addsuffix .$(srcSuf), $(addprefix $(srcDir)/, $(Classes)))
ClassHeaders = $(addsuffix .$(hdrSuf), $(addprefix $(incDir)/, $(Classes)))
ClassHeaders += $(addsuffix .$(hdrSuf), $(addprefix $(incDir)/, $(HeaderOnlyClasses)))
ClassObjs = $(addprefix $(srcDir)/, $(addsuffix .$(objSuf), $(Classes)))

#------------------------------------------------------------------------------



jdbLibSo		= $(libDir)/libJDB.$(DllSuf)
jdbLibStatic	= $(libDir)/libJDB.a
sources := $(shell find src -mindepth 1 -maxdepth 4 -name "*.cpp")

.PHONY: $(ClassHeaders)

#Generate the CINT dictionary using rootcint
# inputs : All headers for classes eg XmlConfig.h, Logger.h, etc. and the LinkDef.h needed by rootcint
# Output: dictionary.cxx and dictionary.h
$(srcDir)/$(Dictionary).$(srcSuf): $(ClassHeaders) $(srcDir)/Linkdef.h
	rootcint -v -f $@ -c -p $^

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
	@echo "Generating clDocs";					\
	/Users/danielbrandenburg/bnl/local/cldoc/bin/usr/local/bin/clDoc generate $(includes) $(CXXFLAGS) -- 	\
			--report 							\
			--output html						\
			$(ClassHeaders)
	python /Users/danielbrandenburg/bnl/local/work/boiler/html/sanitize_std.py html/xml/




