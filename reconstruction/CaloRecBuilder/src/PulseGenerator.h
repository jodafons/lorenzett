#ifndef PulseGenerator_h
#define PulseGenerator_h

#include "GaugiKernel/StatusCode.h"
#include "CaloTool.h"
#include "TPulseGenerator.h"
#include "TTree.h"


class PulseGenerator : public CaloTool
{

  public:
    /** Constructor **/
    PulseGenerator( std::string name );
    virtual ~PulseGenerator();
    
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode executeTool( const xAOD::EventInfo *, xAOD::RawCell * ) const override;
    virtual StatusCode executeTool( const xAOD::EventInfo *, xAOD::CaloCell * ) const override;
    virtual StatusCode executeTool( const xAOD::EventInfo *, xAOD::CaloCluster * ) const override;
    virtual StatusCode executeTool( const xAOD::EventInfo *, xAOD::TruthParticle * ) const override;



  private:
 
    /*! Number of samples to be generated */
    int m_nsamples;
    /*! The shaper configuration path */
    std::string m_shaperFile;
    /*! Pulse generator */
    CPK::TPulseGenerator  *m_pulseGenerator;
    /*! Output level message */
    int m_outputLevel;

    mutable TTree *t1;
    mutable std::vector<std::vector<float>> tree_pulse_sum;
    mutable std::vector<std::vector<float>> tree_rawEnergySamples;
    mutable std::vector<float> tree_acc_rawEnergySamples;
};

#endif




