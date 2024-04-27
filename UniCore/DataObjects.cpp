#include "stdafx.h"
#include "DataObjects.h"

namespace vo
{

	//constructor - raw values
	SERVER_USER_PROBE::SERVER_USER_PROBE(
		const int64& uid,
		const std::wstring& nickname,
		const int64& client_sesion_id)
		:
		_uid(uid),
		_nickname(nickname),
		_client_sesion_id(client_sesion_id)
	{
	}

	//constructor copy from dto
	SERVER_USER_PROBE::SERVER_USER_PROBE(const dto::SERVER_USER_PROBE& dto)
		:
		_uid(dto.uid()),
		_nickname(dto.nickname()),
		_client_sesion_id(dto.client_sesion_id())
	{
	}

	//getter
	const int64& SERVER_USER_PROBE::uid() const { return _uid; }
	const std::wstring& SERVER_USER_PROBE::nickname() const { return _nickname; }
	const int64& SERVER_USER_PROBE::client_sesion_id() const { return _client_sesion_id; }

	//constructor - raw values
	GATE_USER_PROBE::GATE_USER_PROBE(
		const SERVER_USER_PROBE& user_probe,
		const int64& server_id)
		:
		_user_probe(user_probe),
		_server_id(server_id)
	{
	}

	//constructor copy from dto
	GATE_USER_PROBE::GATE_USER_PROBE(const dto::GATE_USER_PROBE& dto)
		:
		_user_probe(dto.user_probe()),
		_server_id(dto.server_id())
	{
	}

	//getter
	const SERVER_USER_PROBE& GATE_USER_PROBE::user_probe() const { return _user_probe; }
	const int64& GATE_USER_PROBE::server_id() const { return _server_id; }

	//constructor - raw values
	SERVER_CHATROOM_PROBE::SERVER_CHATROOM_PROBE(
		const int64& room_id,
		const std::wstring& room_name,
		const int32& max_user_count,
		const bool& is_private,
		const std::wstring& room_pw)
		:
		_room_id(room_id),
		_room_name(room_name),
		_max_user_count(max_user_count),
		_is_private(is_private),
		_room_pw(room_pw)
	{
	}

	//constructor copy from dto
	SERVER_CHATROOM_PROBE::SERVER_CHATROOM_PROBE(const dto::SERVER_CHATROOM_PROBE& dto)
		:
		_room_id(dto.room_id()),
		_room_name(dto.room_name()),
		_max_user_count(dto.max_user_count()),
		_is_private(dto.is_private()),
		_room_pw(dto.room_pw())
	{
	}

	//getter
	const int64& SERVER_CHATROOM_PROBE::room_id() const { return _room_id; }
	const std::wstring& SERVER_CHATROOM_PROBE::room_name() const { return _room_name; }
	const int32& SERVER_CHATROOM_PROBE::max_user_count() const { return _max_user_count; }
	const bool& SERVER_CHATROOM_PROBE::is_private() const { return _is_private; }
	const std::wstring& SERVER_CHATROOM_PROBE::room_pw() const { return _room_pw; }

	//constructor - raw values
	DB_CHATROOM_PROBE::DB_CHATROOM_PROBE(
		const SERVER_CHATROOM_PROBE& probe,
		const int32& now_user)
		:
		_probe(probe),
		_now_user(now_user)
	{
	}

	//constructor copy from dto
	DB_CHATROOM_PROBE::DB_CHATROOM_PROBE(const dto::DB_CHATROOM_PROBE& dto)
		:
		_probe(dto.probe()),
		_now_user(dto.now_user())
	{
	}

	//getter
	const SERVER_CHATROOM_PROBE& DB_CHATROOM_PROBE::probe() const { return _probe; }
	const int32& DB_CHATROOM_PROBE::now_user() const { return _now_user; }

	//constructor - raw values
	GATE_CHATROOM_PROBE::GATE_CHATROOM_PROBE(
		const SERVER_CHATROOM_PROBE& chatroom_probe,
		const int64& server_id,
		const std::string& server_ip,
		const int32& server_port)
		:
		_chatroom_probe(chatroom_probe),
		_server_id(server_id),
		_server_ip(server_ip),
		_server_port(server_port)
	{
	}

	//constructor copy from dto
	GATE_CHATROOM_PROBE::GATE_CHATROOM_PROBE(const dto::GATE_CHATROOM_PROBE& dto)
		:
		_chatroom_probe(dto.chatroom_probe()),
		_server_id(dto.server_id()),
		_server_ip(dto.server_ip()),
		_server_port(dto.server_port())
	{
	}

	//getter
	const SERVER_CHATROOM_PROBE& GATE_CHATROOM_PROBE::chatroom_probe() const { return _chatroom_probe; }
	const int64& GATE_CHATROOM_PROBE::server_id() const { return _server_id; }
	const std::string& GATE_CHATROOM_PROBE::server_ip() const { return _server_ip; }
	const int32& GATE_CHATROOM_PROBE::server_port() const { return _server_port; }

	//constructor - raw values
	GATE_CHAT_DATA::GATE_CHAT_DATA(
		const GATE_USER_PROBE& user_probe,
		const std::wstring& chat)
		:
		_user_probe(user_probe),
		_chat(chat)
	{
	}

	//constructor copy from dto
	GATE_CHAT_DATA::GATE_CHAT_DATA(const dto::GATE_CHAT_DATA& dto)
		:
		_user_probe(dto.user_probe()),
		_chat(dto.chat())
	{
	}

	//getter
	const GATE_USER_PROBE& GATE_CHAT_DATA::user_probe() const { return _user_probe; }
	const std::wstring& GATE_CHAT_DATA::chat() const { return _chat; }

	//constructor - raw values
	CLIENT_CHATROOM_PROBE::CLIENT_CHATROOM_PROBE(
		const int64& room_id,
		const std::wstring& room_name,
		const int32& max_user_count,
		const bool& is_private,
		const int32& user_count)
		:
		_room_id(room_id),
		_room_name(room_name),
		_max_user_count(max_user_count),
		_is_private(is_private),
		_user_count(user_count)
	{
	}

	//constructor copy from dto
	CLIENT_CHATROOM_PROBE::CLIENT_CHATROOM_PROBE(const dto::CLIENT_CHATROOM_PROBE& dto)
		:
		_room_id(dto.room_id()),
		_room_name(dto.room_name()),
		_max_user_count(dto.max_user_count()),
		_is_private(dto.is_private()),
		_user_count(dto.user_count())
	{
	}

	//getter
	const int64& CLIENT_CHATROOM_PROBE::room_id() const { return _room_id; }
	const std::wstring& CLIENT_CHATROOM_PROBE::room_name() const { return _room_name; }
	const int32& CLIENT_CHATROOM_PROBE::max_user_count() const { return _max_user_count; }
	const bool& CLIENT_CHATROOM_PROBE::is_private() const { return _is_private; }
	const int32& CLIENT_CHATROOM_PROBE::user_count() const { return _user_count; }

	//constructor - raw values
	Item::Item(
		const int32& item_code,
		const std::string& item_name)
		:
		_item_code(item_code),
		_item_name(item_name)
	{
	}

	//constructor copy from dto
	Item::Item(const dto::Item& dto)
		:
		_item_code(dto.item_code()),
		_item_name(dto.item_name())
	{
	}

	//getter
	const int32& Item::item_code() const { return _item_code; }
	const std::string& Item::item_name() const { return _item_name; }

	//constructor - raw values
	SERVER_ITEM_PROBE::SERVER_ITEM_PROBE(
		const int64& item_id,
		const int64& owner,
		const Item& item)
		:
		_item_id(item_id),
		_owner(owner),
		_item(item)
	{
	}

	//constructor copy from dto
	SERVER_ITEM_PROBE::SERVER_ITEM_PROBE(const dto::SERVER_ITEM_PROBE& dto)
		:
		_item_id(dto.item_id()),
		_owner(dto.owner()),
		_item(dto.item())
	{
	}

	//getter
	const int64& SERVER_ITEM_PROBE::item_id() const { return _item_id; }
	const int64& SERVER_ITEM_PROBE::owner() const { return _owner; }
	const Item& SERVER_ITEM_PROBE::item() const { return _item; }

	//constructor - raw values
	AUCTION_ITEM_PROBE::AUCTION_ITEM_PROBE(
		const int64& auction_id,
		const SERVER_ITEM_PROBE& probe,
		const int64& max_price,
		const int64& now_bidder,
		const int64& now_price,
		const int32& rest_mlisec)
		:
		_auction_id(auction_id),
		_probe(probe),
		_max_price(max_price),
		_now_bidder(now_bidder),
		_now_price(now_price),
		_rest_mlisec(rest_mlisec)
	{
	}

	//constructor copy from dto
	AUCTION_ITEM_PROBE::AUCTION_ITEM_PROBE(const dto::AUCTION_ITEM_PROBE& dto)
		:
		_auction_id(dto.auction_id()),
		_probe(dto.probe()),
		_max_price(dto.max_price()),
		_now_bidder(dto.now_bidder()),
		_now_price(dto.now_price()),
		_rest_mlisec(dto.rest_mlisec())
	{
	}

	//getter
	const int64& AUCTION_ITEM_PROBE::auction_id() const { return _auction_id; }
	const SERVER_ITEM_PROBE& AUCTION_ITEM_PROBE::probe() const { return _probe; }
	const int64& AUCTION_ITEM_PROBE::max_price() const { return _max_price; }
	const int64& AUCTION_ITEM_PROBE::now_bidder() const { return _now_bidder; }
	const int64& AUCTION_ITEM_PROBE::now_price() const { return _now_price; }
	const int32& AUCTION_ITEM_PROBE::rest_mlisec() const { return _rest_mlisec; }

	//constructor - raw values
	USER_DATA_SHARABLE::USER_DATA_SHARABLE(
		const int32& level)
		:
		_level(level)
	{
	}

	//constructor copy from dto
	USER_DATA_SHARABLE::USER_DATA_SHARABLE(const dto::USER_DATA_SHARABLE& dto)
		:
		_level(dto.level())
	{
	}

	//getter
	const int32& USER_DATA_SHARABLE::level() const { return _level; }

	//constructor - raw values
	USER_DATA::USER_DATA(
		const int64& mileage,
		const std::vector<SERVER_ITEM_PROBE>& items)
		:
		_mileage(mileage)
	{
		for(int i = 0 ; i < items.size(); i++)
			_items.push_back(std::move(items[i]));
	}

	//constructor copy from dto
	USER_DATA::USER_DATA(const dto::USER_DATA& dto)
		:
		_mileage(dto.mileage())
	{
		const std::vector<dto::SERVER_ITEM_PROBE>& mutable_items = dto.items();
		for(int i = 0 ; i < mutable_items.size(); i++)
			_items.push_back(SERVER_ITEM_PROBE(mutable_items[i]));
	}

	//getter
	const int64& USER_DATA::mileage() const { return _mileage; }
	const std::vector<SERVER_ITEM_PROBE>& USER_DATA::items() const { return _items; }

}

namespace dto
{


//constructor - raw values
	SERVER_USER_PROBE::SERVER_USER_PROBE(
		const int64& uid,
		const std::wstring& nickname,
		const int64& client_sesion_id)
		:
		_uid(uid),
		_nickname(nickname),
		_client_sesion_id(client_sesion_id)
	{
	}

	//constructor copy from vo
	SERVER_USER_PROBE::SERVER_USER_PROBE(const vo::SERVER_USER_PROBE& vo)
		:
		_uid(vo.uid()),
		_nickname(vo.nickname()),
		_client_sesion_id(vo.client_sesion_id())
	{
	}

	void SERVER_USER_PROBE::TransferTo(OUT dto::SERVER_USER_PROBE& other)
	{
		other.set_uid(std::move(_uid));
		other.set_nickname(std::move(_nickname));
		other.set_client_sesion_id(std::move(_client_sesion_id));
	}

	//getter - const
	const int64& SERVER_USER_PROBE::uid() const { return _uid; }
	const std::wstring& SERVER_USER_PROBE::nickname() const { return _nickname; }
	const int64& SERVER_USER_PROBE::client_sesion_id() const { return _client_sesion_id; }

	//getter - mutable
	int64& SERVER_USER_PROBE::mutable_uid() { return _uid; }
	std::wstring& SERVER_USER_PROBE::mutable_nickname() { return _nickname; }
	int64& SERVER_USER_PROBE::mutable_client_sesion_id() { return _client_sesion_id; }

	//setter
	void SERVER_USER_PROBE::set_uid(const int64& uid) { _uid = uid; }
	void SERVER_USER_PROBE::set_nickname(const std::wstring& nickname) { _nickname = nickname; }
	void SERVER_USER_PROBE::set_client_sesion_id(const int64& client_sesion_id) { _client_sesion_id = client_sesion_id; }


//constructor - raw values
	GATE_USER_PROBE::GATE_USER_PROBE(
		const SERVER_USER_PROBE& user_probe,
		const int64& server_id)
		:
		_user_probe(user_probe),
		_server_id(server_id)
	{
	}

	//constructor copy from vo
	GATE_USER_PROBE::GATE_USER_PROBE(const vo::GATE_USER_PROBE& vo)
		:
		_user_probe(vo.user_probe()),
		_server_id(vo.server_id())
	{
	}

	void GATE_USER_PROBE::TransferTo(OUT dto::GATE_USER_PROBE& other)
	{
		auto m_user_probe = other.mutable_user_probe();
		_user_probe.TransferTo(OUT m_user_probe);
		other.set_server_id(std::move(_server_id));
	}

	//getter - const
	const SERVER_USER_PROBE& GATE_USER_PROBE::user_probe() const { return _user_probe; }
	const int64& GATE_USER_PROBE::server_id() const { return _server_id; }

	//getter - mutable
	SERVER_USER_PROBE& GATE_USER_PROBE::mutable_user_probe() { return _user_probe; }
	int64& GATE_USER_PROBE::mutable_server_id() { return _server_id; }

	//setter
	void GATE_USER_PROBE::set_user_probe(const SERVER_USER_PROBE& user_probe) { _user_probe = user_probe; }
	void GATE_USER_PROBE::set_server_id(const int64& server_id) { _server_id = server_id; }


//constructor - raw values
	SERVER_CHATROOM_PROBE::SERVER_CHATROOM_PROBE(
		const int64& room_id,
		const std::wstring& room_name,
		const int32& max_user_count,
		const bool& is_private,
		const std::wstring& room_pw)
		:
		_room_id(room_id),
		_room_name(room_name),
		_max_user_count(max_user_count),
		_is_private(is_private),
		_room_pw(room_pw)
	{
	}

	//constructor copy from vo
	SERVER_CHATROOM_PROBE::SERVER_CHATROOM_PROBE(const vo::SERVER_CHATROOM_PROBE& vo)
		:
		_room_id(vo.room_id()),
		_room_name(vo.room_name()),
		_max_user_count(vo.max_user_count()),
		_is_private(vo.is_private()),
		_room_pw(vo.room_pw())
	{
	}

	void SERVER_CHATROOM_PROBE::TransferTo(OUT dto::SERVER_CHATROOM_PROBE& other)
	{
		other.set_room_id(std::move(_room_id));
		other.set_room_name(std::move(_room_name));
		other.set_max_user_count(std::move(_max_user_count));
		other.set_is_private(std::move(_is_private));
		other.set_room_pw(std::move(_room_pw));
	}

	//getter - const
	const int64& SERVER_CHATROOM_PROBE::room_id() const { return _room_id; }
	const std::wstring& SERVER_CHATROOM_PROBE::room_name() const { return _room_name; }
	const int32& SERVER_CHATROOM_PROBE::max_user_count() const { return _max_user_count; }
	const bool& SERVER_CHATROOM_PROBE::is_private() const { return _is_private; }
	const std::wstring& SERVER_CHATROOM_PROBE::room_pw() const { return _room_pw; }

	//getter - mutable
	int64& SERVER_CHATROOM_PROBE::mutable_room_id() { return _room_id; }
	std::wstring& SERVER_CHATROOM_PROBE::mutable_room_name() { return _room_name; }
	int32& SERVER_CHATROOM_PROBE::mutable_max_user_count() { return _max_user_count; }
	bool& SERVER_CHATROOM_PROBE::mutable_is_private() { return _is_private; }
	std::wstring& SERVER_CHATROOM_PROBE::mutable_room_pw() { return _room_pw; }

	//setter
	void SERVER_CHATROOM_PROBE::set_room_id(const int64& room_id) { _room_id = room_id; }
	void SERVER_CHATROOM_PROBE::set_room_name(const std::wstring& room_name) { _room_name = room_name; }
	void SERVER_CHATROOM_PROBE::set_max_user_count(const int32& max_user_count) { _max_user_count = max_user_count; }
	void SERVER_CHATROOM_PROBE::set_is_private(const bool& is_private) { _is_private = is_private; }
	void SERVER_CHATROOM_PROBE::set_room_pw(const std::wstring& room_pw) { _room_pw = room_pw; }


//constructor - raw values
	DB_CHATROOM_PROBE::DB_CHATROOM_PROBE(
		const SERVER_CHATROOM_PROBE& probe,
		const int32& now_user)
		:
		_probe(probe),
		_now_user(now_user)
	{
	}

	//constructor copy from vo
	DB_CHATROOM_PROBE::DB_CHATROOM_PROBE(const vo::DB_CHATROOM_PROBE& vo)
		:
		_probe(vo.probe()),
		_now_user(vo.now_user())
	{
	}

	void DB_CHATROOM_PROBE::TransferTo(OUT dto::DB_CHATROOM_PROBE& other)
	{
		auto m_probe = other.mutable_probe();
		_probe.TransferTo(OUT m_probe);
		other.set_now_user(std::move(_now_user));
	}

	//getter - const
	const SERVER_CHATROOM_PROBE& DB_CHATROOM_PROBE::probe() const { return _probe; }
	const int32& DB_CHATROOM_PROBE::now_user() const { return _now_user; }

	//getter - mutable
	SERVER_CHATROOM_PROBE& DB_CHATROOM_PROBE::mutable_probe() { return _probe; }
	int32& DB_CHATROOM_PROBE::mutable_now_user() { return _now_user; }

	//setter
	void DB_CHATROOM_PROBE::set_probe(const SERVER_CHATROOM_PROBE& probe) { _probe = probe; }
	void DB_CHATROOM_PROBE::set_now_user(const int32& now_user) { _now_user = now_user; }


//constructor - raw values
	GATE_CHATROOM_PROBE::GATE_CHATROOM_PROBE(
		const SERVER_CHATROOM_PROBE& chatroom_probe,
		const int64& server_id,
		const std::string& server_ip,
		const int32& server_port)
		:
		_chatroom_probe(chatroom_probe),
		_server_id(server_id),
		_server_ip(server_ip),
		_server_port(server_port)
	{
	}

	//constructor copy from vo
	GATE_CHATROOM_PROBE::GATE_CHATROOM_PROBE(const vo::GATE_CHATROOM_PROBE& vo)
		:
		_chatroom_probe(vo.chatroom_probe()),
		_server_id(vo.server_id()),
		_server_ip(vo.server_ip()),
		_server_port(vo.server_port())
	{
	}

	void GATE_CHATROOM_PROBE::TransferTo(OUT dto::GATE_CHATROOM_PROBE& other)
	{
		auto m_chatroom_probe = other.mutable_chatroom_probe();
		_chatroom_probe.TransferTo(OUT m_chatroom_probe);
		other.set_server_id(std::move(_server_id));
		other.set_server_ip(std::move(_server_ip));
		other.set_server_port(std::move(_server_port));
	}

	//getter - const
	const SERVER_CHATROOM_PROBE& GATE_CHATROOM_PROBE::chatroom_probe() const { return _chatroom_probe; }
	const int64& GATE_CHATROOM_PROBE::server_id() const { return _server_id; }
	const std::string& GATE_CHATROOM_PROBE::server_ip() const { return _server_ip; }
	const int32& GATE_CHATROOM_PROBE::server_port() const { return _server_port; }

	//getter - mutable
	SERVER_CHATROOM_PROBE& GATE_CHATROOM_PROBE::mutable_chatroom_probe() { return _chatroom_probe; }
	int64& GATE_CHATROOM_PROBE::mutable_server_id() { return _server_id; }
	std::string& GATE_CHATROOM_PROBE::mutable_server_ip() { return _server_ip; }
	int32& GATE_CHATROOM_PROBE::mutable_server_port() { return _server_port; }

	//setter
	void GATE_CHATROOM_PROBE::set_chatroom_probe(const SERVER_CHATROOM_PROBE& chatroom_probe) { _chatroom_probe = chatroom_probe; }
	void GATE_CHATROOM_PROBE::set_server_id(const int64& server_id) { _server_id = server_id; }
	void GATE_CHATROOM_PROBE::set_server_ip(const std::string& server_ip) { _server_ip = server_ip; }
	void GATE_CHATROOM_PROBE::set_server_port(const int32& server_port) { _server_port = server_port; }


//constructor - raw values
	GATE_CHAT_DATA::GATE_CHAT_DATA(
		const GATE_USER_PROBE& user_probe,
		const std::wstring& chat)
		:
		_user_probe(user_probe),
		_chat(chat)
	{
	}

	//constructor copy from vo
	GATE_CHAT_DATA::GATE_CHAT_DATA(const vo::GATE_CHAT_DATA& vo)
		:
		_user_probe(vo.user_probe()),
		_chat(vo.chat())
	{
	}

	void GATE_CHAT_DATA::TransferTo(OUT dto::GATE_CHAT_DATA& other)
	{
		auto m_user_probe = other.mutable_user_probe();
		_user_probe.TransferTo(OUT m_user_probe);
		other.set_chat(std::move(_chat));
	}

	//getter - const
	const GATE_USER_PROBE& GATE_CHAT_DATA::user_probe() const { return _user_probe; }
	const std::wstring& GATE_CHAT_DATA::chat() const { return _chat; }

	//getter - mutable
	GATE_USER_PROBE& GATE_CHAT_DATA::mutable_user_probe() { return _user_probe; }
	std::wstring& GATE_CHAT_DATA::mutable_chat() { return _chat; }

	//setter
	void GATE_CHAT_DATA::set_user_probe(const GATE_USER_PROBE& user_probe) { _user_probe = user_probe; }
	void GATE_CHAT_DATA::set_chat(const std::wstring& chat) { _chat = chat; }


//constructor - raw values
	CLIENT_CHATROOM_PROBE::CLIENT_CHATROOM_PROBE(
		const int64& room_id,
		const std::wstring& room_name,
		const int32& max_user_count,
		const bool& is_private,
		const int32& user_count)
		:
		_room_id(room_id),
		_room_name(room_name),
		_max_user_count(max_user_count),
		_is_private(is_private),
		_user_count(user_count)
	{
	}

	//constructor copy from vo
	CLIENT_CHATROOM_PROBE::CLIENT_CHATROOM_PROBE(const vo::CLIENT_CHATROOM_PROBE& vo)
		:
		_room_id(vo.room_id()),
		_room_name(vo.room_name()),
		_max_user_count(vo.max_user_count()),
		_is_private(vo.is_private()),
		_user_count(vo.user_count())
	{
	}

	void CLIENT_CHATROOM_PROBE::TransferTo(OUT dto::CLIENT_CHATROOM_PROBE& other)
	{
		other.set_room_id(std::move(_room_id));
		other.set_room_name(std::move(_room_name));
		other.set_max_user_count(std::move(_max_user_count));
		other.set_is_private(std::move(_is_private));
		other.set_user_count(std::move(_user_count));
	}

	//getter - const
	const int64& CLIENT_CHATROOM_PROBE::room_id() const { return _room_id; }
	const std::wstring& CLIENT_CHATROOM_PROBE::room_name() const { return _room_name; }
	const int32& CLIENT_CHATROOM_PROBE::max_user_count() const { return _max_user_count; }
	const bool& CLIENT_CHATROOM_PROBE::is_private() const { return _is_private; }
	const int32& CLIENT_CHATROOM_PROBE::user_count() const { return _user_count; }

	//getter - mutable
	int64& CLIENT_CHATROOM_PROBE::mutable_room_id() { return _room_id; }
	std::wstring& CLIENT_CHATROOM_PROBE::mutable_room_name() { return _room_name; }
	int32& CLIENT_CHATROOM_PROBE::mutable_max_user_count() { return _max_user_count; }
	bool& CLIENT_CHATROOM_PROBE::mutable_is_private() { return _is_private; }
	int32& CLIENT_CHATROOM_PROBE::mutable_user_count() { return _user_count; }

	//setter
	void CLIENT_CHATROOM_PROBE::set_room_id(const int64& room_id) { _room_id = room_id; }
	void CLIENT_CHATROOM_PROBE::set_room_name(const std::wstring& room_name) { _room_name = room_name; }
	void CLIENT_CHATROOM_PROBE::set_max_user_count(const int32& max_user_count) { _max_user_count = max_user_count; }
	void CLIENT_CHATROOM_PROBE::set_is_private(const bool& is_private) { _is_private = is_private; }
	void CLIENT_CHATROOM_PROBE::set_user_count(const int32& user_count) { _user_count = user_count; }


//constructor - raw values
	Item::Item(
		const int32& item_code,
		const std::string& item_name)
		:
		_item_code(item_code),
		_item_name(item_name)
	{
	}

	//constructor copy from vo
	Item::Item(const vo::Item& vo)
		:
		_item_code(vo.item_code()),
		_item_name(vo.item_name())
	{
	}

	void Item::TransferTo(OUT dto::Item& other)
	{
		other.set_item_code(std::move(_item_code));
		other.set_item_name(std::move(_item_name));
	}

	//getter - const
	const int32& Item::item_code() const { return _item_code; }
	const std::string& Item::item_name() const { return _item_name; }

	//getter - mutable
	int32& Item::mutable_item_code() { return _item_code; }
	std::string& Item::mutable_item_name() { return _item_name; }

	//setter
	void Item::set_item_code(const int32& item_code) { _item_code = item_code; }
	void Item::set_item_name(const std::string& item_name) { _item_name = item_name; }


//constructor - raw values
	SERVER_ITEM_PROBE::SERVER_ITEM_PROBE(
		const int64& item_id,
		const int64& owner,
		const Item& item)
		:
		_item_id(item_id),
		_owner(owner),
		_item(item)
	{
	}

	//constructor copy from vo
	SERVER_ITEM_PROBE::SERVER_ITEM_PROBE(const vo::SERVER_ITEM_PROBE& vo)
		:
		_item_id(vo.item_id()),
		_owner(vo.owner()),
		_item(vo.item())
	{
	}

	void SERVER_ITEM_PROBE::TransferTo(OUT dto::SERVER_ITEM_PROBE& other)
	{
		other.set_item_id(std::move(_item_id));
		other.set_owner(std::move(_owner));
		auto m_item = other.mutable_item();
		_item.TransferTo(OUT m_item);
	}

	//getter - const
	const int64& SERVER_ITEM_PROBE::item_id() const { return _item_id; }
	const int64& SERVER_ITEM_PROBE::owner() const { return _owner; }
	const Item& SERVER_ITEM_PROBE::item() const { return _item; }

	//getter - mutable
	int64& SERVER_ITEM_PROBE::mutable_item_id() { return _item_id; }
	int64& SERVER_ITEM_PROBE::mutable_owner() { return _owner; }
	Item& SERVER_ITEM_PROBE::mutable_item() { return _item; }

	//setter
	void SERVER_ITEM_PROBE::set_item_id(const int64& item_id) { _item_id = item_id; }
	void SERVER_ITEM_PROBE::set_owner(const int64& owner) { _owner = owner; }
	void SERVER_ITEM_PROBE::set_item(const Item& item) { _item = item; }


//constructor - raw values
	AUCTION_ITEM_PROBE::AUCTION_ITEM_PROBE(
		const int64& auction_id,
		const SERVER_ITEM_PROBE& probe,
		const int64& max_price,
		const int64& now_bidder,
		const int64& now_price,
		const int32& rest_mlisec)
		:
		_auction_id(auction_id),
		_probe(probe),
		_max_price(max_price),
		_now_bidder(now_bidder),
		_now_price(now_price),
		_rest_mlisec(rest_mlisec)
	{
	}

	//constructor copy from vo
	AUCTION_ITEM_PROBE::AUCTION_ITEM_PROBE(const vo::AUCTION_ITEM_PROBE& vo)
		:
		_auction_id(vo.auction_id()),
		_probe(vo.probe()),
		_max_price(vo.max_price()),
		_now_bidder(vo.now_bidder()),
		_now_price(vo.now_price()),
		_rest_mlisec(vo.rest_mlisec())
	{
	}

	void AUCTION_ITEM_PROBE::TransferTo(OUT dto::AUCTION_ITEM_PROBE& other)
	{
		other.set_auction_id(std::move(_auction_id));
		auto m_probe = other.mutable_probe();
		_probe.TransferTo(OUT m_probe);
		other.set_max_price(std::move(_max_price));
		other.set_now_bidder(std::move(_now_bidder));
		other.set_now_price(std::move(_now_price));
		other.set_rest_mlisec(std::move(_rest_mlisec));
	}

	//getter - const
	const int64& AUCTION_ITEM_PROBE::auction_id() const { return _auction_id; }
	const SERVER_ITEM_PROBE& AUCTION_ITEM_PROBE::probe() const { return _probe; }
	const int64& AUCTION_ITEM_PROBE::max_price() const { return _max_price; }
	const int64& AUCTION_ITEM_PROBE::now_bidder() const { return _now_bidder; }
	const int64& AUCTION_ITEM_PROBE::now_price() const { return _now_price; }
	const int32& AUCTION_ITEM_PROBE::rest_mlisec() const { return _rest_mlisec; }

	//getter - mutable
	int64& AUCTION_ITEM_PROBE::mutable_auction_id() { return _auction_id; }
	SERVER_ITEM_PROBE& AUCTION_ITEM_PROBE::mutable_probe() { return _probe; }
	int64& AUCTION_ITEM_PROBE::mutable_max_price() { return _max_price; }
	int64& AUCTION_ITEM_PROBE::mutable_now_bidder() { return _now_bidder; }
	int64& AUCTION_ITEM_PROBE::mutable_now_price() { return _now_price; }
	int32& AUCTION_ITEM_PROBE::mutable_rest_mlisec() { return _rest_mlisec; }

	//setter
	void AUCTION_ITEM_PROBE::set_auction_id(const int64& auction_id) { _auction_id = auction_id; }
	void AUCTION_ITEM_PROBE::set_probe(const SERVER_ITEM_PROBE& probe) { _probe = probe; }
	void AUCTION_ITEM_PROBE::set_max_price(const int64& max_price) { _max_price = max_price; }
	void AUCTION_ITEM_PROBE::set_now_bidder(const int64& now_bidder) { _now_bidder = now_bidder; }
	void AUCTION_ITEM_PROBE::set_now_price(const int64& now_price) { _now_price = now_price; }
	void AUCTION_ITEM_PROBE::set_rest_mlisec(const int32& rest_mlisec) { _rest_mlisec = rest_mlisec; }


//constructor - raw values
	USER_DATA_SHARABLE::USER_DATA_SHARABLE(
		const int32& level)
		:
		_level(level)
	{
	}

	//constructor copy from vo
	USER_DATA_SHARABLE::USER_DATA_SHARABLE(const vo::USER_DATA_SHARABLE& vo)
		:
		_level(vo.level())
	{
	}

	void USER_DATA_SHARABLE::TransferTo(OUT dto::USER_DATA_SHARABLE& other)
	{
		other.set_level(std::move(_level));
	}

	//getter - const
	const int32& USER_DATA_SHARABLE::level() const { return _level; }

	//getter - mutable
	int32& USER_DATA_SHARABLE::mutable_level() { return _level; }

	//setter
	void USER_DATA_SHARABLE::set_level(const int32& level) { _level = level; }


//constructor - raw values
	USER_DATA::USER_DATA(
		const int64& mileage,
		const std::vector<SERVER_ITEM_PROBE>& items)
		:
		_mileage(mileage)
	{
		for(int i = 0 ; i < items.size(); i++)
			_items.push_back(std::move(items[i]));
	}

	//constructor copy from vo
	USER_DATA::USER_DATA(const vo::USER_DATA& vo)
		:
		_mileage(vo.mileage())
	{
		const std::vector<vo::SERVER_ITEM_PROBE>& mutable_items = vo.items();
		for(int i = 0 ; i < mutable_items.size(); i++)
			_items.push_back(SERVER_ITEM_PROBE(mutable_items[i]));
	}

	void USER_DATA::TransferTo(OUT dto::USER_DATA& other)
	{
		other.set_mileage(std::move(_mileage));
		for(int i = 0; i < _items.size(); i++)
		{
			auto m_items = other.add_items();
			_items[i].ToPacket(OUT m_items);
		}
	}

	//getter - const
	const int64& USER_DATA::mileage() const { return _mileage; }
	const std::vector<SERVER_ITEM_PROBE>& USER_DATA::items() const { return _items; }

	//getter - mutable
	int64& USER_DATA::mutable_mileage() { return _mileage; }
	std::vector<SERVER_ITEM_PROBE>& USER_DATA::mutable_items() { return _items; }

	//setter
	void USER_DATA::set_mileage(const int64& mileage) { _mileage = mileage; }
	void USER_DATA::set_items(const std::vector<SERVER_ITEM_PROBE>& items) { _items = items; }
	SERVER_ITEM_PROBE& USER_DATA::add_items() 
	{
		SERVER_ITEM_PROBE add_one;
		_items.push_back(add_one);
		return add_one;
	}

}