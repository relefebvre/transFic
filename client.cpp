#include "client.h"

int Client::maxSock=0;

Client::Client(int newSock)
{
	this->sock = newSock;
	if (newSock+1 > Client::maxSock)
		Client::maxSock = newSock+1;
	this->first = true;
}

Client::~Client()
{}


void Client::setSock(int newSock)
{
	this->sock = newSock;
}


int Client::getSock()
{
	return this->sock;
}


void Client::setFilename(char *newFilename)
{
	strcpy(this->filename,newFilename);
	perror("Copie ");
}


void Client::setFirst()
{
	this->first = false;
}


bool Client::getFirst()
{
	return this->first;
}


void Client::setFd(int newFd)
{
	this->fd = newFd;
}


int Client::getFd()
{
	return this->fd;
}
