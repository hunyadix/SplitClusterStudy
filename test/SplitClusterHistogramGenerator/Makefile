include Makefile.arch
SrcSuf = cc

# Utility

CONSOLECOLORS_S = ../../src/ConsoleColors.$(SrcSuf)
CONSOLECOLORS_O = ./obj/ConsoleColors.$(ObjSuf)
OBJS     += $(CONSOLECOLORS_O)

CONSOLEACTOR_S = ../../src/ConsoleActor.$(SrcSuf)
CONSOLEACTOR_O = ./obj/ConsoleActor.$(ObjSuf)
OBJS     += $(CONSOLEACTOR_O)

COMMONACTORS_S = ../../src/CommonActors.$(SrcSuf)
COMMONACTORS_O = ./obj/CommonActors.$(ObjSuf)
OBJS     += $(COMMONACTORS_O)

TIMER_S = ../../TimerForBenchmarking/src/Timer.$(SrcSuf)
TIMER_O = ./obj/Timer.$(ObjSuf)
OBJS     += $(TTREETOOLS_O)

TIMERCOL_S = ../../TimerForBenchmarking/src/TimerColored.$(SrcSuf)
TIMERCOL_O = ./obj/TimerColored.$(ObjSuf)
OBJS     += $(TTREETOOLS_O)

TTREETOOLS_S = ../../src/TTreeTools.$(SrcSuf)
TTREETOOLS_O = ./obj/TTreeTools.$(ObjSuf)
OBJS     += $(TTREETOOLS_O)

CLUSTERDATATREE_S = ../../src/ClusterDataTree.$(SrcSuf)
CLUSTERDATATREE_O = ./obj/ClusterDataTree.$(ObjSuf)
OBJS     += $(TTREETOOLS_O)

# PROGRAMS

PLOTEVENTCLUSTERS_S = ./src/plotEventClusters.$(SrcSuf)
PLOTEVENTCLUSTERS_O = ./obj/plotEventClusters.$(ObjSuf)
PLOTEVENTCLUSTERS_A = ./bin/plotEventClusters$(ExeSuf)
OBJS     += $(PLOTEVENTCLUSTERS_O)
PROGRAMS += $(PLOTEVENTCLUSTERS_A)

PLOTCLUSTERANGLES_S = ./src/plotClusterAngles.$(SrcSuf)
PLOTCLUSTERANGLES_O = ./obj/plotClusterAngles.$(ObjSuf)
PLOTCLUSTERANGLES_A = ./bin/plotClusterAngles$(ExeSuf)
OBJS     += $(PLOTCLUSTERANGLES_O)
PROGRAMS += $(PLOTCLUSTERANGLES_A)

PLOTCLUSTERWEIGHTEDANGLES_S = ./src/plotClusterAnglesAdcWeights.$(SrcSuf)
PLOTCLUSTERWEIGHTEDANGLES_O = ./obj/plotClusterAnglesAdcWeights.$(ObjSuf)
PLOTCLUSTERWEIGHTEDANGLES_A = ./bin/plotClusterAnglesAdcWeights$(ExeSuf)
OBJS     += $(PLOTCLUSTERWEIGHTEDANGLES_O)
PROGRAMS += $(PLOTCLUSTERWEIGHTEDANGLES_A)

PLOTCLUSTERANGLESADCPARAMMAP_S = ./src/plotClusterAnglesEndAdcParamMap.$(SrcSuf)
PLOTCLUSTERANGLESADCPARAMMAP_O = ./obj/plotClusterAnglesEndAdcParamMap.$(ObjSuf)
PLOTCLUSTERANGLESADCPARAMMAP_A = ./bin/plotClusterAnglesEndAdcParamMap$(ExeSuf)
OBJS     += $(PLOTCLUSTERANGLESADCPARAMMAP_O)
PROGRAMS += $(PLOTCLUSTERANGLESADCPARAMMAP_A)

all: $(PROGRAMS)

# Executables

$(PLOTEVENTCLUSTERS_A): $(PLOTEVENTCLUSTERS_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTCLUSTERANGLES_A): $(PLOTCLUSTERANGLES_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTCLUSTERWEIGHTEDANGLES_A): $(PLOTCLUSTERWEIGHTEDANGLES_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTCLUSTERANGLESADCPARAMMAP_A): $(PLOTCLUSTERANGLESADCPARAMMAP_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."


# Obj files

$(CONSOLECOLORS_O): $(CONSOLECOLORS_S)
	@printf "Compiling utility: \"ConsoleColors\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(CONSOLEACTOR_O): $(CONSOLEACTOR_S)
	@printf "Compiling utility: \"ConsoleActor\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(COMMONACTORS_O): $(COMMONACTORS_S)
	@printf "Compiling utility: \"CommonActors\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TIMER_O): $(TIMER_S)
	@printf "Compiling utility: \"TimerColored\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TIMERCOL_O): $(TIMERCOL_S)
	@printf "Compiling utility: \"TimerColored\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TTREETOOLS_O): $(TTREETOOLS_S)
	@printf "Compiling utility: \"TTreeTools\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(CLUSTERDATATREE_O): $(CLUSTERDATATREE_S)
	@printf "Compiling utility: \"ClusterDataTree\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

###

$(PLOTEVENTCLUSTERS_O): $(PLOTEVENTCLUSTERS_S)  
	@printf "Compiling test: \"plotEventClusters\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERANGLES_O): $(PLOTCLUSTERANGLES_S)  
	@printf "Compiling test: \"plotClusterAngles\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERWEIGHTEDANGLES_O): $(PLOTCLUSTERWEIGHTEDANGLES_S)  
	@printf "Compiling test: \"plotClusterAnglesAdcWeights\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERANGLESADCPARAMMAP_O): $(PLOTCLUSTERANGLESADCPARAMMAP_S)  
	@printf "Compiling test: \"plotClusterAnglesEndAdcParamMap\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core


distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)
