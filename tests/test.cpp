/*
* Copyright (C) 2014 Jacob Hacker
* License: http://www.gnu.org/licenses/gpl.html GPL version 3
*/
#include <gtest/gtest.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

#include "../src/crypto.h"
#include "../src/mega.h"

using namespace MegaPP;

TEST(Crypto, A32){
	std::string testString{"Wikipedia"};
	auto a32 = stringToA32(testString);
	ASSERT_EQ(a32.size(), 3);
	
	ASSERT_EQ(a32[0], 1466526569);
	ASSERT_EQ(a32[1], 1885693033);
	ASSERT_EQ(a32[2], 1627389952);
	
	// Cast to cstr to remove extra null character padding
	ASSERT_EQ(testString, a32ToString(a32).c_str());

	for(uint i=0; i<1000; ++i){
		auto x = a32ToString({i});
		ASSERT_EQ(x, a32ToString(stringToA32(x)));
	}
}

TEST(Crypto, AesCbc){
	std::string testString = "1234567812345678";
	std::string key = "1234567812345678";
	std::vector<byte> iv;
	iv.assign(16, '\0');
	
	std::string cbcAes = aesCbcEncrypt(testString, key, iv);
	ASSERT_EQ(toHex(cbcAes), "6DAC1C56E747FAE03ACF8C6891E428E0");
}

TEST(Crypto, AesCbcA32){
	std::vector<uint> data = {0, 0, 0, 0};
	std::vector<uint> key = {0, 0, 0, 0};
	auto out = aesCbcEncryptA32(data, key);

	ASSERT_EQ(out.size(), 4);
	ASSERT_EQ(out[0], 1726565332);
	ASSERT_EQ(out[1], 4018809915);
	ASSERT_EQ(out[2], 2286746201);
	ASSERT_EQ(out[3], 3392416558);
}

TEST(Crypto, PrepareKey){	
	std::string testString = "Wikipedia";
	auto k = prepareKey(stringToA32(testString));

	ASSERT_EQ(k.size(), 4);
	ASSERT_EQ(k[0], 4294366237);
	ASSERT_EQ(k[1], 3347210462);
	ASSERT_EQ(k[2], 4216150900);
	ASSERT_EQ(k[3], 2125261106);
}

TEST(Crypto, Encoding){
	ASSERT_EQ(toB64("Wikipedia.com"), "V2lraXBlZGlhLmNvbQ==");
	ASSERT_EQ(toHex("Wikipedia"), "57696B697065646961");
	ASSERT_EQ(b64UrlEncode("+/=="), "-_");
	ASSERT_EQ(a32ToB64({1234,4321,3333,4567}), "AAAE0gAAEOEAAA0FAAAR1w");
}

TEST(Crypto, StringHash){
	std::string b64 = stringHash("Wikipedia", {1, 2, 3, 4});
	ASSERT_EQ(b64, "cDGRqi8wFw8");
}

TEST(Mega, login){
	std::ifstream tacFile{"test_account"};
	if(tacFile.good()){
		std::string username, password;
		tacFile >> username;
		tacFile >> password;

		Mega mega;
		mega.login(username, password);
		ASSERT_TRUE(mega.loggedIn());
	}else{
		std::cout << "Skipping login test, the file 'test_account' does not exist"
			<< std::endl;
	}
}

int main(int argc, char *argv[]){
	testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
