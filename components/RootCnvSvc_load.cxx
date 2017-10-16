#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootCnvSvc.h"

#include "RootCnvSvc/Dst/MdcDedxCnv.h"
#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Digi/EmcDigiCnv.h"
#include "RootCnvSvc/Mc/EmcMcHitCnv.h"
#include "RootCnvSvc/Dst/EmcTrackCnv.h"
#include "RootCnvSvc/EventCnv.h"
#include "RootCnvSvc/Dst/ExtTrackCnv.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/McParticleCnv.h"
#include "RootCnvSvc/Digi/MdcDigiCnv.h"
#include "RootCnvSvc/Dst/MdcKalTrackCnv.h"
#include "RootCnvSvc/Mc/MdcMcHitCnv.h"
#include "RootCnvSvc/Dst/MdcTrackCnv.h"
#include "RootCnvSvc/Digi/MucDigiCnv.h"
#include "RootCnvSvc/Mc/MucMcHitCnv.h"
#include "RootCnvSvc/Dst/MucTrackCnv.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/TofDigiCnv.h"
#include "RootCnvSvc/Digi/LumiDigiCnv.h"
#include "RootCnvSvc/Mc/TofMcHitCnv.h"
#include "RootCnvSvc/Dst/TofTrackCnv.h"

#include "RootCnvSvc/Rec/RecMdcTrackCnv.h"   
#include "RootCnvSvc/Rec/RecMdcHitCnv.h"  
#include "RootCnvSvc/Rec/RecMdcKalTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcKalHelixSegCnv.h"
#include "RootCnvSvc/Rec/RecMdcDedxCnv.h"    
#include "RootCnvSvc/Rec/RecMdcDedxHitCnv.h"
#include "RootCnvSvc/Rec/RecTofTrackCnv.h"  
#include "RootCnvSvc/Rec/RecBTofCalHitCnv.h" 
#include "RootCnvSvc/Rec/RecETofCalHitCnv.h" 
#include "RootCnvSvc/Rec/RecMucTrackCnv.h"   
#include "RootCnvSvc/Rec/RecMucRecHitCnv.h"  
#include "RootCnvSvc/Rec/RecEmcHitCnv.h"     
#include "RootCnvSvc/Rec/RecEmcClusterCnv.h" 
#include "RootCnvSvc/Rec/RecEmcShowerCnv.h"   
#include "RootCnvSvc/Rec/RecEvTimeCnv.h"      
#include "RootCnvSvc/Rec/RecExtTrackCnv.h"   
#include "RootCnvSvc/Rec/RecZddChannelCnv.h"
#include "RootCnvSvc/Rec/RecTrackCnv.h"       
#include "RootCnvSvc/EvtHeaderCnv.h"
#include "RootCnvSvc/EvtNavigatorCnv.h"

#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEventCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecTrackCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPrimaryVertexCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecVeeVertexCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPi0Cnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEtaToGGCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecDTagCnv.h"

//caogf
#include "RootCnvSvc/Trig/TrigCnv.h"
#include "RootCnvSvc/Trig/TrigDataCnv.h"

//fucd
#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/Hlt/HltRawCnv.h"
#include "RootCnvSvc/Hlt/HltInfCnv.h"
#include "RootCnvSvc/Hlt/DstHltInfCnv.h"

DECLARE_SERVICE_FACTORY(RootCnvSvc)
DECLARE_SERVICE_FACTORY(RootEvtSelector)

DECLARE_CONVERTER_FACTORY(EventCnv)
DECLARE_CONVERTER_FACTORY(DigiCnv)
DECLARE_CONVERTER_FACTORY(MdcDigiCnv)
DECLARE_CONVERTER_FACTORY(EmcDigiCnv)
DECLARE_CONVERTER_FACTORY(MucDigiCnv)
DECLARE_CONVERTER_FACTORY(TofDigiCnv)
DECLARE_CONVERTER_FACTORY(LumiDigiCnv)
DECLARE_CONVERTER_FACTORY(DstCnv)
DECLARE_CONVERTER_FACTORY(EmcTrackCnv)
DECLARE_CONVERTER_FACTORY(MdcTrackCnv)
DECLARE_CONVERTER_FACTORY(MucTrackCnv)
DECLARE_CONVERTER_FACTORY(TofTrackCnv)
DECLARE_CONVERTER_FACTORY(MdcDedxCnv)
DECLARE_CONVERTER_FACTORY(ExtTrackCnv)
DECLARE_CONVERTER_FACTORY(MdcKalTrackCnv)
DECLARE_CONVERTER_FACTORY(McCnv)
DECLARE_CONVERTER_FACTORY(MdcMcHitCnv)
DECLARE_CONVERTER_FACTORY(EmcMcHitCnv)
DECLARE_CONVERTER_FACTORY(MucMcHitCnv)
DECLARE_CONVERTER_FACTORY(TofMcHitCnv)
DECLARE_CONVERTER_FACTORY(McParticleCnv)
  
DECLARE_CONVERTER_FACTORY(RecMucTrackCnv)   
DECLARE_CONVERTER_FACTORY(RecMdcTrackCnv)  
DECLARE_CONVERTER_FACTORY(RecMdcHitCnv) 
DECLARE_CONVERTER_FACTORY(RecMdcKalTrackCnv)
DECLARE_CONVERTER_FACTORY(RecMdcKalHelixSegCnv)
DECLARE_CONVERTER_FACTORY(RecMdcDedxCnv)   
DECLARE_CONVERTER_FACTORY(RecMdcDedxHitCnv)
DECLARE_CONVERTER_FACTORY(RecTofTrackCnv) 
DECLARE_CONVERTER_FACTORY(RecBTofCalHitCnv)
DECLARE_CONVERTER_FACTORY(RecETofCalHitCnv) 
DECLARE_CONVERTER_FACTORY(RecMucRecHitCnv) 
DECLARE_CONVERTER_FACTORY(RecEmcHitCnv)    
DECLARE_CONVERTER_FACTORY(RecEmcClusterCnv) 
DECLARE_CONVERTER_FACTORY(RecEmcShowerCnv)  
DECLARE_CONVERTER_FACTORY(RecEvTimeCnv)    
DECLARE_CONVERTER_FACTORY(RecExtTrackCnv)   
DECLARE_CONVERTER_FACTORY(RecZddChannelCnv)
DECLARE_CONVERTER_FACTORY(RecTrackCnv)      
DECLARE_CONVERTER_FACTORY(EvtHeaderCnv)
DECLARE_CONVERTER_FACTORY(EvtNavigatorCnv)

DECLARE_CONVERTER_FACTORY(EvtRecCnv)   
DECLARE_CONVERTER_FACTORY(EvtRecEventCnv)   
DECLARE_CONVERTER_FACTORY(EvtRecTrackCnv)  
DECLARE_CONVERTER_FACTORY(EvtRecPrimaryVertexCnv)
DECLARE_CONVERTER_FACTORY(EvtRecVeeVertexCnv)
DECLARE_CONVERTER_FACTORY(EvtRecPi0Cnv)
DECLARE_CONVERTER_FACTORY(EvtRecEtaToGGCnv)
DECLARE_CONVERTER_FACTORY(EvtRecDTagCnv)

//caogf 
DECLARE_CONVERTER_FACTORY(TrigCnv)
DECLARE_CONVERTER_FACTORY(TrigDataCnv)
//fucd
DECLARE_CONVERTER_FACTORY(HltCnv)
DECLARE_CONVERTER_FACTORY(HltRawCnv)
DECLARE_CONVERTER_FACTORY(HltInfCnv)
DECLARE_CONVERTER_FACTORY(DstHltInfCnv)
 
DECLARE_FACTORY_ENTRIES(RootCnvSvc) {
  DECLARE_SERVICE(RootCnvSvc);
  DECLARE_SERVICE(RootEvtSelector);
  DECLARE_CONVERTER(EventCnv);
  DECLARE_CONVERTER(DigiCnv);
  DECLARE_CONVERTER(MdcDigiCnv);
  DECLARE_CONVERTER(EmcDigiCnv);
  DECLARE_CONVERTER(MucDigiCnv);
  DECLARE_CONVERTER(TofDigiCnv);
  DECLARE_CONVERTER(LumiDigiCnv);
  DECLARE_CONVERTER(DstCnv);
  DECLARE_CONVERTER(MdcTrackCnv);
  DECLARE_CONVERTER(EmcTrackCnv);
  DECLARE_CONVERTER(MucTrackCnv);
  DECLARE_CONVERTER(TofTrackCnv);
  DECLARE_CONVERTER(MdcDedxCnv);
  DECLARE_CONVERTER(ExtTrackCnv);
  DECLARE_CONVERTER(MdcKalTrackCnv);
  DECLARE_CONVERTER(McCnv);
  DECLARE_CONVERTER(MdcMcHitCnv);
  DECLARE_CONVERTER(EmcMcHitCnv);
  DECLARE_CONVERTER(MucMcHitCnv);
  DECLARE_CONVERTER(TofMcHitCnv);
  DECLARE_CONVERTER(McParticleCnv);   
  DECLARE_CONVERTER(RecMucTrackCnv);   
  DECLARE_CONVERTER(RecMdcTrackCnv);
  DECLARE_CONVERTER(RecMdcHitCnv); 
  DECLARE_CONVERTER(RecMdcKalTrackCnv);
  DECLARE_CONVERTER(RecMdcKalHelixSegCnv);
  DECLARE_CONVERTER(RecMdcDedxCnv);   
  DECLARE_CONVERTER(RecMdcDedxHitCnv);
  DECLARE_CONVERTER(RecTofTrackCnv); 
  DECLARE_CONVERTER(RecBTofCalHitCnv); 
  DECLARE_CONVERTER(RecETofCalHitCnv); 
  DECLARE_CONVERTER(RecMucRecHitCnv); 
  DECLARE_CONVERTER(RecEmcHitCnv);    
  DECLARE_CONVERTER(RecEmcClusterCnv); 
  DECLARE_CONVERTER(RecEmcShowerCnv);  
  DECLARE_CONVERTER(RecEvTimeCnv);    
  DECLARE_CONVERTER(RecExtTrackCnv);   
  DECLARE_CONVERTER(RecZddChannelCnv);
  DECLARE_CONVERTER(RecTrackCnv);     
  DECLARE_CONVERTER(EvtHeaderCnv);
  DECLARE_CONVERTER(EvtNavigatorCnv);
//  DECLARE_CONVERTER(DecayModeCnv);
  DECLARE_CONVERTER(EvtRecCnv);
  DECLARE_CONVERTER(EvtRecEventCnv);
  DECLARE_CONVERTER(EvtRecTrackCnv);
  DECLARE_CONVERTER(EvtRecPrimaryVertexCnv);
  DECLARE_CONVERTER(EvtRecVeeVertexCnv);
  //chunlei liu
  DECLARE_CONVERTER(EvtRecPi0Cnv);
  DECLARE_CONVERTER(EvtRecEtaToGGCnv);
  DECLARE_CONVERTER(EvtRecDTagCnv);
  //caogf
  DECLARE_CONVERTER(TrigCnv);
  DECLARE_CONVERTER(TrigDataCnv);
  //fucd
  DECLARE_CONVERTER(HltCnv);
  DECLARE_CONVERTER(HltRawCnv);
  DECLARE_CONVERTER(HltInfCnv);
  DECLARE_CONVERTER(DstHltInfCnv);
}
