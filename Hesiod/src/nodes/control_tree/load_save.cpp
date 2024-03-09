/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

#include "gnode.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/control_tree.hpp"
#include "hesiod/serialization.hpp"

namespace hesiod::cnode
{

// ControlTree

void ControlTree::load_state(std::string fname)
{
  this->remove_all_nodes();

  std::ifstream  input_file_stream = std::ifstream(fname);
  nlohmann::json input_serialized_data = nlohmann::json();

  if (this->serialization_type ==
      hesiod::serialization::SerializationType::PLAIN)
  {
    input_file_stream >> input_serialized_data;
  }
  else
  {
    std::vector<char> buffer = std::vector<char>(
        std::istreambuf_iterator<char>(input_file_stream),
        std::istreambuf_iterator<char>());

    switch (this->serialization_type)
    {
    case hesiod::serialization::SerializationType::BJDATA:
      input_serialized_data = nlohmann::json::from_bjdata(buffer);
      break;
    case hesiod::serialization::SerializationType::BSON:
      input_serialized_data = nlohmann::json::from_bson(buffer);
      break;
    case hesiod::serialization::SerializationType::CBOR:
      input_serialized_data = nlohmann::json::from_cbor(buffer);
      break;
    case hesiod::serialization::SerializationType::MESSAGEPACK:
      input_serialized_data = nlohmann::json::from_msgpack(buffer);
      break;
    case hesiod::serialization::SerializationType::UBJSON:
      input_serialized_data = nlohmann::json::from_ubjson(buffer);
      break;
    default:
      LOG_ERROR("Unknown load type");
      break;
    }
  }

  this->deserialize_json_v2("data", input_serialized_data);

  input_file_stream.close();
}

void ControlTree::save_state(std::string fname)
{
  std::ofstream  outputFileStream = std::ofstream(fname, std::ios::trunc);
  nlohmann::json outputSerializedData = nlohmann::json();

  this->serialize_json_v2("data", outputSerializedData);

  if (this->serialization_type ==
      hesiod::serialization::SerializationType::PLAIN)
  {
    outputFileStream << outputSerializedData.dump(1) << std::endl;
  }
  else
  {
    std::vector<uint8_t> bytes = {};

    switch (this->serialization_type)
    {
    case hesiod::serialization::SerializationType::BJDATA:
      bytes = nlohmann::json::to_bjdata(outputSerializedData);
      break;
    case hesiod::serialization::SerializationType::BSON:
      bytes = nlohmann::json::to_bson(outputSerializedData);
      break;
    case hesiod::serialization::SerializationType::CBOR:
      bytes = nlohmann::json::to_cbor(outputSerializedData);
      break;
    case hesiod::serialization::SerializationType::MESSAGEPACK:
      bytes = nlohmann::json::to_msgpack(outputSerializedData);
      break;
    case hesiod::serialization::SerializationType::UBJSON:
      bytes = nlohmann::json::to_ubjson(outputSerializedData);
      break;
    default:
      LOG_ERROR("Unknown load type");
      break;
    }

    outputFileStream.write(reinterpret_cast<char *>(bytes.data()),
                           bytes.size());
    outputFileStream.flush();
  }

  outputFileStream.close();
}

bool ControlTree::serialize_json_v2(std::string     field_name,
                                    nlohmann::json &output_data)
{
  // TODO
  return true;
}

bool ControlTree::deserialize_json_v2(std::string     field_name,
                                      nlohmann::json &input_data)
{
  // TODO
  return true;
}

} // namespace hesiod::cnode
