// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

class Socket : public std::enable_shared_from_this<Socket>
{
public:
	Socket()
	{
		std::cout << "Create Sockets" << std::endl;
	};
	~Socket()
	{
		std::cout << "Delete Sockets" << std::endl;
	};

	void Initialize()
	{
		std::cout << "Init!" << std::endl;
	}

	bool Send(std::string message)
	{
		std::cout << "Send " << message << std::endl;
		return true;
	}
	
	std::shared_ptr<Socket> Copy()
	{
		return shared_from_this();
	}
private:

};

class Comm
{
public:
	Comm()
	{
		std::cout << "Create Comm" << std::endl;
		socket = new Socket();
	}
	~Comm()
	{
		std::cout << "Destroy Comm" << std::endl;
		delete socket;
	}

	bool SendMessage(std::string message)
	{
		socket->Initialize();
		return socket->Send(message);
	}

private:
	Socket* socket;
};

class Radio
{
public:
	Radio()
	{
		std::cout << "Create Radio" << std::endl;
	}
	~Radio()
	{
		std::cout << "Destroy Radio" << std::endl;
	}

	void SendMessage(std::string message)
	{
		comm->SendMessage(message);
	}

	void SendMessageViaExternalComm(Comm* comm, std::string message)
	{
		comm->SendMessage(message);
	}

	void SendMessageViaExternalComm(std::weak_ptr<Comm> comm, std::string message)
	{
		std::shared_ptr<Comm> commBuff = comm.lock();
		if(commBuff)
			commBuff->SendMessage(message);
	}

	std::unique_ptr<Comm> GetComm()
	{
		return std::move(comm);
	}

	void SetComm(std::unique_ptr<Comm> externalComm)
	{
		comm = std::move(externalComm);
	}

private:
	std::unique_ptr<Comm> comm;
};

int main()
{
	{
		std::shared_ptr<Radio> radio = std::make_shared<Radio>();
		std::unique_ptr<Comm> comm = std::make_unique<Comm>();
		radio->SetComm(std::move(comm));

		std::thread thread1([radio]() 
		{
			std::shared_ptr<Radio> threadRadio = radio;
			threadRadio->SendMessage("Radio1 In Thread"); 
		});
		std::thread thread2([radio]()
		{
			std::shared_ptr<Radio> threadRadio = radio;
			threadRadio->SendMessage("Radio2 In Thread");
		});
		thread1.join();
		thread2.join();

	}
	
	//std::cout << "comm use count " << comm3.use_count() << std::endl;
	return 0;
}
