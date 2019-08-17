// SqliteCpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ssqll.h"

class Statement
{
public:

	void bind(int c, short i)
	{
		std::cout << "bind [" << c << "] " << typeid(i).name() << ": " << i << '\n';
	}

	void bind(int c, int i)
	{
		std::cout << "bind [" << c << "] " << typeid(i).name() << ": " << i << '\n';
	}

	void bind(int c, long i)
	{
		std::cout << "bind [" << c << "] " << typeid(i).name() << ": " << i << '\n';
	}

	void bind(int c, long long i)
	{
		std::cout << "bind [" << c << "] " << typeid(i).name() << ": " << i << '\n';
	}

	void bind(int c, double i)
	{
		std::cout << "bind [" << c << "] " << typeid(i).name() << ": " << i << '\n';
	}

	void bind(int c, const char* s)
	{
		std::cout << "bind [" << c << "] " << typeid(s).name() << ": " << s << '\n';
	}

	void exec()
	{
		std::cout << "Execute\n";
	}

	template <typename T, typename... Args>
	void exec(T t, Args... args)
	{
		bind(0, t);
		bind(1, args...);
		std::cout << "Execute\n";
	}

private:

	template <typename T, typename... Args>
	void bind(int c, T t, Args... args)
	{
		bind(c, t);
		bind(++c, args...);
	}
};


int x_main()
{
    std::cout << "Hello World!\n";
	
	try
	{
		ltc::Sqlite_db mydb("test.db");
		Statement x;
		x.exec(1, "To", 3.0,1L,2LL);

		std::cout << "Open OK\n";
		mydb.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar);");
		std::cout << "Create table OK\n";
		mydb.exec("INSERT INTO test (id, name) values (1, 'Series 1')");
		mydb.exec("INSERT INTO test (id, name) values (2, 'Serier 2')");
		//mydb.exec("select * from test;", [&](int cols, char** vals, char** names) 
		//	{
		//		return true;
		//	});

		ltc::Sqlite_stmt qry(&mydb, "select * from test where id>=? and id<=?");
		for (auto it = qry.exec(1,2); it != qry.end(); ++it)
			std::cout << " row id: " << it->column_int(0) << "\n";

		mydb.exec("DELETE FROM test");
		std::cout << "OK\n";
	}
	catch (ltc::Sqlite_error ex)
	{
		std::cerr << "Error: " << ex.what() << '\n';
		std::cerr << "Error code: " << ex.error_code() << ' ' << ex.error_code_str() << '\n';
	}
	return 0;
}
