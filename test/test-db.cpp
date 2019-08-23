#pragma warning(disable: 26495) // Google test warnings
#include <gtest/gtest.h>
#include <stdio.h>
#include <fstream>
#include <ssqll/ssqll.h>

using namespace ltc;

inline bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}


TEST(Sqlite_db, open)
{
    remove("test.db");
	Sqlite_db db("test.db");
	EXPECT_TRUE(db.is_open());
}

TEST(Sqlite_db, close)
{
    {
        remove("test.db");
        Sqlite_db db("test.db");
        EXPECT_TRUE(db.is_open());
        db.close();
        EXPECT_FALSE(db.is_open());
        db.open("test.db");
    }

    EXPECT_TRUE(file_exists("test.db"));
    remove("test.db");
    EXPECT_FALSE(file_exists("test.db"));
}

TEST(Sqlite_db, prepare)
{
    remove("test.db");
    Sqlite_db db("test.db");
    db.exec("CREATE TABLE test (id int, name varchar);");
    auto s = db.prepare("SELECT * FROM test");
    int count{};
    for (const auto& v : s.exec())
        count++;

    EXPECT_EQ(count, 0);
}

TEST(Sqlite_db, exec)
{
    remove("test.db");
	Sqlite_db db("test.db");
	db.exec("CREATE TABLE test (id int, name varchar);");
	db.exec("INSERT INTO test (id, name) values (1, 'Series 1')");
    EXPECT_EQ(db.changes(), 1);
    db.exec("INSERT INTO test (id, name) values (2, 'Serier 2')");
    EXPECT_EQ(db.changes(), 1);
	db.exec("DELETE FROM test");
    EXPECT_EQ(db.changes(), 2);
    EXPECT_EQ(db.total_changes(), 4);
}


