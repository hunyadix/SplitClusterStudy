# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: -s GEN,SIM,DIGI,L1,DIGI2RAW,RAW2DIGI,RECO --mc --evt_type SingleNuE10_cfi --era Run2_25ns --conditions auto:run2_mc --beamspot NominalCollision2015 --magField 38T_PostLS1 --pileup=Flat_0_50_25ns --filein=/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10000/004CC894-4877-E511-A11E-0025905C3DF8.root --pileup_input=/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10002/082C3FE4-7479-E511-BCC5-0025904C8254.root --fileout file:GENSIMRECO.root --python_filename=test/TimingStudy_GenNu_DynIneffDB_cfg.py -n 10 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('RECO',eras.Run2_25ns)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mix_Flat_0_50_25ns_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedNominalCollision2015_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('SingleNuE10_cfi nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('file:GENSIMRECO.root'),
    outputCommands = process.RECOSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.input.fileNames = cms.untracked.vstring(['/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10002/082C3FE4-7479-E511-BCC5-0025904C8254.root'])
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# Flat pileup modification: 1-20 uniform
process.mix.input.nbPileupEvents.probFunctionVariable = cms.vint32(range(0, 21))
process.mix.input.nbPileupEvents.probValue = cms.vdouble([0.0] + [0.05] * 20)

process.generator = cms.EDProducer("FlatRandomEGunProducer",
    AddAntiParticle = cms.bool(False),
    PGunParameters = cms.PSet(
        MaxE = cms.double(10.01),
        MaxEta = cms.double(2.5),
        MaxPhi = cms.double(3.14159265359),
        MinE = cms.double(9.99),
        MinEta = cms.double(-2.5),
        MinPhi = cms.double(-3.14159265359),
        PartID = cms.vint32(12)
    ),
    Verbosity = cms.untracked.int32(0),
    firstRun = cms.untracked.uint32(1),
    psethack = cms.string('single Nu E 10')
)

# SplitClusterAnalyzer
process.siPixelClusters.src = cms.InputTag('simSiPixelDigis')
process.SplitClusterAnalyzerPlugin = cms.EDAnalyzer('SplitClusterMergerAnalyzer',
    rawDataCollection = cms.InputTag("rawDataCollector"),
    dcolLostDigis     = cms.InputTag("dcolLostDigis"),
    siPixelClusters   = cms.InputTag("siPixelClusters"),
    trajectoryInput   = cms.InputTag('generalTracks')
    # trajectoryInput   = trajectoriesFromRefitter
)

# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.digitisation_step = cms.Path(process.pdigi)
process.SplitClusterMerger_step = cms.Path(
    # process.zerobiasTriggerFilter * # Uncomment if running on data 
    process.MeasurementTrackerEvent * 
    # process.TrackRefitter *
    process.SplitClusterAnalyzerPlugin
)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
# process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.simulation_step,process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RECOSIMoutput_step)
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.simulation_step,process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.raw2digi_step,process.reconstruction_step,process.SplitClusterMerger_step,process.endjob_step,process.RECOSIMoutput_step)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 


#--------------- Added for TimingStudy ---------------

#---------------------------
#  Settings
#---------------------------

setAutoGT = True
runOnGrid = False
useSqlite = True

if setAutoGT:
    print "GlobalTag (auto:run2_mc): "+str(process.GlobalTag.globaltag)
else:
    process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_v14'
    print "GlobalTag: "+str(process.GlobalTag.globaltag)

# Number of events
process.maxEvents.input = 10

# MessageLogger
process.MessageLogger.cerr.FwkReport.reportEvery = 1

#---------------------------
#  Pile-up (RunIISummer15GS)
#---------------------------
if runOnGrid:
    from DPGAnalysis.PixelTimingStudy.PoolSource_13TeV_RunIISummer15GS import *
    process.mix.input.fileNames = pileupFileNames
else:
    process.mix.input.fileNames = cms.untracked.vstring(
        'file:/data/store/mc/RunIISummer15GS_/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10002/082C3FE4-7479-E511-BCC5-0025904C8254.root',
    )

#---------------------------
#  DynIneff from DB
#---------------------------
if useSqlite:
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.DynIneffDBSource = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        connect = cms.string('sqlite_file:dynineff_dcol_0_70.db'),
        toGet = cms.VPSet(cms.PSet(
            record = cms.string('SiPixelDynamicInefficiencyRcd'),
            tag = cms.string('SiPixelDynamicInefficiency_v1')
        ))
    )
    process.es_prefer_DynIneffDBSource = cms.ESPrefer("PoolDBESSource","DynIneffDBSource")

#---------------------------
#  Path/Schedule
#---------------------------
#process.TimingStudy_step = cms.Path(process.TimingStudy)
#process.schedule.remove(process.RECOSIMoutput_step)
#process.schedule.remove(process.endjob_step)
#process.schedule.remove(process.genfiltersummary_step)
#process.schedule.append(process.TimingStudy_step)