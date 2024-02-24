#pragma once
#include <cstring>
#include <nlohmann/json.hpp>
#include <string>

#define SERIALIZATION_V2_IMPLEMENT_BATCH_BASE() SerializationBatchHelper BuildBatchHelperData();

namespace hesiod::serialization
{

    class SerializationBase
    {
    public:
        virtual ~SerializationBase() = default;
        virtual bool serialize_json_v2(std::string, nlohmann::json&) { return false; }
        virtual bool deserialize_json_v2(std::string, nlohmann::json&) { return false; }
    };

    enum class SerializationBatchHelperElementType : char
    {
        BYTE = 0,
        CHARACTER = 1,
        UNSIGNED_SHORT = 2,
        SHORT = 3,
        UNSIGNED_INT = 4,
        INT = 5,
        LONG_LONG = 6,
        UNSIGNED_LONG_LONG = 7,
        FLOAT = 8,
        DOUBLE = 9,
        STL_STRING = 10,
        OBJECT = 11,
        OBJECT_PTR = 12,
        INVALID = 13
    };

    class SerializationBatchHelperElement
    {
    public:
        SerializationBatchHelperElement():
            name(),
            data(),
            type(SerializationBatchHelperElementType::INVALID)
        {}

        SerializationBatchHelperElement(SerializationBatchHelperElement& S)
        {
            this->operator=(S);
        }

        ~SerializationBatchHelperElement() = default;

        SerializationBatchHelperElement& operator=(SerializationBatchHelperElement s)
        {
            this->name = s.name;
            this->data = s.data;
            this->type = s.type;

            return *this;
        }
    public:
        std::string name;
        void* data;
        SerializationBatchHelperElementType type;
    };

    class SerializationBatchHelper : SerializationBase
    {
    public:
        SerializationBatchHelper();
        ~SerializationBatchHelper();

        bool serialize_json_v2(std::string fieldName, nlohmann::json& outputData);
        bool deserialize_json_v2(std::string fieldName, nlohmann::json& inputData);

        // Wrappers for the ease of use

        SerializationBatchHelper& AddByte(std::string key, unsigned char* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::BYTE;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddCharacter(std::string key, char* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::CHARACTER;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddUnsignedShort(std::string key, unsigned short* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::UNSIGNED_SHORT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddShort(std::string key, short* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::SHORT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddUnsignedInt(std::string key, unsigned int* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::UNSIGNED_INT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddInt(std::string key, int* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::INT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddLongLong(std::string key, long long* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::LONG_LONG;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddUnsignedLongLong(std::string key, unsigned long long* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::UNSIGNED_LONG_LONG;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddFloat(std::string key, float* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::FLOAT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddDouble(std::string key, double* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::DOUBLE;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddStlString(std::string key, std::string* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::STL_STRING;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddObject(std::string key, SerializationBase* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::OBJECT;
            this->elements.push_back(e);

            return *this;
        }

        SerializationBatchHelper& AddObjectPtr(std::string key, SerializationBase* value) 
        {
            SerializationBatchHelperElement* e = new SerializationBatchHelperElement();
            e->name = key;
            e->data = value;
            e->type = SerializationBatchHelperElementType::OBJECT_PTR;
            this->elements.push_back(e);

            return *this;
        }
    private:
        std::vector<SerializationBatchHelperElement*> elements;
    };

    class SerializationBatchBase : public SerializationBase
    {
    public:
        SerializationBatchBase() = default;
        ~SerializationBatchBase() = default;
        
        virtual SerializationBatchHelper BuildBatchHelperData() {
            return SerializationBatchHelper();
        }

        bool serialize_json_v2(std::string fieldName, nlohmann::json& outputData);
        bool deserialize_json_v2(std::string fieldName, nlohmann::json& inputData);
    };

}