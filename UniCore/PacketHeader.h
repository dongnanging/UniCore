#pragma once
//32bit
class PacketHeader
{
public:
	uint16 size;
	uint16 id;
};

struct RelayPacketInfo
{
public:
	bool cycle = false;
	int64 session_id;
	int64 client_id;
	int64 enter_id;
};

class RelayPacketHeader : public PacketHeader
{
public:
	RelayPacketInfo info;
};

