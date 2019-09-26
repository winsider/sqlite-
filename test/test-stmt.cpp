#pragma warning(disable: 26495) // Google test warnings
#include <gtest/gtest.h>
#include <stdio.h>
#include <fstream>
#include <ssqll/ssqll.h>

using namespace ltc;

class Test_Sqlite_stmt : public ::testing::Test
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



TEST_F(Test_Sqlite_stmt, exec_one_par)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int);");
    auto insert = db.prepare("INSERT INTO test (id) values (?)");
    insert.exec(1);
    insert.exec(2);
    int found = 0;
    db.prepare("select * from test where id<=?")
        .query([&found](auto& row)
        {
            found++;
            return true;
        }, 2);
    EXPECT_EQ(found, 2);
}


TEST_F(Test_Sqlite_stmt, exec_two_par)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar);");
    auto insert = db.prepare("INSERT INTO test (id, name) values (?, ?)");
    insert.exec(1, "One");
    insert.exec(2, "Two");
    int found = 0;
    db.prepare("select * from test where id>=? and id<=?")
        .query([&found](auto& row)
        {
            found++;
            return true;
        }, 1, 2);

    EXPECT_EQ(found, 2);
}

TEST_F(Test_Sqlite_stmt, exec_three_par)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar, value real);");
    auto insert = db.prepare("INSERT INTO test (id, name, value) values (?, ?, ?)");
    insert.exec(1, "One", 1.0f);
    insert.exec(2, "Two", 2.0f);
    int found = 0;
    db
        .prepare("select * from test where id>=? and id<=? and value<?")
        .query([&found](auto& row)
        {
            found++;
            return true;
        }, 1, 2, 10.0f);

    EXPECT_EQ(found, 2);
}

TEST_F(Test_Sqlite_stmt, is_null)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar, value real);");
    auto insert = db.prepare("INSERT INTO test (id, name, value) values (?, ?, ?)");
    insert.exec(1, "One", Null);
    insert.exec(2, "Two", Null);
    int found = 0;
    int found_nulls = 0;
    db.prepare("select * from test where id>=? and id<=? and value is null")
        .query([&found,&found_nulls](auto& row)
        {
            ++found;
            if (row.is_null(2))
                found_nulls++;
            return true;
        }, 1, 2);

    EXPECT_EQ(found, 2);
}

TEST_F(Test_Sqlite_stmt, type)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar, value real);");
    auto insert = db.prepare("INSERT INTO test (id, name, value) values (?, ?, ?)");
    insert.exec(1, "One", Null);
    insert.exec(2, "Two", Null);
    int found = 0;
    db.prepare("select * from test where id>=? and id<=? and value is null")
        .query([&found](auto& row)
        {
            found++;
            EXPECT_EQ(row.type(0), Datatype::db_integer);
            EXPECT_EQ(row.type(1), Datatype::db_text);
            EXPECT_EQ(row.type(2), Datatype::db_null);
            return true;
        }, 1, 2);
    EXPECT_EQ(found, 2);
}

TEST_F(Test_Sqlite_stmt, scalar)
{
    Sqlite_db db(DbName);
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar, value real);");
    auto insert = db.prepare("INSERT INTO test (id, name, value) values (?, ?, ?)");
    insert.exec(1, "One", 1.0);
    insert.exec(2, "Two", 2.0);

    // auto count = db.prepare("SELECT COUNT(*) FROM TEST");
    // int count1;
    // const auto countOK = count.scalar(count1);
    // ASSERT_TRUE(countOK); 
    // ASSERT_EQ(count1, 2);

    // auto max1 = db.prepare("SELECT MAX(id) FROM TEST WHERE id<?");
    // EXPECT_EQ(max1.scalar_int(3), 2);
    // EXPECT_EQ(max1.scalar_int(2), 1);

    // auto max2 = db.prepare("SELECT MAX(id) FROM Test");
    // EXPECT_EQ(max2.scalar_int(), 1);
}