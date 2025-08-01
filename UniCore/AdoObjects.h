#pragma once
#include "ADOUtils.h"
#include <ATLComTime.h>

class DBConnector;
class SQL_SENDER;
class ado_command;
class ado_datetime;

using sql_varchar = char*;
using sql_nvarchar = wchar_t*;
using sql_adodate = ado_datetime;

#define CQstr(query_string) static_cast<char*>(query_string)



class WaitingJob : objpool::NoPlacemnetNew
{
    void set(std::function<void()>&& task)
    {

    }
};

class ado_datetime
{
public:
    ado_datetime() = default;
    ado_datetime(DATE ado_date)
    {
        _bstr_t variant_date_string;
        if (FAILED(VarBstrFromDate(ado_date, 0, VAR_FOURDIGITYEARS, variant_date_string.GetAddress())))
        {
            DYNAMIC_ASSERT(false, "date convert failed :: %lf", static_cast<double>(ado_date));
            return;
        }

        if (FAILED(VarBstrFromDate(_cdate, 0, VAR_FOURDIGITYEARS, variant_date_string.GetAddress())))
        {
            DYNAMIC_ASSERT(false, "date convert failed :: %s", static_cast<char*>(variant_date_string));
            return;
        }
    }

    const auto& day() const noexcept { return _cdate.GetDay(); }
    const auto& month() const noexcept { return _cdate.GetMonth(); }
    const auto& year() const noexcept { return _cdate.GetYear(); }
    const auto& hour() const noexcept { return _cdate.GetHour(); }
    const auto& minute() const noexcept { return _cdate.GetMinute(); }
    const auto& second() const noexcept { return _cdate.GetSecond(); }

private:
    COleDateTime _cdate;
};

class ado_parameter
{
    friend ado_command;
    friend SQL_SENDER;
public:
    ado_parameter();

public:
    template <typename _Cast>
    const auto& value() { return get<_Cast>(); }

private:
    template <typename _Cast>
    _Cast get();

    template <>
    bool get<bool>()
    {
        return static_cast<bool>(_param->Value.boolVal);
    }
    template <>
    int8 get<int8>()
    {
        return static_cast<int8>(_param->Value.bVal);
    }
    template <>
    int16 get<int16>()
    {
        return static_cast<int16>(_param->Value.iVal);
    }
    template <>
    int32 get<int32>()
    {
        return static_cast<int32>(_param->Value.intVal);
    }
    template <>
    int64 get<int64>()
    {
        return static_cast<int64>(_param->Value.llVal);
    }
    template <>
    sql_varchar get<sql_varchar>()
    {
        return static_cast<sql_varchar>(bstr_t(_param->Value.bstrVal));
    }
    template <>
    sql_nvarchar get<sql_nvarchar>()
    {
        return static_cast<sql_nvarchar>(bstr_t(_param->Value.bstrVal));
    }
    template <>
    sql_adodate get<sql_adodate>()
    {
        return sql_adodate(_param->Value.date);
    }

private:
    _ParameterPtr _param;
};


template <typename _Ty>
concept _Shared_Ptr_Ado_Param = std::is_same_v<std::remove_reference_t<stdex::remove_all_const_t<_Ty>>, std::shared_ptr<ado_parameter>>;
class ado_result
{
    friend SQL_SENDER;
public:
    ado_result() : _ret(nullptr), _record(nullptr) {}
    ~ado_result()
    {
        try { _record->Close(); }
        catch (...) { /* _record는 있었지만 유효하지 않은 상태 정보가 없는 상태*/ }
    }

    auto set_record(_RecordsetPtr record) { _record = std::forward<_RecordsetPtr>(record); return this; }

    const auto ret() { try { return _ret->value<int32>(); } catch (...) { return -1; } }
    auto empty()
    { 
        try {
            return !_record || _record->EndOfFile;
        }
        catch (...)
        {
            return true;
        } 
    }
    const auto error_code() { return errorCode_; }
    bool next_record();
    bool move_first();
    bool move_next();


    template <typename _Cast>
    _Cast get(const int& row)
    {
        DYNAMIC_ASSERT(false, "invalid cast type %s", typeid(_Cast).name());

        if constexpr (std::is_class_v<_Cast>)
            return _Cast();

        return _Cast;
    }
    template <>
    bool get<bool>(const int32& row)
    {
        return static_cast<bool>(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.boolVal);
    }
    template <>
    int8 get<int8>(const int32& row)
    {
         return static_cast<int8>(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.bVal);
    }
    template <>
    int16 get<int16>(const int32& row)
    {
         return static_cast<int16>(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.iVal);
    }
    template <>
    int32 get<int32>(const int32& row)
    {
         return static_cast<int32>(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.intVal);
    }
    template <>
    int64 get<int64>(const int32& row)
    {
         return static_cast<int64>(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.llVal);
    }
    template <>
    sql_varchar get<sql_varchar>(const int32& row)
    {
         return static_cast<sql_varchar>(bstr_t(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.bstrVal));
    }
    template <>
    sql_nvarchar get<sql_nvarchar>(const int32& row)
    {
        return static_cast<sql_nvarchar>(bstr_t(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.bstrVal));
    }
    template <>
    sql_adodate get<sql_adodate>(const int32& row)
    {
        return sql_adodate(_record->Fields->Item[variant_t(static_cast<long>(row))]->Value.date);
    }

    template<typename _Cast>
    void get(_Cast& out, const int32& row)
    {
        out = get<_Cast>(row);
    }

    template <typename _Cast>
    _Cast get(const char* raw_id)
    {
        DYNAMIC_ASSERT(false, "invalid cast type %s", typeid(_Cast).name());

        if constexpr (std::is_class_v<_Cast>)
            return _Cast();

        return _Cast;
    }
    template <>
    bool get<bool>(const char* raw_id)
    {
        return static_cast<bool>(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.boolVal);
    }
    template <>
    int8 get<int8>(const char* raw_id)
    {
        return static_cast<int8>(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.bVal);
    }
    template <>
    int16 get<int16>(const char* raw_id)
    {
        return static_cast<int16>(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.iVal);
    }
    template <>
    int32 get<int32>(const char* raw_id)
    {
        return static_cast<int32>(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.intVal);
    }
    template <>
    int64 get<int64>(const char* raw_id)
    {
        return static_cast<int64>(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.llVal);
    }
    template <>
    sql_varchar get<sql_varchar>(const char* raw_id)
    {
        return static_cast<sql_varchar>(bstr_t(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.bstrVal));
    }
    template <>
    sql_nvarchar get<sql_nvarchar>(const char* raw_id)
    {
        return static_cast<sql_nvarchar>(bstr_t(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.bstrVal));
    }
    template <>
    sql_adodate get<sql_adodate>(const char* raw_id)
    {
        return sql_adodate(_record->Fields->Item[variant_t(bstr_t(raw_id))]->Value.date);
    }

    template<typename _Cast>
    void get(_Cast& out, const char* raw_id)
    {
        out = get<_Cast>(raw_id);
    }

    
    template <typename _Ado_Param_Shared>
        requires _Shared_Ptr_Ado_Param<_Ado_Param_Shared>
    void bind_ret(_Ado_Param_Shared&& input_param)
    {
        _ret = std::forward<_Ado_Param_Shared>(input_param);
    }

    template <typename _Ado_Param_Shared>
        requires _Shared_Ptr_Ado_Param<_Ado_Param_Shared>
    void bind_input(_Ado_Param_Shared&& input_param)
    {
        _inputs.push_back(std::forward<_Ado_Param_Shared>(input_param));
    }

    template <typename _Ado_Param_Shared>
        requires _Shared_Ptr_Ado_Param<_Ado_Param_Shared>
    void bind_output(_Ado_Param_Shared&& output_param)
    {
        _outputs.push_back(std::forward<_Ado_Param_Shared>(output_param));
    }

    auto get_output(const std::size_t& idx)
        -> std::shared_ptr<ado_parameter>
    {
        if (idx > _outputs.size() - 1)
            return nullptr;

        return _outputs[idx];
    }
    

private:
    _RecordsetPtr _record;
    std::shared_ptr<ado_parameter> _ret;
    stdex::vector<std::shared_ptr<ado_parameter>> _inputs;
    stdex::vector<std::shared_ptr<ado_parameter>> _outputs;

    int32 errorCode_ = 0;
};

class ado_conn_object : public std::enable_shared_from_this<ado_conn_object>
{
    friend DBConnector;
public:
    ado_conn_object() = default;
    ado_conn_object(std::shared_ptr<DBConnector> owner, const _bstr_t& conn_string);
    ~ado_conn_object();

    const auto state() { return _conn->State; }
    void connect(const _bstr_t& conn_string);

    const auto& conn() noexcept { return _conn; }

    void pool_back();

private:
    void _close() noexcept;

private:
    _ConnectionPtr _conn;
    std::weak_ptr<DBConnector> _owner;
};


template<typename _Ty>
concept _Shared_Ptr_Ado_Conn = std::is_same_v<std::remove_reference_t<stdex::remove_all_const_t<_Ty>>, std::shared_ptr<ado_conn_object>>;

class SQL_SENDER : public std::enable_shared_from_this<SQL_SENDER>
{
#define SQL_CALLBACK_PARAMS const std::shared_ptr<ado_result>& result
#define SQL_CB(...) [__VA_ARGS__](SQL_CALLBACK_PARAMS)
public:
    using sql_callback = std::function<void(SQL_CALLBACK_PARAMS)>;

private:
    friend DBConnector;

    _CommandPtr _comm;
    sql_callback _callback = nullptr;

    std::string _query;

    std::shared_ptr<ado_conn_object> _active_conn;
    std::shared_ptr<ado_result> _result;

    bool _break;

public:
    SQL_SENDER(_bstr_t query_string, CommandTypeEnum comm_type = CommandTypeEnum::adCmdStoredProc);
    ~SQL_SENDER();

private:
    template <typename _Ty>
    auto _set_parameter(_Ty&& value, const ParameterDirectionEnum& direction, const DataTypeEnum& type, const int& size)
    {
        DYNAMIC_ASSERT(_comm && _comm->CommandType == adCmdStoredProc, "storedproc only can insert new param");

        auto param = ObjectPool<ado_parameter>::PoolShared();
        param->_param->Type = type;
        param->_param->Size = size;
        param->_param->Direction = direction;
        param->_param->Value = _variant_t(std::forward<_Ty>(value));

        _comm->Parameters->Append(param->_param);

        //ref ++
        _result->bind_input(std::move(param));

        return this;
    }

    auto _set_parameter(const ParameterDirectionEnum& direction, const DataTypeEnum& type, const int& size = 0)
    {
        DYNAMIC_ASSERT(_comm && _comm->CommandType == adCmdStoredProc, "storedproc only can insert new param");

        auto param = ObjectPool<ado_parameter>::PoolShared();
        param->_param->Type = type;
        param->_param->Direction = direction;

        if (size > 0)
            param->_param->Size = size;

        _comm->Parameters->Append(param->_param);

        //ref ++
        _result->bind_output(std::move(param));

        return this;
    }

public:
    template<typename _Ty>
    decltype(auto) set_input(_Ty&& value)
    {
        if constexpr (std::is_same_v<stdex::pure_type_t<_Ty>, bool> )
            _set_parameter(std::forward<_Ty>(value), ParameterDirectionEnum::adParamInput, DataTypeEnum::adBoolean, sizeof(_Ty));
        else if constexpr (std::is_same_v<stdex::pure_type_t<_Ty>, int8> )
            _set_parameter(std::forward<_Ty>(value), ParameterDirectionEnum::adParamInput, DataTypeEnum::adTinyInt, sizeof(_Ty));
        else if constexpr (std::is_same_v<stdex::pure_type_t<_Ty>, int16> )
            _set_parameter(std::forward<_Ty>(value), ParameterDirectionEnum::adParamInput, DataTypeEnum::adSmallInt, sizeof(_Ty));
        else if constexpr (std::is_same_v<stdex::pure_type_t<_Ty>, int32> )
            _set_parameter(std::forward<_Ty>(value), ParameterDirectionEnum::adParamInput, DataTypeEnum::adInteger, sizeof(_Ty));
        else if constexpr (std::is_same_v<stdex::pure_type_t<_Ty>, int64> )
            _set_parameter(std::forward<_Ty>(value), ParameterDirectionEnum::adParamInput, DataTypeEnum::adBigInt, sizeof(_Ty));
        else if constexpr (stdex::is_convertible_ctype_char_v<_Ty>)
            _set_parameter(stdex::ctype_traits<_Ty>::ctype(std::forward<_Ty>(value)), ParameterDirectionEnum::adParamInput, DataTypeEnum::adVarChar, -1);
        else if constexpr (stdex::is_convertible_ctype_wchar_v<_Ty>)
            _set_parameter(stdex::ctype_traits<_Ty>::ctype(std::forward<_Ty>(value)), ParameterDirectionEnum::adParamInput, DataTypeEnum::adVarWChar, -1);
        else
        {
            static_assert(
                std::is_same_v<stdex::pure_type_t<_Ty>, bool> ||
                std::is_same_v<stdex::pure_type_t<_Ty>, int8> ||
                std::is_same_v<stdex::pure_type_t<_Ty>, int16> ||
                std::is_same_v<stdex::pure_type_t<_Ty>, int32> ||
                std::is_same_v<stdex::pure_type_t<_Ty>, int64> ||
                stdex::is_convertible_ctype_char_v<_Ty> ||
                stdex::is_convertible_ctype_wchar_v<_Ty>
                , "invalid input paramter");
        }

        return this;
    }

    template <typename _OutType>
    auto set_output()
    {
        DYNAMIC_ASSERT(false, "invalid ado output type");
        return this;
    }
    template<>
    auto set_output<bool>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adBoolean, sizeof(bool));
        return this;
    }
    template<>
    auto set_output<int8>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adTinyInt, sizeof(int8));
        return this;
    }
    template<>
    auto set_output<int16>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adSmallInt, sizeof(int16));
        return this;
    }
    template<>
    auto set_output<int32>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adInteger, sizeof(int32));
        return this;
    }
    template<>
    auto set_output<int64>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adBigInt, sizeof(int64));
        return this;
    }
    template<>
    auto set_output<sql_varchar>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adVarChar, -1);
        return this;
    }
    template<>
    auto set_output<sql_nvarchar>()
    {
        _set_parameter(ParameterDirectionEnum::adParamOutput, DataTypeEnum::adVarWChar, -1);
        return this;
    }

    auto set_callback(sql_callback&& callback)
    {
        _callback = std::forward<sql_callback>(callback);
        return this;
    }

    auto set_query_string(const _bstr_t& query_string) 
    { 
        _comm->CommandText = query_string; 
        return this; 
    }

    // set ado connection
    template<typename _Shared_Ptr>
        requires _Shared_Ptr_Ado_Conn<_Shared_Ptr>
    auto set_conn(_Shared_Ptr&& conn)
    {
        _active_conn = std::forward<_Shared_Ptr>(conn);
        _comm->ActiveConnection = conn->conn();
        return this;
    }

public:
    inline const auto& query_string() const noexcept { return _query; }
    inline const auto& querying_state() const noexcept { return _comm->State; }
    inline const auto& result() const noexcept { return _result; }

    inline bool isDone() const { 
        if (_break)
            return true;

        try 
        {
            return _comm->State != adStateExecuting;
        }
        catch (...)
        {
            return false;
        }
    }
    void execute(const ConnectOptionEnum& exec_type);
    void on_callback() noexcept;

//for friends
private:
    inline void _clear();
};