/*
 * Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Firestore/core/src/util/byte_stream_istream.h"

#include <string>
#include <vector>

#include "Firestore/core/src/util/statusor.h"

namespace firebase {
namespace firestore {
namespace util {

StreamReadResult ByteStreamIstream::ReadUntil(char delim, size_t max_length) {
  // Fill `result` until a `{` is found.
  std::string result(max_length + 1, '\0');
  input_->get(const_cast<char*>(result.data()), max_length + 1, delim);

  return ToReadResult(result);
}

StreamReadResult ByteStreamIstream::Read(size_t max_length) {
  std::string result(max_length + 1, '\0');
  input_->read(const_cast<char*>(result.data()), max_length);

  return ToReadResult(result);
}

StreamReadResult ByteStreamIstream::ToReadResult(const std::string& result) {
  if (input_->bad()) {
    return StreamReadResult(
        Status::FromErrno(Error::kErrorDataLoss,
                          "Reading input stream failed with error"),
        input_->eof());
  }

  // input_->fail() means less than expected characters are read, we can still
  // continue until eof() is hit, so clearing failbit here.
  if (input_->fail() && !input_->eof()) {
    input_->clear(std::istream::goodbit);
  }

  return StreamReadResult(
      StatusOr<std::string>(result.substr(0, input_->gcount())), input_->eof());
}
}  // namespace util
}  // namespace firestore
}  // namespace firebase
