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

class Test_Sqlite_db : public ::testing::Test
{
protected:
    void SetUp() override
    {
        remove(DbName.c_str());
    }

    void TearDown() override
    {
        remove(DbName.c_str());
    }

    std::string DbName{ "test.db" };
};

TEST_F(Test_Sqlite_db, open)
{
    remove("test.db");
	Sqlite_db db("test.db");
	EXPECT_TRUE(db.is_open());
}

TEST_F(Test_Sqlite_db, close)
{
    {
    	Sqlite_db db(DbName);
        EXPECT_TRUE(db.is_open());
        db.close();
        EXPECT_FALSE(db.is_open());
        db.open(DbName);
    }

    EXPECT_TRUE(file_exists(DbName));
    remove(DbName.c_str());
    EXPECT_FALSE(file_exists(DbName));
}

TEST_F(Test_Sqlite_db, prepare)
{
	Sqlite_db db(DbName);
    db.exec("CREATE TABLE test (id int, name varchar);");
    auto s = db.prepare("SELECT * FROM test");
    int count{};
    s.query([&count](auto& row)
    {
        count++;
        return true;
    });
    EXPECT_EQ(count, 0);
}

TEST_F(Test_Sqlite_db, exec)
{
 	Sqlite_db db(DbName);
	db.exec("CREATE TABLE test (id int, name varchar);");
	db.exec("INSERT INTO test (id, name) values (1, 'Series 1')");
    EXPECT_EQ(db.changes(), 1);
    db.exec("INSERT INTO test (id, name) values (2, 'Serier 2')");
    EXPECT_EQ(db.changes(), 1);
	db.exec("DELETE FROM test");
    EXPECT_EQ(db.changes(), 2);
    EXPECT_EQ(db.total_changes(), 4);
}

TEST_F(Test_Sqlite_db, multistep_exec)
{
    const char * sql = R"sql(
        CREATE TABLE test (id int, name varchar);
        INSERT INTO test (id, name) values (1, 'Series 1');
        INSERT INTO test (id, name) values (2, 'Serier 2');
        DELETE FROM test;
    )sql";
	Sqlite_db db(DbName);
	db.exec(sql);
    EXPECT_EQ(db.total_changes(), 4);
}

