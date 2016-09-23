include Makefile.arch

PLOTEVENTCLUSTERS_S = ./src/plotEventClusters.$(SrcSuf)
PLOTEVENTCLUSTERS_O = ./src/plotEventClusters.$(ObjSuf)
PLOTEVENTCLUSTERS_A = ./bin/plotEventClusters$(ExeSuf)
OBJS     += $(PLOTEVENTCLUSTERS_O)
PROGRAMS += $(PLOTEVENTCLUSTERS_A)

TTREETOOLS_S = ./src/TTreeTools.$(SrcSuf)
TTREETOOLS_O = ./src/TTreeTools.$(ObjSuf)

all: $(PROGRAMS)

$(PLOTEVENTCLUSTERS_A): $(PLOTEVENTCLUSTERS_O) $(TTREETOOLS_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PLOTEVENTCLUSTERS_O): $(PLOTEVENTCLUSTERS_S)
	@printf "Compiling test: \"plotEventClusters\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TTREETOOLS_O): $(TTREETOOLS_S)
	@printf "Compiling test: \"TTreeTools\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core


distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)
