add_library(hesiod_qt_logging INTERFACE)

set(tmp_qt_logging_rules
    "qt.widgets.gestures.debug=false;qt.qpa.*=false;qt.pointer.*=false")
string(REPLACE ";" "\\;" tmp_qt_logging_rules_escaped "${qt_logging_rules}")

target_compile_definitions(
  hesiod_qt_logging
  INTERFACE
    HESIOD_QPUTENV_QT_LOGGING_RULES="\"${tmp_qt_logging_rules_escaped}\"")
