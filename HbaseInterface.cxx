#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include "TClonesArray.h"

#undef PACKAGE_VERSION
#include "RootCnvSvc/HConnection.h"
#include "RootCnvSvc/HbaseInterface.h"
#include "RootEventData/TJobInfo.h"
#include "DataInfoSvc/DataInfoSvc.h"
#include "DataInfoSvc/IDataInfoSvc.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm> // for find
#include <iostream>
#include <fstream>
#include <string>

#include "GaudiKernel/ISvcLocator.h"
#include "RootEventData/TBossFullEvent.h"
#include "RootCnvSvc/serialize_deserialize.h"
#include <stdlib.h>
#define CACHE_SIZE 100

const std::string default_table("eventable");
const std::string default_family("fullEvent");
const int32_t numRows = 4;


HbaseInterface::HbaseInterface(HConnection *connection,const string& _table,const string &_family)
  : m_mask(0),
    BossFullEvent(0)
{
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
	this->connection = connection;
	table = _table;
	family=_family;
	this->head_ = new HbaseEvent;
	this->tail_ = new HbaseEvent;
	head_->prev = NULL;
	head_->next = tail_;
	tail_->prev = head_;
	tail_->next = NULL;
	event_list = new HbaseEvent[CACHE_SIZE];
	for(int i = 0;i<CACHE_SIZE;i++){
		this->free_pos.push_back(event_list+i);
	}
}

//HbaseInterface::HbaseInterface(HConnection *connection){
//        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
//	HbaseInterface(connection,"eventable","fullEvent");
//}
StatusCode HbaseInterface::getHbaseValue(const string& key){
	MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
	//批量读取到双链表中，并将读到的数据放到hashmap中
	TScan scan;
	scan.__set_startRow(key);
	vector<TResult> _return;
	int32_t id = connection->openScanner(table,scan);
	log << MSG::INFO <<"HbaseInterface::getHbaseValue:"<<"extract data"<<endreq;
	connection->getScannerRows(_return,id,numRows);
	for(int32_t i = 0 ;i<numRows;i++){
		string key = _return[i].row;
		log << MSG::INFO<<"HbaseInterface::getHbaseValue:"<<"KEY IS"<<key<<endreq;

		vector<TColumnValue> vec = _return[i].columnValues;
		string len = vec[0].value;
		string buffer = vec[1].value;
		if(!free_pos.empty()){
			HbaseEvent* data = free_pos.back();
			free_pos.pop_back();
			data->BossFullEvent = buffer;
			data->Length = atoi(len.c_str());
			data->key = key;
			_hashmap[key] = data;
			attach(data);
		}
		else{//not enough room
			HbaseEvent* data = tail_->prev;
			detach(data);
			_hashmap.erase(data->key);
			data->key = key;
			data->BossFullEvent = buffer;
			data->Length = atoi(len.c_str());
			attach(data);
			_hashmap[key] = data;
		}
	}
	connection->closeScanner(id);
	return StatusCode::SUCCESS;
}
StatusCode HbaseInterface::getEntry(const string& rowKey,const char* branch_name){
	MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");

	if(!strcmp(branch_name,"TEvtHeader"))
	{	
		if(!(m_mask&hasEvtHeader)){
			if(BossFullEvent->getEvtHeader()){
				//	(*(this->EvtHeader)) = (*(BossFullEvent->getEvtHeader()));
				this->EvtHeader = BossFullEvent->getEvtHeader();
				m_mask |= hasEvtHeader;
                              //  delete BossFullEvent; 
				return StatusCode::SUCCESS;

			}
			else
			{
				log << MSG::ERROR << "No TEvtHeader in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"TDigiEvent")){
		if(!(m_mask&hasDigiEvent)){
			if(BossFullEvent->getDigiEvent()){
				this->DigiEvent = BossFullEvent->getDigiEvent();
				m_mask    |=hasDigiEvent;
				return StatusCode::SUCCESS;
			}
			else
			{
				log << MSG::ERROR << "No TDigiEvent in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"TDstEvent")){
		if(!(m_mask&hasDstEvent)){
			if(BossFullEvent->getDstEvent()){
				this->DstEvent = BossFullEvent->getDstEvent();
				m_mask		|=hasDstEvent;
                                //delete BossFullEvent; 
				return StatusCode::SUCCESS;
			}
			else
			{
				log << MSG::ERROR << "No TDstEvent in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"TMcEvent")){
		if(!(m_mask&hasMcEvent)){
			if(BossFullEvent->getMcEvent()){
				this->McEvent = BossFullEvent->getMcEvent();
				m_mask 		|=hasMcEvent;
				return StatusCode::SUCCESS;
			}
			else
			{
				log << MSG::ERROR << "No TMcEvent in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"TTrigEvent")){
		if(!(m_mask&hasTrigEvent)){
			if(BossFullEvent->getTrigEvent()){
				this->TrigEvent = BossFullEvent->getTrigEvent();
				m_mask		|=hasTrigEvent;
				return StatusCode::SUCCESS;
			}
			else
			{
				log << MSG::ERROR << "No TTrigEvent in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"THltEvent")){
		if(!(m_mask&hasHltEvent)){
			if(BossFullEvent->getHltEvent()){
				this->HltEvent = BossFullEvent->getHltEvent();
				m_mask		|=hasHltEvent;
				return StatusCode::SUCCESS;
			}
			else
			{
				log << MSG::ERROR << "No THltEvent in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
	if(!strcmp(branch_name,"TEvtRecObject")){
		if(!(m_mask&hasEvtRecObject)){
			if(BossFullEvent->getEvtRecObject()){
				this->EvtRecObject = BossFullEvent->getEvtRecObject();
				m_mask		|= hasEvtRecObject;
				return StatusCode::SUCCESS;
			}
			else{
				log << MSG::ERROR << "No TEvtRecObject in this entry"<< endreq;
				return StatusCode::FAILURE;
			}
		}
	}
               std::cout << __FILE__ << __LINE__ << std::endl;
	return StatusCode::FAILURE;
}
bool HbaseInterface::exist(const string rowkey){
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
	TGet get;
	get.__set_row(rowkey);
	return connection->exist(table,get);
}
void HbaseInterface::setTableName(const string& _table){
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
	this->table = _table;
}
void HbaseInterface::setFamily(const string &_family){
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
	this->family=_family;
}
void HbaseInterface::detach(HbaseEvent* node){
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    // 将结点插入头部
void HbaseInterface::attach(HbaseEvent* node){
        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
        node->prev = head_;
        node->next = head_->next;
        head_->next = node;
        node->next->prev = node;
}

void* HbaseInterface::getAttr(const string& rowKey, const string& attrName, size_t& size)
{
    size = sizeof(TObjArray*);
    MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
    if ( rowKey != m_currentRowKey ) nextEvt(rowKey);
        log << MSG::INFO <<"HbaseInterface::getAttr:"<<"rowKey:"<<rowKey<<" attrName:"<<attrName<<"mask is: "<<m_mask<<endreq;

    if ( attrName == "m_runId" ) {
        if ( ! (m_mask&hasEvtHeader) ) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(Int_t);
        m_runId = EvtHeader->getRunId();
      
        log << MSG::INFO <<"HbaseInterface::getAttr:"<<"m_runId:"<<m_runId<<endreq;
        return &m_runId;
    }
    else if ( attrName == "m_eventId" ) {
        if ( ! (m_mask&hasEvtHeader) ) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(Int_t);
        m_eventId = EvtHeader->getEventId();
        return &m_eventId;
    }
    else if ( attrName == "m_time" ) {
        if ( ! (m_mask&hasEvtHeader) ) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(UInt_t);
        m_time = EvtHeader->time();
        return &m_time;
    }
    else if ( attrName == "m_eventTag" ) {
        if ( ! (m_mask&hasEvtHeader) ) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(UInt_t);
        m_eventTag = EvtHeader->getEventTag();
        return &m_eventTag;
    }
    else if ( attrName == "m_flag1" ) {
        if ( ! (m_mask&hasEvtHeader) ) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(UInt_t);
        m_flag1 = EvtHeader->getFlag1();
        return &m_flag1;
    }
    else if ( attrName == "m_flag2" ) {
        if ( !(m_mask&hasEvtHeader)) {
            getEntry(rowKey, "TEvtHeader");
	}
        size = sizeof(UInt_t);
        m_flag2 = EvtHeader->getFlag2();
        return &m_flag2;
    }
    else if ( attrName == "m_fromMc" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        size = sizeof(Bool_t);

        m_fromMc = DigiEvent->getFromMc();
        return &m_fromMc;
    }
    else if ( attrName == "m_mdcDigiCol" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        m_mdcDigiCol = const_cast<TObjArray*>(DigiEvent->getMdcDigiCol());
        return &m_mdcDigiCol;
    }
    else if ( attrName == "m_emcDigiCol" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        m_emcDigiCol = const_cast<TObjArray*>(DigiEvent->getEmcDigiCol());
        return &m_emcDigiCol;
    }
    else if ( attrName == "m_tofDigiCol" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        m_tofDigiCol = const_cast<TObjArray*>(DigiEvent->getTofDigiCol());
        return &m_tofDigiCol;
    }
    else if ( attrName == "m_mucDigiCol" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        m_mucDigiCol = const_cast<TObjArray*>(DigiEvent->getMucDigiCol());
        return &m_mucDigiCol;
    }
    else if ( attrName == "m_lumiDigiCol" ) {
        if ( ! (m_mask&hasDigiEvent) ) {
            getEntry(rowKey, "TDigiEvent");
	}
        m_lumiDigiCol = const_cast<TObjArray*>(DigiEvent->getLumiDigiCol());
        return &m_lumiDigiCol;
    }
    else if ( attrName == "m_emcTrackCol" ) {
        if ( ! (m_mask&hasDstEvent) ) {
            getEntry(rowKey, "TDstEvent");
	}
        m_emcTrackCol = const_cast<TObjArray*>(DstEvent->getEmcTrackCol());
        return &m_emcTrackCol;
    }
    else if ( attrName == "m_mdcTrackCol" ) {
        if ( !(m_mask&hasDstEvent)) {
            getEntry(rowKey, "TDstEvent");
	}
        m_mdcTrackCol = const_cast<TObjArray*>(DstEvent->getMdcTrackCol());
        return &m_mdcTrackCol;
    }
    else if ( attrName == "m_tofTrackCol" ) {
        if ( !(m_mask&hasDstEvent)) {
            getEntry(rowKey, "TDstEvent");
	}
        m_tofTrackCol = const_cast<TObjArray*>(DstEvent->getTofTrackCol());
        return &m_tofTrackCol;
    }
    else if ( attrName == "m_mucTrackCol" ) {
        if ( !(m_mask&hasDstEvent)) {
            getEntry(rowKey, "TDstEvent");
	}
        m_mucTrackCol = const_cast<TObjArray*>(DstEvent->getMucTrackCol());
        return &m_mucTrackCol;
    }
    else if ( attrName == "m_mdcDedxCol" ) {
        log << MSG::INFO<<"HbaseInterface::getHbaseValue:"<<"m_mask IS"<<m_mask<<"hasDstEvent"<<hasDstEvent<<endreq;
        if ( !(m_mask&hasDstEvent)) {
            getEntry(rowKey, "TDstEvent");
	}
        m_mdcDedxCol = const_cast<TObjArray*>(DstEvent->getMdcDedxCol());
        return &m_mdcDedxCol;
    }
    else if ( attrName == "m_extTrackCol" ) {
        if ( !(m_mask&hasDstEvent )) {
            getEntry(rowKey, "TDstEvent");
	}
        m_extTrackCol = const_cast<TObjArray*>(DstEvent->getExtTrackCol());
        return &m_extTrackCol;
    }
    else if ( attrName == "m_mdcKalTrackCol" ) {
        if ( !(m_mask&hasDstEvent)) {
            getEntry(rowKey, "TDstEvent");
	}
        m_mdcKalTrackCol = const_cast<TObjArray*>(DstEvent->getMdcKalTrackCol());
        return &m_mdcKalTrackCol;
    }
    else if ( attrName == "m_evtRecEvent" ) {
        if ( ! (m_mask&hasEvtRecObject) ) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecEvent = const_cast<TEvtRecEvent*>(EvtRecObject->getEvtRecEvent());
        return &m_evtRecEvent;
    }
    else if ( attrName == "m_evtRecTrackCol" ) {
               std::cout << __FILE__ << __LINE__ << std::endl;
               std::cout << std::hex << m_mask << " : " << hasEvtRecObject << std::dec << std::endl;
        if ( !(m_mask&hasEvtRecObject)) {
               std::cout << __FILE__ << __LINE__ << std::endl;
            getEntry(rowKey, "TEvtRecObject");
	}
               std::cout << __FILE__ << __LINE__ << std::endl;
        m_evtRecTrackCol = const_cast<TObjArray*>(EvtRecObject->getEvtRecTrackCol());
               std::cout << __FILE__ << __LINE__ << " : " << m_evtRecTrackCol << std::endl;
               std::cout << __FILE__ << __LINE__ << " : " << m_evtRecTrackCol->GetEntries() << std::endl;
        return &m_evtRecTrackCol;
    }
    else if ( attrName == "m_evtRecPrimaryVertex" ) {
        if ( !(m_mask&hasEvtRecObject)) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecPrimaryVertex = const_cast<TEvtRecPrimaryVertex*>(EvtRecObject->getEvtRecPrimaryVertex());
        return &m_evtRecPrimaryVertex;
    }
    else if ( attrName == "m_evtRecVeeVertexCol" ) {
        if ( !(m_mask&hasEvtRecObject)) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecVeeVertexCol = const_cast<TObjArray*>(EvtRecObject->getEvtRecVeeVertexCol());
        return &m_evtRecVeeVertexCol;
    }
    else if ( attrName == "m_evtRecPi0Col" ) {
        if ( !(m_mask&hasEvtRecObject)) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecPi0Col = const_cast<TObjArray*>(EvtRecObject->getEvtRecPi0Col());
        return &m_evtRecPi0Col;
    }
    else if ( attrName == "m_evtRecEtaToGGCol" ) {
        if ( !(m_mask&hasEvtRecObject)) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecEtaToGGCol = const_cast<TObjArray*>(EvtRecObject->getEvtRecEtaToGGCol());
        return &m_evtRecEtaToGGCol;
    }
    else if ( attrName == "m_evtRecDTagCol" ) {
        if ( !(m_mask&hasEvtRecObject)) {
            getEntry(rowKey, "TEvtRecObject");
	}
        m_evtRecDTagCol = const_cast<TObjArray*>(EvtRecObject->getEvtRecDTagCol());
        return &m_evtRecDTagCol;
    }
    else if ( attrName == "m_hltRawCol" ) {
        if ( !(m_mask&hasHltEvent)) {
            getEntry(rowKey, "THltEvent");
	}
        m_hltRawCol = const_cast<TObjArray*>(HltEvent->getHltRawCol());
        return &m_hltRawCol;
    }
    else if ( attrName == "m_hltInf" ) {
        if ( !(m_mask&hasHltEvent)) {
            getEntry(rowKey, "THltEvent");
	}
        m_hltInf = const_cast<THltInf*>(HltEvent->getHltInf());
        return &m_hltInf;
    }
    else if ( attrName == "m_dstHltInf" ) {
        if ( !(m_mask&hasHltEvent)) {
            getEntry(rowKey, "THltEvent");
	}
        m_dstHltInf = const_cast<TDstHltInf*>(HltEvent->getDstHltInf());
        return &m_dstHltInf;
    }
    else if ( attrName == "m_mdcMcHitCol" ) {
        if ( !(m_mask&hasMcEvent)) {
            getEntry(rowKey, "TMcEvent");
	}
        m_mdcMcHitCol = const_cast<TObjArray*>(McEvent->getMdcMcHitCol());
        return &m_mdcMcHitCol;
    }
    else if ( attrName == "m_emcMcHitCol" ) {
        if ( !(m_mask&hasMcEvent)) {
            getEntry(rowKey, "TMcEvent");
	}
        m_emcMcHitCol = const_cast<TObjArray*>(McEvent->getEmcMcHitCol());
        return &m_emcMcHitCol;
    }
    else if ( attrName == "m_tofMcHitCol" ) {
        if ( !(m_mask&hasMcEvent)) {
            getEntry(rowKey, "TMcEvent");
	}
        m_tofMcHitCol = const_cast<TObjArray*>(McEvent->getTofMcHitCol());
        return &m_tofMcHitCol;
    }
    else if ( attrName == "m_mucMcHitCol" ) {
        if ( !(m_mask&hasMcEvent)) {
            getEntry(rowKey, "TMcEvent");
	}
        m_mucMcHitCol = const_cast<TObjArray*>(McEvent->getMucMcHitCol());
        return &m_mucMcHitCol;
    }
    else if ( attrName == "m_mcParticleCol" ) {
        if ( !(m_mask&hasMcEvent)) {
            getEntry(rowKey, "TMcEvent");
	}
        m_mcParticleCol = const_cast<TObjArray*>(McEvent->getMcParticleCol());
        return &m_mcParticleCol;
    }
    else if ( attrName == "m_trigData" ) {
        if ( !(m_mask&hasTrigEvent)) {
            getEntry(rowKey, "TTrigEvent");
	}
        m_trigData = const_cast<TTrigData*>(TrigEvent->getTrigData());
        return &m_trigData;
    }
}

void HbaseInterface::nextEvt(const string& rowKey)
{
    m_currentRowKey = rowKey;
    //delete BossFullEvent;
    //BossFullEvent = 0;
    m_mask = 0;

    //先在hashmap中查找
    HbaseEvent* event;
    //UInt_t data_size = 0;
    event = this->_hashmap[rowKey];
    std::cout << __FILE__ << __LINE__ << std::endl;
    if(!event){
	    std::cout << __FILE__ << __LINE__ << std::endl;
	    getHbaseValue(rowKey);
	    event = this->_hashmap[rowKey];
    }
    std::cout << __FILE__ << __LINE__ << " : " << event->BossFullEvent<<" event->length "<<event->Length<< std::endl;
    TBossFullEvent* pevt = (TBossFullEvent*)deserialize(event->BossFullEvent,event->Length);
    if ( BossFullEvent == 0 ) {
	    BossFullEvent = pevt;
    }
    else {
	    BossFullEvent->copy(pevt);
    }
    std::cout << __FILE__ << __LINE__ << std::endl;
    MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HbaseInterface");
    log << MSG::INFO<<"getEvtRecEvent:"<<BossFullEvent->getEvtRecObject()->getEvtRecEvent()<<endreq;
    log << MSG::INFO<<"HbaseInterface::getHbaseValue:"<<"BossFullEvent"<<BossFullEvent->getEvtHeader()->getRunId()<<endreq;
}
