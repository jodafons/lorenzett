
__all__ = ["CaloCellBuilder"]

from Gaugi import Logger
from Gaugi.messenger.macros import *
from RecCommon import treatPropertyValue, recordable
import os



default_basepath = os.environ['LZT_PATH']+'/reconstruction/CaloRec/data/'


class CaloCellBuilder( Logger ):

  # ( CollectionKey, Detector Cell File, Pulse Shaper File )
  __configs = [
      # Lar Barrel Calorimter
      ("Collection_EM1" , 'detector_sampling_1.dat' , 'larcalorimeter_pulse_shape.dat'  , 7),
      ("Collection_EM2" , 'detector_sampling_2.dat' , 'larcalorimeter_pulse_shape.dat'  , 7),
      ("Collection_EM3" , 'detector_sampling_3.dat' , 'larcalorimeter_pulse_shape.dat'  , 7),
      # Tile Barrel Calorimeter
      ("Collection_HAD1", 'detector_sampling_4.dat' , 'tilecalorimeter_pulse_shape.dat' , 7),
      ("Collection_HAD2", 'detector_sampling_5.dat' , 'tilecalorimeter_pulse_shape.dat' , 7),
      ("Collection_HAD3", 'detector_sampling_6.dat' , 'tilecalorimeter_pulse_shape.dat' , 7),
     ] 

            


  def __init__( self, name, basepath=default_basepath , HistogramPath = "Expert"):

    Logger.__init__(self)
    self.__recoAlgs = []
    self.__basepath = basepath
    self.__histpath = HistogramPath
    self.configure()




  def configure(self):


    from CaloRec import CaloCellMaker, CaloCellMerge, PulseGenerator, OptimalFilter
    for idx, config in enumerate( self.__configs ):
      pulse = PulseGenerator("PulseGenerator", NSamples = config[3], ShaperFile = self.__basepath+config[2])
      of = OptimalFilter("OptimalFilter")

      alg = CaloCellMaker("CaloCellMaker", 
                          CollectionKey = recordable( config[0] ), 
                          CaloCellFile = self.__basepath+config[1], 
                          HistogramPath = self.__histpath)
      alg.Tools = [pulse, of]
      self.__recoAlgs.append( alg )
  

    collectionKeys =  [ recordable(config[0]) for config in self.__configs ]
    
    # Merge all collection into a container and split between truth and reco
    mergeAlg = CaloCellMerge( "CaloCellMerge" , 
                              CollectionKeys  = collectionKeys,
                              CellsKey        = recordable("Cells"),
                              TruthCellsKey   = recordable("TruthCells"),
                              OutputLevel     = 1 )

    self.__recoAlgs.append( mergeAlg )



  def merge( self, acc ):
    for reco in self.__recoAlgs:
      acc+=reco 















