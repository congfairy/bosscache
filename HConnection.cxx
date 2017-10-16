#include "GaudiKernel/MsgStream.h"
#undef PACKAGE_VERSION
#include "RootCnvSvc/THBaseService.h"
#include "RootCnvSvc/HConnection.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
using namespace apache::hadoop::hbase::thrift2;
using namespace std;

HConnection::HConnection(const char *server, const char *port):
			socket(new TSocket(server, atoi(port))),
			transport(new TBufferedTransport(socket)),
			protocol(new TBinaryProtocol(transport)),
			client(protocol)
			{
                        MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
			_is_connected = false;
}

HConnection::~HConnection() {

}
//获得连接
bool HConnection::connect(){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
	      _is_connected = false;
	     try {
		     transport->open();
		     _is_connected = true;
		     log<<MSG::INFO<<"connection established..."<<endreq;
			} catch (const TException &tx) {
			log << MSG::ERROR  << tx.what() << endreq;
			}
	        return _is_connected;
	      }

bool HConnection::disconnect() {
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
	if(_is_connected){
  try {
    transport->close();
    _is_connected = false;
    log<<MSG::INFO<<"the connection is cut down..."<<endreq;
    return true;
  } catch (const TException &tx) {
    log << MSG::ERROR  << tx.what() << endreq;
    return false;
  }
	}
	return 0;
}

bool HConnection::reconnect() {
  MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
  bool flag = disconnect();
  flag = flag & connect();
  return flag;
}
/*bool exists(
     the table to check on
    1: required binary table,
    2: required TGet tget
  ) throws (1:TIOError io)*/
bool HConnection::exist(const std::string& table,const TGet& tget){
 MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
    bool exs = client.exists(table,tget);
    return exs;
    }
    catch (const TException &tx) {
    log << MSG::ERROR  << tx.what() << endreq;
    return false;
  }
}
int32_t HConnection::openScanner(const std::string& table, const TScan& tscan){
 MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
    int32_t exs = client.openScanner(table,tscan);
    return exs;
    }
    catch (const TException &tx) {
    log << MSG::ERROR  << tx.what() << endreq;
    return -1;
  }
}
void HConnection::getScannerRows(std::vector<TResult> & _return, const int32_t scannerId, const int32_t numRows){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
    client.getScannerRows(_return,scannerId,numRows);
    }
    catch (const TException &tx) {
    log << MSG::ERROR  << tx.what() << endreq;
  }
}
void HConnection::closeScanner(const int32_t scannerId){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
    client.closeScanner(scannerId);
    }
    catch (const TException &tx) {
    log << MSG::ERROR  << tx.what() << endreq;
  }
}
void HConnection::put(const std::string& table,const TPut& tput){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
client.put(table,tput);
}
catch (const TIOError &io){
log << MSG::ERROR  << io.what() << endreq;
}
}
void HConnection::putMultiple(const std::string& table,const std::vector<TPut>& puts){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
try{
    client.putMultiple(table,puts);
}
catch (const TIOError &io){
    log << MSG::ERROR  << io.what() << endreq;}
}
void HConnection::get(TResult& tresult,const std::string& table,const TGet& get){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");

    try{
        client.get(tresult,table,get);}
    catch (const TIOError &io){
    log<< MSG::ERROR  << io.what() << endreq;
    }
}

void HConnection::_delete(const std::string& table, const TDelete& tdelete){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
		try{
			client.deleteSingle(table,tdelete);
		}
		catch (const TIOError &io){
		    log << MSG::ERROR  << io.what() << endreq;
		}
}
void HConnection::getMultiple(std::vector<TResult> & results, const std::string& table,const  std::vector<TGet> & tgets){
             MsgStream log(dynamic_cast<IMessageSvc*>(Gaudi::svcLocator()),"HConnection");
 try{
    client.getMultiple(results,table,tgets);
    }
    catch (const TIOError &io){
    log << MSG::ERROR  << io.what() << endreq;}
}
