#define _RootEventBaseCnv_CXX

#include "TROOT.h"
#include "TObject.h"
#include "TClass.h"

#undef PACKAGE_VERSION
#include "RootCnvSvc/HConnection.h"
#include "RootCnvSvc/HbaseInterface.h"
#include "RootCnvSvc/serialize_deserialize.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/SmartIF.h"


#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/RootCnvSvc.h"
#include "RootCnvSvc/RootAddress.h"
#include <iostream>
#include "RootEventData/TBossFullEvent.h"//
#include "RootEventData/TEvtRecEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TDstEvent.h"
#include "RootEventData/TEvtHeader.h"
#include "RootEventData/THltEvent.h"
#include "RootEventData/TMcEvent.h"
#include "RootEventData/TTrigEvent.h"
#include "RootEventData/TEvtRecObject.h"
#include <sstream>
#include <stdlib.h>
using namespace std;
#define BITN 6       //统一entryid的位数
/*void completeEntryid(int entryid,string& sentryid){
	stringstream stream;
	stream<<entryid;
	sentryid = stream.str();
	while(sentryid.length()<BITN)sentryid="0"+sentryid;
}*/
extern const InterfaceID IID_IRootCnvSvc;

using namespace DataSvcHelpers;

RootEventBaseCnv::RootEventBaseCnv(const CLID& clid, ISvcLocator* svc)
: Converter(ROOT_StorageType, clid, svc) ,m_cnvSvc(0),m_rootBranchname(""),m_rootTreename("Event"), m_evtsel(0)
{
    
    StatusCode sc;
    MsgStream log(msgSvc(), "RootEventBaseCnv");

    // Get DataSvc
    IService* isvc = 0;
    sc = serviceLocator()->getService("EventDataSvc", isvc, true);
    if(sc.isSuccess() ) {
        sc = isvc->queryInterface(IDataProviderSvc::interfaceID(), (void**)&m_eds);
    }
    if(sc.isFailure()){
        log << MSG::ERROR << "Unable start EventData service within RootEventBaseCnv" << endreq;
    } 

   m_rootInterface=RootInterface::Instance(log);
   if (!m_rootInterface)      log << MSG::ERROR << "Unable to start Root interface within RootCnvSvc" << endreq;

   ///////////////////// liangyt, try to get event id from evt selector ///////////////////////////////
   //// the former method to retrieve the event number of one branch is to calculate///////////////////
   //// division of branch number of one event (brN) and total readed branch num(entryN)///////////////
   //// there are some disadvantages for this method. NOW, we try to get event id from evt selector////
   
   sc = serviceLocator()->getService ("RootEvtSelector", isvc, false);
   if (!sc.isSuccess())  sc = serviceLocator()->getService ("EventSelector", isvc, false);
   if (sc.isSuccess()) {
     sc = isvc->queryInterface(IID_IRootEvtSelector, (void**)&m_evtsel);
   }
   if(sc.isFailure()) {
     log << MSG::WARNING << "Unable to start event selector service within RootCnvSvc" << endreq;
   }
   ////////////////////////

   
    m_branchNr=-1;
    m_branchNrDst=-1;
    m_branchNrMc=-1;
    m_branchNrRecon=-1;
    m_branchNrEvtRec=-1;
    m_branchNrEvtHeader=-1;
    m_branchNrEvtNavigator=-1;
    m_objRead=0;
    CLID_top=0;
    m_branchNumbers= new TArrayS(0);
}


StatusCode RootEventBaseCnv::createRep(DataObject* obj, 
                                  IOpaqueAddress*& addr)   {
  // Purpose and Method: Convert the transient object to ROOT

  MsgStream log(msgSvc(), "RootEventBaseCnv");
 
  StatusCode sc= StatusCode::SUCCESS;
  // get the corresponding address 
  RootAddress *rootaddr;
  sc=m_cnvSvc->createAddress(obj,addr);

  rootaddr = dynamic_cast<RootAddress *>(addr);

  if (sc.isFailure() || !rootaddr  ) { 
    log << MSG::ERROR << "Could not create address for clid " <<obj->clID()<<", objname "<<obj->name()<<endreq;
    return StatusCode::FAILURE;
  }

  //  do the real conversion in the derived converter
  sc = DataObjectToTObject(obj,rootaddr);

  delete addr;
  addr = NULL;

  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not transform object" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}
StatusCode RootEventBaseCnv::fillRepRefs(IOpaqueAddress* ,
                                DataObject* )    {
    // Purpose and Method:  Resolve the references of the converted object.
    //     It is expected that derived classes will override this method.
    MsgStream log(msgSvc(), "RootEventBaseCnv");
    return StatusCode::SUCCESS;
}

StatusCode RootEventBaseCnv::fillObjRefs(IOpaqueAddress* ,
                                DataObject* )    {
    // Purpose and Method:  Resolve the references of the converted object.
    //     It is expected that derived classes will override this method.
    MsgStream log(msgSvc(), "RootEventBaseCnv");
    return StatusCode::SUCCESS;
}


StatusCode RootEventBaseCnv::initialize()   {
 
  StatusCode status = Converter::initialize();

  if ( status.isSuccess() )   {
    IService* isvc = 0;
    status = serviceLocator()->service("RootCnvSvc", isvc, false);
    if ( !status.isSuccess() )   status = serviceLocator()->service("EventCnvSvc", isvc, true);
    if ( status.isSuccess() )   {
      status = isvc->queryInterface(IID_IRootCnvSvc, (void**)&m_cnvSvc);
    }
  }
  
  return status;
}

StatusCode RootEventBaseCnv::finalize()   {
    if ( m_cnvSvc )     {
        m_cnvSvc->release();
        m_cnvSvc=0;
    }
    return Converter::finalize();
}

void RootEventBaseCnv::declareObject(const std::string& path, const CLID& cl, 
                            const std::string& treename,  const std::string& branchname)  {
    // Purpose and Method:  Save the path on the TDS, treename, pathname in the m_leaves vector, 
    //   corresponding to the DataObject that the converter handles.
   m_leaves.push_back(RootCnvSvc::Leaf(path, cl,treename,branchname));
}

StatusCode RootEventBaseCnv::createObj(IOpaqueAddress* addr, 
                                  DataObject*& refpObject) {
  // transform ROOT object to TDS object
  MsgStream log(msgSvc(), "RootEventBaseCnv");
  log << MSG::DEBUG << "RootEventBaseCnv::createObj with clid " <<addr->clID()<< endreq;
  StatusCode sc;

  // add 2005-11-29

  RootAddress *raddr=dynamic_cast<RootAddress *>(addr);
  if (!raddr) {
    log << MSG::ERROR << "Could not downcast to Root address" << endreq;
    return StatusCode::FAILURE;
  }
   log<<MSG::INFO<<"######### RootEventBaseCnv ::createObj begin of createObj:  m_branchNumbers->GetSize() "<<m_branchNumbers->GetSize()<<"###############"<<endreq;
   log<<MSG::INFO<<"######### RootEventBaseCnv ::createObj begin of createObj:  raddr->getNrBranches() "<<raddr->getNrBranches()<<"###############"<<endreq;
 
  static int temp =0;   //control the begin of each files 2005-12-01
  static int entryN =0; //control the event number of each files 2005-21-01
  static int brN =0; //control munber of branch of the tree;
  int lastBrn = brN;
  //lastBrn = brN; 
  static int branchN=0;
  static bool isSet=true;

  static int entryBefore = 0;
  static bool addEntryEachFile = true;
 
  //edited by wangcong
  //connect to hbase
  static bool first_time = true;
  static HbaseInterface* hbaseInterface = 0;
  if ( first_time ) {
    HConnection *connection = new HConnection("192.168.60.64","9091");
    connection->connect();
    hbaseInterface=new HbaseInterface(connection,"eventable","fullEvent");
    first_time = false;
  }
  
  std::string rowkey;
  m_currentFileName = m_rootInterface->getCurrentFileName();
  rowkey.append(m_currentFileName);
  rowkey.append("-");
  stringstream stream;
  stream << m_evtsel->getRecId();
  rowkey.append(stream.str());
  log << MSG::INFO << "rowkey is "<<rowkey  << endreq;
  //raddr->Print();
  if(hbaseInterface->exist(rowkey))//existed in hbase
  { 
    log << MSG::INFO << "using hbase 1" << endreq;
  //  if (m_branchNumbers->GetSize()<=0) {
  //  if(isSet) brN++;
  //  int branchNumber;
    size_t size;
    for (int nb=0;nb<raddr->getNrBranches();nb++) {
       
     /* if (!sc.isSuccess()) 
      {
        if(isSet) brN--;    //liangyt: if fail to retrieve this branch, this will be not a effective branch.
        //entryN++;   //liangyt: this is the second method
        if(temp>0) temp--;  //temp > 0 means recording effective branch number.
        return sc;
      }*/
      void* ptmp = hbaseInterface->getAttr(rowkey, raddr->getBranchname(nb), size);
      if ( *((void**)ptmp) == 0 ) return StatusCode::FAILURE;
      memcpy(m_adresses[nb], ptmp, size);
      log << MSG::INFO << "using hbase 2" << endreq;
      log << MSG::INFO <<" setBranchAddress nb is  "<<nb<<"raddr->getNrBranchename :"<<raddr->getBranchname(nb).c_str()<<endreq;
   //   m_branchNumbers->Set(nb+1);
   //   m_branchNumbers->AddAt(branchNumber,nb); 
      }
   }
 
  else{//not in the database
 
   if(m_rootInterface->getENDFILE() || (temp >0 && temp < branchN)){ // if the file has get the end:y the go to next file to create a new tree
    
    if(m_rootInterface->getENDFILE() ) {
      entryN = 0;
    }
    
    temp++;
    
    delete m_branchNumbers;
    m_branchNumbers = new TArrayS(0);
   
    if(temp == branchN) {
      temp =0;
    }
   }

   if(m_rootInterface->getENDFILE()) addEntryEachFile = true;
   // the 2nd method 
   if(m_evtsel->getRecId() - entryBefore == 0) { // first event in this file
     delete m_branchNumbers;
     m_branchNumbers = new TArrayS(0);
   }

   //new method to initialize the branchNumber 
   if(m_currentFileName=="") m_currentFileName = m_rootInterface->getCurrentFileName();
   if(!(m_currentFileName == m_rootInterface->getCurrentFileName())){ 
	   m_currentFileName = m_rootInterface->getCurrentFileName();
	   delete m_branchNumbers;
	   m_branchNumbers = new TArrayS(0);
   }  
   log << MSG::INFO <<" m_currentFileName by ccong= "<<m_currentFileName<<endreq;
   //---------------------------------------- 
  if (m_branchNumbers->GetSize()<=0) {
    if(isSet) brN++;
    int branchNumber;
    for (int nb=0;nb<raddr->getNrBranches();nb++) {
      sc=m_rootInterface->setBranchAddress(raddr->getTreename().c_str(),raddr->getBranchname(nb).c_str(),m_adresses[nb],branchNumber);
      log << MSG::INFO <<" setBranchAddress nb is  "<<nb<<"raddr->getNrBranches :"<<raddr->getNrBranches()<<endreq;
      if (!sc.isSuccess()) 
      {
        if(isSet) brN--;    //liangyt: if fail to retrieve this branch, this will be not a effective branch.
        //entryN++;   //liangyt: this is the second method
        if(temp>0) temp--;  //temp > 0 means recording effective branch number.
        return sc;
      }
      m_branchNumbers->Set(nb+1);
      m_branchNumbers->AddAt(branchNumber,nb);
     
    }
  }

  //// after a file initialized, m_rootInterface get entries 
  if(addEntryEachFile&&(m_evtsel->getRecId()>entryBefore)){   // for a new file, add entry for ONLY one time.
    entryBefore += m_rootInterface->getEntries();
    addEntryEachFile = false;
  }

  if(lastBrn == brN && isSet ){
    branchN = brN;
    isSet=false;    
  }

  if(isSet==false) log << MSG::INFO <<" 1st method set event as : "<<int(entryN/branchN)<<endreq;
  if(isSet==false) raddr->setEntryNr(int(entryN/branchN));//former method, keep it to be backup.
  if(m_evtsel) log << MSG::INFO <<" event id by ccong= "<<m_evtsel->getRecId()<<endreq;


  int eventID = 0;
  if(entryBefore > m_evtsel->getRecId())
    eventID = m_evtsel->getRecId() + m_rootInterface->getEntries() - entryBefore;
  else if(entryBefore == m_evtsel->getRecId()) eventID = 0;
  else log << MSG::ERROR <<"eventId error!!!"<<endreq;
  
  log << MSG::INFO <<" 2nd  method set event as : "<<eventID<<endreq;
  
  if(m_evtsel) raddr->setEntryNr(eventID);

  //add dengzy
  if(m_rootInterface->getSelectFromTag()!=0)
  {
    if(m_evtsel)
      raddr->setEntryNr( m_evtsel->getRecId() );
  }//end of add by dengzy

  // read branch
  
  if (m_branchNumbers->GetSize()>0) {
    int nbtot=0,nb;
    for (int ib=0;ib<m_branchNumbers->GetSize();ib++) {
      //sc=m_rootInterface->getBranchEntry(m_branchNumbers->At(ib),raddr->getEntryNr(),nb);
      //change to get branch entry with addr(set address for each entry) liangyt
      sc=m_rootInterface->getBranchEntry(m_branchNumbers->At(ib),raddr->getEntryNr(),m_adresses[ib],nb);      
      log << MSG::INFO <<" m_adresses[ib] is : "<<m_adresses[ib]<<endreq;
      if (sc.isFailure()) {
        log << MSG::ERROR << "Could not read branch " << raddr->getBranchname(nb) << endreq;
        return sc;
      }
      nbtot+=nb;
    }
  }
     
  else {   // get ROOT object
    if (CLID_top) {
      IConverter *p=conversionSvc()->converter(CLID_top);
      RootEventBaseCnv *cnv=dynamic_cast<RootEventBaseCnv *>(p);
      if (!cnv) {
        log << MSG::ERROR << "Could not downcast to RootEventBaseCnv "  << endreq;
        return StatusCode::FAILURE;
      }
      m_objRead=cnv->getReadObject();
    }
  }
}//ended with file
  //do concrete transformation in derived converter
  sc = TObjectToDataObject(refpObject);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not transform object" << endreq;
    return sc;
  }

  // verify if we have to register
  IRegistry* ent = addr->registry();
  if ( ent == 0)   {
       sc=m_eds->registerObject(raddr->getPath(),refpObject);
       if (sc.isFailure()) {
         log << MSG::ERROR << "Could not register object " << raddr->getPath()<<" status "<<sc.getCode()<<endreq;
       }
       //    }
  }

  entryN++;
  return StatusCode::SUCCESS;
}
