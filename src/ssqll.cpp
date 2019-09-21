// SqliteCpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string.h>
#include "ssqll/ssqll.h"

using namespace ltc;


class invalid_operation : std::logic_error
{
public:
    invalid_operation()
        : std::logic_error("Operaion not allowed in this state.")
    {}
};

// Sqlite_err

Sqlite_err::Sqlite_err(int result_code)
    : m_result_code{ result_code }
    , std::runtime_error(error_code_str())
{
}

Sqlite_err::Sqlite_err(int result_code, std::string what)
		: m_result_code{ result_code }
		, std::runtime_error(what)
{
}

int Sqlite_err::error_code() const noexcept
{
	return m_result_code;
}

const char* Sqlite_err::error_code_str() const noexcept
{
	return sqlite3_errstr(m_result_code);
}


// Sqlite_stmt

Sqlite_stmt::Sqlite_stmt(sqlite3_stmt* stmt) 
	: m_stmt{ stmt, sqlite3_finalize }
{}

sqlite3_stmt* Sqlite_stmt::handle() const
{
	return m_stmt.get();
}

void Sqlite_stmt::bind(int c, short i)                  { sqlite3_bind_int(handle(), c, i); }
void Sqlite_stmt::bind(int c, int i)                    { sqlite3_bind_int(handle(), c, i); }
void Sqlite_stmt::bind(int c, long v)                   { sqlite3_bind_int64(handle(), c, v); }
void Sqlite_stmt::bind(int c, long long v)              { sqlite3_bind_int64(handle(), c, v); }
void Sqlite_stmt::bind(int c, float v)                  { sqlite3_bind_double(handle(), c, v); }
void Sqlite_stmt::bind(int c, double v)	                { sqlite3_bind_double(handle(), c, v); }
void Sqlite_stmt::bind(int c, const std::string& v)	    { sqlite3_bind_text(handle(), c, v.c_str(), v.length(), nullptr); }
void Sqlite_stmt::bind(int c, const char* v)            { sqlite3_bind_text(handle(), c, v, strlen(v), nullptr); }
void Sqlite_stmt::bind(int c, SqlNull n)		        { sqlite3_bind_null(handle(), c); }
void Sqlite_stmt::bind(int c, const Blob& v)	        { sqlite3_bind_blob(handle(), c, v.data(), v.size(), nullptr); }

void Sqlite_stmt::exec()
{
    const int result_code = sqlite3_step(handle());
    if (result_code != SQLITE_DONE)
        throw Sqlite_err(result_code);
}

void Sqlite_stmt::query(Row_callback cb)
{
    int result_code;
    Sqlite_stmt::row row(m_stmt);
    while ((result_code = sqlite3_step(handle()))==SQLITE_ROW)
    {
        if (!cb(row))
            break;
    }   
}


// Sqlite_stmt::row
Sqlite_stmt::row::row(sqlite3_stmt_ptr stmt) 
	: m_stmt(std::move(stmt)) 
{}

sqlite3_stmt* ltc::Sqlite_stmt::row::handle() const
{
    return m_stmt.get();
}

int Sqlite_stmt::row::cols() const
{
    return sqlite3_column_count(handle());
}

std::string Sqlite_stmt::row::name(int col) const
{
    return std::string{ sqlite3_column_name(handle(), col) };
}

#ifdef SQLITE_ENABLE_COLUMN_METADATA
std::string Sqlite_stmt::row::origin(int col) const
{
    return std::string{ sqlite3_column_origin_name(handle(), col) };
}
#endif

#ifdef SQLITE_ENABLE_COLUMN_METADATA
std::string Sqlite_stmt::row::dbname(int col) const
{
    return std::string{ sqlite3_column_database_name(handle(), col) };
}
#endif

#ifdef SQLITE_ENABLE_COLUMN_METADATA
std::string Sqlite_stmt::row::table(int col) const
{
    return std::string{ sqlite3_column_table_name(handle(), col) };
}
#endif

int Sqlite_stmt::row::as_int(int col) const
{
	return sqlite3_column_int(handle(), col);
}

long long Sqlite_stmt::row::as_int64(int col) const
{
	return sqlite3_column_int64(handle(), col);
}

double Sqlite_stmt::row::as_double(int col) const
{
	return sqlite3_column_double(handle(), col);
}

std::string Sqlite_stmt::row::as_string(int col) const
{
	const auto len = sqlite3_column_bytes(handle(), col);
	const auto buf = sqlite3_column_text(handle(), col);
	return (buf && len) ? std::string(reinterpret_cast<const char*>(buf), len) : std::string{};
}

Blob Sqlite_stmt::row::as_blob(int col) const
{
	const auto len = sqlite3_column_bytes(handle(), col);
	const auto buf = sqlite3_column_text(handle(), col);
	return (buf && len) ? Blob(buf, buf + len) : Blob{};
}

bool Sqlite_stmt::row::is_null(int col) const
{
    return sqlite3_column_type(handle(), col) == SQLITE_NULL;
}

Datatype Sqlite_stmt::row::type(int col) const
{
    return static_cast<Datatype>(sqlite3_column_type(handle(), col));
}


// Sqlite_db

int exec_cb(void* data, int columns, char** values, char** names)
{
    std::function<bool(int, char**, char**)> cb = *static_cast<std::function<bool(int, char**, char**)>*>(data);
    return cb(columns, values, names);
}

void check_error(int result_code, char*& msg_ptr, int expected_value = SQLITE_OK)
{
    if (result_code != expected_value && msg_ptr != nullptr)
    {
        std::string err_msg{ msg_ptr };
        sqlite3_free(msg_ptr);
        msg_ptr = nullptr;
        throw Sqlite_err(result_code, err_msg);
    }
}

Sqlite_db::Sqlite_db(const char* filename)
{
    open(filename);    
}

void Sqlite_db::open(const char* filename)
{
    if (m_db)
        throw std::logic_error("Close database before calling open again.");

    sqlite3* db;
    check_error(sqlite3_open(filename, &db));
    m_db.reset(db, sqlite3_close);
}

bool Sqlite_db::is_open() const
{
    return m_db != nullptr;
}

void Sqlite_db::close()
{
    m_db.reset();
}

void Sqlite_db::exec(const char* sql, Callback callback)
{
    char* errmsg;
    ::check_error(sqlite3_exec(handle(), sql, exec_cb, &callback, &errmsg), errmsg, SQLITE_OK);
}

void Sqlite_db::exec(const char* sql)
{
    char* errmsg;
    ::check_error(sqlite3_exec(handle(), sql, exec_cb, nullptr, &errmsg), errmsg, SQLITE_OK);
}

Sqlite_stmt Sqlite_db::prepare(const std::string sql)
{
    sqlite3_stmt* stmt;
    check_error(sqlite3_prepare_v2(handle(), sql.c_str(), sql.size() + 1, &stmt, nullptr));
    return Sqlite_stmt{ stmt };
}

sqlite3* Sqlite_db::handle() const
{
    return m_db.get();
}

void Sqlite_db::check_error(int result_code, int expected_value) const
{
    if (result_code != expected_value)
        throw Sqlite_err(result_code, sqlite3_errmsg(handle()));
}

int Sqlite_db::changes() const
{
    return sqlite3_changes(handle());
}

int Sqlite_db::total_changes() const
{
    return sqlite3_total_changes(handle());
}
