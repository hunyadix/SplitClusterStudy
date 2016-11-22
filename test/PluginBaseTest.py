import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process("SplitClusterMergerPluginTest")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.Geometry.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# Message logger service
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
	destinations = cms.untracked.vstring('cerr'),
	cerr = cms.untracked.PSet(threshold  = cms.untracked.string('DEBUG')),
	debugModules = cms.untracked.vstring('SplitClusterAnalyzerPlugin'))

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))
process.source = cms.Source("PoolSource",
	# fileNames = cms.untracked.vstring('file:./GENSIMRECO.root'),
	fileNames = cms.untracked.vstring(
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_1.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_10.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_11.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_12.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_13.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_14.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_15.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_16.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_17.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_18.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_19.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_2.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_20.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_21.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_22.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_23.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_24.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_25.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_3.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_4.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_5.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_7.root",
		"file:/data/gridout/ahunyadi/MinBias/GEN_SIM/Run2_CMSSW80X/MinBias_80X_mcRun2_DynIneff_90_GEN_SIM_RAWTODIGI_evt100000/161122_093253/0000/GENSIMRECO_8.root"),
	secondaryFileNames = cms.untracked.vstring()
)


# process.load("Configuration.StandardSequences.Reconstruction_cff")
# process.load("RecoVertex.BeamSpotProducer.BeamSpot_cff")
# process.load("EventFilter.SiPixelRawToDigi.SiPixelRawToDigi_cfi")

# import EventFilter.SiPixelRawToDigi.siPixelRawToDigi_cfi
# siPixelDigis = EventFilter.SiPixelRawToDigi.siPixelRawToDigi_cfi.siPixelRawToDigi.clone()
# siPixelDigis.Timing = cms.untracked.bool(False)
# siPixelDigis.IncludeErrors = cms.bool(True)
# siPixelDigis.InputLabel = cms.InputTag("siPixelRawData")

# process.siPixelDigis.InputLabel = 'source'
# process.siPixelDigis.IncludeErrors = True

# process.load("EventFilter.SiPixelRawToDigi.SiPixelRawToDigi_cfi")

#---------------------------
#  HLT Filter
#---------------------------
process.zerobiasTriggerFilter = cms.EDFilter( "TriggerResultsFilter",
    triggerConditions = cms.vstring( 'HLT_ZeroBias_v* / 1' ),
    hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
    l1tResults = cms.InputTag( "" ),
    l1tIgnoreMask = cms.bool( False ),
    l1techIgnorePrescales = cms.bool( True ),
    daqPartitions = cms.uint32( 1 ),
    throw = cms.bool( True )
)

#---------------------------
#  Track Refitter
#---------------------------
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
trajectoriesFromRefitter = cms.InputTag('TrackRefitter')

#---------------------------
#  Split Cluster Merger
#---------------------------
process.SplitClusterAnalyzerPlugin = cms.EDAnalyzer('SplitClusterAnalyzer',
	rawDataCollection = cms.InputTag("rawDataCollector"),
	# siPixelDigis      = cms.InputTag("siPixelDigis"),
	# siPixelDigis      = cms.InputTag("rawDataCollector"),
	# siPixelDigis      = cms.InputTag("source"),
	dcolLostNeighbourDigiFlags = cms.InputTag("simSiPixelDigis", "dcolLostNeighbourDigiFlags"),
	siPixelClusters   = cms.InputTag("siPixelClusters"),
	# trajectoryInput   = cms.InputTag('generalTracks')
	trajectoryInput   = trajectoriesFromRefitter
)


# process.LocalReco = cms.Sequence(
# 	process.siPixelDigis * 
# 	process.siPixelClusters * 
# 	process.siPixelRecHits
# )

# process.SplitClusterMerger_step = cms.Path(process.SplitClusterMergerPlugin)

process.SplitClusterMerger_step = cms.Path(
	# process.zerobiasTriggerFilter * # Uncomment if running on data 
	process.MeasurementTrackerEvent * 
	process.TrackRefitter *
	process.SplitClusterAnalyzerPlugin
)

# process.p = cms.Path(
# 	process.LocalReco * 
# 	process.SplitClusterMergerPlugin
# )
