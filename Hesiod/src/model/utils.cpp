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

} // namespace hesiod
