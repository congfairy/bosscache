#include "RootCnvSvc/serialize_deserialize.h"
#include <sstream>
std::string serialize(TObject* object,UInt_t& length){

	std::stringstream ss;
	TMessage* message = new TMessage(kMESS_OBJECT);
	message->WriteObject(object);
	char* buffer = message->Buffer();
        length = message->Length();
	//cout<<"length:"<<length<<endl;
	for(UInt_t i = 0 ;i<length;i++)
		ss<<buffer[i];
	std::string serStr = ss.str();
	return serStr;
//	return buffer;
}

TObject* deserialize(std::string buffer,UInt_t& length){
         std::cout << __FILE__ << __LINE__ << std::endl;
	char * data = (char *)malloc((length+1)*sizeof(char));
         std::cout << __FILE__ << __LINE__ << std::endl;
	buffer.copy(data,length,0);
         std::cout << __FILE__ << __LINE__ << std::endl;
	//const char * buffer=serStr.c_str();
	MyTMessage *message_demo = new MyTMessage(data,length);
         std::cout << __FILE__ << __LINE__ << std::endl;
        std::cout<<"start deserialize ..."<<std::endl;
	TObject *object = message_demo->ReadObject(message_demo->GetClass());
         std::cout << __FILE__ << __LINE__ << std::endl;
//	cout<<"deserialize finished ..."<<endl;
	return object;

}
