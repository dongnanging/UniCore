#pragma once
#include "Cryptonizer.h"

namespace dto
{
	//forward
	class SERVER_USER_PROBE;
	class GATE_USER_PROBE;
	class SERVER_CHATROOM_PROBE;
	class DB_CHATROOM_PROBE;
	class GATE_CHATROOM_PROBE;
	class GATE_CHAT_DATA;
	class CLIENT_CHATROOM_PROBE;
	class Item;
	class SERVER_ITEM_PROBE;
	class AUCTION_ITEM_PROBE;
	class USER_DATA_SHARABLE;
	class USER_DATA;
}
namespace vo
{
	//forward
	class SERVER_USER_PROBE;
	class GATE_USER_PROBE;
	class SERVER_CHATROOM_PROBE;
	class DB_CHATROOM_PROBE;
	class GATE_CHATROOM_PROBE;
	class GATE_CHAT_DATA;
	class CLIENT_CHATROOM_PROBE;
	class Item;
	class SERVER_ITEM_PROBE;
	class AUCTION_ITEM_PROBE;
	class USER_DATA_SHARABLE;
	class USER_DATA;
}

namespace vo
{
	
	class SERVER_USER_PROBE
	{
	public:
		//constructor - raw values
		SERVER_USER_PROBE(
			const int64& uid,
			const std::wstring& nickname,
			const int64& client_sesion_id);
		//constructor copy from dto
		SERVER_USER_PROBE(const dto::SERVER_USER_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_USER_PROBE(const _Packet& packet)
			:
			_uid(packet.uid()),
			_nickname(Cryptonizer::BytesToWstring(packet.nickname())),
			_client_sesion_id(packet.client_sesion_id())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_USER_PROBE(const _Packet* packet)
			:
			_uid(packet->uid()),
			_nickname(Cryptonizer::BytesToWstring(packet->nickname())),
			_client_sesion_id(packet->client_sesion_id())
		{
		}
	public:
		//getter
		const int64& uid() const;
		const std::wstring& nickname() const;
		const int64& client_sesion_id() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_uid(_uid);
			packet.set_nickname(Cryptonizer::WstringToBytes(_nickname));
			packet.set_client_sesion_id(_client_sesion_id);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_uid(_uid);
			packet->set_nickname(Cryptonizer::WstringToBytes(_nickname));
			packet->set_client_sesion_id(_client_sesion_id);
		}
		 
	private:
		int64 _uid;
		std::wstring _nickname;
		int64 _client_sesion_id;
	};
	
	class GATE_USER_PROBE
	{
	public:
		//constructor - raw values
		GATE_USER_PROBE(
			const SERVER_USER_PROBE& user_probe,
			const int64& server_id);
		//constructor copy from dto
		GATE_USER_PROBE(const dto::GATE_USER_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_USER_PROBE(const _Packet& packet)
			:
			_user_probe(packet.user_probe()),
			_server_id(packet.server_id())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_USER_PROBE(const _Packet* packet)
			:
			_user_probe(packet->user_probe()),
			_server_id(packet->server_id())
		{
		}
	public:
		//getter
		const SERVER_USER_PROBE& user_probe() const;
		const int64& server_id() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_user_probe = packet.mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet.set_server_id(_server_id);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_user_probe = packet->mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet->set_server_id(_server_id);
		}
		 
	private:
		SERVER_USER_PROBE _user_probe;
		int64 _server_id;
	};
	
	class SERVER_CHATROOM_PROBE
	{
	public:
		//constructor - raw values
		SERVER_CHATROOM_PROBE(
			const int64& room_id,
			const std::wstring& room_name,
			const int32& max_user_count,
			const bool& is_private,
			const std::wstring& room_pw);
		//constructor copy from dto
		SERVER_CHATROOM_PROBE(const dto::SERVER_CHATROOM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_CHATROOM_PROBE(const _Packet& packet)
			:
			_room_id(packet.room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet.room_name())),
			_max_user_count(packet.max_user_count()),
			_is_private(packet.is_private()),
			_room_pw(Cryptonizer::BytesToWstring(packet.room_pw()))
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_CHATROOM_PROBE(const _Packet* packet)
			:
			_room_id(packet->room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet->room_name())),
			_max_user_count(packet->max_user_count()),
			_is_private(packet->is_private()),
			_room_pw(Cryptonizer::BytesToWstring(packet->room_pw()))
		{
		}
	public:
		//getter
		const int64& room_id() const;
		const std::wstring& room_name() const;
		const int32& max_user_count() const;
		const bool& is_private() const;
		const std::wstring& room_pw() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_room_id(_room_id);
			packet.set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet.set_max_user_count(_max_user_count);
			packet.set_is_private(_is_private);
			packet.set_room_pw(Cryptonizer::WstringToBytes(_room_pw));
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_room_id(_room_id);
			packet->set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet->set_max_user_count(_max_user_count);
			packet->set_is_private(_is_private);
			packet->set_room_pw(Cryptonizer::WstringToBytes(_room_pw));
		}
		 
	private:
		int64 _room_id;
		std::wstring _room_name;
		int32 _max_user_count;
		bool _is_private;
		std::wstring _room_pw;
	};
	
	class DB_CHATROOM_PROBE
	{
	public:
		//constructor - raw values
		DB_CHATROOM_PROBE(
			const SERVER_CHATROOM_PROBE& probe,
			const int32& now_user);
		//constructor copy from dto
		DB_CHATROOM_PROBE(const dto::DB_CHATROOM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		DB_CHATROOM_PROBE(const _Packet& packet)
			:
			_probe(packet.probe()),
			_now_user(packet.now_user())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		DB_CHATROOM_PROBE(const _Packet* packet)
			:
			_probe(packet->probe()),
			_now_user(packet->now_user())
		{
		}
	public:
		//getter
		const SERVER_CHATROOM_PROBE& probe() const;
		const int32& now_user() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_probe = packet.mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet.set_now_user(_now_user);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_probe = packet->mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet->set_now_user(_now_user);
		}
		 
	private:
		SERVER_CHATROOM_PROBE _probe;
		int32 _now_user;
	};
	
	class GATE_CHATROOM_PROBE
	{
	public:
		//constructor - raw values
		GATE_CHATROOM_PROBE(
			const SERVER_CHATROOM_PROBE& chatroom_probe,
			const int64& server_id,
			const std::string& server_ip,
			const int32& server_port);
		//constructor copy from dto
		GATE_CHATROOM_PROBE(const dto::GATE_CHATROOM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_CHATROOM_PROBE(const _Packet& packet)
			:
			_chatroom_probe(packet.chatroom_probe()),
			_server_id(packet.server_id()),
			_server_ip(packet.server_ip()),
			_server_port(packet.server_port())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_CHATROOM_PROBE(const _Packet* packet)
			:
			_chatroom_probe(packet->chatroom_probe()),
			_server_id(packet->server_id()),
			_server_ip(packet->server_ip()),
			_server_port(packet->server_port())
		{
		}
	public:
		//getter
		const SERVER_CHATROOM_PROBE& chatroom_probe() const;
		const int64& server_id() const;
		const std::string& server_ip() const;
		const int32& server_port() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_chatroom_probe = packet.mutable_chatroom_probe();
			_chatroom_probe.ToPacket(OUT m_chatroom_probe);
			packet.set_server_id(_server_id);
			packet.set_server_ip(_server_ip);
			packet.set_server_port(_server_port);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_chatroom_probe = packet->mutable_chatroom_probe();
			_chatroom_probe.ToPacket(OUT m_chatroom_probe);
			packet->set_server_id(_server_id);
			packet->set_server_ip(_server_ip);
			packet->set_server_port(_server_port);
		}
		 
	private:
		SERVER_CHATROOM_PROBE _chatroom_probe;
		int64 _server_id;
		std::string _server_ip;
		int32 _server_port;
	};
	
	class GATE_CHAT_DATA
	{
	public:
		//constructor - raw values
		GATE_CHAT_DATA(
			const GATE_USER_PROBE& user_probe,
			const std::wstring& chat);
		//constructor copy from dto
		GATE_CHAT_DATA(const dto::GATE_CHAT_DATA& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_CHAT_DATA(const _Packet& packet)
			:
			_user_probe(packet.user_probe()),
			_chat(Cryptonizer::BytesToWstring(packet.chat()))
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_CHAT_DATA(const _Packet* packet)
			:
			_user_probe(packet->user_probe()),
			_chat(Cryptonizer::BytesToWstring(packet->chat()))
		{
		}
	public:
		//getter
		const GATE_USER_PROBE& user_probe() const;
		const std::wstring& chat() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_user_probe = packet.mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet.set_chat(Cryptonizer::WstringToBytes(_chat));
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_user_probe = packet->mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet->set_chat(Cryptonizer::WstringToBytes(_chat));
		}
		 
	private:
		GATE_USER_PROBE _user_probe;
		std::wstring _chat;
	};
	
	class CLIENT_CHATROOM_PROBE
	{
	public:
		//constructor - raw values
		CLIENT_CHATROOM_PROBE(
			const int64& room_id,
			const std::wstring& room_name,
			const int32& max_user_count,
			const bool& is_private,
			const int32& user_count);
		//constructor copy from dto
		CLIENT_CHATROOM_PROBE(const dto::CLIENT_CHATROOM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		CLIENT_CHATROOM_PROBE(const _Packet& packet)
			:
			_room_id(packet.room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet.room_name())),
			_max_user_count(packet.max_user_count()),
			_is_private(packet.is_private()),
			_user_count(packet.user_count())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		CLIENT_CHATROOM_PROBE(const _Packet* packet)
			:
			_room_id(packet->room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet->room_name())),
			_max_user_count(packet->max_user_count()),
			_is_private(packet->is_private()),
			_user_count(packet->user_count())
		{
		}
	public:
		//getter
		const int64& room_id() const;
		const std::wstring& room_name() const;
		const int32& max_user_count() const;
		const bool& is_private() const;
		const int32& user_count() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_room_id(_room_id);
			packet.set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet.set_max_user_count(_max_user_count);
			packet.set_is_private(_is_private);
			packet.set_user_count(_user_count);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_room_id(_room_id);
			packet->set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet->set_max_user_count(_max_user_count);
			packet->set_is_private(_is_private);
			packet->set_user_count(_user_count);
		}
		 
	private:
		int64 _room_id;
		std::wstring _room_name;
		int32 _max_user_count;
		bool _is_private;
		int32 _user_count;
	};
	
	class Item
	{
	public:
		//constructor - raw values
		Item(
			const int32& item_code,
			const std::string& item_name);
		//constructor copy from dto
		Item(const dto::Item& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		Item(const _Packet& packet)
			:
			_item_code(packet.item_code()),
			_item_name(packet.item_name())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		Item(const _Packet* packet)
			:
			_item_code(packet->item_code()),
			_item_name(packet->item_name())
		{
		}
	public:
		//getter
		const int32& item_code() const;
		const std::string& item_name() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_item_code(_item_code);
			packet.set_item_name(_item_name);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_item_code(_item_code);
			packet->set_item_name(_item_name);
		}
		 
	private:
		int32 _item_code;
		std::string _item_name;
	};
	
	class SERVER_ITEM_PROBE
	{
	public:
		//constructor - raw values
		SERVER_ITEM_PROBE(
			const int64& item_id,
			const int64& owner,
			const Item& item);
		//constructor copy from dto
		SERVER_ITEM_PROBE(const dto::SERVER_ITEM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_ITEM_PROBE(const _Packet& packet)
			:
			_item_id(packet.item_id()),
			_owner(packet.owner()),
			_item(packet.item())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_ITEM_PROBE(const _Packet* packet)
			:
			_item_id(packet->item_id()),
			_owner(packet->owner()),
			_item(packet->item())
		{
		}
	public:
		//getter
		const int64& item_id() const;
		const int64& owner() const;
		const Item& item() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_item_id(_item_id);
			packet.set_owner(_owner);
			auto m_item = packet.mutable_item();
			_item.ToPacket(OUT m_item);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_item_id(_item_id);
			packet->set_owner(_owner);
			auto m_item = packet->mutable_item();
			_item.ToPacket(OUT m_item);
		}
		 
	private:
		int64 _item_id;
		int64 _owner;
		Item _item;
	};
	
	class AUCTION_ITEM_PROBE
	{
	public:
		//constructor - raw values
		AUCTION_ITEM_PROBE(
			const int64& auction_id,
			const SERVER_ITEM_PROBE& probe,
			const int64& max_price,
			const int64& now_bidder,
			const int64& now_price,
			const int32& rest_mlisec);
		//constructor copy from dto
		AUCTION_ITEM_PROBE(const dto::AUCTION_ITEM_PROBE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		AUCTION_ITEM_PROBE(const _Packet& packet)
			:
			_auction_id(packet.auction_id()),
			_probe(packet.probe()),
			_max_price(packet.max_price()),
			_now_bidder(packet.now_bidder()),
			_now_price(packet.now_price()),
			_rest_mlisec(packet.rest_mlisec())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		AUCTION_ITEM_PROBE(const _Packet* packet)
			:
			_auction_id(packet->auction_id()),
			_probe(packet->probe()),
			_max_price(packet->max_price()),
			_now_bidder(packet->now_bidder()),
			_now_price(packet->now_price()),
			_rest_mlisec(packet->rest_mlisec())
		{
		}
	public:
		//getter
		const int64& auction_id() const;
		const SERVER_ITEM_PROBE& probe() const;
		const int64& max_price() const;
		const int64& now_bidder() const;
		const int64& now_price() const;
		const int32& rest_mlisec() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_auction_id(_auction_id);
			auto m_probe = packet.mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet.set_max_price(_max_price);
			packet.set_now_bidder(_now_bidder);
			packet.set_now_price(_now_price);
			packet.set_rest_mlisec(_rest_mlisec);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_auction_id(_auction_id);
			auto m_probe = packet->mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet->set_max_price(_max_price);
			packet->set_now_bidder(_now_bidder);
			packet->set_now_price(_now_price);
			packet->set_rest_mlisec(_rest_mlisec);
		}
		 
	private:
		int64 _auction_id;
		SERVER_ITEM_PROBE _probe;
		int64 _max_price;
		int64 _now_bidder;
		int64 _now_price;
		int32 _rest_mlisec;
	};
	
	class USER_DATA_SHARABLE
	{
	public:
		//constructor - raw values
		USER_DATA_SHARABLE(
			const int32& level);
		//constructor copy from dto
		USER_DATA_SHARABLE(const dto::USER_DATA_SHARABLE& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		USER_DATA_SHARABLE(const _Packet& packet)
			:
			_level(packet.level())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		USER_DATA_SHARABLE(const _Packet* packet)
			:
			_level(packet->level())
		{
		}
	public:
		//getter
		const int32& level() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_level(_level);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_level(_level);
		}
		 
	private:
		int32 _level;
	};
	
	class USER_DATA
	{
	public:
		//constructor - raw values
		USER_DATA(
			const int64& mileage,
			const std::vector<SERVER_ITEM_PROBE>& items);
		//constructor copy from dto
		USER_DATA(const dto::USER_DATA& dto);
		//constructor copy from packet ref
		template<typename _Packet>
		USER_DATA(const _Packet& packet)
			:
			_mileage(packet.mileage())
		{
			auto m_datas = packet.items();
			for(int i = 0; i < m_datas.size(); i++)
				_items.emplace_back(m_datas->Get(i));
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		USER_DATA(const _Packet* packet)
			:
			_mileage(packet->mileage())
		{
			auto m_datas = packet->items();
			for(int i = 0; i < m_datas.size(); i++)
				_items.emplace_back(m_datas->Get(i));
		}
	public:
		//getter
		const int64& mileage() const;
		const std::vector<SERVER_ITEM_PROBE>& items() const;

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_mileage(_mileage);
			for(int i = 0; i < _items.size(); i++)
			{
				auto m_items = packet.add_items();
				_items[i].ToPacket(OUT m_items);
			}
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_mileage(_mileage);
			for(int i = 0; i < _items.size(); i++)
			{
				auto m_items = packet->add_items();
				_items[i].ToPacket(OUT m_items);
			}
		}
		 
	private:
		int64 _mileage;
		std::vector<SERVER_ITEM_PROBE> _items;
	};
	
}

namespace dto
{
	
	class SERVER_USER_PROBE
	{
	public:
		SERVER_USER_PROBE() = default;
		//constructor - raw values
		SERVER_USER_PROBE(
			const int64& uid,
			const std::wstring& nickname,
			const int64& client_sesion_id);
		//constructor copy from vo
		SERVER_USER_PROBE(const vo::SERVER_USER_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_USER_PROBE(const _Packet& packet)
			:
			_uid(packet.uid()),
			_nickname(Cryptonizer::BytesToWstring(packet.nickname())),
			_client_sesion_id(packet.client_sesion_id())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_USER_PROBE(const _Packet* packet)
			:
			_uid(packet->uid()),
			_nickname(Cryptonizer::BytesToWstring(packet->nickname())),
			_client_sesion_id(packet->client_sesion_id())
		{
		}
	public:
		//getter - const
		const int64& uid() const;
		const std::wstring& nickname() const;
		const int64& client_sesion_id() const;

		//getter - mutable
		int64& mutable_uid();
		std::wstring& mutable_nickname();
		int64& mutable_client_sesion_id();

		//setter
		void set_uid(const int64& uid);
		void set_nickname(const std::wstring& nickname);
		void set_client_sesion_id(const int64& client_sesion_id);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_uid(_uid);
			packet.set_nickname(Cryptonizer::WstringToBytes(_nickname));
			packet.set_client_sesion_id(_client_sesion_id);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_uid(_uid);
			packet->set_nickname(Cryptonizer::WstringToBytes(_nickname));
			packet->set_client_sesion_id(_client_sesion_id);
		}

		void TransferTo(OUT dto::SERVER_USER_PROBE& other);

		std::tuple<int64&,std::wstring&,int64&> FeedForward()
		{
			return std::tie(_uid,_nickname,_client_sesion_id);
		}

	private:
		int64 _uid;
		std::wstring _nickname;
		int64 _client_sesion_id;
	};
	
	class GATE_USER_PROBE
	{
	public:
		GATE_USER_PROBE() = default;
		//constructor - raw values
		GATE_USER_PROBE(
			const SERVER_USER_PROBE& user_probe,
			const int64& server_id);
		//constructor copy from vo
		GATE_USER_PROBE(const vo::GATE_USER_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_USER_PROBE(const _Packet& packet)
			:
			_user_probe(packet.user_probe()),
			_server_id(packet.server_id())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_USER_PROBE(const _Packet* packet)
			:
			_user_probe(packet->user_probe()),
			_server_id(packet->server_id())
		{
		}
	public:
		//getter - const
		const SERVER_USER_PROBE& user_probe() const;
		const int64& server_id() const;

		//getter - mutable
		SERVER_USER_PROBE& mutable_user_probe();
		int64& mutable_server_id();

		//setter
		void set_user_probe(const SERVER_USER_PROBE& user_probe);
		void set_server_id(const int64& server_id);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_user_probe = packet.mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet.set_server_id(_server_id);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_user_probe = packet->mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet->set_server_id(_server_id);
		}

		void TransferTo(OUT dto::GATE_USER_PROBE& other);

		std::tuple<SERVER_USER_PROBE&,int64&> FeedForward()
		{
			return std::tie(_user_probe,_server_id);
		}

	private:
		SERVER_USER_PROBE _user_probe;
		int64 _server_id;
	};
	
	class SERVER_CHATROOM_PROBE
	{
	public:
		SERVER_CHATROOM_PROBE() = default;
		//constructor - raw values
		SERVER_CHATROOM_PROBE(
			const int64& room_id,
			const std::wstring& room_name,
			const int32& max_user_count,
			const bool& is_private,
			const std::wstring& room_pw);
		//constructor copy from vo
		SERVER_CHATROOM_PROBE(const vo::SERVER_CHATROOM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_CHATROOM_PROBE(const _Packet& packet)
			:
			_room_id(packet.room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet.room_name())),
			_max_user_count(packet.max_user_count()),
			_is_private(packet.is_private()),
			_room_pw(Cryptonizer::BytesToWstring(packet.room_pw()))
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_CHATROOM_PROBE(const _Packet* packet)
			:
			_room_id(packet->room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet->room_name())),
			_max_user_count(packet->max_user_count()),
			_is_private(packet->is_private()),
			_room_pw(Cryptonizer::BytesToWstring(packet->room_pw()))
		{
		}
	public:
		//getter - const
		const int64& room_id() const;
		const std::wstring& room_name() const;
		const int32& max_user_count() const;
		const bool& is_private() const;
		const std::wstring& room_pw() const;

		//getter - mutable
		int64& mutable_room_id();
		std::wstring& mutable_room_name();
		int32& mutable_max_user_count();
		bool& mutable_is_private();
		std::wstring& mutable_room_pw();

		//setter
		void set_room_id(const int64& room_id);
		void set_room_name(const std::wstring& room_name);
		void set_max_user_count(const int32& max_user_count);
		void set_is_private(const bool& is_private);
		void set_room_pw(const std::wstring& room_pw);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_room_id(_room_id);
			packet.set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet.set_max_user_count(_max_user_count);
			packet.set_is_private(_is_private);
			packet.set_room_pw(Cryptonizer::WstringToBytes(_room_pw));
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_room_id(_room_id);
			packet->set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet->set_max_user_count(_max_user_count);
			packet->set_is_private(_is_private);
			packet->set_room_pw(Cryptonizer::WstringToBytes(_room_pw));
		}

		void TransferTo(OUT dto::SERVER_CHATROOM_PROBE& other);

		std::tuple<int64&,std::wstring&,int32&,bool&,std::wstring&> FeedForward()
		{
			return std::tie(_room_id,_room_name,_max_user_count,_is_private,_room_pw);
		}

	private:
		int64 _room_id;
		std::wstring _room_name;
		int32 _max_user_count;
		bool _is_private;
		std::wstring _room_pw;
	};
	
	class DB_CHATROOM_PROBE
	{
	public:
		DB_CHATROOM_PROBE() = default;
		//constructor - raw values
		DB_CHATROOM_PROBE(
			const SERVER_CHATROOM_PROBE& probe,
			const int32& now_user);
		//constructor copy from vo
		DB_CHATROOM_PROBE(const vo::DB_CHATROOM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		DB_CHATROOM_PROBE(const _Packet& packet)
			:
			_probe(packet.probe()),
			_now_user(packet.now_user())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		DB_CHATROOM_PROBE(const _Packet* packet)
			:
			_probe(packet->probe()),
			_now_user(packet->now_user())
		{
		}
	public:
		//getter - const
		const SERVER_CHATROOM_PROBE& probe() const;
		const int32& now_user() const;

		//getter - mutable
		SERVER_CHATROOM_PROBE& mutable_probe();
		int32& mutable_now_user();

		//setter
		void set_probe(const SERVER_CHATROOM_PROBE& probe);
		void set_now_user(const int32& now_user);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_probe = packet.mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet.set_now_user(_now_user);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_probe = packet->mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet->set_now_user(_now_user);
		}

		void TransferTo(OUT dto::DB_CHATROOM_PROBE& other);

		std::tuple<SERVER_CHATROOM_PROBE&,int32&> FeedForward()
		{
			return std::tie(_probe,_now_user);
		}

	private:
		SERVER_CHATROOM_PROBE _probe;
		int32 _now_user;
	};
	
	class GATE_CHATROOM_PROBE
	{
	public:
		GATE_CHATROOM_PROBE() = default;
		//constructor - raw values
		GATE_CHATROOM_PROBE(
			const SERVER_CHATROOM_PROBE& chatroom_probe,
			const int64& server_id,
			const std::string& server_ip,
			const int32& server_port);
		//constructor copy from vo
		GATE_CHATROOM_PROBE(const vo::GATE_CHATROOM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_CHATROOM_PROBE(const _Packet& packet)
			:
			_chatroom_probe(packet.chatroom_probe()),
			_server_id(packet.server_id()),
			_server_ip(packet.server_ip()),
			_server_port(packet.server_port())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_CHATROOM_PROBE(const _Packet* packet)
			:
			_chatroom_probe(packet->chatroom_probe()),
			_server_id(packet->server_id()),
			_server_ip(packet->server_ip()),
			_server_port(packet->server_port())
		{
		}
	public:
		//getter - const
		const SERVER_CHATROOM_PROBE& chatroom_probe() const;
		const int64& server_id() const;
		const std::string& server_ip() const;
		const int32& server_port() const;

		//getter - mutable
		SERVER_CHATROOM_PROBE& mutable_chatroom_probe();
		int64& mutable_server_id();
		std::string& mutable_server_ip();
		int32& mutable_server_port();

		//setter
		void set_chatroom_probe(const SERVER_CHATROOM_PROBE& chatroom_probe);
		void set_server_id(const int64& server_id);
		void set_server_ip(const std::string& server_ip);
		void set_server_port(const int32& server_port);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_chatroom_probe = packet.mutable_chatroom_probe();
			_chatroom_probe.ToPacket(OUT m_chatroom_probe);
			packet.set_server_id(_server_id);
			packet.set_server_ip(_server_ip);
			packet.set_server_port(_server_port);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_chatroom_probe = packet->mutable_chatroom_probe();
			_chatroom_probe.ToPacket(OUT m_chatroom_probe);
			packet->set_server_id(_server_id);
			packet->set_server_ip(_server_ip);
			packet->set_server_port(_server_port);
		}

		void TransferTo(OUT dto::GATE_CHATROOM_PROBE& other);

		std::tuple<SERVER_CHATROOM_PROBE&,int64&,std::string&,int32&> FeedForward()
		{
			return std::tie(_chatroom_probe,_server_id,_server_ip,_server_port);
		}

	private:
		SERVER_CHATROOM_PROBE _chatroom_probe;
		int64 _server_id;
		std::string _server_ip;
		int32 _server_port;
	};
	
	class GATE_CHAT_DATA
	{
	public:
		GATE_CHAT_DATA() = default;
		//constructor - raw values
		GATE_CHAT_DATA(
			const GATE_USER_PROBE& user_probe,
			const std::wstring& chat);
		//constructor copy from vo
		GATE_CHAT_DATA(const vo::GATE_CHAT_DATA& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		GATE_CHAT_DATA(const _Packet& packet)
			:
			_user_probe(packet.user_probe()),
			_chat(Cryptonizer::BytesToWstring(packet.chat()))
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		GATE_CHAT_DATA(const _Packet* packet)
			:
			_user_probe(packet->user_probe()),
			_chat(Cryptonizer::BytesToWstring(packet->chat()))
		{
		}
	public:
		//getter - const
		const GATE_USER_PROBE& user_probe() const;
		const std::wstring& chat() const;

		//getter - mutable
		GATE_USER_PROBE& mutable_user_probe();
		std::wstring& mutable_chat();

		//setter
		void set_user_probe(const GATE_USER_PROBE& user_probe);
		void set_chat(const std::wstring& chat);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			auto m_user_probe = packet.mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet.set_chat(Cryptonizer::WstringToBytes(_chat));
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			auto m_user_probe = packet->mutable_user_probe();
			_user_probe.ToPacket(OUT m_user_probe);
			packet->set_chat(Cryptonizer::WstringToBytes(_chat));
		}

		void TransferTo(OUT dto::GATE_CHAT_DATA& other);

		std::tuple<GATE_USER_PROBE&,std::wstring&> FeedForward()
		{
			return std::tie(_user_probe,_chat);
		}

	private:
		GATE_USER_PROBE _user_probe;
		std::wstring _chat;
	};
	
	class CLIENT_CHATROOM_PROBE
	{
	public:
		CLIENT_CHATROOM_PROBE() = default;
		//constructor - raw values
		CLIENT_CHATROOM_PROBE(
			const int64& room_id,
			const std::wstring& room_name,
			const int32& max_user_count,
			const bool& is_private,
			const int32& user_count);
		//constructor copy from vo
		CLIENT_CHATROOM_PROBE(const vo::CLIENT_CHATROOM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		CLIENT_CHATROOM_PROBE(const _Packet& packet)
			:
			_room_id(packet.room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet.room_name())),
			_max_user_count(packet.max_user_count()),
			_is_private(packet.is_private()),
			_user_count(packet.user_count())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		CLIENT_CHATROOM_PROBE(const _Packet* packet)
			:
			_room_id(packet->room_id()),
			_room_name(Cryptonizer::BytesToWstring(packet->room_name())),
			_max_user_count(packet->max_user_count()),
			_is_private(packet->is_private()),
			_user_count(packet->user_count())
		{
		}
	public:
		//getter - const
		const int64& room_id() const;
		const std::wstring& room_name() const;
		const int32& max_user_count() const;
		const bool& is_private() const;
		const int32& user_count() const;

		//getter - mutable
		int64& mutable_room_id();
		std::wstring& mutable_room_name();
		int32& mutable_max_user_count();
		bool& mutable_is_private();
		int32& mutable_user_count();

		//setter
		void set_room_id(const int64& room_id);
		void set_room_name(const std::wstring& room_name);
		void set_max_user_count(const int32& max_user_count);
		void set_is_private(const bool& is_private);
		void set_user_count(const int32& user_count);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_room_id(_room_id);
			packet.set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet.set_max_user_count(_max_user_count);
			packet.set_is_private(_is_private);
			packet.set_user_count(_user_count);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_room_id(_room_id);
			packet->set_room_name(Cryptonizer::WstringToBytes(_room_name));
			packet->set_max_user_count(_max_user_count);
			packet->set_is_private(_is_private);
			packet->set_user_count(_user_count);
		}

		void TransferTo(OUT dto::CLIENT_CHATROOM_PROBE& other);

		std::tuple<int64&,std::wstring&,int32&,bool&,int32&> FeedForward()
		{
			return std::tie(_room_id,_room_name,_max_user_count,_is_private,_user_count);
		}

	private:
		int64 _room_id;
		std::wstring _room_name;
		int32 _max_user_count;
		bool _is_private;
		int32 _user_count;
	};
	
	class Item
	{
	public:
		Item() = default;
		//constructor - raw values
		Item(
			const int32& item_code,
			const std::string& item_name);
		//constructor copy from vo
		Item(const vo::Item& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		Item(const _Packet& packet)
			:
			_item_code(packet.item_code()),
			_item_name(packet.item_name())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		Item(const _Packet* packet)
			:
			_item_code(packet->item_code()),
			_item_name(packet->item_name())
		{
		}
	public:
		//getter - const
		const int32& item_code() const;
		const std::string& item_name() const;

		//getter - mutable
		int32& mutable_item_code();
		std::string& mutable_item_name();

		//setter
		void set_item_code(const int32& item_code);
		void set_item_name(const std::string& item_name);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_item_code(_item_code);
			packet.set_item_name(_item_name);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_item_code(_item_code);
			packet->set_item_name(_item_name);
		}

		void TransferTo(OUT dto::Item& other);

		std::tuple<int32&,std::string&> FeedForward()
		{
			return std::tie(_item_code,_item_name);
		}

	private:
		int32 _item_code;
		std::string _item_name;
	};
	
	class SERVER_ITEM_PROBE
	{
	public:
		SERVER_ITEM_PROBE() = default;
		//constructor - raw values
		SERVER_ITEM_PROBE(
			const int64& item_id,
			const int64& owner,
			const Item& item);
		//constructor copy from vo
		SERVER_ITEM_PROBE(const vo::SERVER_ITEM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		SERVER_ITEM_PROBE(const _Packet& packet)
			:
			_item_id(packet.item_id()),
			_owner(packet.owner()),
			_item(packet.item())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		SERVER_ITEM_PROBE(const _Packet* packet)
			:
			_item_id(packet->item_id()),
			_owner(packet->owner()),
			_item(packet->item())
		{
		}
	public:
		//getter - const
		const int64& item_id() const;
		const int64& owner() const;
		const Item& item() const;

		//getter - mutable
		int64& mutable_item_id();
		int64& mutable_owner();
		Item& mutable_item();

		//setter
		void set_item_id(const int64& item_id);
		void set_owner(const int64& owner);
		void set_item(const Item& item);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_item_id(_item_id);
			packet.set_owner(_owner);
			auto m_item = packet.mutable_item();
			_item.ToPacket(OUT m_item);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_item_id(_item_id);
			packet->set_owner(_owner);
			auto m_item = packet->mutable_item();
			_item.ToPacket(OUT m_item);
		}

		void TransferTo(OUT dto::SERVER_ITEM_PROBE& other);

		std::tuple<int64&,int64&,Item&> FeedForward()
		{
			return std::tie(_item_id,_owner,_item);
		}

	private:
		int64 _item_id;
		int64 _owner;
		Item _item;
	};
	
	class AUCTION_ITEM_PROBE
	{
	public:
		AUCTION_ITEM_PROBE() = default;
		//constructor - raw values
		AUCTION_ITEM_PROBE(
			const int64& auction_id,
			const SERVER_ITEM_PROBE& probe,
			const int64& max_price,
			const int64& now_bidder,
			const int64& now_price,
			const int32& rest_mlisec);
		//constructor copy from vo
		AUCTION_ITEM_PROBE(const vo::AUCTION_ITEM_PROBE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		AUCTION_ITEM_PROBE(const _Packet& packet)
			:
			_auction_id(packet.auction_id()),
			_probe(packet.probe()),
			_max_price(packet.max_price()),
			_now_bidder(packet.now_bidder()),
			_now_price(packet.now_price()),
			_rest_mlisec(packet.rest_mlisec())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		AUCTION_ITEM_PROBE(const _Packet* packet)
			:
			_auction_id(packet->auction_id()),
			_probe(packet->probe()),
			_max_price(packet->max_price()),
			_now_bidder(packet->now_bidder()),
			_now_price(packet->now_price()),
			_rest_mlisec(packet->rest_mlisec())
		{
		}
	public:
		//getter - const
		const int64& auction_id() const;
		const SERVER_ITEM_PROBE& probe() const;
		const int64& max_price() const;
		const int64& now_bidder() const;
		const int64& now_price() const;
		const int32& rest_mlisec() const;

		//getter - mutable
		int64& mutable_auction_id();
		SERVER_ITEM_PROBE& mutable_probe();
		int64& mutable_max_price();
		int64& mutable_now_bidder();
		int64& mutable_now_price();
		int32& mutable_rest_mlisec();

		//setter
		void set_auction_id(const int64& auction_id);
		void set_probe(const SERVER_ITEM_PROBE& probe);
		void set_max_price(const int64& max_price);
		void set_now_bidder(const int64& now_bidder);
		void set_now_price(const int64& now_price);
		void set_rest_mlisec(const int32& rest_mlisec);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_auction_id(_auction_id);
			auto m_probe = packet.mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet.set_max_price(_max_price);
			packet.set_now_bidder(_now_bidder);
			packet.set_now_price(_now_price);
			packet.set_rest_mlisec(_rest_mlisec);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_auction_id(_auction_id);
			auto m_probe = packet->mutable_probe();
			_probe.ToPacket(OUT m_probe);
			packet->set_max_price(_max_price);
			packet->set_now_bidder(_now_bidder);
			packet->set_now_price(_now_price);
			packet->set_rest_mlisec(_rest_mlisec);
		}

		void TransferTo(OUT dto::AUCTION_ITEM_PROBE& other);

		std::tuple<int64&,SERVER_ITEM_PROBE&,int64&,int64&,int64&,int32&> FeedForward()
		{
			return std::tie(_auction_id,_probe,_max_price,_now_bidder,_now_price,_rest_mlisec);
		}

	private:
		int64 _auction_id;
		SERVER_ITEM_PROBE _probe;
		int64 _max_price;
		int64 _now_bidder;
		int64 _now_price;
		int32 _rest_mlisec;
	};
	
	class USER_DATA_SHARABLE
	{
	public:
		USER_DATA_SHARABLE() = default;
		//constructor - raw values
		USER_DATA_SHARABLE(
			const int32& level);
		//constructor copy from vo
		USER_DATA_SHARABLE(const vo::USER_DATA_SHARABLE& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		USER_DATA_SHARABLE(const _Packet& packet)
			:
			_level(packet.level())
		{
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		USER_DATA_SHARABLE(const _Packet* packet)
			:
			_level(packet->level())
		{
		}
	public:
		//getter - const
		const int32& level() const;

		//getter - mutable
		int32& mutable_level();

		//setter
		void set_level(const int32& level);

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_level(_level);
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_level(_level);
		}

		void TransferTo(OUT dto::USER_DATA_SHARABLE& other);

		std::tuple<int32&> FeedForward()
		{
			return std::tie(_level);
		}

	private:
		int32 _level;
	};
	
	class USER_DATA
	{
	public:
		USER_DATA() = default;
		//constructor - raw values
		USER_DATA(
			const int64& mileage,
			const std::vector<SERVER_ITEM_PROBE>& items);
		//constructor copy from vo
		USER_DATA(const vo::USER_DATA& vo);
		//constructor copy from packet ref
		template<typename _Packet>
		USER_DATA(const _Packet& packet)
			:
			_mileage(packet.mileage())
		{
			auto m_datas = packet.items();
			for(int i = 0; i < m_datas.size(); i++)
				_items.emplace_back(m_datas->Get(i));
		}
		//constructor copy from packet pointer
		template<typename _Packet>
		USER_DATA(const _Packet* packet)
			:
			_mileage(packet->mileage())
		{
			auto m_datas = packet->items();
			for(int i = 0; i < m_datas.size(); i++)
				_items.emplace_back(m_datas->Get(i));
		}
	public:
		//getter - const
		const int64& mileage() const;
		const std::vector<SERVER_ITEM_PROBE>& items() const;

		//getter - mutable
		int64& mutable_mileage();
		std::vector<SERVER_ITEM_PROBE>& mutable_items();

		//setter
		void set_mileage(const int64& mileage);
		void set_items(const std::vector<SERVER_ITEM_PROBE>& items);
		SERVER_ITEM_PROBE& add_items();

	public:
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet& packet) const
		{
			packet.set_mileage(_mileage);
			for(int i = 0; i < _items.size(); i++)
			{
				auto m_items = packet.add_items();
				_items[i].ToPacket(OUT m_items);
			}
		}
		//packets must have same interface with it
		template<typename _Packet>
		void ToPacket(OUT _Packet* packet) const
		{
			packet->set_mileage(_mileage);
			for(int i = 0; i < _items.size(); i++)
			{
				auto m_items = packet->add_items();
				_items[i].ToPacket(OUT m_items);
			}
		}

		void TransferTo(OUT dto::USER_DATA& other);

		std::tuple<int64&,std::vector<SERVER_ITEM_PROBE>&> FeedForward()
		{
			return std::tie(_mileage,_items);
		}

	private:
		int64 _mileage;
		std::vector<SERVER_ITEM_PROBE> _items;
	};
	
}