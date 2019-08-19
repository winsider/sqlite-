#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <sqlite3.h>

namespace ltc
{
	class Sqlite_error;
	class Sqlite_db;
	class Sqlite_stmt;

	struct SqlNull {};
	constexpr SqlNull Null{};

	class Sqlite_error final : public std::exception
	{
	public:
		Sqlite_error(int result_code)
			: m_result_code{ result_code }
			, m_what{ sqlite3_errstr(result_code) }
		{}

		Sqlite_error(int result_code, std::string what)
			: m_result_code{ result_code }
			, m_what{ std::move(what) }
		{}

		const char* what() const noexcept override
		{
			return m_what.c_str();
		}

		int error_code() const noexcept
		{
			return m_result_code;
		}

		const char* error_code_str() const noexcept
		{
			return sqlite3_errstr(m_result_code);
		}

	private:
		int m_result_code;
		std::string m_what;
	};


	class Sqlite_stmt final
	{
		using sqlite3_stmt_ptr = std::shared_ptr<sqlite3_stmt>;

	public:
		sqlite3_stmt * handle() const
		{
			return m_stmt.get();
		}

		class row final
		{
		public:
			int column_int(int col) const
			{
				return sqlite3_column_int(m_stmt.get(), col);
			}

		private:
			friend Sqlite_stmt;
			row(sqlite3_stmt_ptr stmt) : m_stmt(std::move(stmt)) {}
			sqlite3_stmt_ptr m_stmt;
		};

		class row_iterator final
		{
		public:
			bool operator==(const row_iterator& iter) const
			{
				return (iter.m_stmt == m_stmt && iter.m_result_code == m_result_code);
			}

			bool operator!=(const row_iterator& iter) const
			{
				return !(*this == iter);
			}

			row_iterator& operator++()
			{
				step();
				return *this;
			}

			row* operator->()
			{
				if (m_result_code == SQLITE_ROW)
					return &m_row;
				else
					throw std::logic_error("Dereference result iterator not allowed in this state.");
			}
			row& operator*()
			{
				if (m_result_code == SQLITE_ROW)
					return m_row;
				else
					throw std::logic_error("Dereference result iterator not allowed in this state.");
			}

		private:
			friend Sqlite_stmt;

			row_iterator() : m_stmt{ nullptr }, m_row{ nullptr }
			{
				m_result_code = SQLITE_DONE;
			}

			row_iterator(sqlite3_stmt_ptr stmt) : m_stmt{ std::move(stmt) }, m_row{ stmt }
			{
				step();
			}

			void step()
			{
				if (!m_stmt)
					throw std::logic_error("Calling result_iterator after it's done.");
				
				m_result_code = sqlite3_step(m_stmt.get());
				
				if (m_result_code == SQLITE_DONE)
					m_stmt.reset();
				else if (m_result_code != SQLITE_ROW)
					throw Sqlite_error(m_result_code, sqlite3_errstr(m_result_code));
			}

			row m_row;
			std::shared_ptr<sqlite3_stmt> m_stmt;
			int m_result_code;
		};

		const row_iterator end() const
		{
			static row_iterator end{};
			return end;
		}

		Sqlite_stmt(sqlite3_stmt* stmt) : m_stmt{ stmt, sqlite3_finalize }
		{
		}


		row_iterator exec()
		{
			return row_iterator(m_stmt);
		}

		template<typename T>
		row_iterator exec(T t)
		{
			bind(1, t);
			return exec();
		}

		template <typename T, typename... Args>
		row_iterator exec(T t, Args... args)
		{
			bind(1, t);
			bind(2, args...);
			return exec();
		}

	private:
		template <typename T, typename... Args>
		void bind(int c, T t, Args... args)
		{
			bind(c, t);
			bind(++c, args...);
		}

		void bind(int c, short i)
		{
			sqlite3_bind_int(m_stmt.get(), c, i);
		}

		void bind(int c, int i)
		{
			sqlite3_bind_int(m_stmt.get(), c, i);
		}

		void bind(int c, long v)
		{
			sqlite3_bind_int64(m_stmt.get(), c, v);
		}

		void bind(int c, long long v)
		{
			sqlite3_bind_int64(m_stmt.get(), c, v);
		}

		void bind(int c, float v)
		{
			sqlite3_bind_double(m_stmt.get(), c, v);
		}

		void bind(int c, double v)
		{
			sqlite3_bind_double(m_stmt.get(), c, v);
		}

		void bind(int c, const std::string& v)
		{
			sqlite3_bind_text(m_stmt.get(), c, v.c_str(), v.length(), nullptr);
		}

		void bind(int c, SqlNull n)
		{
			sqlite3_bind_null(m_stmt.get(), c);
		}

		void bind(int c, const std::vector<unsigned char>& v)
		{
			sqlite3_bind_blob(m_stmt.get(), c, v.data(), v.size(), nullptr);
		}

		sqlite3_stmt_ptr m_stmt;
	};

	class Sqlite_db final
	{
	public:
		using Callback = std::function<int(int, char**, char**)>;

		Sqlite_db(const char* filename)
		{
			open(filename);
		}

		void open(const char* filename)
		{
			sqlite3* db;
			check_error(sqlite3_open(filename, &db));
			m_db.reset(db, close_db);
		}

		void close()
		{
			m_db.reset();
		}

		void exec(const char* sql, Callback callback)
		{
			char* errmsg;
			check_error(sqlite3_exec(m_db.get(), sql, exec_cb, &callback, &errmsg), errmsg);
		}

		void exec(const char* sql)
		{
			char* errmsg;
			check_error(sqlite3_exec(m_db.get(), sql, exec_cb, nullptr, &errmsg), errmsg);
		}

		Sqlite_stmt prepare(const std::string sql)
		{
			sqlite3_stmt* stmt;
			check_error(sqlite3_prepare_v2(m_db.get(), sql.c_str(), sql.size() + 1, &stmt, nullptr));
			return Sqlite_stmt{ stmt };
		}

		sqlite3* handle() const
		{
			return m_db.get();
		}

	private:

		static void check_error(int result_code, char*& msg_ptr, int expected_value = SQLITE_OK)
		{
			if (result_code != expected_value && msg_ptr != nullptr)
			{
				std::string err_msg{ msg_ptr };
				sqlite3_free(msg_ptr);
				msg_ptr = nullptr;
				throw Sqlite_error(result_code, err_msg);
			}
		}

		void check_error(int result_code, int expected_value = SQLITE_OK) const
		{
			if (result_code != expected_value)
				throw Sqlite_error(result_code, sqlite3_errmsg(m_db.get()));
		}

		static int exec_cb(void* data, int columns, char** values, char** names)
		{
			std::function<bool(int, char**, char**)> cb = *static_cast<std::function<bool(int, char**, char**)>*>(data);
			return cb(columns, values, names);
		}

		static void close_db(sqlite3* db)
		{
			const auto result_code = sqlite3_close(db);
			if (result_code != SQLITE_OK)
				throw Sqlite_error(result_code);
		}

		std::shared_ptr<sqlite3> m_db;
	};


}
