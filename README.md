# SplitClusterMerger
Split Cluster Merger plugin for CMSSW

### &#x1F539; Base recipe:

```bash
export SCRAM_ARCH=slc6_amd64_gcc530
cmsrel CMSSW_8_0_18
cd CMSSW_8_0_18/src
cmsenv
git cms-addpkg SimTracker/SiPixelDigitizer
git cms-addpkg RecoLocalTracker/SiPixelClusterizer
git remote add hunyadix git@github.com:hunyadix/cmssw.git
git fetch hunyadix
git checkout -t hunyadix/CMSSW_8_0_18_for_SplitClusterMerger
```

### &#x1F539; Generating MinBias events using cmsDriver.py:

```bash
cmsDriver.py \
-s GEN,SIM,DIGI,L1,DIGI2RAW,RAW2DIGI \
--mc  \
--evt_type SingleNuE10_cfi  \
--era Run2_25ns  \
--conditions auto:run2_mc  \
--beamspot NominalCollision2015  \
--magField 38T_PostLS1  \
--pileup=Flat_0_50_25ns  \
--filein=/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10000/004CC894-4877-E511-A11E-0025905C3DF8.root  \
--pileup_input=/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10002/082C3FE4-7479-E511-BCC5-0025904C8254.root  \
--fileout file:GENSIMRECO.root  \
--python_filename=GENSIM2RAW2DIGI_dynineff_layer1_dcol_0_70_cfg.py \
-n 10  \
--no_exec
```

##### Modifications:

Switches:

```python
#---------------------------
#  Settings
#---------------------------

setAutoGT = True
runOnGrid = True
useSqlite = True
```

Pileup:

```python
# Flat pileup modification: 1-20 uniform
process.mix.input.nbPileupEvents.probFunctionVariable = cms.vint32(range(0, 21))
process.mix.input.nbPileupEvents.probValue = cms.vdouble([0.0] + [0.05] * 20)

# Other statements
if runOnGrid:
    from DPGAnalysis.SplitClusterMerger.PoolSource_13TeV_RunIISummer15GS import pileupFileNames
    process.mix.input.fileNames = pileupFileNames
else:
    process.mix.input.fileNames = cms.untracked.vstring(['/store/mc/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v2/10002/082C3FE4-7479-E511-BCC5-0025904C8254.root'])
```

Reading Dynamic inefficiencies from DB:

```python
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
```

GlobalTag:

```python
if setAutoGT:
    print "GlobalTag (auto:run2_mc): " +str(process.GlobalTag.globaltag)
else:
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_v17'
    print "GlobalTag: "+str(process.GlobalTag.globaltag)
```

Changing the source might be necessary for crab processes (make sure to correct for this while setting up the pileup scenario):

```python
process.source = cms.Source("EmptySource")
```