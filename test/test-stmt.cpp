#pragma warning(disable: 26495) // Google test warnings
#include <gtest/gtest.h>
#include <stdio.h>
#include <fstream>
#include <ssqll/ssqll.h>

using namespace ltc;

TEST(Sqlite_stmt, exec_one_par)
{
    remove("test.db");
    Sqlite_db db("test.db");
    db.exec("CREATE TABLE IF NOT EXISTS test (id int);");
    auto insert = db.prepare("INSERT INTO test (id) values (?)");
    insert.exec(1);
    insert.exec(2);
    int found = 0;
    for (const auto& row : db.prepare("select * from test where id<=?").exec_range(2))
    {
        found++;
    }
    EXPECT_EQ(found, 2);
}


TEST(Sqlite_stmt, exec_two_par)
{
    remove("test.db");
    Sqlite_db db("test.db");
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar);");
    auto insert = db.prepare("INSERT INTO test (id, name) values (?, ?)");
    insert.exec(1, "One");
    insert.exec(2, "Two");
    int found = 0;
    for (const auto& row : db.prepare("select * from test where id>=? and id<=?").exec_range(1, 2))
    {
        found++;
    }
    EXPECT_EQ(found, 2);
}

TEST(Sqlite_stmt, exec_three_par)
{
    remove("test.db");
    Sqlite_db db("test.db");
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar, value real);");
    auto insert = db.prepare("INSERT INTO test (id, name, value) values (?, ?, ?)");
    insert.exec(1, "One", 1.0f);
    insert.exec(2, "Two", 2.0f);
    int found = 0;
    for (const auto& row : db.prepare("select * from test where id>=? and id<=?").exec_range(1, 2))
    {
        found++;
    }
    EXPECT_EQ(found, 2);
}


