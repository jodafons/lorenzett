#include "PulseGenerator.h"
#include "TPulseGenerator.h"
#include "TTree.h"

using namespace Gaugi;


TFile f("tree1.root","recreate");

PulseGenerator::PulseGenerator( std::string name ) : 
  IMsgService(name),
  CaloTool()
{
  static int layer = 1;
  declareProperty( "NSamples"     , m_nsamples=7            );
  declareProperty( "ShaperFile"   , m_shaperFile            );
  declareProperty( "OutputLevel"  , m_outputLevel=1         );
  f.cd();
  t1 = new TTree( ("layer_" + std::to_string(layer++) ).c_str(), "layer");
}


PulseGenerator::~PulseGenerator()
{
  delete t1;
  delete m_pulseGenerator;
}


StatusCode PulseGenerator::initialize()
{
  setMsgLevel( (MSG::Level)m_outputLevel );
  MSG_DEBUG( "Reading shaper values from: " << m_shaperFile << " and " << m_nsamples << " samples.");
  m_pulseGenerator = new CPK::TPulseGenerator( m_nsamples, m_shaperFile.c_str());
  t1->Branch("raw_energy_samples",&tree_rawEnergySamples);
  t1->Branch("pulse_sum",&tree_pulse_sum);
  t1->Branch("acc_raw_energy_samples",&tree_acc_rawEnergySamples);
  return StatusCode::SUCCESS;
}


StatusCode PulseGenerator::finalize()
{
  MSG_DEBUG("=====================Filling tree========================");
  t1->Fill(); 
  f.Write();
  return StatusCode::SUCCESS;
}


StatusCode PulseGenerator::executeTool( const xAOD::EventInfo * evt, xAOD::RawCell *cell ) const
{
  auto pulse_size = m_pulseGenerator->GetPulseSize();
  
  // Get all energies for each bunch crossing 
  auto rawEnergySamples = std::vector<float>(cell->rawEnergySamples());

  static int eventNumber = -1;
  static int counter = 0;

  if ( eventNumber == -1 ) {
    tree_acc_rawEnergySamples.resize( rawEnergySamples.size(), 0.);
  }

  if ( eventNumber != evt->eventNumber() ){
    if( eventNumber != -1 ) { 
      t1->Fill(); 
      MSG_DEBUG("Filling tree");
    }
    tree_acc_rawEnergySamples.clear();
    tree_acc_rawEnergySamples.resize( rawEnergySamples.size(), 0.);
    tree_pulse_sum.clear();
    tree_rawEnergySamples.clear();
    counter = 0;
    eventNumber = evt->eventNumber();
  }

  // Create an pulse with zeros with n samples
  std::vector<float> pulse_sum(pulse_size, 0.0);
  // Loop over each bunch crossing
  for ( int bc = cell->bcid_start(), i=0;  bc <= cell->bcid_end(); ++bc, ++i )
  {
    // Generate the pulse
    auto pulse = m_pulseGenerator->GenerateDeterministicPulse( rawEnergySamples[i], 0, bc*cell->bc_duration() );
    // Add gaussian noise
    m_pulseGenerator->AddGaussianNoise(pulse, 0.0, 1.5);
    // m_pulseGenerator[cell->layer()]->AddGaussianNoise(pulse);
    // Accumulate into pulse sum (Sum all pulses)
    std::vector<float> bc_pulse;
    for ( int j=0; j < pulse_size; ++j ){
      float value= (float)pulse->operator[](j);
      bc_pulse.push_back(value);
      pulse_sum[j] += value;
    }

    delete pulse; // This must be deleted to avoid memory leak since spk uses "new" internally
    cell->setPulsePerBunch( bc, bc_pulse ); 
  }

  // Add the pulse centered in the bunch crossing zero
  cell->setPulse( pulse_sum );

  tree_pulse_sum.push_back( pulse_sum );
  tree_rawEnergySamples.push_back( rawEnergySamples );
  for ( unsigned int c = 0; c < tree_acc_rawEnergySamples.size(); ++c ) {
    tree_acc_rawEnergySamples[c]+= rawEnergySamples[c];
  }
  ++counter;
  return StatusCode::SUCCESS;
}



// Just for python import in ROOT
StatusCode PulseGenerator::executeTool( const xAOD::EventInfo *, xAOD::CaloCell * ) const {return StatusCode::SUCCESS;}
StatusCode PulseGenerator::executeTool( const xAOD::EventInfo *, xAOD::CaloCluster * ) const {return StatusCode::SUCCESS;}
StatusCode PulseGenerator::executeTool( const xAOD::EventInfo *, xAOD::TruthParticle * ) const {return StatusCode::SUCCESS;}










