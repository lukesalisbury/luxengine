#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H



#ifdef NETWORKENABLED

class NetworkClient
{

	bool Open();
	bool Close();


	bool CreateMessage(uint8_t type, bool reliable);
	bool MessageAppend(fixed data);
	bool MessageAppend(uint8_t data);
	bool MessageAppend(uint32_t data);
	bool MessageSend(bool wait = false);
	int32_t ReadMessage(void * data);
	void Lock();
	void Unlock();
}
#endif



#endif // NETWORK_CLIENT_H

