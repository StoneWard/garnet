// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <deque>
#include <string>
#include <vector>

namespace zxdb {

// This class implements a push model for input of characters, allowing it to
// be used in asynchronous contexts.
//
// The model is you create a LineInput class outside of the input loop. It
// encapsulates the history state and remembers the prompt. When you want to
// read a line:
//
//  1. Call BeginReadLine().
//  2. Push data to it via OnInput() until it returns true.
//  3. Get the input from line().
//  4. Repeat.
class LineInputBase {
 public:
  // Given some typing, returns a prioritized list of completions.
  using CompletionCallback = std::vector<std::string> (*)(const std::string&);

  explicit LineInputBase(const std::string& prompt);
  virtual ~LineInputBase();

  void set_max_cols(int max) { max_cols_ = max; }
  void set_completion_callback(CompletionCallback cc) {
    completion_callback_ = cc;
  }

  // Returns the current line text.
  const std::string& line() const { return history_[history_index_]; }

  // Returns the current insert position.
  size_t pos() const { return pos_; }

  // Call to initialize reading a new line.
  void BeginReadLine();

  // Provides one character of input to the editor. Returns true if the line
  // is complete (the user has pressed enter).
  bool OnInput(char c);

 protected:
  // Abstract output function, overridden by a derived class to output to
  // screen.
  virtual void Write(const std::string& data) = 0;

 private:
  void HandleEscapedInput(char c);

  void HandleBackspace();
  void HandleDelete();
  void HandleEnter();
  void HandleTab();

  void Insert(char c);
  void MoveLeft();
  void MoveRight();
  void MoveUp();
  void MoveDown();
  void MoveHome();
  void MoveEnd();

  void CancelCompletion();
  void AcceptCompletion();

  void RepaintLine();

  // Helper to return the current line of text.
  std::string& cur_line() { return history_[history_index_]; }

  const std::string prompt_;
  int max_cols_ = 80;
  CompletionCallback completion_callback_ = nullptr;

  // The history is basically the line stack going back in time as indices
  // increase. The currently viewed line is at [history_index_] and this is
  // where editing happens. When you start a new text entry, a new history
  // item is added and you delete it.
  //
  // This is simple but can be a bit confusing if you go back, edit, and then
  // press enter. The history item itself will be edited, and the same edited
  // version will be added again as the most recent history entry.
  //
  // This is weird because the editing has actually changed history. A more
  // complex model might be to maintain a virtual shadow copy of history that
  // you edit, and this shadow copy is replaced with the actual history
  // whenever you start editing a new line.
  std::deque<std::string> history_;  // front() is newest.
  size_t history_index_ = 0;  // Offset from history_.front().
  const size_t max_history_ = 256;

  bool completion_mode_ = false;
  std::vector<std::string> completions_;
  size_t completion_index_ = 0;

  // Tracks the current line's state before suggesting completions so we can
  // put them back if necessary. Only valid when completion_mode_ = true.
  std::string line_before_completion_;
  size_t pos_before_completion_;

  // When an escape is read, we enter "escaped input" mode which interprets the
  // next few characters input as an escape sequence.
  bool reading_escaped_input_ = false;
  std::string escape_sequence_;

  size_t pos_;  // Current editing position.
};

// Implementation of LineInput that prints to stdout.
class LineInputStdout : public LineInputBase {
 public:
  LineInputStdout(const std::string& prompt);
  ~LineInputStdout() override;

 protected:
  void Write(const std::string& str) override;
};


// A blocking implementation that reads from stdin and writes to stdout.
class LineInputBlockingStdio : public LineInputStdout {
 public:
  LineInputBlockingStdio(const std::string& prompt);

  std::string ReadLine();
};

}  // namespace zxdb
