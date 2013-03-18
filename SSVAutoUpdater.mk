##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=SSVAutoUpdater
ConfigurationName      :=Release
WorkspacePath          := "D:\Vee\Software\GitHub\OHWorkspace"
ProjectPath            := "D:\Vee\Software\GitHub\OHWorkspace\SSVAutoUpdater"
IntermediateDirectory  :=./INTERMEDIATE
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=vittorio.romeo
Date                   :=18/03/2013
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=./_RELEASE/$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="SSVAutoUpdater.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  -O2
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../SSVEntitySystem $(IncludeSwitch)../SSVStart $(IncludeSwitch)../SSVUtils $(IncludeSwitch)../SSVUtilsJson $(IncludeSwitch)../SSVLuaWrapper $(IncludeSwitch)../SSVMenuSystem $(IncludeSwitch)D:/Vee/Software/GitHub/OHWorkspace/SFML/include $(IncludeSwitch)D:/Vee/Software/GitHub/OHWorkspace/jsoncpp/include $(IncludeSwitch)C:/lua/include $(IncludeSwitch)D:/Vee/Software/Repos/sparsehash/build/built/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)SSVEntitySystem $(LibrarySwitch)SSVStart $(LibrarySwitch)SSVLuaWrapper $(LibrarySwitch)SSVMenuSystem $(LibrarySwitch)SSVUtils $(LibrarySwitch)sfml-window $(LibrarySwitch)sfml-graphics $(LibrarySwitch)sfml-system $(LibrarySwitch)sfml-audio $(LibrarySwitch)sfml-network $(LibrarySwitch)json_mingw_libmt $(LibrarySwitch)lua5.1 $(LibrarySwitch)lua51 $(LibrarySwitch)SSVUtilsJson 
ArLibs                 :=  "SSVEntitySystem" "SSVStart" "SSVLuaWrapper" "SSVMenuSystem" "SSVUtils" "sfml-window" "sfml-graphics" "sfml-system" "sfml-audio" "sfml-network" "json_mingw_libmt" "lua5.1" "lua51" "SSVUtilsJson" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../SSVUtils/_RELEASE $(LibraryPathSwitch)../SSVUtilsJson/_RELEASE $(LibraryPathSwitch)../SSVStart/_RELEASE $(LibraryPathSwitch)../SSVEntitySystem/_RELEASE $(LibraryPathSwitch)../SSVLuaWrapper/_RELEASE $(LibraryPathSwitch)../SSVSCollision/_RELEASE $(LibraryPathSwitch)../SSVMenuSystem/_RELEASE $(LibraryPathSwitch)D:/Vee/Software/GitHub/OHWorkspace/SFML/build2/lib $(LibraryPathSwitch)D:/Vee/Software/GitHub/OHWorkspace/jsoncpp/libs/mingw $(LibraryPathSwitch)c:/lua 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -s -O3 -Wextra -pedantic -W -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
WXWIN:=C:\wxWidgets-2.9.4
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/AutoUpdater$(ObjectSuffix) $(IntermediateDirectory)/Utils_MD5$(ObjectSuffix) $(IntermediateDirectory)/Utils_Utils$(ObjectSuffix) 

Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./INTERMEDIATE"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Vee/Software/GitHub/OHWorkspace/SSVAutoUpdater/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/AutoUpdater$(ObjectSuffix): AutoUpdater.cpp $(IntermediateDirectory)/AutoUpdater$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Vee/Software/GitHub/OHWorkspace/SSVAutoUpdater/AutoUpdater.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AutoUpdater$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AutoUpdater$(DependSuffix): AutoUpdater.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AutoUpdater$(ObjectSuffix) -MF$(IntermediateDirectory)/AutoUpdater$(DependSuffix) -MM "AutoUpdater.cpp"

$(IntermediateDirectory)/AutoUpdater$(PreprocessSuffix): AutoUpdater.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AutoUpdater$(PreprocessSuffix) "AutoUpdater.cpp"

$(IntermediateDirectory)/Utils_MD5$(ObjectSuffix): Utils/MD5.cpp $(IntermediateDirectory)/Utils_MD5$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Vee/Software/GitHub/OHWorkspace/SSVAutoUpdater/Utils/MD5.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Utils_MD5$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Utils_MD5$(DependSuffix): Utils/MD5.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Utils_MD5$(ObjectSuffix) -MF$(IntermediateDirectory)/Utils_MD5$(DependSuffix) -MM "Utils/MD5.cpp"

$(IntermediateDirectory)/Utils_MD5$(PreprocessSuffix): Utils/MD5.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Utils_MD5$(PreprocessSuffix) "Utils/MD5.cpp"

$(IntermediateDirectory)/Utils_Utils$(ObjectSuffix): Utils/Utils.cpp $(IntermediateDirectory)/Utils_Utils$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Vee/Software/GitHub/OHWorkspace/SSVAutoUpdater/Utils/Utils.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Utils_Utils$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Utils_Utils$(DependSuffix): Utils/Utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Utils_Utils$(ObjectSuffix) -MF$(IntermediateDirectory)/Utils_Utils$(DependSuffix) -MM "Utils/Utils.cpp"

$(IntermediateDirectory)/Utils_Utils$(PreprocessSuffix): Utils/Utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Utils_Utils$(PreprocessSuffix) "Utils/Utils.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/AutoUpdater$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/AutoUpdater$(DependSuffix)
	$(RM) $(IntermediateDirectory)/AutoUpdater$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/Utils_MD5$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/Utils_MD5$(DependSuffix)
	$(RM) $(IntermediateDirectory)/Utils_MD5$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/Utils_Utils$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/Utils_Utils$(DependSuffix)
	$(RM) $(IntermediateDirectory)/Utils_Utils$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "../.build-release/SSVAutoUpdater"


