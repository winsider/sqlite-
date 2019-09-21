#pragma once

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <sqlite3.h>

namespace ltc
{
	/// Database NULL value
	constexpr struct SqlNull {} Null;

	/// Datatype for blobs
	using Blob = std::vector<unsigned char>;

    /// Enum mapping to SQLITE datatype constants
    enum Datatype
    {
        db_integer = 1, // SQLITE_INTEGER
        db_float   = 2, // SQLITE_FLOAT
        db_text    = 3, // SQLITE_TEXT
        db_blob    = 4, // SQLITE_BLOB
        db_null    = 5  // SQLITE_NULL
    };

	/**
	 * Exception class for SQLITE error return codes 
	 */
	class Sqlite_err final : public std::runtime_error
	{
	public:
        Sqlite_err(int result_code);
		Sqlite_err(int result_code, std::string what);
		int error_code() const noexcept;
		const char* error_code_str() const noexcept;

	private:
		int m_result_code;
	};


    /**
     * SQLite statemt wrapper
     */
	class Sqlite_stmt final
	{
    private:
        using sqlite3_stmt_ptr = std::shared_ptr<sqlite3_stmt>;

	public:
        class row;
		using Row_callback = std::function<bool(const row&)>;

		Sqlite_stmt() = default;
		Sqlite_stmt(const Sqlite_stmt&) = default;
		Sqlite_stmt(Sqlite_stmt&&) = default;
		~Sqlite_stmt() = default;
		Sqlite_stmt& operator=(const Sqlite_stmt&) = default;
		Sqlite_stmt& operator=(Sqlite_stmt&&) = default;

        /**
         * Result row wrapper
         */
		class row final
		{
			friend class Sqlite_stmt;

		public:
			row() = delete;
			row(const row&) = delete;
            ~row() = default;
			row(row&&) = delete;

			int         as_int(int col) const;
			long long   as_int64(int col) const;
			double      as_double(int col) const;
			std::string as_string(int col) const;
			Blob        as_blob(int col) const;

            bool        is_null(int col) const;

            int cols() const;
            std::string name(int col) const;

            Datatype type(int col) const;

#ifdef SQLITE_ENABLE_COLUMN_METADATA
            std::string origin(int col) const;
            std::string dbname(int col) const;
            std::string table(int col) const;
#endif

		private:
			row(sqlite3_stmt_ptr stmt);
            sqlite3_stmt* handle() const;
            sqlite3_stmt_ptr m_stmt;
		};

        void exec();
        
        template<typename T>
        void exec(T t)
        {
            bind(1, t);
            exec();
        }

        template<typename T, typename... Args>
        void exec(T t, Args... args)
        {
            bind(1, t);
            bind(2, args...);
            exec();
        }

        void query(Row_callback cb);

		template<typename T>
		void query(Row_callback cb, T t)
		{
			bind(1, t);
			query(cb);
		}

		template <typename T, typename... Args>
		void query(Row_callback cb, T t, Args... args)
		{
			bind(1, t);
			bind(2, args...);
			query(cb);
		}

	private:
		friend class Sqlite_db;        
		Sqlite_stmt(sqlite3_stmt* stmt);

		template <typename T, typename... Args>
		void bind(int c, T t, Args... args)
		{
			bind(c, t);
			bind(++c, args...);
		}

		void bind(int c, short i);
		void bind(int c, int i);
		void bind(int c, long v);
		void bind(int c, long long v);
		void bind(int c, float v);
		void bind(int c, double v);
		void bind(int c, const std::string& v);
        void bind(int c, const char* v);
		void bind(int c, SqlNull n);
		void bind(int c, const Blob& v);

        sqlite3_stmt* handle() const;
		sqlite3_stmt_ptr m_stmt;
	};


    /**
     * SQLite database wrapper
     */
	class Sqlite_db final
	{
	public:
		using Callback = std::function<int(int, char**, char**)>;

		Sqlite_db() = default;
		Sqlite_db(const Sqlite_db&) = default;
		Sqlite_db(Sqlite_db&&) = default;
        Sqlite_db(const char* filename);
		~Sqlite_db() = default;
		Sqlite_db& operator=(const Sqlite_db&) = default;
		Sqlite_db& operator=(Sqlite_db&&) = default;

        void open(const char* filename);
        void close();
        void exec(const char* sql, Callback callback);
        void exec(const char* sql);
        Sqlite_stmt prepare(const std::string sql);
        bool is_open() const;
        int changes() const;
        int total_changes() const;

	private:
        sqlite3* handle() const;
        void check_error(int result_code, int expected_value = SQLITE_OK) const;
		std::shared_ptr<sqlite3> m_db;
	};
}
