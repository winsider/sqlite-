#pragma warning(disable: 26495) // Google test warnings
#include <gtest/gtest.h>
#include "sqlite++.h"

using namespace ltc;



TEST(Sqlite_db, open)
{
	Sqlite_db db("test.db");
	EXPECT_NE(db.handle(), nullptr);
}


TEST(Sqlite_db, exec)
{
	Sqlite_db db("test.db");
	db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar);");
	db.exec("INSERT INTO test (id, name) values (1, 'Series 1')");
	db.exec("INSERT INTO test (id, name) values (2, 'Serier 2')");
	ltc::Sqlite_stmt qry(&db, "select * from test where id>=? and id<=?");
	int found = 0;
	for (auto it = qry.exec(1, 2); it != qry.end(); ++it)
		++found;
	EXPECT_EQ(found, 2);
	db.exec("DELETE FROM test");
}

TEST(Sqlite_db, iterate)
{
}

TEST(Sqlite_db, InsertWithSplitLast)
{
}

TEST(Btree, IterateWithSplit)
{
}
