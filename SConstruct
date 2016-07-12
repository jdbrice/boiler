#!python
import subprocess
import os

LIB_NAME 		= "lib/RooBarb"

ROOTCFLAGS    	= subprocess.check_output( ['root-config',  '--cflags'] ).rstrip().split( " " )
ROOTLDFLAGS    	= subprocess.check_output( ["root-config",  "--ldflags"] )
ROOTLIBS      	= subprocess.check_output( ["root-config",  "--libs"] )
ROOTGLIBS     	= subprocess.check_output( ["root-config",  "--glibs"] )
ROOTLIBPATH 	= subprocess.check_output( ["root-config", "--libdir" ] )
ROOT_SYS 		= os.environ[ "ROOTSYS" ]
JDB_LIB			= os.environ[ "JDB_LIB" ]


cppDefines 		= {}
cppFlags 		= ['-Wall' ]#, '-Werror']
cxxFlags 		= ['-std=c++11', '-fPIC' ]
cxxFlags.extend( ROOTCFLAGS )

paths 			= [ '.', 			# dont really like this but ended up needing for root dict to work ok
					'include', 
					'include/jdb',
					ROOT_SYS + "/include" 
					]

vars = Variables()
vars.Add('RELEASE', 'Set to 1 to build for release', 0)
vars.Add('LL', 'LogLevel : ALL = 60, DEBUG = 50, TRACE = 40, INFO = 30, WARN = 20, ERROR = 10, NONE = 0', 60)
vars.Add( 'ROOT6', 'Support ROOT6?', 1 )

# RELEASE = ARGUMENTS.get(  )
# jdb_log_level = ARGUMENTS.get( "ll", 60 )


common_env = Environment( variables = vars )
Help(vars.GenerateHelpText(common_env))

print "RELEASE =", common_env[ "RELEASE" ]
if int(common_env[ "RELEASE" ]) >= 1 :
	common_env[ "LL" ] = 30


common_env.Append(CPPDEFINES 	= cppDefines)
common_env.Append(CPPFLAGS 		= cppFlags)
common_env.Append(CXXFLAGS 		= cxxFlags)
common_env.Append(LINKFLAGS 	= cxxFlags ) 
common_env.Append(CPPPATH		= paths)
# Add the root library flags directly in
common_env[ "_LIBFLAGS" ] = common_env[ "_LIBFLAGS" ] + " " + ROOTLIBS + " "

common_env.Append(CXXFLAGS 		= "-DJDB_LOG_LEVEL=${LL}" )
common_env.Append(CXXFLAGS 		= "-DROOT6=${ROOT6}" )

target = common_env.StaticLibrary( target='lib/RooBarb', source=[Glob( "src/*.cpp" )] )
Default( target )




########################### ROOT dictionary creation ##########################
if "LD_LIBRARY_PATH" in os.environ :
	LD_LIBRARY_PATH = os.environ[ "LD_LIBRARY_PATH" ]
else :
	LD_LIBRARY_PATH = ""
rootcint_env = Environment(ENV = {'PATH' : os.environ['PATH'], 'ROOTSYS' : os.environ[ "ROOTSYS" ], 'LD_LIBRARY_PATH' : LD_LIBRARY_PATH })
rootcint = Builder( action='rootcint -f $TARGET -c $_CPPINCFLAGS $SOURCES.file' )  
rootcint_env.Append( BUILDERS 		= { 'RootCint' : rootcint } )
# hack to make the rootcint use abs path to headers
rootcint_env[ "_CPPINCFLAGS" ] = str( " -I" + Dir(".").abspath + "/src/" ) + str( " -I" + Dir(".").abspath + "/include/" )

root_dict_src = rootcint_env.RootCint( "src/CintDictionary.cpp", [Glob( "include/*.h" ), Glob( "src/Linkdef.h" ) ] )
# Clean( root_dict, "src/TreeData/CintDictionary_rdict.pcm" )

rootcint_env.Alias( 'rootcint', root_dict_src )
rootcint_env[ "_LIBFLAGS" ] = common_env[ "_LIBFLAGS" ] + " " + ROOTLIBS + " "
rootcint_env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1
root_cint_obj = rootcint_env.Object( target='CintDictionary', source=["src/CintDictionary.cpp"] )
root_cint_lib = rootcint_env.SharedLibrary( target='lib/RooBarb', source=[Glob( "src/*.o" )] )
Depends( root_cint_lib, target )
rootcint_env.Alias( 'dll', root_cint_lib )



