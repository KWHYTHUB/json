#include <json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <json/stl_serialize.hpp>

template <class S>
void println(S str) {
	std::cout << str << std::endl;
}

void debug(const json::Value& value) {
	println(value.dump());
}

struct CoolStruct {
	std::string name;
	int value;
};

template <>
struct json::Serialize<CoolStruct> {
	static json::Value to_json(const CoolStruct& cool) {
		return json::Object {
			{ "value", cool.value },
			{ "name", cool.name }
		};
	}
	static CoolStruct from_json(const json::Value& value) {
		return CoolStruct {
			.name = value["name"].as_string(),
			.value = value["value"].as_int()
		};
	}
};

template <class T>
requires std::is_enum_v<T>
struct json::Serialize<T> {
	static json::Value to_json(const T& value) {
		return json::Value(static_cast<int>(value));
	}
};

enum class Hooray {
	Hooray
};

int main() {
	json::Value foo = json::Object {
		{"hello", 69}
	};

	debug(foo);

	foo["world"] = 31;

	debug(foo);

	json::Value bar;

	bar = foo;

	bar["hello"] = 10;

	debug(foo);
	debug(bar);

	{
		json::Value obj1 = json::Object {};
		json::Value obj2 = json::Object {};

		obj1["hello"] = 10;
		obj1["world"] = 20;

		obj2["world"] = 20;
		obj2["hello"] = 10;

		debug(obj1);
		debug(obj2);

		std::cout << std::boolalpha << "equal? " << (obj1 == obj2) << std::endl;
	}

	{
		json::Value obj(
			{ { "hello", "world!"}, {"sux", 123} }
		);
		debug(obj);
	}

	{
		auto obj = json::Value::from_str(R"(
{
    "sapphire": "@PROJECT_VERSION@",
    "id": "sapphire.loader",
    "version": "@PROJECT_VERSION@@PROJECT_VERSION_SUFFIX@",
    "name": "Sapphire",
    "developer": "Sapphire Team",
    "description": "The Sapphire mod loader",
    "repository": "https://github.com/KWHYTHUB/sapphire",
    "resources": {
        "fonts": {
            "mdFont": {
                "path": "fonts/Ubuntu-Regular.ttf",
                "size": 80
            },
            "mdFontB": {
                "path": "fonts/Ubuntu-Bold.ttf",
                "size": 80
            },
            "mdFontI": {
                "path": "fonts/Ubuntu-Italic.ttf",
                "size": 80
            },
            "mdFontBI": {
                "path": "fonts/Ubuntu-BoldItalic.ttf",
                "size": 80
            },
            "mdFontMono": {
                "path": "fonts/UbuntuMono-Regular.ttf",
                "size": 80
            }
        },
        "sprites": [
            "images/*.png"
        ],
        "files": [
            "sounds/*.ogg"
        ],
        "spritesheets": {
            "LogoSheet": [
                "logos/*.png"
            ],
            "APISheet": [
                "*.png"
            ],
            "BlankSheet": [
                "blanks/*.png"
            ]
        }
    }
}
		)");
		debug(obj);
		println(obj["resources"]["spritesheets"]["APISheet"][0].as_string());
		println(obj["resources"]["fonts"]["mdFont"]["size"].as_double());
	}

	{
		CoolStruct cool {
			.name = "hello",
			.value = 123
		};
		json::Value obj = json::Object {};
		obj["cool"] = cool;
		debug(obj);

		auto another = obj.get<CoolStruct>("cool");
		std::cout << another.name << std::endl;
	}

	{
		Hooray value = Hooray::Hooray;
		json::Object obj;
		obj["hooray"] = value;
		debug(obj);
	}

	{
		auto obj = json::parse(R"(
			{
				"hello": "world",
				"nice": null,
				"nested": {
					"objects": ["are", "cool", "\nice \t\ry \buddy, \format me i\f you can \\ \" \\\" "],
					"int": 23,
					"half": 11.5,
					"nested": {
						"again": true
					}
				}
			}
		)");
		println(obj["nested"]["half"].as_double());
		println(obj.dump());
		println(obj.dump(json::NO_INDENTATION));
		println(obj.dump(json::TAB_INDENTATION));

		assert(obj == json::parse(obj.dump()));
		assert(obj == json::parse(obj.dump(json::NO_INDENTATION)));
		assert(obj == json::parse(obj.dump(json::TAB_INDENTATION)));
		assert(obj == json::parse(obj.dump(69)));
	}
	{
		json::Value json;
		json["hello"] = "world";
		println(json.dump());
		assert(json.dump(json::NO_INDENTATION) == R"({"hello":"world"})");
	}
	{
		auto obj = json::parse(R"(
			{
				"key": 5,
				"value": 6,
				"next": 8,
				"hi": 10
			}
		)");

		using UMap = std::unordered_map<std::string, size_t>;
		auto umap = UMap {
			{ "key", 5 },
			{ "value", 6 },
			{ "next", 8 },
			{ "hi", 10 },
		};
		assert(obj.template as<UMap>() == umap);

		using Map = std::map<std::string, size_t>;
		auto map = Map {
			{ "key", 5 },
			{ "value", 6 },
			{ "next", 8 },
			{ "hi", 10 },
		};
		assert(obj.template as<Map>() == map);

		using VMap = std::map<std::string, json::Value>;
		auto vmap = VMap {
			{ "key", 5 },
			{ "value", 6 },
			{ "next", 8 },
			{ "hi", 10 },
		};
		assert(obj.template as<VMap>() == vmap);
	}
	{
		auto arr = json::parse(R"(
			["hi", "mommy", ":3"]
		)");

		using Vec = std::vector<std::string>;
		auto vec = Vec { "hi", "mommy", ":3" };
		assert(arr.template as<Vec>() == vec);

		using Set = std::set<std::string>;
		auto set = Set { "hi", "mommy", ":3" };
		assert(arr.template as<Set>() == set);

		using USet = std::set<std::string>;
		auto uset = USet { "hi", "mommy", ":3" };
		assert(arr.template as<USet>() == uset);
	}
	{
		auto obj = json::parse("{\"hello\": \"Ol\xC3\xA1!\"}");
		assert(obj["hello"].as_string() == "Ol\xC3\xA1!");
		println(obj["hello"].as_string());
	}
	{
		auto val1 = json::parse(R"("value")");
		auto val2 = json::parse(R"({ "key": 4 })");
		auto val3 = json::Object {
			{ "key", 4 },
		};
		auto val4 = json::Object {
			{ "key", 6 },
		};
		std::unordered_set<json::Value> values { val1, val2, val3, val4 };
		std::unordered_set<json::Value> against { val1, val2, val4 };
		std::set<json::Value> setValues { val1, val2, val3, val4 };
		std::set<json::Value> setAgainst { val1, val2, val4 };
		assert(values.size() == against.size());
		assert(values == against);
		assert(setValues.size() == setAgainst.size());
		assert(setValues == setAgainst);
		println(json::Value(values).dump());
		println(json::Value(setValues).dump());
	}
	println("All tests passed :3");
}
