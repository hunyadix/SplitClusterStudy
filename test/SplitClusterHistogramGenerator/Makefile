include Makefile.arch
SrcSuf = cc

CXX += -std=c++11

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

MODULECLUSTERPLOT_S = ./src/ModuleClusterPlot.$(SrcSuf)
MODULECLUSTERPLOT_O = ./obj/ModuleClusterPlot.$(ObjSuf)
OBJS     += $(MODULECLUSTERPLOT_O)

# PROGRAMS

PLOTEVENTCLUSTERS_S = ./src/plotEventClusters.$(SrcSuf)
PLOTEVENTCLUSTERS_O = ./obj/plotEventClusters.$(ObjSuf)
PLOTEVENTCLUSTERS_A = ./bin/plotEventClusters$(ExeSuf)
OBJS     += $(PLOTEVENTCLUSTERS_O)
PROGRAMS += $(PLOTEVENTCLUSTERS_A)

PLOTCLUSTERSIZES_S = ./src/plotClusterSizes.$(SrcSuf)
PLOTCLUSTERSIZES_O = ./obj/plotClusterSizes.$(ObjSuf)
PLOTCLUSTERSIZES_A = ./bin/plotClusterSizes$(ExeSuf)
OBJS     += $(PLOTCLUSTERSIZES_O)
PROGRAMS += $(PLOTCLUSTERSIZES_A)

PLOTCLUSTERANGLES_S = ./src/plotClusterAngles.$(SrcSuf)
PLOTCLUSTERANGLES_O = ./obj/plotClusterAngles.$(ObjSuf)
PLOTCLUSTERANGLES_A = ./bin/plotClusterAngles$(ExeSuf)
OBJS     += $(PLOTCLUSTERANGLES_O)
PROGRAMS += $(PLOTCLUSTERANGLES_A)

PLOTCLUSTERANGLESPARAMMAP_S = ./src/plotClusterAnglesParamMap.$(SrcSuf)
PLOTCLUSTERANGLESPARAMMAP_O = ./obj/plotClusterAnglesParamMap.$(ObjSuf)
PLOTCLUSTERANGLESPARAMMAP_A = ./bin/plotClusterAnglesParamMap$(ExeSuf)
OBJS     += $(PLOTCLUSTERANGLESPARAMMAP_O)
PROGRAMS += $(PLOTCLUSTERANGLESPARAMMAP_A)

ONHITANGLESCHECK_S = ./src/onHitAnglesCheck.$(SrcSuf)
ONHITANGLESCHECK_O = ./obj/onHitAnglesCheck.$(ObjSuf)
ONHITANGLESCHECK_A = ./bin/onHitAnglesCheck$(ExeSuf)
OBJS     += $(ONHITANGLESCHECK_O)
PROGRAMS += $(ONHITANGLESCHECK_A)

PLOTEVENTCLUSTERSBYDIRECTION_S = ./src/plotEventClustersByDirection.$(SrcSuf)
PLOTEVENTCLUSTERSBYDIRECTION_O = ./obj/plotEventClustersByDirection.$(ObjSuf)
PLOTEVENTCLUSTERSBYDIRECTION_A = ./bin/plotEventClustersByDirection$(ExeSuf)
OBJS     += $(PLOTEVENTCLUSTERSBYDIRECTION_O)
PROGRAMS += $(PLOTEVENTCLUSTERSBYDIRECTION_A)

PLOTSMALLCLUFAKEMERGEFF_S = ./src/plotSmallClusterMergingFakeRateAndEfficiency.$(SrcSuf)
PLOTSMALLCLUFAKEMERGEFF_O = ./obj/plotSmallClusterMergingFakeRateAndEfficiency.$(ObjSuf)
PLOTSMALLCLUFAKEMERGEFF_A = ./bin/plotSmallClusterMergingFakeRateAndEfficiency$(ExeSuf)
OBJS     += $(PLOTSMALLCLUFAKEMERGEFF_O)
PROGRAMS += $(PLOTSMALLCLUFAKEMERGEFF_A)

PLOTSMALLCLUFAKEMERGEFFV2_S = ./src/plotSmallClusterMergingFakeRateAndEfficiency_v2.$(SrcSuf)
PLOTSMALLCLUFAKEMERGEFFV2_O = ./obj/plotSmallClusterMergingFakeRateAndEfficiency_v2.$(ObjSuf)
PLOTSMALLCLUFAKEMERGEFFV2_A = ./bin/plotSmallClusterMergingFakeRateAndEfficiency_v2$(ExeSuf)
OBJS     += $(PLOTSMALLCLUFAKEMERGEFFV2_O)
PROGRAMS += $(PLOTSMALLCLUFAKEMERGEFFV2_A)


all: $(PROGRAMS)

# Executables

$(PLOTEVENTCLUSTERS_A): $(PLOTEVENTCLUSTERS_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O) $(MODULECLUSTERPLOT_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTCLUSTERSIZES_A): $(PLOTCLUSTERSIZES_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
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

$(PLOTCLUSTERANGLESPARAMMAP_A): $(PLOTCLUSTERANGLESPARAMMAP_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(ONHITANGLESCHECK_A): $(ONHITANGLESCHECK_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTEVENTCLUSTERSBYDIRECTION_A): $(PLOTEVENTCLUSTERSBYDIRECTION_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTSMALLCLUFAKEMERGEFF_A): $(PLOTSMALLCLUFAKEMERGEFF_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) -Wall -Wshadow $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTSMALLCLUFAKEMERGEFFV2_A): $(PLOTSMALLCLUFAKEMERGEFFV2_O) $(CONSOLECOLORS_O) $(CONSOLEACTOR_O) $(COMMONACTORS_O) $(TIMER_O) $(TIMERCOL_O) $(TTREETOOLS_O) $(CLUSTERDATATREE_O)
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

$(MODULECLUSTERPLOT_O): $(MODULECLUSTERPLOT_S)
	@printf "Compiling utility: \"ModuleClusterPlot\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

###

$(PLOTEVENTCLUSTERS_O): $(PLOTEVENTCLUSTERS_S)  
	@printf "Compiling test: \"plotEventClusters\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERSIZES_O): $(PLOTCLUSTERSIZES_S)  
	@printf "Compiling test: \"plotClusterSizes\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERANGLES_O): $(PLOTCLUSTERANGLES_S)  
	@printf "Compiling test: \"plotClusterAngles\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTCLUSTERANGLESPARAMMAP_O): $(PLOTCLUSTERANGLESPARAMMAP_S)  
	@printf "Compiling test: \"plotClusterAnglesParamMap\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(ONHITANGLESCHECK_O): $(ONHITANGLESCHECK_S)  
	@printf "Compiling test: \"onHitAnglesCheck\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTEVENTCLUSTERSBYDIRECTION_O): $(PLOTEVENTCLUSTERSBYDIRECTION_S)  
	@printf "Compiling test: \"plotEventClustersByDirection\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PLOTSMALLCLUFAKEMERGEFF_O): $(PLOTSMALLCLUFAKEMERGEFF_S)  
	@printf "Compiling test: \"plotSmallClusterMergingFakeRateAndEfficiency\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"


$(PLOTSMALLCLUFAKEMERGEFFV2_O): $(PLOTSMALLCLUFAKEMERGEFFV2_S)  
	@printf "Compiling test: \"plotSmallClusterMergingFakeRateAndEfficiency_v2\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core


distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)
