#include "hesiod/model/base_node.hpp"

namespace hesiod
{

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

} // namespace hesiod
