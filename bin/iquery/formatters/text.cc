// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>

#include <lib/fxl/strings/string_printf.h>

#include "garnet/bin/iquery/formatters/text.h"
#include "garnet/bin/iquery/options.h"

namespace iquery {

namespace {

#define INDENT_SIZE 2

inline std::string Indent(int indent) {
  return std::string(indent * INDENT_SIZE, ' ');
}

// This version exists so we can pass in the indentation.
std::string RecursiveFormatCat(const Options& options, const ObjectNode& root,
                               int indent) {
  const auto& object = root.object;

  std::ostringstream ss;
  if (options.path_format != Options::PathFormatting::NONE) {
    // The path already considers the object name.
    ss << Indent(indent)
       << "PATH: " << FormatPath(options.path_format, root.basepath)
       << std::endl;
  }

  for (const auto& property : *object.properties) {
    ss << Indent(indent) << FormatString(property.key) << " = "
       << FormatString(property.value) << std::endl;
  }

  for (const auto& metric : *object.metrics) {
    ss << Indent(indent) << FormatString(metric.key) << " = "
       << FormatMetricValue(metric) << std::endl;
  }

  // We print recursively. The recursion nature of the cat called is already
  // taken care of by now.
  for (const auto& child : root.children) {
    ss << Indent(indent) << child.object.name << ":" << std::endl;
    ss << RecursiveFormatCat(options, child, indent + 1);
  }

  return ss.str();
}

std::string FormatFind(const Options& options,
                       const std::vector<ObjectNode>& results) {
  std::stringstream ss;
  for (const auto& node : results) {
    ss << FormatPath(options.path_format, node.basepath, node.object.name)
       << std::endl;
  }
  return ss.str();
}

std::string FormatLs(const Options& options,
                     const std::vector<ObjectNode>& results) {
  std::stringstream ss;
  for (const auto& node : results) {
    ss << FormatPath(options.path_format, node.basepath, node.object.name)
       << std::endl;
  }
  return ss.str();
}

std::string FormatCat(const Options& options,
                      const std::vector<ObjectNode>& results) {
  std::ostringstream ss;
  for (const auto& root_node : results) {
    ss << root_node.object.name << ":" << std::endl;
    ss << RecursiveFormatCat(options, root_node, 1);
  }

  return ss.str();
}

}  // namespace

std::string TextFormatter::Format(const Options& options,
                                  const std::vector<ObjectNode>& results) {
  switch (options.mode) {
    case Options::Mode::CAT:
      return FormatCat(options, results);
    case Options::Mode::FIND:
      return FormatFind(options, results);
    case Options::Mode::LS:
      return FormatLs(options, results);
    case Options::Mode::UNSET: {
      FXL_LOG(ERROR) << "Unset Mode";
      return "";
    }
  }
}

}  // namespace iquery
