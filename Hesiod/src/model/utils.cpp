/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <string>

#include <QJsonValue>

#include "hesiod/model/model_utils.hpp"

namespace hesiod
{

bool convert_qjsonvalue_to_bool(QJsonValue v, bool &out)
{
  if (!v.isUndefined())
  {
    QString str_num = v.toString();
    out = (str_num == "true" ? true : false);
    return true;
  }
  else
    return false;
}

bool convert_qjsonvalue_to_float(QJsonValue v, float &out)
{
  if (!v.isUndefined())
  {
    QString str_num = v.toString();

    bool  ok;
    float value = str_num.toFloat(&ok);
    if (ok)
    {
      out = value;
      return true;
    }
    else
      return false;
  }
  return false;
}

bool convert_qjsonvalue_to_int(QJsonValue v, int &out)
{
  if (!v.isUndefined())
  {
    QString str_num = v.toString();

    bool ok;
    int  value = str_num.toInt(&ok);
    if (ok)
    {
      out = value;
      return true;
    }
    else
      return false;
  }
  return false;
}

bool convert_qjsonvalue_to_uint(QJsonValue v, uint &out)
{
  if (!v.isUndefined())
  {
    QString str_num = v.toString();

    bool ok;
    uint value = str_num.toUInt(&ok);
    if (ok)
    {
      out = value;
      return true;
    }
    else
      return false;
  }
  return false;
}

bool convert_qjsonvalue_to_string(QJsonValue v, std::string &out)
{
  if (!v.isUndefined())
  {
    out = v.toString().toStdString();
    return true;
  }
  return false;
}

bool convert_qjsonvalue_to_vector_float(QJsonValue v, std::vector<float> &out)
{
  if (!v.isUndefined())
  {
    out.clear();
    QJsonArray json_array = v.toArray();
    for (const QJsonValue &x : json_array)
      out.push_back((float)x.toDouble());
    return true;
  }
  return false;
}

bool convert_qjsonvalue_to_vector_int(QJsonValue v, std::vector<int> &out)
{
  if (!v.isUndefined())
  {
    out.clear();
    QJsonArray json_array = v.toArray();
    for (const QJsonValue &x : json_array)
      out.push_back((int)x.toDouble());
    return true;
  }
  return false;
}

bool convert_qjsonvalue_to_vector_vector_float(QJsonValue                       v,
                                               std::vector<std::vector<float>> &out,
                                               int                              stride)
{
  if (!v.isUndefined())
  {
    out.clear();
    if (stride > 0)
    {
      QJsonArray         json_array = v.toArray();
      std::vector<float> vec_work = {};
      int                k = 0;

      for (const QJsonValue &x : json_array)
      {
        float v = (float)x.toDouble();

        vec_work.push_back(v);
        k++;

        if (k % stride == 0)
        {
          out.push_back(vec_work);
          vec_work.clear();
        }
      }
    }
    return true;
  }
  return false;
}

QJsonArray std_vector_float_to_qjsonarray(const std::vector<float> &vector)
{
  QJsonArray json_array;
  for (const float &v : vector)
    json_array.append(v);
  return json_array;
}

QJsonArray std_vector_int_to_qjsonarray(const std::vector<int> &vector)
{
  QJsonArray json_array;
  for (const int &v : vector)
    json_array.append(v);
  return json_array;
}

QJsonArray std_vector_vector_float_to_qjsonarray(
    const std::vector<std::vector<float>> &vector)
{
  QJsonArray json_array;
  for (const std::vector<float> &sub_vector : vector)
    for (const float &v : sub_vector)
      json_array.append(v);
  return json_array;
}

} // namespace hesiod
