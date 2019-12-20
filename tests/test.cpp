// Copyright 2019 DM00n <teamvortex@yandex.ru>
#include <gtest/gtest.h>
#include "json.hpp"

TEST(Parse, Text){
Json object("{\"lastname\" : \"Ivanov\"}");
object.parse("{\"lastname\" : \"Ivanov\"}");
EXPECT_EQ(std::any_cast<std::string>(object.keys[0]) , "lastname");
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
