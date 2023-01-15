/*
 * DnsData.cpp
 *
 *  Created on: 11-Jun-2016
 *      Author: debashis
 */

#include "DnsData.h"

using namespace std;

DnsData::DnsData()
{
	this->_name = "DnsData";
	this->setLogLevel(Log::theLog().level());
}

DnsData::~DnsData()
{}

void DnsData::updateDns(uint32_t resolvedip, std::string url)
{ DNSGlobal::dnsLookUpMap[resolvedip % 10][resolvedip] = std::string(url); }

//void DnsData::updateDnsV6(string resolvedip, string url)
//{ DNSGlobal::dnsV6LookUpMap[string(resolvedip)] = std::string(url); }
