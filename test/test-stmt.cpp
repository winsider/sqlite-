#pragma warning(disable: 26495) // Google test warnings
#include <gtest/gtest.h>
#include <stdio.h>
#include <fstream>
#include <ssqll/ssqll.h>

using namespace ltc;

TEST(Sqlite_stmt, bind)
{
    remove("test.db");
    Sqlite_db db("test.db");
    db.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar);");
    auto insert = db.prepare("INSERT INTO test (id, name) values (?, ?)");
    insert.exec(1, "One");
    insert.exec(2, "Two");
    int found = 0;
    for (const auto& row : db.prepare("select * from test where id>=? and id<=?").exec(1, 2))
    {
        found++;
    }
    EXPECT_EQ(found, 2);
}

